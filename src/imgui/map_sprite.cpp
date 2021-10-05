#include "map_sprite.hpp"

#include "append_inserter.hpp"
#include "imgui_format_text.hpp"
#include <utility>
using namespace open_viii::graphics::background;
using namespace open_viii::graphics;
using namespace open_viii::graphics::literals;
using namespace std::string_literals;
// static std::mutex mutex_texture{};
template<typename T, typename lambdaT, typename sortT, typename filterT>
std::vector<T> map_sprite::get_unique_from_tiles(lambdaT &&lambda,
  sortT                                                  &&sort,
  filterT                                                &&filter) const
{
  return m_map.visit_tiles([&lambda, &sort, &filter](auto &&tiles) {
    std::vector<T> ret{};
    if (!std::empty(tiles)) {
      std::ranges::transform(
        tiles | std::views::filter(filter), std::back_inserter(ret), lambda);
      std::ranges::sort(ret, sort);
      auto last = std::unique(ret.begin(), ret.end());
      ret.erase(last, ret.end());
    }
    return ret;
  });
}
std::vector<open_viii::graphics::BPPT> map_sprite::get_bpps()
{
  return get_unique_from_tiles<open_viii::graphics::BPPT>(
    [](const auto &tile) { return tile.depth(); });
}
bool map_sprite::empty() const
{
  return m_map.visit_tiles([](auto &&tiles) { return std::empty(tiles); });
}

std::map<std::uint8_t, std::vector<std::uint8_t>>
  map_sprite::get_unique_animation_frames() const
{
  std::map<std::uint8_t, std::vector<std::uint8_t>> ret = {};
  for (const auto &id : m_unique_animation_ids) {
    ret.emplace(id,
      get_unique_from_tiles<std::uint8_t>(
        [](const auto &tile) { return tile.animation_state(); },
        {},
        [&id](const auto &tile) { return id == tile.animation_id(); }));
  }
  return ret;
}

std::map<open_viii::graphics::BPPT, std::vector<std::uint8_t>>
  map_sprite::get_palettes()
{
  std::map<open_viii::graphics::BPPT, std::vector<std::uint8_t>> ret = {};
  for (const auto &id : m_bpps) {
    ret.emplace(id,
      get_unique_from_tiles<std::uint8_t>(
        [](const auto &tile) { return tile.palette_id(); },
        {},
        [&id](const auto &tile) { return id == tile.depth(); }));
  }
  return ret;
}

std::vector<std::uint8_t> map_sprite::get_unique_animation_ids() const
{
  return get_unique_from_tiles<std::uint8_t>(
    [](const auto &tile) { return tile.animation_id(); });
}

std::vector<std::uint16_t> map_sprite::get_unique_z_axis() const
{
  return get_unique_from_tiles<std::uint16_t>(
    [](const auto &tile) { return tile.z(); }, std::greater<>{});
}

std::vector<std::uint8_t> map_sprite::get_unique_layers() const
{
  return get_unique_from_tiles<std::uint8_t>(
    [](const auto &tile) { return tile.layer_id(); });
}

std::vector<std::uint8_t> map_sprite::get_unique_texture_pages() const
{
  return get_unique_from_tiles<std::uint8_t>(
    [](const auto &tile) { return tile.texture_id(); });
}

std::vector<BlendModeT> map_sprite::get_blend_modes() const
{
  return get_unique_from_tiles<BlendModeT>(
    [](const auto &tile) { return tile.blend_mode(); });
}

std::vector<std::pair<open_viii::graphics::BPPT, std::uint8_t>>
  map_sprite::get_bpp_and_palette() const
{
  return get_unique_from_tiles<
    std::pair<open_viii::graphics::BPPT, std::uint8_t>>([](const auto &tile) {
    return std::pair(tile.depth(), tile.palette_id());
  });
}

Mim map_sprite::get_mim() const
{
  if (m_field != nullptr) {
    auto lang_name =
      fmt::format("_{}{}", open_viii::LangCommon::to_string(m_coo), Mim::EXT);
    return { m_field->get_entry_data({ std::string_view(lang_name), Mim::EXT }),
      m_field->get_base_name() };
  }
  return {};
}

Map map_sprite::get_map(std::string *out_path,
  bool                               sort_remove,
  bool                               shift) const
{
  if (m_field != nullptr) {
    auto lang_name =
      fmt::format("_{}{}", open_viii::LangCommon::to_string(m_coo), Map::EXT);
    return Map{ m_mim.mim_type(),
      m_field->get_entry_data(
        { std::string_view(lang_name), Map::EXT }, out_path),
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
    return palette + std::size(Mim::palette_selections());
  }
  return MAX_TEXTURES - 1;
  // 16bpp doesn't have palettes.
}

const sf::Texture *map_sprite::get_texture(const open_viii::graphics::BPPT bpp,
  const std::uint8_t palette) const
{
  return &m_texture->at(get_texture_pos(bpp, palette));
}

std::shared_ptr<std::array<sf::Texture, map_sprite::MAX_TEXTURES>>
  map_sprite::get_textures() const
{
  auto ret = std::make_shared<std::array<sf::Texture, MAX_TEXTURES>>(
    std::array<sf::Texture, MAX_TEXTURES>{});
  if (!std::empty(m_bpp_and_palette)) {
    for (const auto &[bpp, palette] : m_bpp_and_palette) {
      if (bpp.bpp24()) {
        continue;
      }
      // std::cout << bpp << '\t' << +palette << '\t' << '\t';
      size_t pos = get_texture_pos(bpp, palette);
      if (m_mim.get_width(bpp) != 0U) {
        m_futures.emplace_back(std::async(
          std::launch::async,
          [this](sf::Texture *texture, BPPT bppt, std::uint8_t pal) {
            const auto colors = get_colors(bppt, pal);
            // std::lock_guard<std::mutex> lock(mutex_texture);
            texture->create(m_mim.get_width(bppt), m_mim.get_height());
            texture->setSmooth(false);
            texture->update(reinterpret_cast<const sf::Uint8 *>(colors.data()));
          },
          &(ret->at(pos)),
          bpp,
          palette));
      }
    }
    // std::cout << std::endl;
  }
  return ret;
}
void map_sprite::wait_for_futures() const
{
  std::ranges::for_each(m_futures, [](auto &&f) { f.wait(); });
  m_futures.clear();
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

[[nodiscard]] std::array<sf::Vertex, 4U> map_sprite::get_triangle_strip(
  const sf::Vector2u &new_tileSize,
  const sf::Vector2u &raw_tileSize,
  auto              &&tile) const
{
  using tile_type = std::decay_t<decltype(tile)>;
  using namespace open_viii::graphics::literals;
  const auto src_x =
    tile.source_x()
    + tile.texture_id() * tile_type::texture_page_width(tile.depth());

  const auto dst_x = [this, &tile]() {
    if (m_draw_swizzle) {
      return static_cast<uint32_t>(
        tile.source_x()
        + tile.texture_id() * tile_type::texture_page_width(4_bpp));
    }
    return static_cast<uint32_t>(tile.x());
  }();
  const auto dst_y = [this, &tile]() {
    if (m_draw_swizzle) {
      return static_cast<uint32_t>(tile.source_y());
    }
    return static_cast<uint32_t>(tile.y());
  }();
  return get_triangle_strip(
    new_tileSize, raw_tileSize, src_x, tile.source_y(), dst_x, dst_y);
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
  wait_for_futures();
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
          if (fail_filter(tile)) {
            return;
          }
          const auto raw_tileSize = sf::Vector2u{ 16U, 16U };
          auto quad = get_triangle_strip(new_tileSize, raw_tileSize, tile);
          states.blendMode = sf::BlendAlpha;
          if (tile.blend_mode() == BlendModeT::add) {
            states.blendMode = sf::BlendAdd;
          } else if (tile.blend_mode() == BlendModeT::half_add) {
            states.blendMode = sf::BlendAdd;
            constexpr static std::uint8_t per50 =
              (std::numeric_limits<std::uint8_t>::max)() / 2U;
            set_color(quad, { per50, per50, per50, per50 });// 50% alpha
          } else if (tile.blend_mode() == BlendModeT::quarter_add) {
            states.blendMode = sf::BlendAdd;
            constexpr static std::uint8_t per25 =
              (std::numeric_limits<std::uint8_t>::max)() / 4U;
            set_color(quad, { per25, per25, per25, per25 });// 25% alpha
          } else if (tile.blend_mode() == BlendModeT::subtract) {
            states.blendMode = GetBlendSubtract();
            // states.blendMode = sf::BlendMultiply;
          }
          // apply the tileset texture

          // std::lock_guard<std::mutex> lock(mutex_texture);
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
  return { m_field, coo, m_draw_swizzle, m_filters };
}

map_sprite map_sprite::with_field(
  std::shared_ptr<open_viii::archive::FIFLFS<false>> field) const
{
  return { std::move(field), m_coo, m_draw_swizzle, m_filters };
}

void map_sprite::enable_draw_swizzle() const
{
  m_draw_swizzle = true;
  init_render_texture();
  m_grid = get_grid();
}

void map_sprite::disable_draw_swizzle() const
{
  m_draw_swizzle = false;
  init_render_texture();
  m_grid = get_grid();
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
  // draw texture_page_grid
  if (m_draw_swizzle) {
    target.draw(m_texture_page_grid, states);
  }
}
void map_sprite::update_render_texture() const
{
  if (!fail()) {
    local_draw(*m_render_texture, sf::RenderStates::Default);
    m_render_texture->display();
  }
}
void map_sprite::save(const std::filesystem::path &path) const
{
  if (fail()) {
    return;
  }
  const auto image = m_render_texture->getTexture().copyToImage();
  if (!image.saveToFile(path.string())) {
    std::cerr << "failure to save file:" << path << std::endl;
  }
}
bool map_sprite::fail() const
{
  static bool once = true;
  using namespace open_viii::graphics::literals;
  if (!m_render_texture) {
    if (once) {
      std::cout << "m_render_texture is null" << std::endl;
      once = false;
    }
    return true;
  }
  if (!m_texture) {
    if (once) {
      std::cout << "m_texture is null" << std::endl;
      once = false;
    }
    return true;
  }
  if (m_mim.get_width(4_bpp, false) == 0) {
    if (once) {
      std::cout << "m_mim width is 0" << std::endl;
      once = false;
    }
    return true;
  }
  if (empty()) {
    if (once) {
      std::cout << "m_map is empty" << std::endl;
      once = false;
    }
    return true;
  }
  once = true;
  return false;
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

const map_sprite &map_sprite::toggle_grid(bool enable,
  bool                                         enable_texture_page_grid) const
{
  if (enable) {
    m_grid.enable();
  } else {
    m_grid.disable();
  }

  if (enable_texture_page_grid) {
    // std::cout << "enabled: " << m_texture_page_grid.count() << '\n';
    m_texture_page_grid.enable();
  } else {
    m_texture_page_grid.disable();
  }
  return *this;
}
void map_sprite::resize_render_texture() const
{
  if (!fail()) {
    m_render_texture->create(width(), height());
  }
}
void map_sprite::init_render_texture() const
{
  if (!m_render_texture) {
    m_render_texture = std::make_shared<sf::RenderTexture>();
  }
  resize_render_texture();
  update_render_texture();
}


open_viii::graphics::Rectangle<std::uint32_t> map_sprite::get_canvas() const
{
  return static_cast<open_viii::graphics::Rectangle<std::uint32_t>>(
    m_map.canvas());
}

std::uint32_t map_sprite::width() const
{
  if (m_draw_swizzle) {
    using namespace open_viii::graphics::literals;
    return m_mim.get_width(4_bpp);
  }
  return m_canvas.width();
}

std::uint32_t map_sprite::height() const
{
  if (m_draw_swizzle) {
    return m_mim.get_height();
  }
  return m_canvas.height();
}
grid map_sprite::get_grid() const
{
  return { { 16U, 16U }, { width(), height() } };
}
grid map_sprite::get_texture_page_grid() const
{
  using namespace open_viii::graphics::literals;
  return { { (1U << static_cast<unsigned int>((8 - ((4_bpp).raw() & 3U)))),
             256U },
    { m_mim.get_width(4_bpp), m_mim.get_height() },
    sf::Color::Yellow };
}

bool map_sprite::fail_filter(auto &tile) const
{
  using namespace open_viii::graphics::literals;
  if (m_filters.palette.enabled() && m_filters.bpp.value() != 16_bpp) {
    if (tile.palette_id() != m_filters.palette.value()) {
      return true;
    }
  }
  if (m_filters.bpp.enabled()) {
    if (tile.depth() != m_filters.bpp.value()) {
      return true;
    }
  }
  if (m_filters.blend_mode.enabled()) {
    if (tile.blend_mode() != m_filters.blend_mode.value()) {
      return true;
    }
  }
  if (m_filters.animation_id.enabled()) {
    if (tile.animation_id() != m_filters.animation_id.value()) {
      return true;
    }
  }
  if (m_filters.animation_frame.enabled()) {
    if (tile.animation_state() != m_filters.animation_frame.value()) {
      return true;
    }
  }
  if (m_filters.layer_id.enabled()) {
    if (tile.layer_id() != m_filters.layer_id.value()) {
      return true;
    }
  }
  if (m_filters.texture_page_id.enabled()) {
    if (tile.texture_id() != m_filters.texture_page_id.value()) {
      return true;
    }
  }
  return false;
}
template<typename T, typename U>
std::map<T, std::vector<std::string>> map_sprite::get_strings(
  const std::map<T, std::vector<U>> &data)
{
  std::map<T, std::vector<std::string>> map = {};
  for (const auto &[key, vector] : data) {
    map.emplace(key, get_strings<U>(vector));
  }
  return map;
}
template<typename T>
std::vector<std::string> map_sprite::get_strings(const std::vector<T> &data)
{
  std::vector<std::string> vector;
  vector.reserve(std::size(data));
  std::ranges::transform(data, std::back_inserter(vector), [](const T &t) {
    if constexpr (std::is_same_v<T, BlendModeT>) {
      switch (t) {
      case BlendModeT::quarter_add:
        return "quarter add"s;
      case BlendModeT::half_add:
        return "half add"s;
      case BlendModeT::add:
        return "add"s;
      default:
      case BlendModeT::none:
        return "none"s;
      case BlendModeT::subtract:
        return "subtract"s;
      }
    } else if constexpr (std::is_same_v<T, BPPT>) {
      if (t.bpp8()) return "8"s;
      if (t.bpp16()) return "16"s;
      return "4"s;
    } else {
      return fmt::format("{}", t);
    }
  });
  return vector;
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
