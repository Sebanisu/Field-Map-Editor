//
// Created by pcvii on 11/22/2021.
//

#ifndef FIELD_MAP_EDITOR_TEXTURE_HPP
#define FIELD_MAP_EDITOR_TEXTURE_HPP
#include "Image.hpp"
#include "Renderer.hpp"
#include "UniqueValue.hpp"
#include <concepts.hpp>
#include <filesystem>
#include <stb_image.h>
#include <stb_image_write.h>
#include <utility>
namespace glengine
{
#if __cpp_if_consteval
#define FME_NOT_CONSTEVAL !consteval// c++23
#else
#define FME_NOT_CONSTEVAL (!std::is_constant_evaluated())// c++20
#endif
class Texture
{
   private:
     Glid                  m_renderer_id = {};
     std::filesystem::path m_path        = {};
     std::int32_t          m_width       = {};
     std::int32_t          m_height      = {};
     // std::int32_t             m_bpp         = {};
     // std::vector<open_viii::graphics::Color32RGBA> m_colors       = {};

   public:
     static const constexpr GLint  s_sized_interal_format = GL_RGBA8;
     static const constexpr GLenum s_base_interal_format  = GL_RGBA;
     static const constexpr GLenum s_type                 = GL_UNSIGNED_BYTE;

     operator bool() const noexcept
     {
          return m_renderer_id != 0U;
     }
     constexpr Texture() = default;

     Texture(Glid &&new_id, std::int32_t new_width, std::int32_t new_height)
       : m_renderer_id(std::exchange(new_id, {}))
       , m_width(new_width)
       , m_height(new_height)
     {
     }
     Texture(std::int32_t new_width, std::int32_t new_height)
       : m_width{ new_width }
       , m_height{ new_height }
     {
          init_texture(nullptr);// allocate uninitialized texture
     }
     Texture(Image image);
     Texture(std::filesystem::path path, bool in_flip = false);
     Texture(std::array<std::uint8_t, 4U> color)
       : Texture(std::bit_cast<std::uint32_t>(color))
     {
     }
     Texture(std::uint32_t color)
       : m_width{ 1 }
       , m_height{ 1 }
     {

          init_texture(&color);
     }
     template<std::ranges::contiguous_range R>
     Texture(R r, std::int32_t in_width, std::int32_t in_height, bool in_flip = false)
       : m_width{ in_width }
       , m_height{ in_height }
     {
          if constexpr (!std::is_const_v<std::remove_reference_t<std::ranges::range_reference_t<R>>>)
          {
               if (in_flip)
               {
                    flip(r, in_width);
               }
               init_texture(std::ranges::data(r));
          }
          else
          {
               if (in_flip)
               {
                    std::vector<std::remove_cvref_t<std::ranges::range_value_t<R>>> copy{ std::ranges::begin(r), std::ranges::end(r) };
                    flip(copy, in_width);
                    init_texture(std::ranges::data(copy));
               }
               else
               {
                    init_texture(std::ranges::data(r));
               }
          }
     }
     void init_texture(const void *color)
     {
          if (std::cmp_equal(m_width * m_height, 0))
          {
               return;
          }
          m_renderer_id = Glid{ []() -> std::uint32_t {
                                    std::uint32_t tmp;
                                    GlCall{}(glGenTextures, 1, &tmp);
                                    GlCall{}(glBindTexture, GL_TEXTURE_2D, tmp);
                                    return tmp;
                               }(),
                                destroy };
          GlCall{}(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          GlCall{}(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
          GlCall{}(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
          GlCall{}(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
          GlCall{}(glTexImage2D, GL_TEXTURE_2D, 0, s_sized_interal_format, width(), height(), 0, s_base_interal_format, s_type, color);
          // Unavailable in OpenGL 2.1, use gluBuild2DMipmaps() instead
          GlCall{}(glGenerateMipmap, GL_TEXTURE_2D);
          GlCall{}(glBindTexture, GL_TEXTURE_2D, 0);
     }
     template<std::ranges::random_access_range R>
          requires std::permutable<std::ranges::iterator_t<R>>
     static constexpr void flip_slow(R &range, const std::ranges::range_difference_t<R> stride)
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

     static void save(std::span<uint8_t> data, std::filesystem::path path, int width, int height)
     {
          spdlog::debug(
            "{}\t{} bytes\twidth {}\theight {}", std::filesystem::absolute(path).string().c_str(), std::ranges::size(data), width, height);
          Texture::flip(data, width * 4);
          if (path.has_parent_path())
          {
               std::error_code ec{};
               std::filesystem::create_directories(path.parent_path(), ec);
               if (ec)
               {
                    spdlog::error("{}:{} - {}: {} - path: {}", __FILE__, __LINE__, ec.value(), ec.message(), path.string().c_str());
                    ec.clear();
               }
          }
          stbi_write_png(path.string().c_str(), width, height, 4, data.data(), width * 4);
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
          static constexpr auto sizeof_value    = sizeof(std::ranges::range_value_t<R>);
          const auto            stride_in_bytes = static_cast<std::size_t>(stride) * sizeof_value;
          auto                  buffer          = std::make_unique<char[]>(stride_in_bytes);
          const auto            swap_memory     = [tmp = buffer.get(),
                                    stride_in_bytes](std::ranges::range_reference_t<R> &left, std::ranges::range_reference_t<R> &right) {
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

     GlidCopy              id() const noexcept;
     //use for sampler2d
     void                  bind(int slot = 0) const;
     //use for image2d
     void                  bind_read_only(int slot) const;
     void                  bind_write_only(int slot) const;
     constexpr static void destroy(const std::uint32_t id)
     {
          if FME_NOT_CONSTEVAL
          {
               GlCall{}(glDeleteTextures, 1, &id);
          }
          Texture::unbind();
     }
     constexpr static void unbind()
     {
          if FME_NOT_CONSTEVAL
          {
               GlCall{}(glBindTexture, GL_TEXTURE_2D, 0U);
          }
     }
     constexpr glm::ivec2 get_size() const
     {
          return { m_width, m_height };
     }
     constexpr std::int32_t width() const
     {
          return m_width;
     }
     constexpr std::int32_t height() const
     {
          return m_height;
     }
};
#undef FME_NOT_CONSTEVAL
template<typename T = std::uint64_t>
constexpr inline T
  ConvertGliDtoImTextureId(GlidCopy r_id)// this is for imgui but imgui isn't part of the glengine so i made this a template
{
     // ImTextureID used to be a void pointer or something now it's a 64 bit unsigned int.
     return static_cast<T>(std::uint32_t(r_id));
}
static_assert(Bindable<Texture>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_TEXTURE_HPP
