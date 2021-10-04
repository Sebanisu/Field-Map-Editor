//
// Created by pcvii on 9/7/2021.
//

#ifndef MYPROJECT_MAP_SPRITE_HPP
#define MYPROJECT_MAP_SPRITE_HPP
#include "grid.hpp"
#include "open_viii/archive/Archives.hpp"
#include "open_viii/graphics/background/Map.hpp"
#include "open_viii/graphics/background/Mim.hpp"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>

struct map_sprite
  : public sf::Drawable
  , public sf::Transformable
{
public:
  map_sprite() = default;
  map_sprite(const open_viii::archive::FIFLFS<false> &field,
    open_viii::LangT                                  coo,
    bool                                              draw_swizzle)
    : m_draw_swizzle(draw_swizzle), m_field(&field), m_coo(coo),
      m_mim(get_mim()), m_map(get_map(&m_map_path)), m_canvas(get_canvas()),
      m_blend_modes(get_blend_modes()), m_unique_layers(get_unique_layers()),
      m_unique_z_axis(get_unique_z_axis()),
      m_bpp_and_palette(get_bpp_and_palette()), m_texture(get_textures()),
      m_grid({ 16U, 16U }, { width(), height() })
  {
    init_render_texture();
  }

  const map_sprite &toggle_grid(bool enable) const;
  void              enable_draw_swizzle() const;
  void              disable_draw_swizzle() const;
  std::string       map_filename();
  bool              fail() const;
  std::uint32_t     width() const;
  std::uint32_t     height() const;
  void              save(const std::filesystem::path &path) const;
  void              map_save(const std::filesystem::path &dest_path) const;
  map_sprite        with_coo(open_viii::LangT coo) const;
  map_sprite with_field(const open_viii::archive::FIFLFS<false> &field) const;
  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;

private:
  mutable bool                                  m_draw_swizzle  = { false };
  const open_viii::archive::FIFLFS<false>      *m_field         = {};
  open_viii::LangT                              m_coo           = {};
  open_viii::graphics::background::Mim          m_mim           = {};
  mutable std::string                           m_map_path      = {};
  open_viii::graphics::background::Map          m_map           = {};
  open_viii::graphics::Rectangle<std::uint32_t> m_canvas        = {};
  std::vector<std::uint8_t>                     m_unique_layers = {};
  std::vector<std::uint16_t>                    m_unique_z_axis = {};
  std::vector<open_viii::graphics::background::BlendModeT> m_blend_modes = {};
  std::vector<std::pair<open_viii::graphics::BPPT, std::uint8_t>>
                               m_bpp_and_palette = {};
  static constexpr std::size_t MAX_TEXTURES =
    16 * 13;// 13*16 for texture page / palette combos. 16*2+1 for palette bpp
            // combos.
  std::unique_ptr<std::array<sf::Texture, MAX_TEXTURES>> m_texture = {};
  mutable std::unique_ptr<sf::RenderTexture>             m_render_texture =
    std::make_unique<sf::RenderTexture>();
  grid m_grid       = {};

  using color_type  = open_viii::graphics::Color32RGBA;
  using colors_type = std::vector<color_type>;
  [[nodiscard]] open_viii::graphics::background::Mim get_mim() const;
  open_viii::graphics::background::Map get_map(std::string *out_path = nullptr,
    bool                                                    sort_remove = true,
    bool                                                    shift = true) const;
  [[nodiscard]] colors_type            get_colors(open_viii::graphics::BPPT bpp,
               std::uint8_t                                                 palette,
               bool draw_palette = false) const;
  [[nodiscard]] static constexpr std::size_t
    get_texture_pos(open_viii::graphics::BPPT bpp, std::uint8_t palette);
  [[nodiscard]] const sf::Texture *get_texture(open_viii::graphics::BPPT bpp,
    std::uint8_t palette) const;
  [[nodiscard]] std::unique_ptr<std::array<sf::Texture, MAX_TEXTURES>>
                                           get_textures() const;
  [[nodiscard]] std::vector<std::uint16_t> get_unique_z_axis() const;
  [[nodiscard]] std::vector<std::uint8_t>  get_unique_layers() const;
  [[nodiscard]] std::vector<open_viii::graphics::background::BlendModeT>
    get_blend_modes() const;
  [[nodiscard]] std::vector<std::pair<open_viii::graphics::BPPT, std::uint8_t>>
    get_bpp_and_palette() const;

  [[nodiscard]] open_viii::graphics::Rectangle<std::uint32_t>
                                           get_canvas() const;
  void                                     resize_render_texture() const;
  void                                     init_render_texture() const;

  [[nodiscard]] std::array<sf::Vertex, 4U> get_triangle_strip(
    const sf::Vector2u &new_tileSize,
    const sf::Vector2u &raw_tileSize,
    auto                source_x,
    auto                source_y,
    auto                x,
    auto                y) const;

  [[nodiscard]] std::array<sf::Vertex, 4U> get_triangle_strip(
    const sf::Vector2u &new_tileSize,
    const sf::Vector2u &raw_tileSize,
    auto              &&tile) const;


  void                        update_render_texture() const;
  static const sf::BlendMode &GetBlendSubtract();
  void local_draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
#endif// MYPROJECT_MAP_SPRITE_HPP
