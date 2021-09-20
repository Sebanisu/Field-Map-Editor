//
// Created by pcvii on 9/7/2021.
//

#ifndef MYPROJECT_MAP_SPRITE_HPP
#define MYPROJECT_MAP_SPRITE_HPP
#include "append_inserter.hpp"
#include "imgui_format_text.hpp"
#include "open_viii/archive/Archives.hpp"
#include "open_viii/graphics/background/Map.hpp"
#include "open_viii/graphics/background/Mim.hpp"
#include "tile_map.hpp"
#include <imgui.h>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Texture.hpp>


struct map_sprite
  : public sf::Drawable
  , public sf::Transformable
{
  struct vertex_group
  {
    bool                                        enabled    = { true };
    const sf::Texture                          *texture    = {};
    sf::VertexArray                             vertices   = {};
    open_viii::graphics::background::BlendModeT blend_mode = {
      open_viii::graphics::background::BlendModeT::none
    };
  };

private:
  const open_viii::archive::FIFLFS<false>                 *m_field         = {};
  open_viii::LangT                                         m_coo           = {};
  open_viii::graphics::background::Mim                     m_mim           = {};
  open_viii::graphics::background::Map                     m_map           = {};
  open_viii::graphics::Rectangle<std::uint32_t>            m_canvas        = {};
  std::vector<std::uint8_t>                                m_unique_layers = {};
  std::vector<std::uint16_t>                               m_unique_z_axis = {};
  std::vector<open_viii::graphics::background::BlendModeT> m_blend_modes   = {};
  std::vector<std::pair<open_viii::graphics::BPPT, std::uint8_t>>
                               m_bpp_and_palette                   = {};
  static constexpr std::size_t MAX_TEXTURES                        = 16 * 2 + 1;
  std::unique_ptr<std::array<sf::Texture, MAX_TEXTURES>> m_texture = {};
  std::vector<vertex_group>                              m_vertex_groups = {};
  mutable std::unique_ptr<sf::RenderTexture>             m_render_texture =
    std::make_unique<sf::RenderTexture>();
  using color_type  = open_viii::graphics::Color32RGBA;
  using colors_type = std::vector<color_type>;


  [[nodiscard]] open_viii::graphics::background::Mim get_mim() const;
  [[nodiscard]] open_viii::graphics::background::Map get_map() const;
  [[nodiscard]] colors_type    get_colors(open_viii::graphics::BPPT bpp,
       std::uint8_t                                                 palette,
       bool draw_palette = false);
  static constexpr std::size_t get_texture_pos(open_viii::graphics::BPPT bpp,
    std::uint8_t palette);
  [[nodiscard]] std::unique_ptr<std::array<sf::Texture, MAX_TEXTURES>>
                                                           get_textures();
  std::vector<std::uint16_t>                               get_unique_z_axis();

  std::vector<std::uint8_t>                                get_unique_layers();
  std::vector<open_viii::graphics::background::BlendModeT> get_blend_modes();
  std::vector<std::pair<open_viii::graphics::BPPT, std::uint8_t>>
                            get_bpp_and_palette();

  void                      set_quad(auto  &&quad,
                         const sf::Vector2u &new_tileSize,
                         const sf::Vector2u &raw_tileSize,
                         const auto          source_x,
                         const auto          source_y,
                         const auto          x,
                         const auto          y) const;
  void                      set_quad(auto  &&quad,
                         const sf::Vector2u &new_tileSize,
                         const sf::Vector2u &raw_tileSize,
                         auto              &&tile) const;
  std::vector<vertex_group> get_vertex_groups(
    const sf::Vector2u new_tileSize = { 16U, 16U });

  open_viii::graphics::Rectangle<std::uint32_t> get_canvas() const
  {
    return static_cast<open_viii::graphics::Rectangle<std::uint32_t>>(
      m_map.canvas());
  }

  void get_render_texture() const
  {
    m_render_texture->create(width(), height());
    update_render_texture();
  }

public:
  map_sprite() = default;
  map_sprite(const open_viii::archive::FIFLFS<false> &field,
    open_viii::LangT                                  coo)
    : m_field(&field), m_coo(coo), m_mim(get_mim()), m_map(get_map()),
      m_canvas(get_canvas()), m_blend_modes(get_blend_modes()),
      m_unique_layers(get_unique_layers()),
      m_unique_z_axis(get_unique_z_axis()),
      m_bpp_and_palette(get_bpp_and_palette()), m_texture(get_textures()),
      m_vertex_groups(get_vertex_groups())
  {
    get_render_texture();
  }
  void update_render_texture() const
  {
    local_draw(*m_render_texture, sf::RenderStates::Default);
  }
  static sf::BlendMode &GetBlendSubtract();
  void local_draw(sf::RenderTarget &target, sf::RenderStates states) const;
  map_sprite    with_coo(const open_viii::LangT coo) const;
  map_sprite    with_field(const open_viii::archive::FIFLFS<false> &field);
  std::uint32_t width() const { return m_canvas.width(); }
  std::uint32_t height() const { return m_canvas.height(); }
  template<typename... T>
  requires(sizeof...(T) == 6U) static bool draw_drop_downs();
  static bool draw_drop_downs();
  static bool ImGui_controls(bool changed,
    map_sprite                   &ms,
    int & /*bpp_selected_item*/,
    int & /*palette_selected_item*/,
    std::array<float, 2> &xy,
    float                 scale_width = 0.0F);
  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
};
#endif// MYPROJECT_MAP_SPRITE_HPP
