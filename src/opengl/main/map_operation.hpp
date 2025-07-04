//
// Created by pcvii on 2/28/2023.
//

#ifndef FIELD_MAP_EDITOR_MAP_OPERATION_HPP
#define FIELD_MAP_EDITOR_MAP_OPERATION_HPP
#include "filter.hpp"
#include "map_group.hpp"
#include "source_tile_conflicts.hpp"
#include <glm/glm.hpp>
namespace ff_8
{
struct QuadStrip
{
     glm::vec2 uv_min;
     glm::vec2 uv_max;
     glm::vec2 draw_pos;
};
void flatten_bpp(map_group::Map &map);
void flatten_palette(map_group::Map &map);
void compact_move_conflicts_only(map_group::Map &map, const source_tile_conflicts &conflicts);
void compact_map_order(map_group::Map &map);
void compact_map_order_ffnx(map_group::Map &map);
void compact_rows(map_group::Map &map);
void compact_all(map_group::Map &map);
bool test_if_map_same(const std::filesystem::path &saved_path, const map_group::WeakField &field, const map_group::MimType &type);
void save_modified_map(
  const std::filesystem::path &dest_path,
  const map_group::Map        &map_const,
  const map_group::Map        &map_changed,
  const map_group::Map *const  imported = nullptr);


/**
 * @brief Computes a triangle strip (quad) with correct UV coordinates and draw position
 *        for rendering a tile from a texture atlas to the screen.
 *
 * This function converts tile-based coordinates into normalized texture coordinates (UVs)
 * and calculates the on-screen draw position. It assumes both source and destination
 * positions are in map tile units (e.g., TILE_SIZE = 16x16).
 *
 * @param source_tile_size      Size of one tile in the texture (in pixels).
 * @param destination_tile_size Size to render one tile on screen (in pixels).
 * @param source_texture_size   Size of the full source texture (in pixels).
 * @param source_position       Position in the source texture in TILE_SIZE units.
 * @param destination_position  Position to draw the tile on screen in TILE_SIZE units.
 *
 * @return QuadStrip A structure containing the UV minimum/maximum and draw position.
 */
[[nodiscard]] QuadStrip get_triangle_strip(
  const glm::vec2 &source_tile_size,
  const glm::vec2 &destination_tile_size,
  const glm::vec2 &source_texture_size,
  const glm::vec2 &source_position,
  const glm::vec2 &destination_position);

[[nodiscard]] static inline glm::ivec2 get_triangle_strip_source_imported(const open_viii::graphics::background::is_tile auto &tile_const)
{
     return { tile_const.x(), tile_const.y() };
}
[[nodiscard]] static inline glm::uvec2 get_triangle_strip_source_upscale(const open_viii::graphics::background::is_tile auto &tile_const)
{
     return { tile_const.source_x(), tile_const.source_y() };
}
[[nodiscard]] static inline glm::uvec2 get_triangle_strip_source_default(const open_viii::graphics::background::is_tile auto &tile_const)
{
     using tile_type             = std::remove_cvref_t<decltype(tile_const)>;
     auto                src_tpw = tile_type::texture_page_width(tile_const.depth());
     const std::uint32_t x_shift = tile_const.texture_id() * src_tpw;
     return { tile_const.source_x() + x_shift, tile_const.source_y() };
};
[[nodiscard]] static inline glm::ivec2 get_triangle_strip_source_deswizzle(const open_viii::graphics::background::is_tile auto &tile_const)
{
     return { tile_const.x(), tile_const.y() };
}
[[nodiscard]] static inline glm::ivec2 get_triangle_strip_dest_default(open_viii::graphics::background::is_tile auto &&tile)
{
     return { tile.x(), tile.y() };
}
[[nodiscard]] static inline glm::uvec2 get_triangle_strip_dest_swizzle_disable_shift(open_viii::graphics::background::is_tile auto &&tile)
{
     return { tile.source_x(), tile.source_y() };
}
[[nodiscard]] static inline glm::uvec2 get_triangle_strip_dest_swizzle(open_viii::graphics::background::is_tile auto &&tile)
{
     using namespace open_viii::graphics::literals;
     using tile_type = std::remove_cvref_t<decltype(tile)>;
     return { static_cast<std::uint32_t>(tile.source_x() + tile.texture_id() * tile_type::texture_page_width(4_bpp)), tile.source_y() };
}
struct source_x_y_texture_page
{
     glm::ivec2   source_xy    = {};
     std::uint8_t texture_page = {};
};
[[nodiscard]] static inline source_x_y_texture_page
  get_triangle_strip_dest_horizontal_tile_index_swizzle(const std::integral auto &tile_index, const std::integral auto &size)
{
     static const int TILE_SIZE          = 16;
     static const int TEXTURE_PAGE_WIDTH = 256;
     const int        tiles_per_row = (std::max)((static_cast<int>(size) / TILE_SIZE) + (static_cast<int>(size) % TILE_SIZE == 0 ? 0 : 1),
                                          static_cast<int>(TILE_SIZE));

     const auto       x             = (static_cast<int>(tile_index) % tiles_per_row) * TILE_SIZE;
     const auto       y             = (static_cast<int>(tile_index) / tiles_per_row) * TILE_SIZE;
     const auto       tp            = x / TEXTURE_PAGE_WIDTH;


     return { .source_xy = { x - tp * TEXTURE_PAGE_WIDTH, y }, .texture_page = static_cast<std::uint8_t>(tp) };
}

[[nodiscard]] std::vector<std::size_t> find_intersecting_swizzle(
  const map_group::Map &map,
  const ff_8::filters  &filters,
  const glm::ivec2     &pixel_pos,
  const std::uint8_t   &texture_page,
  bool                  skip_filters,
  bool                  find_all);
[[nodiscard]] std::vector<std::size_t> find_intersecting_deswizzle(
  const map_group::Map &map,
  const ff_8::filters  &filters,
  const glm::ivec2     &pixel_pos,
  bool                  skip_filters,
  bool                  find_all);
// templates
template<std::integral input_t, std::integral low_t, std::integral high_t>
static inline bool find_intersecting_in_bounds(input_t input, low_t low, high_t high)
{
     return std::cmp_greater_equal(input, low) && std::cmp_less(input, high);
}

template<std::ranges::range range_t, std::ranges::range out_t, std::ranges::contiguous_range tiles_t>
static inline void find_intersecting_get_indices(range_t &&range, out_t &out, const tiles_t &tiles)
{
     std::ranges::transform(range, std::back_inserter(out), [&tiles](const auto &tile) {
          const auto *const start = tiles.data();
          const auto *const curr  = &tile;
          // format_tile_text(tile, [](std::string_view name, const auto &value) { spdlog::info("tile {}: {}", name, value); });
          return static_cast<std::size_t>(std::distance(start, curr));
     });
}
template<std::ranges::range tilesT>
[[nodiscard]] static inline std::vector<std::size_t> find_intersecting_swizzle(
  const tilesT        &tiles,
  const ff_8::filters &filters,
  const glm::ivec2    &pixel_pos,
  const std::uint8_t  &texture_page,
  bool                 skip_filters = false,
  bool                 find_all     = false)
{

     using namespace open_viii::graphics::background;
     auto                                                 filtered     = tiles | Map::filter_view_invalid();
     std::vector<std::size_t>                             out          = {};
     static constexpr std::vector<std::size_t>::size_type new_capacity = { 30 };
     out.reserve(new_capacity);
     auto filtered_tiles =
       filtered | std::views::filter([&](const auto &tile) -> bool {
            if (!skip_filters && ff_8::tile_operations::fail_any_filters(filters, tile))
            {
                 return false;
            }
            if (std::cmp_equal(tile.texture_id(), texture_page))
            {
                 static constexpr int max_texture_page_dim = 256;
                 if (find_intersecting_in_bounds(
                       pixel_pos.x % max_texture_page_dim, tile.source_x(), tile.source_x() + static_cast<int>(ff_8::map_group::TILE_SIZE)))
                 {
                      if (find_intersecting_in_bounds(
                            pixel_pos.y % max_texture_page_dim,
                            tile.source_y(),
                            tile.source_y() + static_cast<int>(ff_8::map_group::TILE_SIZE)))
                      {
                           return true;
                      }
                 }
            }
            return false;
       });
     if (!find_all)
     {
          // If palette and bpp are overlapping it causes problems.
          //  This prevents you selecting more than one at a time.
          //  min depth/bpp was chosen because lower bpp can be greater src x.
          const auto min_depth   = (std::ranges::min_element)(filtered_tiles, {}, [](const auto &tile) { return tile.depth(); });
          // min palette well, lower bpp tend to be a lower palette id I think.
          const auto min_palette = (std::ranges::min_element)(filtered_tiles, {}, [](const auto &tile) { return tile.palette_id(); });
          auto       filtered_tiles_with_depth_and_palette =
            filtered_tiles | std::views::filter([&](const auto &tile) -> bool {
                 return min_depth->depth() == tile.depth() && min_palette->palette_id() == tile.palette_id();
            });
          find_intersecting_get_indices(filtered_tiles_with_depth_and_palette, out, tiles);
     }
     else
     {
          find_intersecting_get_indices(filtered_tiles, out, tiles);
     }
     return out;
}
template<std::ranges::range tilesT>
[[nodiscard]] static inline std::vector<std::size_t> find_intersecting_deswizzle(
  const tilesT        &tiles,
  const ff_8::filters &filters,
  const glm::ivec2    &pixel_pos,
  bool                 skip_filters = false,
  bool                 find_all     = false)
{
     using namespace open_viii::graphics::background;
     auto                                                 filtered     = tiles | Map::filter_view_invalid();
     std::vector<std::size_t>                             out          = {};
     static constexpr std::vector<std::size_t>::size_type new_capacity = { 30 };
     out.reserve(new_capacity);
     auto filtered_tiles =
       filtered | std::views::filter([&](const auto &tile) -> bool {
            if (!skip_filters && ff_8::tile_operations::fail_any_filters(filters, tile))
            {
                 return false;
            }
            if (find_intersecting_in_bounds(pixel_pos.x, tile.x(), tile.x() + static_cast<int>(ff_8::map_group::TILE_SIZE)))
            {
                 if (find_intersecting_in_bounds(pixel_pos.y, tile.y(), tile.y() + static_cast<int>(ff_8::map_group::TILE_SIZE)))
                 {
                      return true;
                 }
            }
            return false;
       });


     if (!find_all)
     {
          find_intersecting_get_indices(filtered_tiles | std::views::take(1), out, tiles);
     }
     else
     {
          find_intersecting_get_indices(filtered_tiles, out, tiles);
     }
     return out;
}
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAP_OPERATION_HPP
