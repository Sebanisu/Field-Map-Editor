//
// Created by pcvii on 9/4/2021.
//

#ifndef MYPROJECT_MIM_SPRITE_HPP
#define MYPROJECT_MIM_SPRITE_HPP
#include "imgui_format_text.hpp"
#include "open_viii/archive/Archives.hpp"
#include "open_viii/graphics/background/Mim.hpp"
#include "open_viii/graphics/Png.hpp"
#include <imgui.h>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <vector>
// this will hold class that has enough info to load and draw mim files.
struct mim_sprite
{


private:
  const open_viii::archive::FIFLFS<false>      *m_field        = {};
  open_viii::LangT                              m_coo          = {};
  open_viii::graphics::background::Mim          m_mim          = {};
  open_viii::graphics::BPPT                     m_bpp          = {};
  std::uint8_t                                  m_palette      = {};
  bool                                          m_draw_palette = { false };
  std::vector<open_viii::graphics::Color32RGBA> m_colors       = {};
  std::unique_ptr<sf::Texture>                  m_texture      = {};
  mutable sf::Sprite
    m_sprite = {};// You need to be able to update sprite to control position
                  // and draw it. See getter method below!
  [[nodiscard]] open_viii::graphics::background::Mim get_mim() const
  {
    auto lang_name = "_" + std::string(open_viii::LangCommon::to_string(m_coo))
                     + std::string(open_viii::graphics::background::Mim::EXT);
    return { m_field->get_entry_data({ std::string_view(lang_name),
               open_viii::graphics::background::Mim::EXT }),
      m_field->get_base_name() };
  }
  [[nodiscard]] static auto get_bpp(const open_viii::graphics::BPPT &in_bpp)
  {
    using namespace open_viii::graphics::literals;
    if (in_bpp.bpp4() || in_bpp.bpp8() || in_bpp.bpp16()) {
      return in_bpp;
    }
    return 4_bpp;
  }
  [[nodiscard]] auto get_texture() const
  {
    auto texture = std::make_unique<sf::Texture>(sf::Texture{});

    if (!m_colors.empty() && width() != 0U && texture
        && texture->create(width(), height())) {
      // expects an unsigned char pointer. colors
      // underlying type is an array of chars.
      texture->update(reinterpret_cast<const sf::Uint8 *>(m_colors.data()));
      texture->setSmooth(false);
    }
    return texture;
  }
  [[nodiscard]] auto get_sprite() const { return sf::Sprite(*m_texture); }
  [[nodiscard]] auto get_colors()
  {
    return m_mim.get_colors<open_viii::graphics::Color32RGBA>(
      m_bpp, m_palette, m_draw_palette);
  }

public:
  mim_sprite() = default;
  /**
   * Get sprite of mim file with bpp and palette set
   * @param in_field
   * @param in_bpp
   * @param in_palette
   */
  mim_sprite(const open_viii::archive::FIFLFS<false> &in_field,
    const open_viii::graphics::BPPT                  &in_bpp,
    const std::uint8_t                               &in_palette,
    const open_viii::LangT                            in_coo,
    const bool force_draw_palette = false)
    : m_field(&in_field), m_coo(in_coo), m_mim(get_mim()),
      m_bpp(get_bpp(in_bpp)), m_palette(in_palette),
      m_draw_palette(force_draw_palette), m_colors(get_colors()),
      m_texture(get_texture()), m_sprite(get_sprite())
  {}
  //  /**
  //   * Get sprite of palette texture
  //   * @param in_field
  //   */
  //  explicit mim_sprite(const open_viii::archive::FIFLFS<false> &in_field)
  //    : m_field(&in_field), m_mim(get_mim()), m_draw_palette(true),
  //      m_colors(get_colors()), m_texture(get_texture()),
  //      m_sprite(get_sprite())
  //  {}
  /**
   * Create a new object with a new field and the same settings
   * @param in_field
   * @return mim_sprite object
   */
  [[nodiscard]] auto with_field(
    const open_viii::archive::FIFLFS<false> &in_field) const
  {
    return mim_sprite(in_field, m_bpp, m_palette, m_coo, m_draw_palette);
  }
  /**
   * Create a new object with a new bits per pixel and the same settings
   * @param in_bpp
   * @return mim_sprite object
   */
  [[nodiscard]] auto with_bpp(const open_viii::graphics::BPPT &in_bpp) const
  {
    return mim_sprite(
      *m_field, get_bpp(in_bpp), m_palette, m_coo, m_draw_palette);
  }
  /**
   * Create a new object with a new palette and the same settings
   * @param in_bpp
   * @return mim_sprite object
   */
  [[nodiscard]] auto with_palette(const std::uint8_t &in_palette) const
  {
    return mim_sprite(*m_field, m_bpp, in_palette, m_coo, m_draw_palette);
  }
  /**
   * Create a new object and change coo.
   */
  [[nodiscard]] auto with_coo(const open_viii::LangT in_coo) const
  {
    return mim_sprite(*m_field, m_bpp, m_palette, in_coo, m_draw_palette);
  }
  /**
   * Create a new object and Toggle drawing palette.
   */
  [[nodiscard]] auto with_draw_palette(bool in_draw_palette) const
  {
    return mim_sprite(*m_field, m_bpp, m_palette, m_coo, in_draw_palette);
  }
  /**
   * Getter for sprite, required for changing position and drawing.
   * @todo maybe want to control access.
   * @return Sprite
   */
  [[nodiscard]] auto        &sprite() const noexcept { return m_sprite; }
  /**
   * @return width in px
   */
  [[nodiscard]] unsigned int width() const noexcept
  {
    return m_mim.get_width(m_bpp, m_draw_palette);
  }
  /**
   * @return height in px
   */
  [[nodiscard]] unsigned int height() const noexcept
  {
    return m_mim.get_height(m_draw_palette);
  }
  /**
   * If in draw palette mode
   * @return true or false
   */
  [[nodiscard]] bool draw_palette() const noexcept { return m_draw_palette; }
  /**
   * If failed state
   * @return true or false
   */
  [[nodiscard]] bool fail() const noexcept
  {
    return !m_texture || m_colors.empty() || width() == 0;
  }
  /**
   * bpp selections
   */
  static constexpr auto bpp_selections() noexcept
  {
    using namespace open_viii::graphics::literals;
    return std::array{ 4_bpp, 8_bpp, 16_bpp };
  }
  /**
   * bpp selections strings
   */
  static constexpr auto bpp_selections_c_str() noexcept
  {
    return std::array{ "4", "8", "16" };
  }
  /**
   * palettes selections
   */
  static constexpr auto palette_selections()
  {
    return std::array{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
  }
  /**
   * palettes selections strings
   */
  static constexpr auto palette_selections_c_str()
  {
    return std::array{ "0",
      "1",
      "2",
      "3",
      "4",
      "5",
      "6",
      "7",
      "8",
      "9",
      "10",
      "11",
      "12",
      "13",
      "14",
      "15" };
  }
  static bool ImGui_controls(bool changed,
    mim_sprite                   &ms,
    int                          &bpp_selected_item,
    int                          &palette_selected_item,
    bool                         &draw_palette,
    std::array<float, 2>         &xy,
    float                         scale_width = 0.0F)
  {
    if (!ms.fail()) {
      if (ImGui::Checkbox("Draw Palette Texture", &draw_palette)) {
        ms      = ms.with_draw_palette(draw_palette);
        changed = true;
      }
      if (!ms.draw_palette()) {
        static constexpr std::array bpp_items     = bpp_selections_c_str();
        static constexpr std::array palette_items = palette_selections_c_str();
        if (ImGui::Combo("BPP",
              &bpp_selected_item,
              bpp_items.data(),
              static_cast<int>(bpp_items.size()),
              3)) {
          ms = ms.with_bpp(
            bpp_selections().at(static_cast<std::size_t>(bpp_selected_item)));
          changed = true;
        }
        if (bpp_selected_item != 2) {
          if (ImGui::Combo("Palette",
                &palette_selected_item,
                palette_items.data(),
                static_cast<int>(palette_items.size()),
                10)) {
            ms =
              ms.with_palette(static_cast<std::int8_t>(palette_selections().at(
                static_cast<std::size_t>(palette_selected_item))));
            changed = true;
          }
        }
      }
      format_imgui_text("X: {:>9.3f} px  Width:  {:>4} px",
        ms.sprite().getPosition().x,
        ms.width());
      format_imgui_text("Y: {:>9.3f} px  Height: {:>4} px",
        ms.sprite().getPosition().y,
        ms.height());
      if (!ms.draw_palette()) {
        format_imgui_text("Width == Max Tiles");
      }
      if (ImGui::SliderFloat2("Adjust", xy.data(), -1.0F, 0.0F) || changed) {
        ms.sprite().setPosition(
          xy[0] * (static_cast<float>(ms.width()) - scale_width),
          xy[1] * static_cast<float>(ms.height()));
        changed = true;
      }
    }
    return changed;
  }
  void save(const std::filesystem::path &dest_path)
  {
    if (open_viii::tools::i_ends_with(dest_path.string(), ".ppm")) {
      open_viii::graphics::Ppm::save(
        m_colors, width(), height(), dest_path.string());
    } else if (open_viii::tools::i_ends_with(dest_path.string(), ".png")) {
      open_viii::graphics::Png::save(m_colors,
        width(),
        height(),
        dest_path.string(),
        dest_path.stem().string(),
        "");
    }
  }
};
#endif// MYPROJECT_MIM_SPRITE_HPP