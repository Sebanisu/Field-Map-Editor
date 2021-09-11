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
  const open_viii::archive::FIFLFS<false> *m_field         = {};
  open_viii::LangT                         m_coo           = {};
  open_viii::graphics::background::Mim     m_mim           = {};
  open_viii::graphics::background::Map     m_map           = {};
  std::vector<std::uint16_t>               m_unique_z_axis = {};
  std::vector<std::pair<open_viii::graphics::BPPT, std::uint8_t>>
                               m_bpp_and_palette                   = {};
  static constexpr std::size_t MAX_TEXTURES                        = 16 * 2 + 1;
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
                     + std::string(open_viii::graphics::background::Map::EXT);
    return open_viii::graphics::background::Map{ m_mim.mim_type(),
      m_field->get_entry_data({ std::string_view(lang_name),
        open_viii::graphics::background::Map::EXT }) };
  }
  [[nodiscard]] colors_type get_colors(open_viii::graphics::BPPT bpp,
    std::uint8_t                                                 palette,
    bool draw_palette = false)
  {
    return m_mim.get_colors<open_viii::graphics::Color32RGBA>(
      bpp, palette, draw_palette);
  }
  static constexpr std::size_t get_texture_pos(open_viii::graphics::BPPT bpp,
    std::uint8_t palette)
  {
    if (bpp.bpp4()) {
      return palette;
    }
    if (bpp.bpp8()) {
      return palette
             + std::size(
               open_viii::graphics::background::Mim::palette_selections());
    }
    return MAX_TEXTURES - 1;
    // 16bpp doesn't have palettes.
  }
  [[nodiscard]] std::unique_ptr<std::array<sf::Texture, MAX_TEXTURES>>
    get_textures()
  {
    auto ret = std::make_unique<std::array<sf::Texture, MAX_TEXTURES>>(
      std::array<sf::Texture, MAX_TEXTURES>{});
    for (const auto &[bpp, palette] : m_bpp_and_palette) {
      if (bpp.bpp24()) {
        continue;
      }
      std::cout << bpp << '\t' << +palette << '\t' << '\t';
      size_t pos   = get_texture_pos(bpp, palette);
      auto   width = m_mim.get_width(bpp);
      if (width != 0U) {
        const auto colors = get_colors(bpp, palette);
        ret->at(pos).create(width, m_mim.get_height());
        ret->at(pos).update(reinterpret_cast<const sf::Uint8 *>(colors.data()));
      }
    }
    std::cout << '\n';
    return ret;
  }
  std::vector<std::uint16_t> get_unique_z_axis()
  {
    std::vector<std::uint16_t> ret{};
    m_map.visit_tiles([&ret](auto &&tiles) {
      std::ranges::transform(tiles,
        std::back_inserter(ret),
        [](const auto &tile) { return tile.z(); });
      std::ranges::sort(ret, std::greater<>());
      auto last = std::unique(ret.begin(), ret.end());
      ret.erase(last, ret.end());
    });
    for (const auto z : ret) {
      std::cout << z << '\t';
    }
    std::cout << '\n';
    return ret;
  }
  std::vector<std::pair<open_viii::graphics::BPPT, std::uint8_t>>
    get_bpp_and_palette()
  {
    std::vector<std::pair<open_viii::graphics::BPPT, std::uint8_t>> ret{};
    m_map.visit_tiles([&ret](auto &&tiles) {
      std::ranges::transform(
        tiles, std::back_inserter(ret), [](const auto &tile) {
          return std::pair(tile.depth(), tile.palette_id());
        });
      std::ranges::sort(ret, std::less<>());
      auto last = std::unique(ret.begin(), ret.end());
      ret.erase(last, ret.end());
    });
    return ret;
  }
  std::vector<vertex_group> get_vertex_groups(
    const sf::Vector2u new_tileSize = { 16U, 16U })
  {
    std::vector<vertex_group> ret{};
    m_map.visit_tiles([&ret, this, &new_tileSize](auto &&tiles) {
      using tiles_type                 = std::decay_t<decltype(tiles)>;
      using tile_type [[maybe_unused]] = typename tiles_type::value_type;
      const auto raw_tileSize          = sf::Vector2u{ 16U, 16U };
      for (const auto &z : m_unique_z_axis) {
        for (const auto &[bpp, palette] : m_bpp_and_palette) {
          if (bpp.bpp24()) {
            continue;
          }
          size_t     pos    = get_texture_pos(bpp, palette);
          const auto width  = m_mim.get_width(bpp);
          //const auto height = m_mim.get_height();
          if (width == 0U) {
            continue;
          }
          tiles_type filtered = {};
          std::copy_if(tiles.begin(),
            tiles.end(),
            std::back_inserter(filtered),
            [&z, lbpp = bpp, lpalette = palette](const auto &tile) -> bool {
              return tile.z() == z && tile.depth() == lbpp
                     && tile.palette_id() == lpalette;
            });
          if (std::empty(filtered)) {
            continue;
          }
          vertex_group vg{};
          vg.texture = &m_texture->at(pos);
          vg.vertices.setPrimitiveType(sf::Quads);
          std::ranges::for_each(filtered,
            [&vg, &raw_tileSize, &new_tileSize](const tile_type &tile) {
              auto quad = std::array<sf::Vertex, 4U>{};
              auto i =
                tile.x() / static_cast<decltype(tile.x())>(raw_tileSize.x);
              auto j =
                tile.y() / static_cast<decltype(tile.y())>(raw_tileSize.y);
              auto tu =
                (tile.source_x()
                  + tile.texture_id()
                      * tile_type::texture_page_width(tile.depth()))
                / static_cast<decltype(tile.source_x())>(raw_tileSize.x);
              auto tv =
                tile.source_y()
                / static_cast<decltype(tile.source_y())>(raw_tileSize.y);

              quad[0].position =
                sf::Vector2f(static_cast<float>(
                               i * static_cast<decltype(i)>(new_tileSize.x)),
                  static_cast<float>(
                    j * static_cast<decltype(j)>(new_tileSize.y)));
              quad[1].position = sf::Vector2f(
                static_cast<float>(
                  (i + 1) * static_cast<decltype(i)>(new_tileSize.x)),
                static_cast<float>(
                  j * static_cast<decltype(j)>(new_tileSize.y)));
              quad[2].position = sf::Vector2f(
                static_cast<float>(
                  (i + 1) * static_cast<decltype(i)>(new_tileSize.x)),
                static_cast<float>(
                  (j + 1) * static_cast<decltype(j)>(new_tileSize.y)));
              quad[3].position =
                sf::Vector2f(static_cast<float>(
                               i * static_cast<decltype(i)>(new_tileSize.x)),
                  static_cast<float>(
                    (j + 1) * static_cast<decltype(j)>(new_tileSize.y)));

              // define its 4 texture coordinates
              quad[0].texCoords =
                sf::Vector2f(static_cast<float>(
                               tu * static_cast<decltype(tu)>(new_tileSize.x)),
                  static_cast<float>(
                    tv * static_cast<decltype(tv)>(new_tileSize.y)));
              quad[1].texCoords = sf::Vector2f(
                static_cast<float>(
                  (tu + 1) * static_cast<decltype(tu)>(new_tileSize.x)),
                static_cast<float>(
                  tv * static_cast<decltype(tv)>(new_tileSize.y)));
              quad[2].texCoords = sf::Vector2f(
                static_cast<float>(
                  (tu + 1) * static_cast<decltype(tu)>(new_tileSize.x)),
                static_cast<float>(
                  (tv + 1) * static_cast<decltype(tv)>(new_tileSize.y)));
              quad[3].texCoords =
                sf::Vector2f(static_cast<float>(
                               tu * static_cast<decltype(tu)>(new_tileSize.x)),
                  static_cast<float>(
                    (tv + 1) * static_cast<decltype(tv)>(new_tileSize.y)));
              std::ranges::for_each(
                quad, [&vg](auto &&vertex) { vg.vertices.append(vertex); });
            });
          if (vg.vertices.getVertexCount() != 0) {
            ret.emplace_back(std::move(vg));
          }
        }
      }
    });
    return ret;
  }


public:
  map_sprite() = default;
  map_sprite(const open_viii::archive::FIFLFS<false> &field,
    open_viii::LangT                                  coo)
    : m_field(&field), m_coo(coo), m_mim(get_mim()), m_map(get_map()),
      m_unique_z_axis(get_unique_z_axis()),
      m_bpp_and_palette(get_bpp_and_palette()), m_texture(get_textures()),
      m_vertex_groups(get_vertex_groups())
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
