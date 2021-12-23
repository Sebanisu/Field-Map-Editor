//
// Created by pcvii on 11/22/2021.
//

#ifndef MYPROJECT_TEXTURE_HPP
#define MYPROJECT_TEXTURE_HPP
#include "Renderer.hpp"
#include "unique_value.hpp"
#include <stb_image.h>


class Texture
{
private:
  GLID                                     m_renderer_id  = {};
  std::filesystem::path                    m_path         = {};
  open_viii::graphics::Point<std::int32_t> m_width_height = {};
  // std::int32_t             m_bpp         = {};
  // std::vector<open_viii::graphics::Color32RGBA> m_colors       = {};


public:
  Texture() = default;
  Texture(std::filesystem::path path);
  Texture(std::array<std::uint8_t, 4U> color)
    : Texture(std::bit_cast<std::uint32_t>(color))
  {
  }
  Texture(std::uint32_t color)
    : m_width_height{ 1, 1 }
  {

    init_texture(&color);
  }
  template<std::ranges::contiguous_range R>
  Texture(
    R            r,
    std::int32_t in_width,
    std::int32_t in_height,
    bool         in_flip = false)
    : m_width_height{ in_width, in_height }
  {
    if (in_flip)
    {
      flip(r, in_width);
    }
    init_texture(std::ranges::data(r));
  }
  void init_texture(const void *color)
  {
    if (m_width_height.area() == 0)
    {
      return;
    }
    m_renderer_id = GLID{ []() -> std::uint32_t {
                           std::uint32_t tmp;
                           GLCall{}(glGenTextures, 1, &tmp);
                           GLCall{}(glBindTexture, GL_TEXTURE_2D, tmp);
                           return tmp;
                         }(),
                          [](const std::uint32_t id) {
                            GLCall{}(glDeleteTextures, 1, &id);
                            Texture::UnBind();
                          } };
    GLCall{}(
      &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLCall{}(
      &glTexParameteri,
      GL_TEXTURE_2D,
      GL_TEXTURE_MIN_FILTER,
      GL_NEAREST_MIPMAP_NEAREST);
    GLCall{}(&glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    GLCall{}(&glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GLCall{}(
      &glTexImage2D,
      GL_TEXTURE_2D,
      0,
      GL_RGBA8,
      width(),
      height(),
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      color);
    // Unavailable in OpenGL 2.1, use gluBuild2DMipmaps() instead
    GLCall{}(glGenerateMipmap, GL_TEXTURE_2D);
    GLCall{}(glBindTexture, GL_TEXTURE_2D, 0);
  }
  template<std::ranges::random_access_range R>
  requires std::permutable<std::ranges::iterator_t<R>>
  static constexpr void
    flip_slow(R &range, const std::ranges::range_difference_t<R> stride)
  {
    using std::ranges::begin;
    using std::ranges::end;
    using std::ranges::rotate;
    auto b = begin(range);
    auto e = end(range);
    auto m = e;
    std::ranges::advance(m, -stride);
    while (b < m)
    {
      rotate(b, m, e);
      std::ranges::advance(b, stride);
    }
  }

  static void save(
    std::span<uint8_t>    data,
    std::filesystem::path path,
    int                   width,
    int                   height)
  {
    fmt::print(
      "{}\t{} bytes\twidth {}\theight {}\n",
      path.string().c_str(),
      std::ranges::size(data),
      width,
      height);
    Texture::flip(data, width * 4);
    if (path.has_parent_path())
    {
      std::error_code ec{};
      std::filesystem::create_directories(path.parent_path(), ec);
      if (ec)
      {
        fmt::print(
          stderr,
          "error {}:{} - {}: {} - path: {}\n",
          __FILE__,
          __LINE__,
          ec.value(),
          ec.message(),
          path.string().c_str());
        ec.clear();
      }
    }
    stbi_write_png(
      path.string().c_str(), width, height, 4, data.data(), width * 4);
  }

  template<std::ranges::contiguous_range R>
  requires std::permutable<std::ranges::iterator_t<R>>
  static void flip(R &range, const std::ranges::range_difference_t<R> stride)
  {
    if (std::ranges::empty(range))
    {
      return;
    }
    if (std::ranges::ssize(range) % stride != 0)
    {
      // throw or use another function that's more flexible.
      return flip_slow(range, stride);
    }
    static constexpr auto sizeof_value = sizeof(std::ranges::range_value_t<R>);
    const auto            stride_in_bytes =
      static_cast<std::size_t>(stride) * sizeof_value;
    auto       buffer      = std::make_unique<char[]>(stride_in_bytes);
    const auto swap_memory = [tmp = buffer.get(), stride_in_bytes](
                               std::ranges::range_reference_t<R> &left,
                               std::ranges::range_reference_t<R> &right) {
      std::memcpy(tmp, &left, stride_in_bytes);
      std::memcpy(&left, &right, stride_in_bytes);
      std::memcpy(&right, tmp, stride_in_bytes);
    };
    auto b = std::ranges::begin(range);
    auto m = std::ranges::end(range);
    while (b < m)
    {
      std::ranges::advance(m, -stride);
      swap_memory(*b, *m);
      std::ranges::advance(b, stride);
    }
  }

  GLID_copy    ID() const noexcept;
  void         Bind(int slot = 0) const;
  static void  UnBind();
  std::int32_t width() const;
  std::int32_t height() const;
};
static_assert(Bindable<Texture>);
#endif// MYPROJECT_TEXTURE_HPP
