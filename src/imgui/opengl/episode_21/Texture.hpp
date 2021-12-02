//
// Created by pcvii on 11/22/2021.
//

#ifndef MYPROJECT_TEXTURE_HPP
#define MYPROJECT_TEXTURE_HPP
#include "Renderer.hpp"
#include <algorithm>
#include <array>
#include <filesystem>
#include <open_viii/graphics/Rectangle.hpp>
#include <ranges>
#include <stb_image.h>
class Texture
{
private:
  std::uint32_t                            m_renderer_id  = {};
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
  void
    init_texture(const void *color)
  {
    if(m_width_height.area() == 0)
    {
      return;
    }
    GLCall{ glGenTextures, 1, &m_renderer_id };
    GLCall{ glBindTexture, GL_TEXTURE_2D, m_renderer_id };
    GLCall{
      &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST
    };
    GLCall{ &glTexParameteri,
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_NEAREST_MIPMAP_NEAREST };
    GLCall{ &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT };
    GLCall{ &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT };
    GLCall{ &glTexImage2D,    GL_TEXTURE_2D, 0, GL_RGBA8,
            width(),          height(),      0, GL_RGBA,
            GL_UNSIGNED_BYTE, color };
    // Unavailable in OpenGL 2.1, use gluBuild2DMipmaps() instead
    GLCall{ glGenerateMipmap, GL_TEXTURE_2D };
    GLCall{ glBindTexture, GL_TEXTURE_2D, 0 };
  }
  template<std::ranges::random_access_range R>
  static constexpr void
    flip(R &range, std::ranges::range_difference_t<R> stride)
  {
    auto b = std::ranges::begin(range);
    auto e = std::ranges::end(range);
    auto m = e;
    std::ranges::advance(m, -stride);
    while (b < m)
    {
      std::ranges::rotate(b, m, e);
      std::ranges::advance(b, stride);
    }
  }
  ~Texture();
  std::uint32_t
    ID() const noexcept;
  void
    Bind(int slot = 0) const;
  static void
    UnBind();
  std::int32_t
    width() const;
  std::int32_t
    height() const;
  Texture(const Texture &) = delete;
  Texture &
    operator=(const Texture &) = delete;
  Texture(Texture &&other) noexcept;
  Texture &
    operator=(Texture &&other) noexcept;
  friend void
    swap(Texture &first, Texture &second) noexcept;
};
static_assert(Bindable<Texture>);
#endif// MYPROJECT_TEXTURE_HPP
