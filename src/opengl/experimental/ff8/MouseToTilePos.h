//
// Created by pcvii on 7/12/2022.
//

#ifndef FIELD_MAP_EDITOR_MOUSETOTILEPOS_H
#define FIELD_MAP_EDITOR_MOUSETOTILEPOS_H
#include "MapDims.hpp"
#include "tile_operations.hpp"
namespace ff_8
{
class MouseToTilePos
{
   private:
     [[maybe_unused]] constexpr static std::int16_t lock_to_grid(std::int16_t val)
     {
          return static_cast<std::int16_t>(static_cast<float>(val) / map_dims_statics::TileSize)
                 * static_cast<std::int16_t>(map_dims_statics::TileSize);
     }

   public:
     explicit MouseToTilePos(const glm::ivec3 input)
       : x(static_cast<std::int16_t>(input.x))
       , y(static_cast<std::int16_t>(input.y))
       , texture_page(static_cast<std::uint8_t>(input.z))
     {
     }
     template<typename TileFunctions>
     MouseToTilePos(
       const glm::vec2               offset_mouse_pos,
       const MapDims<TileFunctions> &m_map_dims)
     {
          static constexpr bool has_texture_page = std::is_same_v<typename TileFunctions::TexturePage, tile_operations::TextureId>;
          texture_page                           = [&]() -> std::uint8_t
          {
               if constexpr (has_texture_page)
               {
                    return static_cast<std::uint8_t>(
                      (m_map_dims.offset.x - offset_mouse_pos.x / m_map_dims.tile_scale) / map_dims_statics::TexturePageWidth);
               }
               else
               {
                    return {};
               }
          }();
          const int texture_page_offset = texture_page * map_dims_statics::TexturePageWidth;
          //(x+texture_page*texture_page_width-offset_x)*tile_scale
          x                             = static_cast<std::int16_t>(
            (m_map_dims.offset.x - offset_mouse_pos.x / m_map_dims.tile_scale - static_cast<float>(texture_page_offset)));
          y = static_cast<std::int16_t>((offset_mouse_pos.y / m_map_dims.tile_scale + m_map_dims.offset.y + map_dims_statics::TileSize));
          if constexpr (has_texture_page)
          {
               x = lock_to_grid(x);
               y = lock_to_grid(y);
          }
     }
     operator glm::ivec3() const noexcept
     {
          return glm::ivec3{ x, y, texture_page };
     }
     std::int16_t x            = {};
     std::int16_t y            = {};
     std::uint8_t texture_page = {};
};
template<typename TileFunctions, typename FiltersT>
class MouseTileOverlap
{
   private:
     static constexpr bool cmp_overlap(
       const std::integral auto tilepos,
       const std::integral auto otherpos) noexcept
     {
          auto const size = static_cast<decltype(tilepos)>(map_dims_statics::TileSize);
          return std::cmp_greater_equal(otherpos, tilepos) && std::cmp_less(otherpos, tilepos + size);
     };
     MouseToTilePos m_compare_value;
     FiltersT      &m_filters;

   public:
     MouseTileOverlap(
       MouseToTilePos compare_value,
       FiltersT      &filters)
       : m_compare_value(std::move(compare_value))
       , m_filters(filters)
     {
     }
     bool operator()(const auto &tile) const noexcept
     {
          typename TileFunctions::X           x_f{};
          typename TileFunctions::Y           y_f{};
          typename TileFunctions::TexturePage texture_page_f{};

          return m_filters(tile) && cmp_overlap(x_f(tile), m_compare_value.x) && cmp_overlap(y_f(tile), m_compare_value.y)
                 && std::cmp_equal(texture_page_f(tile), m_compare_value.texture_page);
     }
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MOUSETOTILEPOS_H
