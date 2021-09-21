#include "map_sprite.hpp"
open_viii::graphics::background::Mim map_sprite::get_mim() const
{
  auto lang_name = "_" + std::string(open_viii::LangCommon::to_string(m_coo))
                   + std::string(open_viii::graphics::background::Mim::EXT);
  return { m_field->get_entry_data({ std::string_view(lang_name),
             open_viii::graphics::background::Mim::EXT }),
    m_field->get_base_name() };
}

open_viii::graphics::background::Map map_sprite::get_map() const
{
  auto lang_name = "_" + std::string(open_viii::LangCommon::to_string(m_coo))
                   + std::string(open_viii::graphics::background::Map::EXT);
  return open_viii::graphics::background::Map{ m_mim.mim_type(),
    m_field->get_entry_data({ std::string_view(lang_name),
      open_viii::graphics::background::Map::EXT }),
    true,
    true };
}

map_sprite::colors_type map_sprite::get_colors(open_viii::graphics::BPPT bpp,
  std::uint8_t palette,
  bool         draw_palette) const
{
  return m_mim.get_colors<open_viii::graphics::Color32RGBA>(
    bpp, palette, draw_palette);
}

constexpr std::size_t map_sprite::get_texture_pos(open_viii::graphics::BPPT bpp,
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

const sf::Texture *map_sprite::get_texture(const open_viii::graphics::BPPT bpp,
  const std::uint8_t palette) const
{
  return &m_texture->at(get_texture_pos(bpp, palette));
}

std::unique_ptr<std::array<sf::Texture, map_sprite::MAX_TEXTURES>>
  map_sprite::get_textures() const
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

std::vector<std::uint16_t> map_sprite::get_unique_z_axis() const
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

std::vector<std::uint8_t> map_sprite::get_unique_layers() const
{
  std::vector<std::uint8_t> ret{};
  m_map.visit_tiles([&ret](auto &&tiles) {
    std::ranges::transform(tiles,
      std::back_inserter(ret),
      [](const auto &tile) { return tile.layer_id(); });
    std::ranges::sort(ret, std::greater<>());
    auto last = std::unique(ret.begin(), ret.end());
    ret.erase(last, ret.end());
  });
  for (const auto z : ret) {
    std::cout << +z << '\t';
  }
  std::cout << '\n';
  return ret;
}

std::vector<open_viii::graphics::background::BlendModeT>
  map_sprite::get_blend_modes() const
{
  std::vector<open_viii::graphics::background::BlendModeT> ret{};
  m_map.visit_tiles([&ret](auto &&tiles) {
    std::ranges::transform(tiles,
      std::back_inserter(ret),
      [](const auto &tile) { return tile.blend_mode(); });
    std::ranges::sort(ret, std::less<>());
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
  map_sprite::get_bpp_and_palette() const
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

[[nodiscard]] sf::VertexArray map_sprite::get_quad(
  const sf::Vector2u &new_tileSize,
  const sf::Vector2u &raw_tileSize,
  auto                source_x,
  auto                source_y,
  auto                x,
  auto                y) const
{
  sf::VertexArray quad{};
  quad.resize(4);
  quad.setPrimitiveType(sf::Quads);
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
  return quad;
}

[[nodiscard]] sf::VertexArray map_sprite::get_quad(
  const sf::Vector2u &new_tileSize,
  const sf::Vector2u &raw_tileSize,
  auto              &&tile) const
{

  using tile_type = std::decay_t<decltype(tile)>;
  return get_quad(new_tileSize,
    raw_tileSize,
    tile.source_x()
      + tile.texture_id() * tile_type::texture_page_width(tile.depth()),
    tile.source_y(),
    tile.x(),
    tile.y());
}

// std::vector<map_sprite::vertex_group> map_sprite::get_vertex_groups(
//   const sf::Vector2u new_tileSize)
//{
//   std::vector<vertex_group> ret{};
//   m_map.visit_tiles([&ret, this, &new_tileSize](auto &&tiles) {
//     using tiles_type                 = std::decay_t<decltype(tiles)>;
//     using tile_type [[maybe_unused]] = typename tiles_type::value_type;
//     const auto raw_tileSize          = sf::Vector2u{ 16U, 16U };
//
//     for (const auto &z : m_unique_z_axis) {
//       for (const auto &layer : m_unique_layers) {
//         for (const auto &[bpp, palette] : m_bpp_and_palette) {
//           for (const auto &blend_mode : m_blend_modes) {
//             if (bpp.bpp24()) {
//               continue;
//             }
//             size_t     pos   = get_texture_pos(bpp, palette);
//             const auto width = m_mim.get_width(bpp);
//             // const auto height = m_mim.get_height();
//             if (width == 0U) {
//               continue;
//             }
//             tiles_type filtered = {};
//             std::copy_if(tiles.begin(),
//               tiles.end(),
//               std::back_inserter(filtered),
//               [&z, lbpp = bpp, lpalette = palette, blend_mode, &layer](
//                 const auto &tile) -> bool {
//                 return tile.z() == z && tile.depth() == lbpp
//                        && tile.palette_id() == lpalette
//                        && tile.blend_mode() == blend_mode
//                        && tile.layer_id() == layer;
//               });
//             if (std::empty(filtered)) {
//               continue;
//             }
//             vertex_group vg{};
//             vg.blend_mode = blend_mode;
//             vg.texture    = &m_texture->at(pos);
//             vg.vertices.setPrimitiveType(sf::Quads);
//
//             std::ranges::for_each(filtered,
//               [&vg, &raw_tileSize, &new_tileSize, this](const tile_type
//               &tile) {
//                 auto quad = std::array<sf::Vertex, 4U>{};
//                 get_quad(quad, new_tileSize, raw_tileSize, tile);
//
//                 std::ranges::for_each(
//                   quad, [&vg](auto &&vertex) { vg.vertices.append(vertex);
//                   });
//               });
//             if (vg.vertices.getVertexCount() != 0) {
//               ret.emplace_back(std::move(vg));
//             }
//           }
//         }
//       }
//     }
//   });
//   return ret;
// }
void set_color(sf::VertexArray &v, const sf::Color &color)
{
  for (std::size_t i{}; i != v.getVertexCount(); ++i) v[i].color = color;
}
void map_sprite::local_draw(sf::RenderTarget &target,
  sf::RenderStates                            states) const
{
  const auto new_tileSize = sf::Vector2u{ 16U, 16U };
  target.clear(sf::Color::Transparent);
  m_map.visit_tiles([this, &new_tileSize, &states, &target](auto &&tiles) {
    for (const auto &z : m_unique_z_axis) {
      std::for_each(std::rbegin(tiles),
        std::rend(tiles),
        [this, &new_tileSize, &states, &target, &z](const auto &tile) {
          if (tile.z() != z) {
            return;
          }
          const auto raw_tileSize = sf::Vector2u{ 16U, 16U };
          auto       quad         = get_quad(new_tileSize, raw_tileSize, tile);
          states.blendMode        = sf::BlendAlpha;
          if (tile.blend_mode()
              == open_viii::graphics::background::BlendModeT::add) {
            states.blendMode = sf::BlendAdd;
          } else if (tile.blend_mode()
                     == open_viii::graphics::background::BlendModeT::half_add) {
            states.blendMode = sf::BlendAdd;
            set_color(quad, { 127, 127, 127, 127 });// 50% alpha
          } else if (tile.blend_mode()
                     == open_viii::graphics::background::BlendModeT::
                       quarter_add) {
            states.blendMode = sf::BlendAdd;
            set_color(quad, { 63, 63, 63, 63 });// 25% alpha
          } else if (tile.blend_mode()
                     == open_viii::graphics::background::BlendModeT::subtract) {
            states.blendMode = GetBlendSubtract();
            // states.blendMode = sf::BlendMultiply;
          }
          // apply the tileset texture
          states.texture = get_texture(tile.depth(), tile.palette_id());

          // draw the vertex array
          target.draw(quad, states);
        });
    }
  });
}

sf::BlendMode &map_sprite::GetBlendSubtract()
{
  static auto BlendSubtract = sf::BlendMode{ sf::BlendMode::DstColor,// or One
    sf::BlendMode::One,
    sf::BlendMode::ReverseSubtract,
    sf::BlendMode::One,
    sf::BlendMode::OneMinusSrcAlpha,
    sf::BlendMode::Add };
  return BlendSubtract;
}

map_sprite map_sprite::with_coo(const open_viii::LangT coo) const
{
  return { *m_field, coo };
}

map_sprite map_sprite::with_field(
  const open_viii::archive::FIFLFS<false> &field) const
{
  return { field, m_coo };
}

bool map_sprite::draw_drop_downs()
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

bool map_sprite::ImGui_controls(bool changed,
  map_sprite                        &ms,
  int &,
  int &,
  std::array<float, 2> &xy,
  float                 scale_width)
{
  static constexpr std::array bpp_items =
    open_viii::graphics::background::Mim::bpp_selections_c_str();
  static constexpr std::array palette_items =
    open_viii::graphics::background::Mim::palette_selections_c_str();


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

void map_sprite::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  // apply the transform
  states.transform *= getTransform();
  m_render_texture->display();
  states.texture = &m_render_texture->getTexture();
  auto size      = sf::Vector2u(width(), height());
  auto quad      = get_quad(size, size, 0, 0, 0, 0);
  // draw the vertex array
  target.draw(quad, states);
}

template<typename... T>
requires(sizeof...(T) == 6U) bool map_sprite::draw_drop_downs()
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
          static_cast<int>(std::ranges::ssize(factor)));
      } else if constexpr (std::is_same_v<T, Real_Equation>) {
        return ImGui::Combo(local_name,
          &local_value,
          std::ranges::data(equation),
          static_cast<int>(std::ranges::ssize(equation)));
      } else {
        return false;
      }
    }(*(name++), *(value++)))... },
    std::identity());
  if (result) {
    value              = std::ranges::begin(values);
    GetBlendSubtract() = sf::BlendMode{ (static_cast<T>(*(value++)))... };
  }
  return result;
}
