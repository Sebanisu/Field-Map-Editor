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
        open_viii::graphics::background::Map::EXT }),
      true,
      true };
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
        ret->at(pos).setSmooth(false);
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
  std::vector<open_viii::graphics::background::BlendModeT> get_blend_modes()
  {
    std::vector<open_viii::graphics::background::BlendModeT> ret{};
    m_map.visit_tiles([&ret](auto &&tiles) {
      std::ranges::transform(tiles,
        std::back_inserter(ret),
        [](const auto &tile) { return tile.blend_mode(); });
      std::ranges::sort(ret, std::greater<>());
      auto last = std::unique(ret.begin(), ret.end());
      ret.erase(last, ret.end());
    });
    for (const auto z : ret) {
      std::cout << static_cast<std::uint32_t>(z) << '\t';
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

  void set_quad(auto  &&quad,
    const sf::Vector2u &new_tileSize,
    const sf::Vector2u &raw_tileSize,
    const auto          source_x,
    const auto          source_y,
    const auto          x,
    const auto          y) const
  {
    auto i  = x / static_cast<decltype(x)>(raw_tileSize.x);
    auto j  = y / static_cast<decltype(y)>(raw_tileSize.y);
    auto tu = (source_x) / static_cast<decltype(source_x)>(raw_tileSize.x);
    auto tv = source_y / static_cast<decltype(source_y)>(raw_tileSize.y);

    quad[0].position = sf::Vector2f(
      static_cast<float>(i * static_cast<decltype(i)>(new_tileSize.x)),
      static_cast<float>(j * static_cast<decltype(j)>(new_tileSize.y)));
    quad[1].position = sf::Vector2f(
      static_cast<float>((i + 1) * static_cast<decltype(i)>(new_tileSize.x)),
      static_cast<float>(j * static_cast<decltype(j)>(new_tileSize.y)));
    quad[2].position = sf::Vector2f(
      static_cast<float>((i + 1) * static_cast<decltype(i)>(new_tileSize.x)),
      static_cast<float>((j + 1) * static_cast<decltype(j)>(new_tileSize.y)));
    quad[3].position = sf::Vector2f(
      static_cast<float>(i * static_cast<decltype(i)>(new_tileSize.x)),
      static_cast<float>((j + 1) * static_cast<decltype(j)>(new_tileSize.y)));

    // define its 4 texture coordinates
    quad[0].texCoords = sf::Vector2f(
      static_cast<float>(tu * static_cast<decltype(tu)>(new_tileSize.x)),
      static_cast<float>(tv * static_cast<decltype(tv)>(new_tileSize.y)));
    quad[1].texCoords = sf::Vector2f(
      static_cast<float>((tu + 1) * static_cast<decltype(tu)>(new_tileSize.x)),
      static_cast<float>(tv * static_cast<decltype(tv)>(new_tileSize.y)));
    quad[2].texCoords = sf::Vector2f(
      static_cast<float>((tu + 1) * static_cast<decltype(tu)>(new_tileSize.x)),
      static_cast<float>((tv + 1) * static_cast<decltype(tv)>(new_tileSize.y)));
    quad[3].texCoords = sf::Vector2f(
      static_cast<float>(tu * static_cast<decltype(tu)>(new_tileSize.x)),
      static_cast<float>((tv + 1) * static_cast<decltype(tv)>(new_tileSize.y)));
  }
  void set_quad(auto  &&quad,
    const sf::Vector2u &new_tileSize,
    const sf::Vector2u &raw_tileSize,
    auto              &&tile) const
  {
    using tile_type = std::decay_t<decltype(tile)>;
    set_quad(quad,
      new_tileSize,
      raw_tileSize,
      tile.source_x()
        + tile.texture_id() * tile_type::texture_page_width(tile.depth()),
      tile.source_y(),
      tile.x(),
      tile.y());
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
          for (const auto &blend_mode : m_blend_modes) {
            if (bpp.bpp24()) {
              continue;
            }
            size_t     pos   = get_texture_pos(bpp, palette);
            const auto width = m_mim.get_width(bpp);
            // const auto height = m_mim.get_height();
            if (width == 0U) {
              continue;
            }
            tiles_type filtered = {};
            std::copy_if(tiles.begin(),
              tiles.end(),
              std::back_inserter(filtered),
              [&z, lbpp = bpp, lpalette = palette, blend_mode](
                const auto &tile) -> bool {
                return tile.z() == z && tile.depth() == lbpp
                       && tile.palette_id() == lpalette
                       && tile.blend_mode() == blend_mode;
              });
            if (std::empty(filtered)) {
              continue;
            }
            vertex_group vg{};
            vg.blend_mode = blend_mode;
            vg.texture    = &m_texture->at(pos);
            vg.vertices.setPrimitiveType(sf::Quads);

            std::ranges::for_each(filtered,
              [&vg, &raw_tileSize, &new_tileSize, this](const tile_type &tile) {
                auto quad = std::array<sf::Vertex, 4U>{};
                //              quad[0].position = sf::Vector2f(
                //                static_cast<float>(tile.x()),
                //                static_cast<float>(tile.y()));
                //              quad[1].position =
                //              sf::Vector2f(static_cast<float>(tile.x() + 16),
                //                static_cast<float>(tile.y()));
                //              quad[2].position =
                //              sf::Vector2f(static_cast<float>(tile.x() + 16),
                //                static_cast<float>(tile.y() + 16));
                //              quad[3].position =
                //              sf::Vector2f(static_cast<float>(tile.x()),
                //                static_cast<float>(tile.y() + 16));
                //
                //              // define its 4 texture coordinates
                //              quad[0].texCoords =
                //                sf::Vector2f(static_cast<float>(tile.source_x()),
                //                  static_cast<float>(tile.source_y()));
                //              quad[1].texCoords =
                //                sf::Vector2f(static_cast<float>(tile.source_x()
                //                + 16),
                //                  static_cast<float>(tile.source_y()));
                //              quad[2].texCoords =
                //                sf::Vector2f(static_cast<float>(tile.source_x()
                //                + 16),
                //                  static_cast<float>(tile.source_y() + 16));
                //              quad[3].texCoords =
                //                sf::Vector2f(static_cast<float>(tile.source_x()),
                //                  static_cast<float>(tile.source_y() + 16));

                set_quad(quad, new_tileSize, raw_tileSize, tile);

                std::ranges::for_each(
                  quad, [&vg](auto &&vertex) { vg.vertices.append(vertex); });
              });
            if (vg.vertices.getVertexCount() != 0) {
              ret.emplace_back(std::move(vg));
            }
          }
        }
      }
    });
    return ret;
  }

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
  static sf::BlendMode &GetBlendMode()
  {
    static auto BlendSubtract = sf::BlendMode{ sf::BlendMode::DstColor,// or One
      sf::BlendMode::One,
      sf::BlendMode::ReverseSubtract,
      sf::BlendMode::One,
      sf::BlendMode::OneMinusSrcAlpha,
      sf::BlendMode::Add };
    return BlendSubtract;
  }
  void local_draw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    // apply the transform
    // states.transform *= getTransform();
    target.clear(sf::Color::Transparent);
    std::ranges::for_each(
      m_vertex_groups, [&target, &states](const vertex_group &v) {
        if (!v.enabled) {
          return;
        }

        states.blendMode = sf::BlendAlpha;
        if (v.blend_mode == open_viii::graphics::background::BlendModeT::add) {
          states.blendMode = sf::BlendAdd;
        } else if (v.blend_mode
                   == open_viii::graphics::background::BlendModeT::half_add) {
          states.blendMode = sf::BlendAdd;
        } else if (v.blend_mode
                   == open_viii::graphics::background::BlendModeT::subtract) {
          states.blendMode = GetBlendMode();
          // states.blendMode = sf::BlendMultiply;
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
  std::uint32_t width() const { return m_canvas.width(); }
  std::uint32_t height() const { return m_canvas.height(); }
  template<typename... T>
  requires(sizeof...(T) == 6U) static bool draw_drop_downs()
  {
    static constexpr std::array factor   = { "Zero",
      "One",
      "SrcColor",
      "OneMinusSrcColor",
      "DstColor",
      "OneMinusDstColor",
      "SrcAlpha",
      "OneMinusSrcAlpha",
      "DstAlpha",
      "OneMinusDstAlpha" };
    static constexpr std::array equation = {
      "Add", "Subtract", "ReverseSubtract"//, "Min", "Max"
    };
    static constexpr std::array names = {
      "colorSourceFactor",
      "colorDestinationFactor",
      "colorBlendEquation",
      "alphaSourceFactor",
      "alphaDestinationFactor",
      "alphaBlendEquation",
    };

    static std::array<int, std::ranges::size(names)> values = {};
    auto name  = std::ranges::begin(names);
    auto value = std::ranges::begin(values);
    static_assert(sizeof...(T) == std::ranges::size(names));
    const auto result = std::ranges::any_of(
      std::array{ ([](const char *const local_name, int &local_value) -> bool {
        using Real_Factor   = sf::BlendMode::Factor;
        using Real_Equation = sf::BlendMode::Equation;
        if constexpr (std::is_same_v<T, Real_Factor>) {
          return ImGui::Combo(local_name,
            &local_value,
            std::ranges::data(factor),
            std::ranges::ssize(factor));
        } else if constexpr (std::is_same_v<T, Real_Equation>) {
          return ImGui::Combo(local_name,
            &local_value,
            std::ranges::data(equation),
            std::ranges::ssize(equation));
        } else {
          return false;
        }
      }(*(name++), *(value++)))... },
      std::identity());
    if (result) {
      value          = std::ranges::begin(values);
      GetBlendMode() = sf::BlendMode{ (static_cast<T>(*(value++)))... };
    }
    return result;
  }
  static bool draw_drop_downs()
  {
    using Real_Factor   = sf::BlendMode::Factor;
    using Real_Equation = sf::BlendMode::Equation;
    return draw_drop_downs<Real_Factor,
      Real_Factor,
      Real_Equation,
      Real_Factor,
      Real_Factor,
      Real_Equation>();
  }
  static bool ImGui_controls(bool changed,
    map_sprite                   &ms,
    int & /*bpp_selected_item*/,
    int & /*palette_selected_item*/,
    std::array<float, 2> &xy,
    float                 scale_width = 0.0F)
  {
    static constexpr std::array bpp_items =
      open_viii::graphics::background::Mim::bpp_selections_c_str();
    static constexpr std::array palette_items =
      open_viii::graphics::background::Mim::palette_selections_c_str();
    //    if (ImGui::Combo("BPP",
    //          &bpp_selected_item,
    //          bpp_items.data(),
    //          static_cast<int>(bpp_items.size()),
    //          3)) {
    //      ms =
    //        ms.with_bpp(open_viii::graphics::background::Mim::bpp_selections().at(
    //          static_cast<std::size_t>(bpp_selected_item)));
    //      changed = true;
    //    }
    //    if (bpp_selected_item != 2) {
    //      if (ImGui::Combo("Palette",
    //            &palette_selected_item,
    //            palette_items.data(),
    //            static_cast<int>(palette_items.size()),
    //            10)) {
    //        ms      = ms.with_palette(static_cast<std::uint8_t>(
    //          open_viii::graphics::background::Mim::palette_selections().at(
    //                 static_cast<std::size_t>(palette_selected_item))));
    //        changed = true;
    //      }
    //    }

    if (draw_drop_downs()) {
      ms.update_render_texture();
    }
    format_imgui_text(
      "X: {:>9.3f} px  Width:  {:>4} px", ms.getPosition().x, ms.width());
    format_imgui_text(
      "Y: {:>9.3f} px  Height: {:>4} px", ms.getPosition().y, ms.height());
    if (ImGui::SliderFloat2("Adjust", xy.data(), -1.0F, 0.0F) || changed) {
      ms.setPosition(xy[0] * (static_cast<float>(ms.width()) - scale_width),
        xy[1] * static_cast<float>(ms.height()));
      changed = true;
    }

    return changed;
  }
  void draw(sf::RenderTarget &target, sf::RenderStates states) const final
  {
    // apply the transform
    states.transform *= getTransform();
    m_render_texture->display();
    states.texture = &m_render_texture->getTexture();
    sf::VertexArray out{};
    out.setPrimitiveType(sf::Quads);
    out.resize(4U);
    auto quad = std::span<sf::Vertex>(&out[0], 4);
    auto size = sf::Vector2u(width(), height());
    set_quad(quad, size, size, 0, 0, 0, 0);
    // draw the vertex array
    target.draw(out, states);
  }
};
#endif// MYPROJECT_MAP_SPRITE_HPP
