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
static constexpr auto not_invalid = tile_operations::NotInvalidTile{};
void                  flatten_bpp(map_group::Map &map)
{
     map.visit_tiles([](auto &tiles) {
          std::ranges::transform(tiles, tiles.begin(), [](const auto tile) {
               if (std::invoke(not_invalid, tile))
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
               if (std::invoke(not_invalid, tile))
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
          auto filtered_tiles             = tiles | std::views::filter(not_invalid);
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
          if (!not_invalid(tile))
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
bool test_if_map_same(const std::filesystem::path &saved_path, const map_group::SharedField &field, const map_group::MimType &type)
{
     bool return_value = false;
     if (!field)
     {
          return return_value;
     }
     const auto raw_map   = map_group::Map{ type, field->get_entry_data({ saved_path.filename().string() }), false };
     auto       saved_map = map_group::Map{ type, open_viii::tools::read_entire_file(saved_path), false };

     raw_map.visit_tiles([&](const auto &raw_tiles) {
          saved_map.visit_tiles([&](const auto &saved_tiles) {
               if constexpr (std::is_same_v<std::remove_cvref_t<decltype(raw_tiles)>, std::remove_cvref_t<decltype(saved_tiles)>>)
               {
                    return_value = std::ranges::size(raw_tiles) == std::ranges::size(saved_tiles);
                    if (!return_value)
                    {
                         spdlog::warn(
                           "maps are different, raw_tiles_size({}) != saved_tiles_size({}).",
                           std::ranges::size(raw_tiles),
                           std::ranges::size(saved_tiles));
                         return;
                    }
                    std::vector<bool> pairs_dont_match{};
                    std::ranges::transform(
                      raw_tiles, saved_tiles, std::back_inserter(pairs_dont_match), [](const auto &raw_tile, const auto &saved_tile) {
                           return raw_tile != saved_tile;
                      });
                    pairs_dont_match.erase(std::remove(pairs_dont_match.begin(), pairs_dont_match.end(), false), pairs_dont_match.end());
                    return_value = std::ranges::empty(pairs_dont_match);
                    if (!return_value)
                    {
                         spdlog::warn(
                           "maps are different, count {} different tiles, total {} tiles",
                           std::ranges::size(pairs_dont_match),
                           std::ranges::size(raw_tiles));
                    }
                    else
                    {
                         spdlog::info("maps are the same, total {} tiles.", std::ranges::size(raw_tiles));
                    }
               }
          });
     });
     return return_value;
}
void save_modified_map(
  const std::filesystem::path &dest_path,
  const map_group::Map        &map_const,
  const map_group::Map        &map_changed,
  const map_group::Map *const  imported)
{
     const auto path = dest_path.string();
     spdlog::info("Saving modified map: {}", path);
     open_viii::tools::write_buffer(
       [&](std::ostream &os) {
            bool       used_imports = false;
            const auto append       = [&](auto tile) {
                 // shift to original offset
                 if (not_invalid(tile))
                 {
                      tile = tile.shift_xy(map_changed.offset());
                 }
                 // save tile
                 const auto data = std::bit_cast<std::array<char, sizeof(tile)>>(tile);
                 os.write(data.data(), data.size());
            };
            const auto append_imported_tiles = [&]() {
                 if (imported && !used_imports)
                 {
                      used_imports = true;
                      imported->visit_tiles([&append](const auto &import_tiles) {
                           spdlog::info("Saving imported tiles {} count", std::ranges::size(import_tiles));
                           for (const auto &import_tile : import_tiles)
                           {
                                if (not_invalid(import_tile))
                                {
                                     append(import_tile);
                                }
                           }
                      });
                 }
            };
            append_imported_tiles();
            map_const.visit_tiles([&](const auto &tiles_const) {
                 map_changed.visit_tiles([&](const auto &tiles_changed) {
                      auto it_const    = tiles_const.cbegin();
                      auto it_changed  = tiles_changed.cbegin();
                      auto end_const   = tiles_const.cend();
                      auto end_changed = tiles_changed.cend();
                      for (; it_const != end_const && it_changed != end_changed; (void)++it_const, ++it_changed)
                      {
                           const auto &tile_const    = *it_const;
                           const auto &tile          = *it_changed;
                           bool const  valid_const   = not_invalid(tile_const);
                           bool const  valid_changed = not_invalid(tile);
                           if (valid_const || valid_changed)// one of these is valid.
                           {
                                if (valid_changed)
                                {
                                     append(tile);
                                }
                                else
                                {
                                     append(tile_const);
                                }
                                continue;
                           }
                           append(tile);
                           // write from tiles.
                      }
                 });
            });
       },
       path,
       "");
}
}// namespace ff_8