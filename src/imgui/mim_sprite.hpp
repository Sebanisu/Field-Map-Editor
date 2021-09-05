//
// Created by pcvii on 9/4/2021.
//

#ifndef MYPROJECT_MIM_SPRITE_HPP
#define MYPROJECT_MIM_SPRITE_HPP
#include "open_viii/archive/Archives.hpp"
#include "open_viii/graphics/background/Mim.hpp"
#include <imgui.h>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <vector>
// this will hold class that has enough info to load and draw mim files.
struct mim_sprite
{


private:
  const open_viii::archive::FIFLFS<false>      *m_field   = {};
  open_viii::graphics::background::Mim          m_mim     = {};
  open_viii::graphics::BPPT                     m_bpp     = {};
  std::uint8_t                                  m_palette = {};
  std::vector<open_viii::graphics::Color32RGBA> m_colors  = {};
  std::unique_ptr<sf::Texture>                  m_texture = {};
  mutable sf::Sprite
    m_sprite = {};// You need to be able to update sprite to control position
                  // and draw it. See getter method below!
  open_viii::graphics::background::Mim get_mim() const
  {
    return { m_field->get_entry_data(open_viii::graphics::background::Mim::EXT),
      m_field->get_base_name() };
  }
  static auto get_bpp(const open_viii::graphics::BPPT &in_bpp)
  {
    using namespace open_viii::graphics::literals;
    if (in_bpp.bpp4() || in_bpp.bpp8() || in_bpp.bpp16()) {
      return in_bpp;
    }
    return 4_bpp;
  }

public:
  mim_sprite() = default;
  mim_sprite(const open_viii::archive::FIFLFS<false> &in_field,
    const open_viii::graphics::BPPT                  &in_bpp,
    const std::uint8_t                               &in_palette)
    : m_field(&in_field), m_mim(get_mim()), m_bpp(get_bpp(in_bpp)),
      m_palette(in_palette)
  {
    m_colors =
      m_mim.get_colors<open_viii::graphics::Color32RGBA>(m_bpp, m_palette);
    if(!m_texture)
    {
      m_texture = std::make_unique<sf::Texture>(sf::Texture{});
    }
    if (!m_colors.empty() && width() != 0U
        && m_texture &&  m_texture->create(width(), height())) {
      // expects an unsigned char pointer. colors
      // underlying type is an array of chars.
      m_texture->update(reinterpret_cast<const sf::Uint8 *>(m_colors.data()));
      m_texture->setSmooth(false);
      m_sprite.setTexture(*m_texture);
    }
  }
  /**
   * Create a new object with a new field and the same settings
   * @param in_field
   * @return mim_sprite object
   */
  [[nodiscard]] auto with_field(
    const open_viii::archive::FIFLFS<false> &in_field) const
  {
    return mim_sprite(in_field, m_bpp, m_palette);
  }
  /**
   * Create a new object with a new bits per pixel and the same settings
   * @param in_bpp
   * @return mim_sprite object
   */
  [[nodiscard]] auto with_bpp(const open_viii::graphics::BPPT &in_bpp) const
  {
    return mim_sprite(*m_field, get_bpp(in_bpp), m_palette);
  }
  /**
   * Create a new object with a new palette and the same settings
   * @param in_bpp
   * @return mim_sprite object
   */
  [[nodiscard]] auto with_palette(const std::uint8_t &in_palette) const
  {
    return mim_sprite(*m_field, m_bpp, in_palette);
  }
  /**
   * Getter for sprite, required for changing position and drawing.
   * @todo maybe want to control access.
   * @return Sprite
   */
  [[nodiscard]] auto        &sprite() const { return m_sprite; }
  /**
   * @return width in px
   */
  [[nodiscard]] unsigned int width() const { return m_mim.get_width(m_bpp); }
  /**
   * @return height in px
   */
  [[nodiscard]] unsigned int height() const { return m_mim.get_height(); }
};
#endif// MYPROJECT_MIM_SPRITE_HPP
