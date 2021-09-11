//
// Created by pcvii on 9/7/2021.
//

#ifndef MYPROJECT_MAP_SPRITE_HPP
#define MYPROJECT_MAP_SPRITE_HPP
#include "append_inserter.hpp"
#include "open_viii/archive/Archives.hpp"
#include "open_viii/graphics/background/Map.hpp"
#include "open_viii/graphics/background/Mim.hpp"
#include "tile_map.hpp"
#include <SFML/Graphics/Texture.hpp>
struct map_sprite
  : public sf::Drawable
  , public sf::Transformable
{
  struct vertex_group
  {
    bool               enabled  = { true };
    const sf::Texture *texture  = {};
    sf::VertexArray    vertices = {};
  };

private:
  const open_viii::archive::FIFLFS<false> *m_field                 = {};
  open_viii::LangT                         m_coo                   = {};
  open_viii::graphics::background::Mim     m_mim                   = {};
  open_viii::graphics::background::Map     m_map                   = {};
  static constexpr std::size_t             MAX_TEXTURES            = 16 * 2 + 1;
  std::unique_ptr<std::array<sf::Texture, MAX_TEXTURES>> m_texture = {};
  std::vector<vertex_group>                              m_vertex_groups{};
  using color_type  = open_viii::graphics::Color32RGBA;
  using colors_type = std::vector<color_type>;


  [[nodiscard]] open_viii::graphics::background::Mim get_mim() const
  {
    auto lang_name = "_" + std::string(open_viii::LangCommon::to_string(m_coo))
                     + std::string(open_viii::graphics::background::Mim::EXT);
    return { m_field->get_entry_data({ std::string_view(lang_name),
               open_viii::graphics::background::Mim::EXT }),
      m_field->get_base_name() };
  }
  [[nodiscard]] open_viii::graphics::background::Map get_map() const
  {
    auto lang_name = "_" + std::string(open_viii::LangCommon::to_string(m_coo))
                     + std::string(open_viii::graphics::background::Mim::EXT);
    return open_viii::graphics::background::Map{ m_mim.mim_type(),
      m_field->get_entry_data({ std::string_view(lang_name),
        open_viii::graphics::background::Mim::EXT }) };
  }
  [[nodiscard]] colors_type get_colors(open_viii::graphics::BPPT bpp,
    std::uint8_t                                                 palette,
    bool draw_palette = false)
  {
    return m_mim.get_colors<open_viii::graphics::Color32RGBA>(
      bpp, palette, draw_palette);
  }
  [[nodiscard]] std::unique_ptr<std::array<sf::Texture, MAX_TEXTURES>>
    get_textures()
  {
    auto ret = std::make_unique<std::array<sf::Texture, MAX_TEXTURES>>(
      std::array<sf::Texture, MAX_TEXTURES>{});
    auto b = ret->begin();
    std::ranges::for_each(
      open_viii::graphics::background::Mim::bpp_selections(),
      [this, &b](const open_viii::graphics::BPPT &bpp) {
        if (bpp.bpp8() || bpp.bpp24()) {
          std::ranges::for_each(
            open_viii::graphics::background::Mim::palette_selections(),
            [this, &bpp, &b](int palette) {
              const auto colors =
                get_colors(bpp, static_cast<uint8_t>(palette));
              b->create(m_mim.get_width(bpp), m_mim.get_height());
              b->update(reinterpret_cast<const sf::Uint8 *>(colors.data()));
              ++b;
            });
        } else {
          // 16bpp doesn't have palettes.
          const auto colors = get_colors(bpp, 0);
          b->create(m_mim.get_width(bpp), m_mim.get_height());
          b->update(reinterpret_cast<const sf::Uint8 *>(colors.data()));
        }
      });
    return ret;
  }
  std::vector<vertex_group> get_vertex_groups()
  {
    std::vector<vertex_group> ret{};
    // z axis - which draws first.
    // bpp + palette for texture
    m_map.visit_tiles([&ret](auto &&tiles) {
      using tiles_type = std::decay_t<decltype(tiles)>;
      using tile_type  = typename tiles_type::value_type;
      std::vector<std::decay_t<decltype(tile_type{}.z())>> unique_z_axis{};
      std::ranges::transform(tiles,
        std::back_inserter(unique_z_axis),
        [](const auto &tile) { return tile.z(); });
      std::ranges::sort(unique_z_axis);
      auto last = std::unique(unique_z_axis.begin(), unique_z_axis.end());
      unique_z_axis.erase(last, unique_z_axis.end());
      std::ranges::for_each(
        unique_z_axis, [](const auto &z) { std::cout << z << "\t"; });
      //      std::cout << '\n';
      //      vertex_group              vg{};
      //      const auto                raw_tileSize = sf::Vector2u{ 16U, 16U };
      //      const auto                new_tileSize = sf::Vector2u{ 16U, 16U };
      //      std::ranges::for_each(tiles,
      //        [&raw_tileSize, &new_tileSize, &vg](
      //          open_viii::graphics::background::Tile1
      //            &&tile) {// open_viii::graphics::background::is_tile auto
      //            &&tile;
      //          using tile_type = std::decay_t<decltype(tile)>;
      //          // m_vertices.setPrimitiveType(sf::Quads);
      //          //  define its 4 corners
      //          auto quad       = std::array<sf::Vertex, 4U>{};
      //          auto i          = tile.x() / raw_tileSize.x;
      //          auto j          = tile.x() / raw_tileSize.y;
      //          auto tu =
      //            (tile.source_x()
      //              + tile.texture_id() *
      //              tile_type::texture_page_width(tile.depth()))
      //            / raw_tileSize.x;
      //          auto tv = tile.source_y() / raw_tileSize.y;
      //
      //          quad[0].position =
      //            sf::Vector2f(static_cast<float>(i * new_tileSize.x),
      //              static_cast<float>(j * new_tileSize.y));
      //          quad[1].position =
      //            sf::Vector2f(static_cast<float>((i + 1) * new_tileSize.x),
      //              static_cast<float>(j * new_tileSize.y));
      //          quad[2].position =
      //            sf::Vector2f(static_cast<float>((i + 1) * new_tileSize.x),
      //              static_cast<float>((j + 1) * new_tileSize.y));
      //          quad[3].position =
      //            sf::Vector2f(static_cast<float>(i * new_tileSize.x),
      //              static_cast<float>((j + 1) * new_tileSize.y));
      //
      //          // define its 4 texture coordinates
      //          quad[0].texCoords =
      //            sf::Vector2f(static_cast<float>(tu * new_tileSize.x),
      //              static_cast<float>(tv * new_tileSize.y));
      //          quad[1].texCoords =
      //            sf::Vector2f(static_cast<float>((tu + 1) * new_tileSize.x),
      //              static_cast<float>(tv * new_tileSize.y));
      //          quad[2].texCoords =
      //            sf::Vector2f(static_cast<float>((tu + 1) * new_tileSize.x),
      //              static_cast<float>((tv + 1) * new_tileSize.y));
      //          quad[3].texCoords =
      //            sf::Vector2f(static_cast<float>(tu * new_tileSize.x),
      //              static_cast<float>((tv + 1) * new_tileSize.y));
      //          std::ranges::for_each(
      //            quad, [&vg](auto &&vertex) { vg.vertices.append(vertex); });
      //        });
    });
    return ret;
  }


public:
  map_sprite() = default;
  map_sprite(const open_viii::archive::FIFLFS<false> &field,
    open_viii::LangT                                  coo)
    : m_field(&field), m_coo(coo), m_mim(get_mim()), m_map(get_map()),
      m_texture(get_textures()), m_vertex_groups(get_vertex_groups())
  {}
  void draw(sf::RenderTarget &target, sf::RenderStates states) const final
  {
    // apply the transform
    states.transform *= getTransform();

    std::ranges::for_each(
      m_vertex_groups, [&target, &states](const vertex_group &v) {
        if (!v.enabled) {
          return;
        }
        // apply the tileset texture
        states.texture = v.texture;

        // draw the vertex array
        target.draw(v.vertices, states);
      });
  }
  map_sprite with_coo(const open_viii::LangT coo) const
  {
    return { *m_field, coo };
  }
  map_sprite with_field(const open_viii::archive::FIFLFS<false> &field)
  {
    return { field, m_coo };
  }
};
#endif// MYPROJECT_MAP_SPRITE_HPP
