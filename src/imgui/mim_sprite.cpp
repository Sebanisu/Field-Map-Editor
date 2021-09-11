#include "mim_sprite.hpp"
//
// Created by pcvii on 9/8/2021.
//
open_viii::graphics::background::Mim mim_sprite::get_mim() const
{
  auto lang_name = "_" + std::string(open_viii::LangCommon::to_string(m_coo))
                   + std::string(open_viii::graphics::background::Mim::EXT);
  return { m_field->get_entry_data({ std::string_view(lang_name),
             open_viii::graphics::background::Mim::EXT }),
    m_field->get_base_name() };
}
open_viii::graphics::BPPT mim_sprite::get_bpp(
  const open_viii::graphics::BPPT &in_bpp)
{
  using namespace open_viii::graphics::literals;
  if (in_bpp.bpp4() || in_bpp.bpp8() || in_bpp.bpp16()) {
    return in_bpp;
  }
  return 4_bpp;
}
std::unique_ptr<sf::Texture> mim_sprite::get_texture() const
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
sf::Sprite mim_sprite::get_sprite() const { return sf::Sprite(*m_texture); }
std::vector<open_viii::graphics::Color32RGBA> mim_sprite::get_colors()
{
  return m_mim.get_colors<open_viii::graphics::Color32RGBA>(
    m_bpp, m_palette, m_draw_palette);
}
[[maybe_unused]] mim_sprite::mim_sprite(
  const open_viii::archive::FIFLFS<false> &in_field,
  const open_viii::graphics::BPPT         &in_bpp,
  const uint8_t                           &in_palette,
  const open_viii::LangT                   in_coo,
  const bool                               force_draw_palette)
  : m_field(&in_field), m_coo(in_coo), m_mim(get_mim()), m_bpp(get_bpp(in_bpp)),
    m_palette(in_palette), m_draw_palette(force_draw_palette),
    m_colors(get_colors()), m_texture(get_texture()), m_sprite(get_sprite())
{}
mim_sprite mim_sprite::with_field(
  const open_viii::archive::FIFLFS<false> &in_field) const
{
  return { in_field, m_bpp, m_palette, m_coo, m_draw_palette };
}
mim_sprite mim_sprite::with_bpp(const open_viii::graphics::BPPT &in_bpp) const
{
  return { *m_field, get_bpp(in_bpp), m_palette, m_coo, m_draw_palette };
}
mim_sprite mim_sprite::with_palette(const uint8_t &in_palette) const
{
  return { *m_field, m_bpp, in_palette, m_coo, m_draw_palette };
}
mim_sprite mim_sprite::with_coo(const open_viii::LangT in_coo) const
{
  return { *m_field, m_bpp, m_palette, in_coo, m_draw_palette };
}
mim_sprite mim_sprite::with_draw_palette(bool in_draw_palette) const
{
  return { *m_field, m_bpp, m_palette, m_coo, in_draw_palette };
}
sf::Sprite   &mim_sprite::sprite() const noexcept { return m_sprite; }
std::uint32_t mim_sprite::width() const noexcept
{
  return m_mim.get_width(m_bpp, m_draw_palette);
}
std::uint32_t mim_sprite::height() const noexcept
{
  return m_mim.get_height(m_draw_palette);
}
bool mim_sprite::draw_palette() const noexcept { return m_draw_palette; }
bool mim_sprite::fail() const noexcept
{
  return !m_texture || m_colors.empty() || width() == 0;
}
bool mim_sprite::ImGui_controls(bool changed,
  mim_sprite                        &ms,
  int                               &bpp_selected_item,
  int                               &palette_selected_item,
  bool                              &draw_palette,
  std::array<float, 2>              &xy,
  float                              scale_width)
{
  if (!ms.fail()) {
    if (ImGui::Checkbox("Draw Palette Texture", &draw_palette)) {
      ms      = ms.with_draw_palette(draw_palette);
      changed = true;
    }
    if (!ms.draw_palette()) {
      static constexpr std::array bpp_items =
        open_viii::graphics::background::Mim::bpp_selections_c_str();
      static constexpr std::array palette_items =
        open_viii::graphics::background::Mim::palette_selections_c_str();
      if (ImGui::Combo("BPP",
            &bpp_selected_item,
            bpp_items.data(),
            static_cast<int>(bpp_items.size()),
            3)) {
        ms =
          ms.with_bpp(open_viii::graphics::background::Mim::bpp_selections().at(
            static_cast<std::size_t>(bpp_selected_item)));
        changed = true;
      }
      if (bpp_selected_item != 2) {
        if (ImGui::Combo("Palette",
              &palette_selected_item,
              palette_items.data(),
              static_cast<int>(palette_items.size()),
              10)) {
          ms      = ms.with_palette(static_cast<std::uint8_t>(
            open_viii::graphics::background::Mim::palette_selections().at(
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
void mim_sprite::save(const std::filesystem::path &dest_path) const
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
[[nodiscard]] const open_viii::graphics::background::Mim &
  mim_sprite::mim() const noexcept
{
  return m_mim;
}