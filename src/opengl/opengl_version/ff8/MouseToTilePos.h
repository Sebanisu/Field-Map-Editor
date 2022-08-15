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
    return static_cast<std::int16_t>(
             static_cast<float>(val) / map_dims_statics::TileSize)
           * static_cast<std::int16_t>(map_dims_statics::TileSize);
  }

public:
  template<typename TileFunctions>
  MouseToTilePos(
    const glm::vec2               offset_mouse_pos,
    const MapDims<TileFunctions> &m_map_dims)
  {
    static constexpr bool has_texture_page = std::is_same_v<
      typename TileFunctions::TexturePage,
      tile_operations::TextureId>;
    texture_page = [&]() -> std::uint8_t {
      if constexpr (has_texture_page)
      {
        return static_cast<std::uint8_t>(
          (m_map_dims.offset.x - offset_mouse_pos.x / m_map_dims.tile_scale)
          / map_dims_statics::TexturePageWidth);
      }
      else
      {
        return {};
      }
    }();
    const int texture_page_offset =
      texture_page * map_dims_statics::TexturePageWidth;
    //(x+texture_page*texture_page_width-offset_x)*tile_scale
    x = static_cast<std::int16_t>(
      (m_map_dims.offset.x - offset_mouse_pos.x / m_map_dims.tile_scale
       - static_cast<float>(texture_page_offset)));
    y = static_cast<std::int16_t>(
      (offset_mouse_pos.y / m_map_dims.tile_scale + m_map_dims.offset.y
       + map_dims_statics::TileSize));
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
  std::uint8_t  texture_page = {};
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MOUSETOTILEPOS_H
