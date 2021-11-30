//
// Created by pcvii on 11/22/2021.
//

#ifndef MYPROJECT_TEXTURE_HPP
#define MYPROJECT_TEXTURE_HPP
#include "Renderer.hpp"
#include <filesystem>
#include <open_viii/graphics/Rectangle.hpp>
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
  {
    m_width_height = { 1, 1 };
    init_texture(&color);
  }
  void
    init_texture(const void *color)
  {
    GLCall{ glGenTextures, 1, &m_renderer_id };
    GLCall{ glBindTexture, GL_TEXTURE_2D, m_renderer_id };
    GLCall{ &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST };
    GLCall{ &glTexParameteri,
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_NEAREST_MIPMAP_NEAREST };
    GLCall{
      &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE
    };
    GLCall{
      &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE
    };
    GLCall{ &glTexImage2D,    GL_TEXTURE_2D, 0, GL_RGBA8,
            width(),          height(),      0, GL_RGBA,
            GL_UNSIGNED_BYTE, color };
    // Unavailable in OpenGL 2.1, use gluBuild2DMipmaps() instead
    GLCall{ glGenerateMipmap, GL_TEXTURE_2D };
    GLCall{ glBindTexture, GL_TEXTURE_2D, 0 };
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
