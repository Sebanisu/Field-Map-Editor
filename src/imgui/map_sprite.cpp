#include "map_sprite.hpp"
#include "append_inserter.hpp"
#include "imgui_format_text.hpp"
#include <imgui.h>
open_viii::graphics::background::Mim map_sprite::get_mim() const
{
  if (m_field != nullptr) {
    auto lang_name = fmt::format("_{}{}",
      open_viii::LangCommon::to_string(m_coo),
      open_viii::graphics::background::Mim::EXT);
    return { m_field->get_entry_data({ std::string_view(lang_name),
               open_viii::graphics::background::Mim::EXT }),
      m_field->get_base_name() };
  }
  return {};
}

open_viii::graphics::background::Map
  map_sprite::get_map(std::string *out_path, bool sort_remove, bool shift) const
{
  if (m_field != nullptr) {
    auto lang_name = fmt::format("_{}{}",
      open_viii::LangCommon::to_string(m_coo),
      open_viii::graphics::background::Map::EXT);
    return open_viii::graphics::background::Map{ m_mim.mim_type(),
      m_field->get_entry_data({ std::string_view(lang_name),
                                open_viii::graphics::background::Map::EXT },
        out_path),
      sort_remove,
      shift };
  }
  return {};
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

[[nodiscard]] std::array<sf::Vertex, 4U> map_sprite::get_triangle_strip(
  const sf::Vector2u &new_tileSize,
  const sf::Vector2u &raw_tileSize,
  auto                source_x,
  auto                source_y,
  auto                x,
  auto                y) const
{
  auto i =
    static_cast<std::uint32_t>(x / static_cast<decltype(x)>(raw_tileSize.x));
  auto j =
    static_cast<std::uint32_t>(y / static_cast<decltype(y)>(raw_tileSize.y));
  std::uint32_t tu =
    (source_x) / static_cast<decltype(source_x)>(raw_tileSize.x);
  std::uint32_t tv = source_y / static_cast<decltype(source_y)>(raw_tileSize.y);
  const auto    tovec = [](auto &&x, auto &&y) {
    return sf::Vector2f{ static_cast<float>(x), static_cast<float>(y) };
  };
  const auto tovert = [&tovec](auto &&x, auto &&y, auto &&texx, auto &&texy) {
    return sf::Vertex{ tovec(x, y), tovec(texx, texy) };
  };
  return std::array{ tovert((i + 1) * new_tileSize.x,
                       j * new_tileSize.y,
                       (tu + 1) * new_tileSize.x,
                       tv * new_tileSize.y),
    tovert(i * new_tileSize.x,
      j * new_tileSize.y,
      tu * new_tileSize.x,
      tv * new_tileSize.y),
    tovert((i + 1) * new_tileSize.x,
      (j + 1) * new_tileSize.y,
      (tu + 1) * new_tileSize.x,
      (tv + 1) * new_tileSize.y),
    tovert(i * new_tileSize.x,
      (j + 1) * new_tileSize.y,
      tu * new_tileSize.x,
      (tv + 1) * new_tileSize.y) };
}

[[maybe_unused]] [[nodiscard]] std::array<sf::Vertex, 4U>
  map_sprite::get_triangle_strip(const sf::Vector2u &new_tileSize,
    const sf::Vector2u                              &raw_tileSize,
    auto                                           &&tile) const
{
  using tile_type = std::decay_t<decltype(tile)>;
  return get_triangle_strip(new_tileSize,
    raw_tileSize,
    tile.source_x()
      + tile.texture_id() * tile_type::texture_page_width(tile.depth()),
    tile.source_y(),
    tile.x(),
    tile.y());
}

void set_color(std::array<sf::Vertex, 4U> &vertices, const sf::Color &color)
{
  for (auto &vertex : vertices) {
    vertex.color = color;
  }
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
          auto quad = get_triangle_strip(new_tileSize, raw_tileSize, tile);
          states.blendMode = sf::BlendAlpha;
          if (tile.blend_mode()
              == open_viii::graphics::background::BlendModeT::add) {
            states.blendMode = sf::BlendAdd;
          } else if (tile.blend_mode()
                     == open_viii::graphics::background::BlendModeT::half_add) {
            states.blendMode = sf::BlendAdd;
            constexpr static std::uint8_t per50 =
              (std::numeric_limits<std::uint8_t>::max)() / 2U;
            set_color(quad, { per50, per50, per50, per50 });// 50% alpha
          } else if (tile.blend_mode()
                     == open_viii::graphics::background::BlendModeT::
                       quarter_add) {
            states.blendMode = sf::BlendAdd;
            constexpr static std::uint8_t per25 =
              (std::numeric_limits<std::uint8_t>::max)() / 4U;
            set_color(quad, { per25, per25, per25, per25 });// 25% alpha
          } else if (tile.blend_mode()
                     == open_viii::graphics::background::BlendModeT::subtract) {
            states.blendMode = GetBlendSubtract();
            // states.blendMode = sf::BlendMultiply;
          }
          // apply the tileset texture
          states.texture = get_texture(tile.depth(), tile.palette_id());

          // draw the vertex array
          target.draw(quad.data(), quad.size(), sf::TriangleStrip, states);
        });
    }
  });
}

const sf::BlendMode &map_sprite::GetBlendSubtract()
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

void map_sprite::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  // apply the transform
  states.transform *= getTransform();
  states.texture = &m_render_texture->getTexture();
  auto size      = sf::Vector2u(width(), height());
  auto quad      = get_triangle_strip(size, size, 0, 0, 0, 0);
  // draw the vertex array
  target.draw(quad.data(), quad.size(), sf::TriangleStrip, states);
  // draw grid
  target.draw(m_grid, states);
}
void map_sprite::update_render_texture() const
{
  if (m_render_texture) {
    local_draw(*m_render_texture, sf::RenderStates::Default);
    m_render_texture->display();
  }
}
void map_sprite::save(const std::filesystem::path &path) const
{
  if (!m_render_texture) {
    return;
  }
  const auto image = m_render_texture->getTexture().copyToImage();
  if (!image.saveToFile(path.string())) {
    std::cerr << "failure to save file:" << path << std::endl;
  }
}
bool map_sprite::fail() const
{
  using namespace open_viii::graphics::literals;
  return m_mim.get_width(4_bpp, false) == 0;
}
void map_sprite::map_save(const std::filesystem::path &dest_path) const
{
  const auto map  = get_map(nullptr, false, false);
  const auto path = dest_path.string();

  open_viii::tools::write_buffer(
    [&map](std::ostream &os) {
      map.visit_tiles([&os](auto &&tiles) {
        for (const auto &tile : tiles) {
          //          std::array<char, sizeof(tile)> data{};
          //          std::memcpy(data.data(), &tile, sizeof(tile));
          const auto data = std::bit_cast<std::array<char, sizeof(tile)>>(tile);
          os.write(data.data(), data.size());
        }
      });
    },
    path,
    "");
}
std::string map_sprite::map_filename()
{
  return std::filesystem::path(m_map_path).filename().string();
}

const map_sprite &map_sprite::toggle_grid(bool enable) const
{
  if (enable) {
    m_grid.enable();
  } else {
    m_grid.disable();
  }
  return *this;
}

// template<typename... T>
// requires(sizeof...(T) == 6U) bool map_sprite::draw_drop_downs()
//{
//   static constexpr std::array factor   = { "Zero",
//     "One",
//     "SrcColor",
//     "OneMinusSrcColor",
//     "DstColor",
//     "OneMinusDstColor",
//     "SrcAlpha",
//     "OneMinusSrcAlpha",
//     "DstAlpha",
//     "OneMinusDstAlpha" };
//
//   static constexpr std::array equation = {
//     "Add", "Subtract", "ReverseSubtract"//, "Min", "Max"
//   };
//
//   static constexpr std::array names = {
//     "colorSourceFactor",
//     "colorDestinationFactor",
//     "colorBlendEquation",
//     "alphaSourceFactor",
//     "alphaDestinationFactor",
//     "alphaBlendEquation",
//   };
//
//   static std::array<int, std::ranges::size(names)> values = {};
//   auto name  = std::ranges::begin(names);
//   auto value = std::ranges::begin(values);
//   static_assert(sizeof...(T) == std::ranges::size(names));
//   const auto result = std::ranges::any_of(
//     std::array{ ([](const char *const local_name, int &local_value) -> bool {
//       using Real_Factor   = sf::BlendMode::Factor;
//       using Real_Equation = sf::BlendMode::Equation;
//       if constexpr (std::is_same_v<T, Real_Factor>) {
//         return ImGui::Combo(local_name,
//           &local_value,
//           std::ranges::data(factor),
//           static_cast<int>(std::ranges::ssize(factor)));
//       } else if constexpr (std::is_same_v<T, Real_Equation>) {
//         return ImGui::Combo(local_name,
//           &local_value,
//           std::ranges::data(equation),
//           static_cast<int>(std::ranges::ssize(equation)));
//       } else {
//         return false;
//       }
//     }(*(name++), *(value++)))... },
//     std::identity());
//   if (result) {
//     value              = std::ranges::begin(values);
//     GetBlendSubtract() = sf::BlendMode{ (static_cast<T>(*(value++)))... };
//   }
//   return result;
// }
