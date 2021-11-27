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
  Texture(std::filesystem::path path)
    : m_path(std::move(path))
  {
    if (m_path.empty())
      return;
    stbi_set_flip_vertically_on_load(1);

    int                  x        = {};
    int                  y        = {};
    [[maybe_unused]] int channels = {};
    const auto           deleter  = [](stbi_uc *ptr) { stbi_image_free(ptr); };
    auto                 png      = std::unique_ptr<stbi_uc, decltype(deleter)>(
      stbi_load(m_path.string().c_str(), &x, &y, &channels, 4));
    m_width_height = { x, y };
    GLCall{ glGenTextures, 1, &m_renderer_id };
    GLCall{ glBindTexture, GL_TEXTURE_2D, m_renderer_id };
    GLCall{ &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR };
    GLCall{ &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR };
    GLCall{
      &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE
    };
    GLCall{
      &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE
    };
    GLCall{ &glTexImage2D,    GL_TEXTURE_2D, 0, GL_RGBA8,
            width(),          height(),      0, GL_RGBA,
            GL_UNSIGNED_BYTE, png.get() };
  }
  ~Texture()
  {
    GLCall{ glDeleteTextures, 1, &m_renderer_id };
  }
  void
    Bind(int slot = 0) const
  {
    // GLCall{ glActiveTexture, static_cast<GLenum>(GL_TEXTURE0 + slot) };
    // GLCall{ glBindTexture, GL_TEXTURE_2D, m_renderer_id };
    GLCall{ glBindTextureUnit, slot, m_renderer_id };
  }
  void
    UnBind() const
  {
    GLCall{ glBindTexture, GL_TEXTURE_2D, 0U };
  }
  std::int32_t
    width() const
  {
    return m_width_height.x();
  }
  std::int32_t
    height() const
  {
    return m_width_height.y();
  }
  Texture(const Texture &) = delete;
  Texture &
    operator=(const Texture &) = delete;
  Texture(Texture &&other) noexcept
    : Texture()
  {
    swap(*this, other);
  }
  Texture &
    operator=(Texture &&other) noexcept
  {
    swap(*this, other);
    return *this;
  }
  void
    swap(Texture &first, Texture &second) noexcept// nothrow
  {
    // enable ADL (not necessary in our case, but good practice)
    using std::swap;

    // by swapping the members of two objects,
    // the two objects are effectively swapped
    swap(first.m_renderer_id, second.m_renderer_id);
    swap(first.m_path, second.m_path);
    swap(first.m_width_height, second.m_width_height);
  }
};
static_assert(Bindable<Texture>);
#endif// MYPROJECT_TEXTURE_HPP
