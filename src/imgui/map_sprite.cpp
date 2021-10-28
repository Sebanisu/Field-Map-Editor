#include "map_sprite.hpp"

#include "append_inserter.hpp"
#include "imgui_format_text.hpp"
#include <bit>
#include <utility>
using namespace open_viii::graphics::background;
using namespace open_viii::graphics;
using namespace open_viii::graphics::literals;
using namespace std::string_literals;
static std::string
  str_to_lower(std::string &&input)
{
  std::string output{};
  output.reserve(std::size(input) + 1);
  std::ranges::transform(input,
    std::back_inserter(output),
    [](char c) -> char { return static_cast<char>(::tolower(c)); });
  return output;
}
bool
  map_sprite::empty() const
{
  return m_maps.const_back().visit_tiles(
    [](const auto &tiles) { return std::empty(tiles); });
}

Mim
  map_sprite::get_mim() const
{
  if (m_field)
  {
    auto lang_name =
      fmt::format("_{}{}", open_viii::LangCommon::to_string(m_coo), Mim::EXT);
    return { m_field->get_entry_data({ std::string_view(lang_name), Mim::EXT }),
      str_to_lower(m_field->get_base_name()) };
  }
  return {};
}

Map
  map_sprite::get_map(std::string *out_path, bool shift, bool &coo) const
{
  if (m_field)
  {
    std::size_t out_path_pos = {};
    auto        lang_name =
      fmt::format("_{}{}", open_viii::LangCommon::to_string(m_coo), Map::EXT);
    auto map = Map{ m_mim.mim_type(),
      m_field->get_entry_data(
        { std::string_view(lang_name), Map::EXT }, out_path, &out_path_pos),
      shift };
    coo      = out_path_pos == 0U;
    return map;
  }
  return {};
}

map_sprite::colors_type
  map_sprite::get_colors(open_viii::graphics::BPPT bpp,
    std::uint8_t                                   palette) const
{
  return m_mim.get_colors<open_viii::graphics::Color32RGBA>(
    bpp, palette, false);
}

std::size_t
  map_sprite::get_texture_pos(const open_viii::graphics::BPPT bpp,
    const std::uint8_t                                        palette,
    const std::uint8_t texture_page) const
{
  if (!m_filters.upscale.enabled())
  {
    if (bpp.bpp4())
    {
      return palette;
    }
    if (bpp.bpp8())
    {
      return palette + std::size(Mim::palette_selections());
    }
    return MAX_TEXTURES - 1;// 16bpp doesn't have palettes.
  }
  const size_t j = static_cast<size_t>(texture_page) * 16U + palette;
  if (m_texture->at(j).getSize().y == 0)
  {
    return MAX_TEXTURES - 13U + texture_page;
  }
  return j;
}

const sf::Texture *
  map_sprite::get_texture(const open_viii::graphics::BPPT bpp,
    const std::uint8_t                                    palette,
    const std::uint8_t                                    texture_page) const
{
  return &m_texture->at(get_texture_pos(bpp, palette, texture_page));
}

std::shared_ptr<std::array<sf::Texture, map_sprite::MAX_TEXTURES>>
  map_sprite::get_textures() const
{

  auto ret = std::make_shared<std::array<sf::Texture, MAX_TEXTURES>>(
    std::array<sf::Texture, MAX_TEXTURES>{});
  const auto &range = m_all_unique_values_and_strings.bpp().values();
  if (!std::empty(range))
  {
    for (const auto &bpp : range)
    {
      const auto &map = m_all_unique_values_and_strings.palette();
      if (map.contains(bpp))
      {

        for (const auto &palette : map.at(bpp).values())
        {
          if (bpp.bpp24())
          {
            continue;
          }
          if (!m_filters.upscale.enabled())
          {
            load_mim_textures(ret, bpp, palette);
          }
          else
          {
            find_upscale_path(ret, palette);
          }
        }
      }
    }
    if (m_filters.upscale.enabled())
    {
      find_upscale_path(ret);
    }
    wait_for_futures();
  }
  size_t i = {};
  for (const auto &texture : *ret)
  {
    const auto size = texture.getSize();
    if (size.x != 0)
    {
      // fmt::print("\ttex: {:3} - ({:4}, {:4})\n", i++, size.x, size.y);
    }
    else
    {
      ++i;
    }
  }
  return ret;
}
void
  map_sprite::load_mim_textures(
    std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret,
    open_viii::graphics::BPPT                               bpp,
    std::uint8_t                                            palette) const
{// std::cout << bpp << '\t' << +palette << '\t' << '\t';
  if (m_mim.get_width(bpp) != 0U)
  {
    size_t pos = get_texture_pos(bpp, palette, 0);
    m_futures.emplace_back(std::async(
      std::launch::async,
      [this](sf::Texture *texture, BPPT bppt, uint8_t pal)
      {
        const auto colors = get_colors(bppt, pal);
        // std::lock_guard<std::mutex> lock(mutex_texture);
        texture->create(m_mim.get_width(bppt), m_mim.get_height());
        texture->setSmooth(false);
        texture->generateMipmap();
        texture->update(reinterpret_cast<const sf::Uint8 *>(colors.data()));
      },
      &(ret->at(pos)),
      bpp,
      palette));
  }
}
void
  map_sprite::find_upscale_path(
    std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret) const
{
  for (const auto &texture_page :
    m_all_unique_values_and_strings.texture_page_id().values())
  {
    const size_t i  = MAX_TEXTURES - 13 + texture_page;
    const auto   fn = [this, texture_page](sf::Texture *texture) -> void
    {
      const auto &root  = m_filters.upscale.value();
      const auto  paths = m_upscales.get_file_paths(root, texture_page);
      auto        filtered_paths = paths
                            | std::views::filter(
                              [](const std::filesystem::path &path)
                              {
                                return std::filesystem::exists(path)
                                       && !std::filesystem::is_directory(path);
                              });
      if (filtered_paths.begin() != filtered_paths.end())
      {
        const auto &path = *(filtered_paths.begin());
        fmt::print("{}\n", path.string());
        texture->loadFromFile(path.string());
        texture->setSmooth(false);
        texture->generateMipmap();
      }
    };
    m_futures.emplace_back(std::async(std::launch::async, fn, &(ret->at(i))));
  }
}
void
  map_sprite::find_upscale_path(
    std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret,
    std::uint8_t                                            palette) const
{
  for (const auto &texture_page :
    m_all_unique_values_and_strings.texture_page_id().values())
  {
    const size_t i  = size_t{ texture_page } * 16U + palette;
    auto const   fn = [texture_page, palette, this](sf::Texture *const texture)
    {
      const auto &root = m_filters.upscale.value();
      const auto paths = m_upscales.get_file_paths(root, texture_page, palette);
      auto       filtered_paths = paths
                            | std::views::filter(
                              [](const std::filesystem::path &path)
                              {
                                return std::filesystem::exists(path)
                                       && !std::filesystem::is_directory(path);
                              });

      if (filtered_paths.begin() != filtered_paths.end())
      {
        const auto &path = *(filtered_paths.begin());
        fmt::print("{}\n", path.string());
        texture->loadFromFile(path.string());
        texture->setSmooth(false);
        texture->generateMipmap();
      }
    };
    m_futures.emplace_back(std::async(std::launch::async, fn, &(ret->at(i))));
  }
}
void
  map_sprite::wait_for_futures() const
{
  std::ranges::for_each(m_futures, [](auto &&f) { f.wait(); });
  m_futures.clear();
}

[[nodiscard]] std::array<sf::Vertex, 4U>
  map_sprite::get_triangle_strip(const sf::Vector2u &draw_size,
    const sf::Vector2u                              &texture_size,
    auto                                             source_x,
    auto                                             source_y,
    auto                                             x,
    auto                                             y) const
{
  constexpr static auto tile_size = 16U;
  auto i = static_cast<std::uint32_t>(x / static_cast<decltype(x)>(tile_size));
  auto j = static_cast<std::uint32_t>(y / static_cast<decltype(y)>(tile_size));
  std::uint32_t tu = static_cast<std::uint32_t>(
    source_x / static_cast<decltype(source_x)>(tile_size));
  std::uint32_t tv = static_cast<std::uint32_t>(
    source_y / static_cast<decltype(source_y)>(tile_size));
  const auto tovec = [](auto &&in_x, auto &&in_y) {
    return sf::Vector2f{ static_cast<float>(in_x), static_cast<float>(in_y) };
  };
  const auto tovert = [&tovec](
                        auto &&in_x, auto &&in_y, auto &&texx, auto &&texy) {
    return sf::Vertex{ tovec(in_x, in_y), tovec(texx, texy) };
  };
  return std::array{ tovert((i + 1) * draw_size.x,
                       j * draw_size.y,
                       (tu + 1) * texture_size.x,
                       tv * texture_size.y),
    tovert(i * draw_size.x,
      j * draw_size.y,
      tu * texture_size.x,
      tv * texture_size.y),
    tovert((i + 1) * draw_size.x,
      (j + 1) * draw_size.y,
      (tu + 1) * texture_size.x,
      (tv + 1) * texture_size.y),
    tovert(i * draw_size.x,
      (j + 1) * draw_size.y,
      tu * texture_size.x,
      (tv + 1) * texture_size.y) };
}

[[nodiscard]] std::array<sf::Vertex, 4U>
  map_sprite::get_triangle_strip(const sf::Vector2u &draw_size,
    const sf::Vector2u                              &texture_size,
    const auto                                      &tile_const,
    auto                                           &&tile) const
{
  using namespace open_viii::graphics::literals;
  using tile_type    = std::decay_t<decltype(tile)>;
  auto       src_tpw = tile_type::texture_page_width(tile_const.depth());
  const auto x       = [this, &tile_const, &src_tpw]() -> std::uint32_t
  {
    if (m_filters.upscale.enabled())
    {
      return 0;
    }
    return tile_const.texture_id() * src_tpw;
  }();
  const auto src_x = tile_const.source_x() + x;
  const auto src_y = tile_const.source_y();

  const auto dst_x = [this, &tile]()
  {
    if (m_draw_swizzle)
    {
      if (!m_disable_texture_page_shift)
        return static_cast<uint32_t>(
          tile.source_x()
          + tile.texture_id() * tile_type::texture_page_width(4_bpp));
      return static_cast<uint32_t>(tile.source_x());
    }
    return static_cast<uint32_t>(tile.x());
  }();
  const auto dst_y = [this, &tile]()
  {
    if (m_draw_swizzle)
    {
      return static_cast<uint32_t>(tile.source_y());
    }
    return static_cast<uint32_t>(tile.y());
  }();
  return get_triangle_strip(
    draw_size, texture_size, src_x, src_y, dst_x, dst_y);
}

void
  set_color(std::array<sf::Vertex, 4U> &vertices, const sf::Color &color)
{
  for (auto &vertex : vertices)
  {
    vertex.color = color;
  }
}

std::uint8_t
  map_sprite::max_x_for_saved() const
{
  static constexpr std::uint8_t tile_size = 16U;
  return m_maps.const_back().visit_tiles(
    [this](const auto &tiles)
    {
      auto transform_range = m_saved_indicies
                             | std::views::transform(
                               [this, &tiles](std::size_t i)
                               {
                                 auto &tile = tiles[i];
                                 if (m_draw_swizzle)
                                 {
                                   if (tile.depth().bpp4())
                                   {
                                     return 256U;
                                   }
                                   if (tile.depth().bpp16())
                                   {
                                     return 64U;
                                   }
                                   return 128U;
                                 }
                                 return std::uint32_t{ tile_size };
                               });

      const auto it =
        std::min_element(transform_range.begin(), transform_range.end());
      if (it != std::end(transform_range))
        return static_cast<std::uint8_t>((*it - tile_size) / tile_size);
      return std::uint8_t(255U);
    });
}

template<typename key_lambdaT, typename weight_lambdaT>
[[maybe_unused]] void
  map_sprite::compact_generic(key_lambdaT &&key_lambda,
    weight_lambdaT                        &&weight_lambda,
    const int                               passes) const
{
  static constexpr auto tile_size = 16U;
  m_maps.copy_back().visit_tiles(
    [&key_lambda, &weight_lambda, &passes, this](auto &&tiles)
    {
      for (int pass = passes; pass != 0;
           --pass)// at least 2 passes needed as things might get shifted to
                  // other texture pages and then the keys are less valuable.
      {
        auto pointers =
          generate_map(tiles, key_lambda, [](auto &&tile) { return &tile; });
        std::uint8_t col        = {};
        std::uint8_t row        = {};
        std::uint8_t page       = {};
        std::size_t  row_weight = {};
        for (auto &[key, tps] : pointers)
        {
          const auto weight = weight_lambda(key, tps);

          if (std::cmp_greater_equal(col, tile_size)
              || std::cmp_greater_equal(row_weight, tile_size)
              || std::cmp_greater(row_weight + weight, tile_size))
          {
            ++row;
            col        = {};
            row_weight = {};
          }

          if (std::cmp_greater_equal(row, tile_size))
          {
            ++page;
            row = {};
          }

          using tileT =
            std::decay_t<typename std::decay_t<decltype(tiles)>::value_type>;
          for (tileT *const tp : tps)
          {
            *tp =
              tp->with_source_xy(
                  static_cast<decltype(tileT{}.source_x())>(col * tile_size),
                  static_cast<decltype(tileT{}.source_y())>(row * tile_size))
                .with_texture_id(
                  static_cast<decltype(tileT{}.texture_id())>(page));
          }

          row_weight += weight;
          ++col;
        }
      }
    });
  update_render_texture();
}

void
  map_sprite::compact() const
{
  compact_generic(
    [](const auto &tile)
    {
      return std::make_tuple(tile.texture_id(),
        tile.source_y(),
        static_cast<std::uint8_t>(3U - (tile.depth().raw() & 3U)),
        tile.source_x(),
        tile.palette_id());
    },
    [](const auto &key, const auto &)
    { return static_cast<std::uint8_t>(1U << (3U - std::get<2>(key))); });
}


void
  map_sprite::compact2() const
{
  compact_generic(
    [](const auto &tile)
    {
      return std::make_tuple(
        static_cast<std::uint8_t>(3U - (tile.depth().raw() & 3U)),
        tile.texture_id(),
        tile.source_y(),
        tile.source_x(),
        tile.palette_id());
    },
    [](const auto &key, const auto &)
    { return static_cast<std::uint8_t>(1U << (3U - std::get<0>(key))); });
}

void
  map_sprite::flatten_bpp() const
{
  m_maps.copy_back().visit_tiles(
    [](auto &tiles)
    {
      std::ranges::transform(tiles,
        tiles.begin(),
        [](const auto tile)
        {
          if (filter_invalid(tile))
          {
            return tile.with_depth(4_bpp);
          }
          return tile;
        });
    });
}
void
  map_sprite::flatten_palette() const
{
  m_maps.copy_back().visit_tiles(
    [](auto &tiles)
    {
      std::ranges::transform(tiles,
        tiles.begin(),
        [](const auto tile)
        {
          if (filter_invalid(tile))
          {
            return tile.with_palette_id(0);
          }
          return tile;
        });
    });
}

std::size_t
  map_sprite::row_empties(std::uint8_t tile_y,
    std::uint8_t                       texture_page,
    const bool                         move_from_row)
{
  return m_maps.const_back().visit_tiles(
    [&tile_y, &texture_page, &move_from_row](const auto &tiles) -> std::size_t
    {
      std::vector<std::pair<std::uint8_t, std::int8_t>> values = {};
      auto                                              filtered_range =
        tiles
        | std::views::filter(
          [&tile_y, &texture_page](const auto &tile) -> bool
          {
            static constexpr auto tile_size = 16U;
            return std::cmp_equal((tile.source_y() / tile_size), tile_y)
                   && std::cmp_equal(texture_page, tile.texture_id());
          });
      std::transform(std::ranges::begin(filtered_range),
        std::ranges::end(filtered_range),
        std::back_inserter(values),
        [](const auto &tile)
        {
          static constexpr auto tile_size = 16U;
          return std::make_pair(
            static_cast<std::uint8_t>(1U << (tile.depth().raw() & 3U)),
            static_cast<std::uint8_t>(tile.source_x() / tile_size));
        });
      std::ranges::sort(values);
      const auto [first, last] = std::ranges::unique(values);
      values.erase(first, last);
      static constexpr auto tile_size = 16U;
      auto                  transform_values =
        values
        | std::views::transform(
          [](const auto &pair) -> std::size_t { return pair.first; });
      std::size_t total =
        tile_size
        - std::reduce(
          transform_values.begin(), transform_values.end(), std::size_t{});
      if (move_from_row)
      {
        total += 1U;
      }
      return total;
    });
}

void
  map_sprite::update_position(const sf::Vector2i &pixel_pos,
    const sf::Vector2i                           &tile_pos,
    const std::uint8_t                           &texture_page)
{
  if (m_saved_indicies.empty())
  {
    return;
  }
  m_maps.copy_back().visit_tiles(
    [this, &tile_pos, &texture_page, &pixel_pos](auto &&tiles)
    {
      static constexpr std::uint8_t tile_size = 16U;
      std::uint8_t                  max_x     = max_x_for_saved() * tile_size;
      if (m_draw_swizzle)
      {
        if (auto intersecting =
              find_intersecting(pixel_pos, tile_pos, texture_page, true);
            !intersecting.empty())
        {
          // this might not be good enough as two 4 bpp tiles fit in the
          // same location as 8 bpp. and two 8 bpp fit in space for 16 bpp.
          // but this should catch obvious problems.
          fmt::print(
            "There are {} tiles at this location. Choose an empty "
            "location!\n",
            intersecting.size());
          return;
        }
        const auto &tile     = tiles[m_saved_indicies.front()];
        bool        same_row = ((tile.source_y() / tile_size) == tile_pos.y)
                        && (texture_page == tile.texture_id());
        //        fmt::print("{} == {} && {} == {}\n",
        //          (tile.source_y() / tile_size),
        //          tile_pos.y,
        //          texture_page,
        //          tile.texture_id());
        const auto empty_count = row_empties(
          static_cast<std::uint8_t>(tile_pos.y), texture_page, same_row);
        fmt::print("Empty cells in row = {}\n", empty_count);
        if (empty_count == 0)
        {
          return;
        }
      }
      for (auto i : m_saved_indicies)
      {
        auto &tile = tiles[i];
        if (m_draw_swizzle)
        {
          if (auto intersecting =
                find_intersecting(pixel_pos, tile_pos, texture_page, true);
              !intersecting.empty())
          {
            if (std::ranges::any_of(intersecting,
                  [&tile, &tiles](const auto &j)
                  {
                    const auto &other_tile = tiles[j];
                    return (tile.depth() != other_tile.depth())
                           || (tile.palette_id() != other_tile.palette_id());
                  }))
            {
              // this second pass is to make sure tiles you are moving aren't
              // conflicting with different bpp or palette.
              continue;
            }
          }
          tile =
            tile
              .with_source_xy(
                (std::min)(
                  static_cast<std::uint8_t>(tile_pos.x * tile_size), max_x),
                static_cast<std::uint8_t>(tile_pos.y * tile_size))
              .with_texture_id(texture_page);
        }
        else
        {
          tile = tile.with_xy(
            static_cast<std::int16_t>((pixel_pos.x / tile_size) * tile_size),
            static_cast<std::int16_t>((pixel_pos.y / tile_size) * tile_size));
        }
      }
    });
  update_render_texture();
}

sf::Sprite
  map_sprite::save_intersecting(const sf::Vector2i &pixel_pos,
    const sf::Vector2i                             &tile_pos,
    const std::uint8_t                             &texture_page)
{
  static constexpr auto tile_size       = 16;
  static constexpr auto tile_size_float = 16.F;
  sf::Sprite            sprite          = {};
  sprite.setTexture(m_render_texture->getTexture());
  sprite.setTextureRect(
    { (pixel_pos.x / static_cast<int>(tile_size)) * static_cast<int>(tile_size)
        * static_cast<int>(m_scale),

      tile_pos.y * static_cast<int>(tile_size) * static_cast<int>(m_scale),
      static_cast<int>(tile_size) * static_cast<int>(m_scale),

      static_cast<int>(tile_size) * static_cast<int>(m_scale) });
  sprite.setPosition(
    static_cast<float>(pixel_pos.x / tile_size) * tile_size_float,
    static_cast<float>(tile_pos.y) * tile_size_float);
  sprite.setScale(
    1.F / static_cast<float>(m_scale), 1.F / static_cast<float>(m_scale));
  m_saved_indicies = find_intersecting(pixel_pos, tile_pos, texture_page);
  return sprite;
}

std::vector<size_t>
  map_sprite::find_intersecting(const sf::Vector2i &pixel_pos,
    const sf::Vector2i                             &tile_pos,
    const std::uint8_t                             &texture_page,
    const bool                                      skip_filters)
{
  return m_maps.const_back().visit_tiles(
    [this, &tile_pos, &texture_page, &pixel_pos, &skip_filters](
      const auto &tiles)
    {
      std::vector<std::size_t> out = {};
      auto                     filtered_tiles =
        tiles
        | std::views::filter(
          [this, &skip_filters, &tile_pos, &texture_page, &pixel_pos](
            const auto &tile) -> bool
          {
            static constexpr auto in_bounds = [](auto i, auto low, auto high) {
              return std::cmp_greater_equal(i, low)
                     && std::cmp_less_equal(i, high);
            };
            static constexpr auto tile_size = 16U;
            if (!skip_filters && fail_filter(tile))
            {
              return false;
            }
            if (m_draw_swizzle)
            {
              if (std::cmp_equal(tile_pos.x, tile.source_x() / tile_size))
              {
                if (std::cmp_equal(tile_pos.y, tile.source_y() / tile_size))
                {
                  if (std::cmp_equal(tile.texture_id(), texture_page))
                  {
                    return true;
                  }
                }
              }
            }
            else if (in_bounds(pixel_pos.x,
                       tile.x(),
                       tile.x() + static_cast<int>(tile_size)))
            {
              if (in_bounds(pixel_pos.y,
                    tile.y(),
                    tile.y() + static_cast<int>(tile_size)))
              {
                return true;
              }
            }
            return false;
          });
      std::transform(std::begin(filtered_tiles),
        std::end(filtered_tiles),
        std::back_inserter(out),
        [&tiles](const auto &tile)
        {
          const auto *const start = tiles.data();
          const auto *const curr  = &tile;
          std::cout << tile << std::endl;
          return static_cast<std::size_t>(std::distance(start, curr));
        });
      fmt::print("\n\tFound {:3}\n", out.size());
      for (const auto &i : out)
      {
        fmt::print("{:4} ", i);
      }
      puts("\n");
      return out;
    });
}

auto
  map_sprite::duel_visitor(auto &&lambda) const
{
  return m_maps.front().visit_tiles(
    [this, &lambda](auto const &tiles_const)
    {
      return m_maps.back().visit_tiles([&lambda, &tiles_const](auto &&tiles)
        { return lambda(tiles_const, std::forward<decltype(tiles)>(tiles)); });
    });
}

void
  map_sprite::for_all_tiles(auto const &tiles_const,
    auto                              &&tiles,
    auto                              &&lambda,
    bool                                skip_invalid,
    bool                                regular_order) const
{
  // todo move pupu generation to constructor
  std::map<std::tuple<PupuID, std::int32_t, std::int32_t>, std::uint8_t>
                      pupu_map{};
  std::vector<PupuID> pupu_ids = {};
  pupu_ids.reserve(std::size(tiles_const));
  const auto get_pupu = [&pupu_map](const auto &tile_const)
  {
    const auto tuple = std::make_tuple(PupuID(tile_const.layer_id(),
                                         tile_const.blend_mode(),
                                         tile_const.animation_id(),
                                         tile_const.animation_state()),
      std::int32_t{ tile_const.x() },
      std::int32_t{ tile_const.y() });

    if (pupu_map.contains(tuple))
    {
      ++(pupu_map.at(tuple));
    }
    else
    {
      pupu_map.emplace(tuple, std::uint8_t{});
    }
    return std::get<0>(tuple) + pupu_map.at(tuple);
  };
  std::ranges::transform(tiles_const, std::back_inserter(pupu_ids), get_pupu);
  assert(std::size(tiles_const) == std::size(tiles));
  assert(std::size(tiles_const) == std::size(pupu_ids));
  const auto process = [this, &skip_invalid, &lambda](
                         auto tc, const auto tce, auto t, auto pupu_t)
  {
    for (; /*t != te &&*/ tc != tce; (void)++tc, ++t, ++pupu_t)
    {
      const is_tile auto &tile_const = *tc;
      if (skip_invalid && !filter_invalid(tile_const))
      {
        continue;
      }
      is_tile auto &tile       = *t;
      const PupuID &pupu_const = *pupu_t;
      lambda(tile_const, tile, pupu_const);
    }
  };
  if (!regular_order)
  {
    process(std::crbegin(tiles_const),
      std::crend(tiles_const),
      std::rbegin(tiles),
      std::rbegin(pupu_ids));
  }
  else
  {
    process(std::cbegin(tiles_const),
      std::cend(tiles_const),
      std::begin(tiles),
      std::begin(pupu_ids));
  }
}

void
  map_sprite::for_all_tiles(auto &&lambda,
    bool                           skip_invalid,
    bool                           regular_order) const
{
  duel_visitor(
    [&lambda, &skip_invalid, &regular_order, this](
      auto const &tiles_const, auto &&tiles)
    {
      for_all_tiles(tiles_const,
        std::forward<decltype(tiles)>(tiles),
        std::forward<decltype(lambda)>(lambda),
        skip_invalid,
        regular_order);
    });
}

[[nodiscard]] bool
  map_sprite::local_draw(sf::RenderTarget &target,
    sf::RenderStates                       states) const
{
  bool drew = false;
  target.clear(sf::Color::Transparent);
  for (const auto &z : m_all_unique_values_and_strings.z().values())
  {
    for_all_tiles(
      [this, &states, &target, &z, &drew](
        [[maybe_unused]] const auto &tile_const,
        const auto                  &tile,
        const PupuID                &pupu_id)
      {
        if (m_filters.pupu.enabled())
        {
          if (m_filters.pupu.value() != pupu_id)
          {
            return;
          }
        }
        if (tile.z() != z)
        {
          return;
        }
        if (fail_filter(tile))
        {
          return;
        }
        if (!filter_invalid(tile_const))
        {
          return;
        }
        states.texture = get_texture(
          tile_const.depth(), tile_const.palette_id(), tile_const.texture_id());
        const auto raw_texture_size = states.texture->getSize();
        const auto i                = raw_texture_size.y / 16U;
        const auto texture_size     = sf::Vector2u{ i, i };
        const auto draw_size = sf::Vector2u{ 16U * m_scale, 16U * m_scale };
        auto       quad =
          get_triangle_strip(draw_size, texture_size, tile_const, tile);
        states.blendMode = sf::BlendAlpha;
        if (!m_disable_blends)
        {
          if (tile.blend_mode() == BlendModeT::add)
          {
            states.blendMode = sf::BlendAdd;
          }
          else if (tile.blend_mode() == BlendModeT::half_add)
          {
            states.blendMode = sf::BlendAdd;
            constexpr static std::uint8_t per50 =
              (std::numeric_limits<std::uint8_t>::max)() / 2U;
            set_color(quad, { per50, per50, per50, per50 });// 50% alpha
          }
          else if (tile.blend_mode() == BlendModeT::quarter_add)
          {
            states.blendMode = sf::BlendAdd;
            constexpr static std::uint8_t per25 =
              (std::numeric_limits<std::uint8_t>::max)() / 4U;
            set_color(quad, { per25, per25, per25, per25 });// 25% alpha
          }
          else if (tile.blend_mode() == BlendModeT::subtract)
          {
            states.blendMode = GetBlendSubtract();
            // states.blendMode = sf::BlendMultiply;
          }
        }
        // apply the tileset texture

        // std::lock_guard<std::mutex> lock(mutex_texture);
        // fmt::print("({}, {})\t", raw_texture_size.x, raw_texture_size.y);
        // draw the vertex array
        target.draw(quad.data(), quad.size(), sf::TriangleStrip, states);
        drew = true;
      });
  }
  return drew;
}

const sf::BlendMode &
  map_sprite::GetBlendSubtract()
{
  const static auto BlendSubtract =
    sf::BlendMode{ sf::BlendMode::DstColor,// or One
      sf::BlendMode::One,
      sf::BlendMode::ReverseSubtract,
      sf::BlendMode::One,
      sf::BlendMode::OneMinusSrcAlpha,
      sf::BlendMode::Add };
  return BlendSubtract;
}

map_sprite
  map_sprite::with_coo(const open_viii::LangT coo) const
{
  return { m_field, coo, m_draw_swizzle, m_filters };
}

map_sprite
  map_sprite::with_field(
    std::shared_ptr<open_viii::archive::FIFLFS<false>> field) const
{
  return { std::move(field), m_coo, m_draw_swizzle, m_filters };
}

void
  map_sprite::enable_draw_swizzle() const
{
  m_draw_swizzle = true;
  init_render_texture();
  m_grid = get_grid();
}

void
  map_sprite::disable_draw_swizzle() const
{
  m_draw_swizzle = false;
  init_render_texture();
  m_grid = get_grid();
}

void
  map_sprite::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  // apply the transform
  states.transform *= getTransform();
  states.texture    = &m_render_texture->getTexture();
  auto texture_size = m_render_texture->getSize();
  auto draw_size    = sf::Vector2u(width(), height());
  auto quad         = get_triangle_strip(draw_size, texture_size, 0, 0, 0, 0);
  // draw the vertex array
  target.draw(quad.data(), quad.size(), sf::TriangleStrip, states);
  // draw grid
  target.draw(m_grid, states);
  // draw texture_page_grid
  if (m_draw_swizzle)
  {
    target.draw(m_texture_page_grid, states);
  }
}
void
  map_sprite::update_render_texture(bool reload_textures) const
{
  if (reload_textures)
  {
    m_texture = get_textures();
    init_render_texture();
    return;
  }
  if (!fail())
  {
    (void)local_draw(*m_render_texture, sf::RenderStates::Default);
    m_render_texture->display();
    m_render_texture->setSmooth(false);
    m_render_texture->generateMipmap();
  }
}
void
  map_sprite::save(const std::filesystem::path &path) const
{
  if (fail())
  {
    return;
  }
  const auto image = m_render_texture->getTexture().copyToImage();
  if (!image.saveToFile(path.string()))
  {
    std::cerr << "failure to save file:" << path << std::endl;
  }
}
bool
  map_sprite::fail() const
{
  static bool once = true;
  using namespace open_viii::graphics::literals;
  if (!m_render_texture)
  {
    if (once)
    {
      std::cout << "m_render_texture is null" << std::endl;
      once = false;
    }
    return true;
  }
  if (!m_texture)
  {
    if (once)
    {
      std::cout << "m_texture is null" << std::endl;
      once = false;
    }
    return true;
  }
  if (m_mim.get_width(4_bpp, false) == 0)
  {
    if (once)
    {
      std::cout << "m_mim width is 0" << std::endl;
      once = false;
    }
    return true;
  }
  if (empty())
  {
    if (once)
    {
      std::cout << "m_map is empty" << std::endl;
      once = false;
    }
    return true;
  }
  once = true;
  return false;
}
void
  map_sprite::map_save(const std::filesystem::path &dest_path) const
{
  bool       coo  = false;
  const auto map  = get_map(nullptr, false, coo);
  const auto path = dest_path.string();

  open_viii::tools::write_buffer(
    [&map](std::ostream &os)
    {
      map.visit_tiles(
        [&os](auto &&tiles)
        {
          for (const auto &tile : tiles)
          {
            //          std::array<char, sizeof(tile)> data{};
            //          std::memcpy(data.data(), &tile, sizeof(tile));
            const auto data =
              std::bit_cast<std::array<char, sizeof(tile)>>(tile);
            os.write(data.data(), data.size());
          }
        });
    },
    path,
    "");
}
std::string
  map_sprite::map_filename()
{
  return std::filesystem::path(m_map_path).filename().string();
}

const map_sprite &
  map_sprite::toggle_grid(bool enable, bool enable_texture_page_grid) const
{
  if (enable)
  {
    m_grid.enable();
  }
  else
  {
    m_grid.disable();
  }

  if (enable_texture_page_grid)
  {
    // std::cout << "enabled: " << m_texture_page_grid.count() << '\n';
    m_texture_page_grid.enable();
  }
  else
  {
    m_texture_page_grid.disable();
  }
  return *this;
}
void
  map_sprite::resize_render_texture() const
{
  if (!fail())
  {
    auto filtered_textures = *m_texture
                             | std::views::filter(
                               [](const auto &texture)
                               {
                                 const auto &size = texture.getSize();
                                 return size.x != 0 && size.y != 0;
                               });
    if (filtered_textures.begin() != filtered_textures.end())
    {
      const auto y        = filtered_textures.begin()->getSize().y;
      m_scale             = y / 256U;
      const auto max_size = sf::Texture::getMaximumSize();
      while (width() * m_scale > max_size || height() * m_scale > max_size)
      {
        m_scale >>= 1U;
        if (m_scale <= 1U)
        {
          m_scale = 1U;
          break;
        }
      }
      // fmt::print(
      //   "{}, ({}, {})\n", m_scale, width() * m_scale, height() * m_scale);
      m_render_texture->create(width() * m_scale, height() * m_scale);
    }
    else
    {
      m_scale = 1;
      // fmt::print("{}, ({}, {})\n", m_scale, width(), height());
      m_render_texture->create(width(), height());
    }
  }
}
void
  map_sprite::init_render_texture() const
{
  if (!m_render_texture)
  {
    m_render_texture = std::make_shared<sf::RenderTexture>();
  }
  resize_render_texture();
  update_render_texture();
}


open_viii::graphics::Rectangle<std::uint32_t>
  map_sprite::get_canvas() const
{
  return static_cast<open_viii::graphics::Rectangle<std::uint32_t>>(
    m_maps.const_back().canvas());
}

std::uint32_t
  map_sprite::width() const
{
  if (m_draw_swizzle)
  {
    using namespace open_viii::graphics::literals;
    return m_mim.get_width(4_bpp);
  }
  return m_canvas.width();
}

std::uint32_t
  map_sprite::height() const
{
  if (m_draw_swizzle)
  {
    return m_mim.get_height();
  }
  return m_canvas.height();
}
grid
  map_sprite::get_grid() const
{
  return { { 16U, 16U }, { width(), height() } };
}
grid
  map_sprite::get_texture_page_grid() const
{
  using namespace open_viii::graphics::literals;
  return { { (1U << (8U - ((4_bpp).raw() & 3U))), 256U },
    { m_mim.get_width(4_bpp), m_mim.get_height() },
    sf::Color::Yellow };
}

bool
  map_sprite::fail_filter(auto &tile) const
{
  using namespace open_viii::graphics::literals;
#if 1
  const auto test = []<typename T>(
                      const ::filter<T> &in_filter, const auto &value) -> bool
  { return (in_filter && value != in_filter); };
  return (m_filters.bpp.value() != 16_bpp
           && test(m_filters.palette, tile.palette_id()))
         || test(m_filters.bpp, tile.depth())
         || test(m_filters.blend_mode, tile.blend_mode())
         || test(m_filters.blend_other, tile.blend())
         || test(m_filters.animation_id, tile.animation_id())
         || test(m_filters.animation_frame, tile.animation_state())
         || test(m_filters.layer_id, tile.layer_id())
         || test(m_filters.texture_page_id, tile.texture_id())
         || test(m_filters.z, tile.z());

  // TODO need to generate pupus for this map so I get the offset.
#else
  if (m_filters.palette.enabled() && m_filters.bpp.value() != 16_bpp)
  {
    if (tile.palette_id() != m_filters.palette.value())
    {
      return true;
    }
  }
  if (m_filters.bpp.enabled())
  {
    if (tile.depth() != m_filters.bpp.value())
    {
      return true;
    }
  }
  if (m_filters.blend_mode.enabled())
  {
    if (tile.blend_mode() != m_filters.blend_mode.value())
    {
      return true;
    }
  }
  if (m_filters.blend_other.enabled())
  {
    if (tile.blend() != m_filters.blend_other.value())
    {
      return true;
    }
  }
  if (m_filters.animation_id.enabled())
  {
    if (tile.animation_id() != m_filters.animation_id.value())
    {
      return true;
    }
  }
  if (m_filters.animation_frame.enabled())
  {
    if (tile.animation_state() != m_filters.animation_frame.value())
    {
      return true;
    }
  }
  if (m_filters.layer_id.enabled())
  {
    if (tile.layer_id() != m_filters.layer_id.value())
    {
      return true;
    }
  }
  if (m_filters.texture_page_id.enabled())
  {
    if (tile.texture_id() != m_filters.texture_page_id.value())
    {
      return true;
    }
  }
  return false;
#endif
}

all_unique_values_and_strings
  map_sprite::get_all_unique_values_and_strings() const
{
  return m_maps.const_back().visit_tiles(
    [](const auto &tiles) { return all_unique_values_and_strings(tiles); });
}

filters &
  map_sprite::filter() const
{
  return m_filters;
}
map_sprite
  map_sprite::update(std::shared_ptr<open_viii::archive::FIFLFS<false>> field,
    open_viii::LangT                                                    coo,
    bool draw_swizzle) const
{
  return { std::move(field), coo, draw_swizzle, m_filters };
}
const all_unique_values_and_strings &
  map_sprite::uniques() const
{
  return m_all_unique_values_and_strings;
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
//
// Each image is a texture page use the scale of the largest
// source_texture_height/256. The texture is square. or you could look at it as
// 16x16 tiles. Though I'd rather keep the same dimensions as the original
// image. If you have overlapping palettes you can use texture page and palette.
// Though for successful drawing this shouldn't really be usable. Unless
// demaster fixes the palette filtering this really isn't 100% helpful though it
// does keep the tiles from being lost.
//
// Add option to Menu Save "Save new textures".
// Pop up a directory picker dialog box. So They can choose a directory to save
// the images too. Pre populated with the 2 letter prefix\fieldname. If path
// doesn't exist create it. create blank textures same dims as existing textures
// height in a square. draw all tiles to that texture that exist with only alpha
// blending no special sauce. With the matching texture page and or palette.
// Save to png with {fieldname}_{texture_page}_{palette}.png or
// {fieldname}_{texture_page}.png Do this till all the textures are created. Add
// detection for overlapping palettes and make sure to place them into other
// files. map - save changes #48 - this should be done at the same time
// especially when map has changed.
template<typename T>
struct setting_backup
{
private:
  const T m_backup;
  T      &m_value;

public:
  const T &
    backup() const
  {
    return m_backup;
  }
  T &
    value() const
  {
    return m_value;
  }
  auto &
    operator=(const T &in_value) const
  {
    m_value = in_value;
    return *this;
  }
  bool
    operator==(const T &in_value) const
  {
    return m_value == in_value;
  }
  setting_backup(T &in_value)
    : m_backup(in_value)
    , m_value(in_value)
  {
  }
  ~setting_backup()
  {
    m_value = m_backup;
  }
};
struct settings_backup
{
public:
  setting_backup<::filters> filters;
  setting_backup<bool>      draw_swizzle;
  setting_backup<bool>      disable_texture_page_shift;
  setting_backup<bool>      disable_blends;
  setting_backup<uint32_t>  scale;

  settings_backup(::filters &in_filters,
    bool                    &in_draw_swizzle,
    bool                    &in_disable_texture_page_shift,
    bool                    &in_disable_blends,
    std::uint32_t           &in_scale)
    : filters(in_filters)
    , draw_swizzle(in_draw_swizzle)
    , disable_texture_page_shift(in_disable_texture_page_shift)
    , disable_blends(in_disable_blends)
    , scale(in_scale)
  {
  }
};
bool
  map_sprite::check_if_one_palette(const std::uint8_t &texture_page) const
{
  return m_maps.const_back().visit_tiles(
    [&texture_page](const auto &tiles) -> bool
    {
      auto filtered_tiles =
        tiles
        | std::views::filter([&texture_page](const auto &tile) -> bool
          { return std::cmp_equal(tile.texture_id(), texture_page); });
      auto [min, max] = std::ranges::minmax_element(
        filtered_tiles, {}, [](const auto &tile) { return tile.palette_id(); });
      if (min != filtered_tiles.end() && max != filtered_tiles.end())
      {
        fmt::print("{}\t{}\n", min->palette_id(), max->palette_id());
        return !std::cmp_equal(min->palette_id(), max->palette_id());
      }
      return false;
    });
}
void
  map_sprite::save_new_textures(const std::filesystem::path &path) const
{
  // assert(std::filesystem::path.is_directory(path));
  const std::string     field_name = { str_to_lower(m_field->get_base_name()) };
  static constexpr char pattern_texture_page[]             = { "{}_{}.png" };
  static constexpr char pattern_texture_page_palette[]     = { "{}_{}_{}.png" };
  static constexpr char pattern_coo_texture_page[]         = { "{}_{}_{}.png" };
  static constexpr char pattern_coo_texture_page_palette[] = {
    "{}_{}_{}_{}.png"
  };

  const auto  unique_values = get_all_unique_values_and_strings();
  const auto &unique_texture_page_ids =
    unique_values.texture_page_id().values();
  const auto           &unique_bpp = unique_values.bpp().values();
  const settings_backup settings(m_filters,
    m_draw_swizzle,
    m_disable_texture_page_shift,
    m_disable_blends,
    m_scale);
  settings.filters                    = {};
  settings.filters.value().upscale    = settings.filters.backup().upscale;
  settings.draw_swizzle               = true;
  settings.disable_texture_page_shift = true;
  settings.disable_blends             = true;
  uint32_t height                     = get_max_texture_height();
  settings.scale                      = height / 256U;
  if (m_scale == 0U)
    m_scale = 1U;
  bool map_test =
    unique_bpp.size() == 1U
    && unique_values.palette().at(unique_bpp.front()).values().size() <= 1U;


  for (const auto &texture_page : unique_texture_page_ids)
  {
    settings.filters.value().texture_page_id.update(texture_page).enable();
    if (!map_test)
    {
      if (check_if_one_palette(texture_page))
      {
        const auto conflicting_palettes =
          get_conflicting_palettes(texture_page);
        for (const auto &bpp : unique_bpp)
        {
          const auto &unique_palette = unique_values.palette().at(bpp).values();
          auto        filter_palette =
            unique_palette
            | std::views::filter(
              [&conflicting_palettes](const std::uint8_t &palette)
              {
                return std::ranges::any_of(conflicting_palettes,
                  [&palette](const std::uint8_t &other)
                  { return palette == other; });
              });
          for (const auto &palette : filter_palette)
          {
            settings.filters.value().palette.update(palette).enable();
            settings.filters.value().bpp.update(bpp).enable();
            auto out_path    = m_using_coo
                                 ? save_path_coo(pattern_coo_texture_page_palette,
                                   path,
                                   field_name,
                                   texture_page,
                                   palette)
                                 : save_path(pattern_texture_page_palette,
                                   path,
                                   field_name,
                                   texture_page,
                                   palette);
            auto out_texture = save_texture(height, height);
            async_save(out_path, out_texture);
          }
        }
      }
    }
    settings.filters.value().palette.disable();
    settings.filters.value().bpp.disable();
    auto out_path =
      m_using_coo
        ? save_path_coo(
          pattern_coo_texture_page, path, field_name, texture_page)
        : save_path(pattern_texture_page, path, field_name, texture_page);
    auto out_texture = save_texture(height, height);
    async_save(out_path, out_texture);
  }
  wait_for_futures();
}
std::vector<std::uint8_t>
  map_sprite::get_conflicting_palettes(const std::uint8_t &texture_page) const
{
  const auto palettes = m_maps.front().visit_tiles(
    [&texture_page, this](const auto &tiles)
    {
      //        using tileT =
      //          std::decay_t<typename
      //          std::decay_t<decltype(tiles)>::value_type>;

      auto map_xy_palette = generate_map(
        tiles,
        [](const auto &tile)
        { return std::make_tuple(tile.source_x(), tile.source_y()); },
        [](const auto &tile) { return tile.palette_id(); },
        [&texture_page](const auto &tile)
        { return std::cmp_equal(texture_page, tile.texture_id()); });
      std::vector<uint8_t> conflict_palettes{};
      for (auto &[xy, palette_vector] : map_xy_palette)
      {
        std::ranges::sort(palette_vector);
        auto [first, last] = std::ranges::unique(palette_vector);
        palette_vector.erase(first, last);
        if (palette_vector.size() <= 1U)
        {
          map_xy_palette.erase(xy);
        }
        else
        {
          conflict_palettes.insert(conflict_palettes.end(),
            palette_vector.begin(),
            palette_vector.end());
        }
      }
      std::ranges::sort(conflict_palettes);
      auto [first, last] = std::ranges::unique(conflict_palettes);
      conflict_palettes.erase(first, last);
      for (auto p : conflict_palettes)
      {
        fmt::print("{}\n", p);
      }
      return conflict_palettes;
    });
  return palettes;
}

void
  map_sprite::save_pupu_textures(const std::filesystem::path &path) const
{
  // assert(std::filesystem::path.is_directory(path));
  const std::string     field_name = { str_to_lower(m_field->get_base_name()) };
  static constexpr char pattern_pupu[]     = { "{}_{}.png" };
  static constexpr char pattern_coo_pupu[] = { "{}_{}_{}.png" };

  const auto &unique_pupu_ids = m_all_unique_values_and_strings.pupu().values();
  const settings_backup settings(m_filters,
    m_draw_swizzle,
    m_disable_texture_page_shift,
    m_disable_blends,
    m_scale);
  settings.filters                    = {};
  settings.filters.value().upscale    = settings.filters.backup().upscale;
  settings.draw_swizzle               = false;
  settings.disable_texture_page_shift = true;
  settings.disable_blends             = true;
  // todo maybe draw with blends enabled to transparent black or white.
  uint32_t tex_height                 = get_max_texture_height();
  settings.scale                      = tex_height / 256U;
  const auto canvas = m_maps.const_back().canvas() * static_cast<int>(m_scale);
  if (settings.scale == 0U)
  {
    settings.scale = 1U;
  }
  for (const PupuID &pupu : unique_pupu_ids)
  {
    settings.filters.value().pupu.update(pupu).enable();
    std::filesystem::path out_path =
      m_using_coo ? save_path_coo(pattern_coo_pupu, path, field_name, pupu)
                  : save_path(pattern_pupu, path, field_name, pupu);
    std::shared_ptr<sf::RenderTexture> out_texture =
      save_texture(static_cast<std::uint32_t>(canvas.width()),
        static_cast<std::uint32_t>(canvas.height()));
    async_save(out_path, out_texture);
  }
  wait_for_futures();
}
void
  map_sprite::async_save(const std::filesystem::path &out_path,
    const std::shared_ptr<sf::RenderTexture>         &out_texture) const
{
  if (out_texture)
  {
    m_futures.emplace_back(std::async(
      std::launch::async,
      [](std::filesystem::path op, auto ot) { ot.saveToFile(op.string()); },
      out_path,
      out_texture->getTexture().copyToImage()));
  }
}
uint32_t
  map_sprite::get_max_texture_height() const
{
  auto transform_range = (*m_texture)
                         | std::views::transform([](const sf::Texture &texture)
                           { return texture.getSize().y; });
  auto max_height_it =
    std::max_element(transform_range.begin(), transform_range.end());
  uint32_t tex_height = {};
  if (max_height_it != transform_range.end())
  {
    tex_height = *max_height_it;
  }
  else
  {
    tex_height = 256U;
  }
  return tex_height;
}
std::filesystem::path
  map_sprite::save_path_coo(
    fmt::format_string<std::string_view, std::string_view, uint8_t> pattern,
    const std::filesystem::path                                    &path,
    const std::string_view                                         &field_name,
    uint8_t texture_page) const
{
  return path
         / fmt::vformat(fmt::string_view(pattern),
           fmt::make_format_args(field_name,
             open_viii::LangCommon::to_string(m_coo),
             texture_page));
}
std::filesystem::path
  map_sprite::save_path_coo(
    fmt::format_string<std::string_view, std::string_view, uint8_t, uint8_t>
                                 pattern,
    const std::filesystem::path &path,
    const std::string_view      &field_name,
    uint8_t                      texture_page,
    uint8_t                      palette) const
{
  return path
         / fmt::vformat(fmt::string_view(pattern),
           fmt::make_format_args(field_name,
             open_viii::LangCommon::to_string(m_coo),
             texture_page,
             palette));
}
std::filesystem::path
  map_sprite::save_path_coo(
    fmt::format_string<std::string_view, std::string_view, PupuID> pattern,
    const std::filesystem::path                                   &path,
    const std::string_view                                        &field_name,
    PupuID                                                         pupu) const
{
  return path
         / fmt::vformat(fmt::string_view(pattern),
           fmt::make_format_args(
             field_name, open_viii::LangCommon::to_string(m_coo), pupu));
}
std::filesystem::path
  map_sprite::save_path(
    fmt::format_string<std::string_view, std::uint8_t> pattern,
    const std::filesystem::path                       &path,
    const std::string_view                            &field_name,
    const std::uint8_t                                 texture_page) const
{
  return path
         / fmt::vformat(fmt::string_view(pattern),
           fmt::make_format_args(field_name, texture_page));
}
std::filesystem::path
  map_sprite::save_path(
    fmt::format_string<std::string_view, std::uint8_t, std::uint8_t> pattern,
    const std::filesystem::path                                     &path,
    const std::string_view                                          &field_name,
    std::uint8_t texture_page,
    std::uint8_t palette) const
{
  return path
         / fmt::vformat(fmt::string_view(pattern),
           fmt::make_format_args(field_name, texture_page, palette));
}
std::filesystem::path
  map_sprite::save_path(fmt::format_string<std::string_view, PupuID> pattern,
    const std::filesystem::path                                     &path,
    const std::string_view                                          &field_name,
    PupuID                                                           pupu) const
{
  return path
         / fmt::vformat(
           fmt::string_view(pattern), fmt::make_format_args(field_name, pupu));
}
// std::filesystem::path
//   map_sprite::save_path(fmt::string_view pattern,
//     const std::filesystem::path         &path,
//     const std::string                   &field_name,
//     std::optional<std::uint8_t>          texture_page,
//     std::optional<std::uint8_t>          palette,
//     std::optional<PupuID>                pupu) const
//{
//   // todo put language code in filename. because of remaster multilanguage
//   // maps.
//   std::string filename = {};
//   if (texture_page.has_value())
//   {
//     if (palette.has_value())
//     {
//       filename =
//         fmt::format(fmt::runtime(pattern), field_name, *texture_page,
//         *palette);
//     }
//     else
//     {
//       filename = fmt::format(fmt::runtime(pattern), field_name,
//       *texture_page);
//     }
//   }
//   else if (pupu.has_value())
//   {
//     filename = fmt::format(fmt::runtime(pattern), field_name, *pupu);
//   }
//   return path / filename;
// }

std::shared_ptr<sf::RenderTexture>
  map_sprite::save_texture(std::uint32_t width, std::uint32_t height) const
{
  auto texture = std::make_shared<sf::RenderTexture>();
  texture->create(width, height);
  if (local_draw(*texture, sf::RenderStates::Default))
  {
    texture->display();
    texture->setSmooth(false);
    texture->generateMipmap();
    return texture;
  }
  return nullptr;
}
void
  map_sprite::load_map(const std::filesystem::path &src_path) const
{
  const auto filesize  = std::filesystem::file_size(src_path);
  const auto tilesize  = m_maps.const_back().visit_tiles([](auto &&tiles)
     { return sizeof(typename std::decay_t<decltype(tiles)>::value_type); });
  const auto tilecount = m_maps.front().visit_tiles(
    [](const auto &tiles) { return std::size(tiles); });
  assert(std::cmp_equal(tilecount, filesize / tilesize));
  if (!std::cmp_equal(tilecount, filesize / tilesize))
  {
    fmt::print("Error wrong size map file, {} != {} / {}\n",
      tilecount,
      filesize,
      tilesize);
    return;
  }
  const auto path = src_path.string();
  open_viii::tools::read_from_file(
    [this](std::istream &os)
    {
      (void)m_maps.copy_back();
      for_all_tiles(
        [this, &os](const auto &, auto &tile, const auto &)
        {
          const auto append = [this, &os](auto &t)
          {
            // load tile
            std::array<char, sizeof(t)> data = {};
            os.read(data.data(), data.size());
            t = std::bit_cast<std::decay_t<decltype(t)>>(data);

            // shift to origin
            t = t.shift_xy(m_maps.const_back().offset().abs());
          };
          //          if (!filter_invalid(tile_const))
          //          {
          //            return;
          //          }
          append(tile);
          // write from tiles.
        },
        false,
        true);
      (void)m_maps.copy_back_to_front();
    },
    path);
  update_render_texture();
}
void
  map_sprite::save_modified_map(const std::filesystem::path &dest_path) const
{
  const auto path = dest_path.string();
  open_viii::tools::write_buffer(
    [this](std::ostream &os)
    {
      for_all_tiles(
        [this, &os](const auto &tile_const, const auto &tile, const auto &)
        {
          const auto append = [this, &os](auto t)
          {
            // shift to original offset
            t               = t.shift_xy(m_maps.const_back().offset());
            // save tile
            const auto data = std::bit_cast<std::array<char, sizeof(t)>>(t);
            os.write(data.data(), data.size());
          };
          if (!filter_invalid(tile_const))
          {// write from tiles const
            append(tile_const);
            return;
          }
          append(tile);
          // write from tiles.
        },
        false,
        true);
    },
    path,
    "");
}
