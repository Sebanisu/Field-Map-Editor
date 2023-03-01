//
// Created by pcvii on 2/28/2023.
//

#include "map_operation.hpp"
#include "formatters.hpp"
#include "tile_operations.hpp"
#include <algorithm>
#include <functional>
#include <ranges>
namespace ff_8
{
static constexpr auto invalid = tile_operations::InvalidTile{};
void                  flatten_bpp(map_group::Map &map)
{
     map.visit_tiles([](auto &tiles) {
          std::ranges::transform(tiles, tiles.begin(), [](const auto tile) {
               if (std::invoke(invalid, tile))
               {
                    using namespace open_viii::graphics::literals;
                    return tile.with_depth(4_bpp);
               }
               return tile;
          });
     });
}
void flatten_palette(map_group::Map &map)
{
     map.visit_tiles([](auto &tiles) {
          std::ranges::transform(tiles, tiles.begin(), [](const auto tile) {
               if (std::invoke(invalid, tile))
               {
                    return tile.with_palette_id(0);
               }
               return tile;
          });
     });
}
void compact_map_order(map_group::Map &map)
{
     map.visit_tiles([](auto &&tiles) {
          auto filtered_tiles             = tiles | std::views::filter(invalid);
          using tile_t                    = std::remove_cvref_t<std::ranges::range_value_t<decltype(tiles)>>;
          const auto with_depth_operation = ff_8::tile_operations::WithDepth<tile_t>{ open_viii::graphics::BPPT::BPP4_CONST() };
          for (std::size_t tile_index = {}; tile_t & tile : filtered_tiles)
          {
               const auto texture_page    = static_cast<ff_8::tile_operations::TextureIdT<tile_t>>(tile_index / 256);
               const auto file_tile_index = tile_index % 256;
               const auto source_x        = static_cast<ff_8::tile_operations::SourceXT<tile_t>>(
                 (file_tile_index % ff_8::map_group::TILE_SIZE) * ff_8::map_group::TILE_SIZE);
               const auto source_y = static_cast<ff_8::tile_operations::SourceYT<tile_t>>(
                 (file_tile_index / ff_8::map_group::TILE_SIZE) * ff_8::map_group::TILE_SIZE);
               ++tile_index;
               const auto with_texture_id_operation = ff_8::tile_operations::WithTextureId<tile_t>{ texture_page };
               const auto with_source_xy_operation  = ff_8::tile_operations::WithSourceXY<tile_t>{ { source_x, source_y } };
               tile                                 = tile | with_depth_operation | with_source_xy_operation | with_texture_id_operation;
          }
     });
}
static constexpr auto default_filter_lambda = [](auto &&) { return true; };
template<typename tilesT, typename key_lambdaT, typename value_lambdaT, typename filterT = decltype(default_filter_lambda)>
static auto generate_map(tilesT &&tiles, key_lambdaT &&key_lambda, value_lambdaT &&value_lambda, filterT &&filter = {})
{
     using tile_t  = std::remove_cvref_t<typename std::remove_cvref_t<tilesT>::value_type>;
     using key_t   = decltype(key_lambda(tile_t{}));
     using value_t = decltype(value_lambda(tile_t{}));
     std::map<key_t, std::vector<value_t>> map{};
     auto                                  filtered_tiles = tiles | std::views::filter(filter);
     std::ranges::for_each(filtered_tiles, [&map, &key_lambda, &value_lambda](auto &&tile) {
          if (!invalid(tile))
          {
               return;
          }
          value_t value = value_lambda(tile);
          key_t   key   = key_lambda(tile);
          if (map.contains(key))
          {
               map.at(key).push_back(value);
          }
          else
          {
               map.emplace(key, std::vector<value_t>{ value });
          }
     });
     return map;
}

template<typename key_lambdaT, typename weight_lambdaT>
[[maybe_unused]] static void compact_generic(map_group::Map &map, key_lambdaT &&key_lambda, weight_lambdaT &&weight_lambda, int passes = 2)
{
     map.visit_tiles([&key_lambda, &weight_lambda, &passes](auto &&tiles) {
          for (int pass = passes; pass != 0; --pass)// at least 2 passes needed as things might get shifted to
                                                    // other texture pages and then the keys are less valuable.
          {
               auto         pointers   = generate_map(tiles, key_lambda, [](auto &&tile) { return &tile; });
               std::uint8_t col        = {};
               std::uint8_t row        = {};
               std::uint8_t page       = {};
               std::size_t  row_weight = {};
               for (auto &[key, tps] : pointers)
               {
                    const auto weight = weight_lambda(key, tps);

                    if (
                      std::cmp_greater_equal(col, ff_8::map_group::TILE_SIZE)
                      || std::cmp_greater_equal(row_weight, ff_8::map_group::TILE_SIZE)
                      || std::cmp_greater(row_weight + weight, ff_8::map_group::TILE_SIZE))
                    {
                         ++row;
                         col        = {};
                         row_weight = {};
                    }

                    if (std::cmp_greater_equal(row, ff_8::map_group::TILE_SIZE))
                    {
                         ++page;
                         row = {};
                    }

                    using tile_t = std::remove_cvref_t<typename std::remove_cvref_t<decltype(tiles)>::value_type>;
                    for (tile_t *const tp : tps)
                    {
                         *tp = tp->with_source_xy(
                                   static_cast<decltype(tile_t{}.source_x())>(col * ff_8::map_group::TILE_SIZE),
                                   static_cast<decltype(tile_t{}.source_y())>(row * ff_8::map_group::TILE_SIZE))
                                 .with_texture_id(static_cast<decltype(tile_t{}.texture_id())>(page));
                    }

                    row_weight += weight;
                    ++col;
               }
          }
     });
}

void compact_rows(map_group::Map &map)
{
     compact_generic(
       map,
       [](const auto &tile) {
            return std::make_tuple(
              tile.texture_id(),
              tile.source_y(),
              static_cast<std::uint8_t>(3U - (tile.depth().raw() & 3U)),
              tile.source_x(),
              tile.palette_id());
       },
       [](const auto &key, const auto &) { return static_cast<std::uint8_t>(1U << (3U - std::get<2>(key))); });
}
void compact_all(map_group::Map &map)
{
     compact_generic(
       map,
       [](const auto &tile) {
            return std::make_tuple(
              static_cast<std::uint8_t>(3U - (tile.depth().raw() & 3U)),
              tile.texture_id(),
              tile.source_y(),
              tile.source_x(),
              tile.palette_id());
       },
       [](const auto &key, const auto &) { return static_cast<std::uint8_t>(1U << (3U - std::get<0>(key))); });
}


[[nodiscard]] std::vector<std::size_t> find_intersecting_swizzle(
  const map_group::Map &map,
  const ff_8::filters  &filters,
  const sf::Vector2i   &pixel_pos,
  const std::uint8_t   &texture_page,
  bool                  skip_filters,
  bool                  find_all)
{
     return map.visit_tiles(
       [&](const auto &tiles) { return find_intersecting_swizzle(tiles, filters, pixel_pos, texture_page, skip_filters, find_all); });
}
[[nodiscard]] std::vector<std::size_t> find_intersecting_deswizzle(
  const map_group::Map &map,
  const ff_8::filters  &filters,
  const sf::Vector2i   &pixel_pos,
  bool                  skip_filters,
  bool                  find_all)
{
     return map.visit_tiles(
       [&](const auto &tiles) { return find_intersecting_deswizzle(tiles, filters, pixel_pos, skip_filters, find_all); });
}
std::array<sf::Vertex, 4U>
  get_triangle_strip(const sf::Vector2f &draw_size, const sf::Vector2f &texture_size, sf::Vector2f source, sf::Vector2f dest)

{
     constexpr static auto tile_size_f = static_cast<float>(map_group::TILE_SIZE);
     source /= tile_size_f;
     dest /= tile_size_f;
     const auto tovec  = [](auto &&in_x, auto &&in_y) { return sf::Vector2f{ static_cast<float>(in_x), static_cast<float>(in_y) }; };
     const auto tovert = [&tovec](auto &&draw_x, auto &&draw_y, auto &&texture_x, auto &&texture_y) {
          return sf::Vertex{ tovec(draw_x, draw_y), tovec(texture_x, texture_y) };
     };
     return std::array{
          tovert((dest.x + 1.F) * draw_size.x, dest.y * draw_size.y, (source.x + 1.F) * texture_size.x, source.y * texture_size.y),
          tovert(dest.x * draw_size.x, dest.y * draw_size.y, source.x * texture_size.x, source.y * texture_size.y),
          tovert(
            (dest.x + 1.F) * draw_size.x,
            (dest.y + 1.F) * draw_size.y,
            (source.x + 1.F) * texture_size.x,
            (source.y + 1.F) * texture_size.y),
          tovert(dest.x * draw_size.x, (dest.y + 1.F) * draw_size.y, source.x * texture_size.x, (source.y + 1.F) * texture_size.y)
     };
}
}// namespace ff_8