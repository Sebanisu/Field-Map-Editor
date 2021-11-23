//
// Created by pcvii on 11/22/2021.
//

#ifndef MYPROJECT_TEXTURE_HPP
#define MYPROJECT_TEXTURE_HPP
#include "Renderer.hpp"
#include <filesystem>
#include <open_viii/graphics/BPPT.hpp>
#include <open_viii/graphics/Png.hpp>
//#include <open_viii/graphics/Rectangle.hpp>
class Texture
{
private:
  std::uint32_t            m_renderer_id = {};
  std::filesystem::path    m_path        = {};
  // open_viii::graphics::Point<std::uint32_t>     m_width_height = {};
  // std::int32_t             m_bpp         = {};
  // std::vector<open_viii::graphics::Color32RGBA> m_colors       = {};
  open_viii::graphics::Png m_png{};
  Texture() = default;

public:
  Texture(std::filesystem::path path)
    : m_path(std::move(path))
    , m_png(open_viii::graphics::Png{ m_path, true })
  {
    GLCall{ glGenTextures, 1, &m_renderer_id };
    Bind();
    GLCall{ &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR };
    GLCall{ &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR };
    GLCall{ &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE };
    GLCall{ &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE };
    GLCall{ &glTexImage2D,    GL_TEXTURE_2D,    0, GL_RGBA8,
            width(),          height(),         0, GL_RGBA,
            GL_UNSIGNED_BYTE, &(*m_png.begin()) };
    UnBind();
  }
  ~Texture()
  {
    GLCall{ glDeleteTextures, 1, &m_renderer_id };
  }
  void
    Bind(int slot = 0) const
  {
    GLCall{ glActiveTexture, static_cast<GLenum>(GL_TEXTURE0 + slot) };
    GLCall{ glBindTexture, GL_TEXTURE_2D, m_renderer_id };
  }
  void
    UnBind() const
  {
    GLCall{ glBindTexture, GL_TEXTURE_2D, 0U };
  }
  std::uint32_t
    width() const
  {
    return m_png.width();
  }
  std::uint32_t
    height() const
  {
    return m_png.height();
  }
  Texture(const Texture &) = delete;
  Texture &
    operator=(const Texture &) = delete;
  Texture(Texture &&other)
    : Texture()
  {
    swap(*this, other);
  }
  Texture &
    operator=(Texture &&other)
  {
    swap(*this, other);
    return *this;
  }
  void
    swap(Texture &first, Texture &second)// nothrow
  {
    // enable ADL (not necessary in our case, but good practice)
    using std::swap;

    // by swapping the members of two objects,
    // the two objects are effectively swapped
    swap(first.m_renderer_id, second.m_renderer_id);
    swap(first.m_path, second.m_path);
    swap(first.m_png, second.m_png);
  }
};

#endif// MYPROJECT_TEXTURE_HPP
