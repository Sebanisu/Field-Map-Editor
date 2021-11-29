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
  ~Texture();
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
  void
    swap(Texture &first, Texture &second) noexcept;
};
static_assert(Bindable<Texture>);
#endif// MYPROJECT_TEXTURE_HPP
