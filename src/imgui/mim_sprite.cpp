#include "mim_sprite.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
//
// Created by pcvii on 9/8/2021.
//
open_viii::graphics::background::Mim mim_sprite::get_mim() const
{
  auto lang_name = fmt::format("_{}{}",
    std::string(open_viii::LangCommon::to_string(m_coo)),
    open_viii::graphics::background::Mim::EXT);
  if (m_field) {
    return { m_field->get_entry_data({ std::string_view(lang_name),
               open_viii::graphics::background::Mim::EXT }, &m_mim_path),
      m_field->get_base_name() };
  }
  return {};
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
    m_colors(get_colors()), m_texture(get_texture()), m_vertices(get_vertices())
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
// sf::Sprite   &mim_sprite::sprite() const noexcept { return m_sprite; }
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
std::string mim_sprite::mim_filename() const
{
  return std::filesystem::path(m_mim_path).filename().string();
}
void mim_sprite::mim_save(const std::filesystem::path &dest_path) const
{
  const auto path = dest_path.string();
  open_viii::tools::write_buffer(m_mim.buffer(),path,"");
}
[[nodiscard]] const open_viii::graphics::background::Mim &
  mim_sprite::mim() const noexcept
{
  return m_mim;
}

void mim_sprite::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  if (m_texture) {
    states.transform *= getTransform();
    states.texture = m_texture.get();
    target.draw(m_vertices.data(), 4U, sf::TriangleStrip, states);
  }
}
std::array<sf::Vertex, 4U> mim_sprite::get_vertices() const
{
  if (m_texture) {

    const sf::Vector2u size  = m_texture->getSize();
    const auto         frect = sf::FloatRect(
              0.F, 0.F, static_cast<float>(size.x), static_cast<float>(size.y));
    float      left   = frect.left;
    float      right  = left + frect.width;
    float      top    = frect.top;
    float      bottom = top + frect.height;
    std::array ret    = {
      sf::Vertex{ sf::Vector2f{ left, top }, sf::Vector2f{ left, top } },
      sf::Vertex{ sf::Vector2f{ left, bottom }, sf::Vector2f{ left, bottom } },
      sf::Vertex{ sf::Vector2f{ right, top }, sf::Vector2f{ right, top } },
      sf::Vertex{ sf::Vector2f{ right, bottom }, sf::Vector2f{ right, bottom } }
    };
    return ret;
  }
  return {};
}
