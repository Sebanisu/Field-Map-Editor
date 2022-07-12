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

public:
  template<typename TileFunctions>
  MouseToTilePos(
    const glm::vec2               offset_mouse_pos,
    const MapDims<TileFunctions> &m_map_dims)
  {
    static constexpr bool has_texture_page = std::is_same_v<
      typename TileFunctions::TexturePage,
      tile_operations::TextureId>;
    texture_page = [&]() -> int {
      if constexpr (has_texture_page)
      {
        return static_cast<int>(
          (m_map_dims.offset.x - offset_mouse_pos.x / m_map_dims.tile_scale)
          / map_dims_statics::TexturePageWidth);
      }
      else
      {
        return 0;
      }
    }();
    const int texture_page_offset =
      texture_page * map_dims_statics::TexturePageWidth;
    //(x+texture_page*texture_page_width-offset_x)*tile_scale
    x = static_cast<int>(
          (m_map_dims.offset.x - offset_mouse_pos.x / m_map_dims.tile_scale
           - static_cast<float>(texture_page_offset))
          / map_dims_statics::TileSize)
        * static_cast<int>(map_dims_statics::TileSize);
    y = static_cast<int>(
          (offset_mouse_pos.y / m_map_dims.tile_scale + m_map_dims.offset.y
           + map_dims_statics::TileSize)
          / map_dims_statics::TileSize)
        * static_cast<int>(map_dims_statics::TileSize);
  }
  operator glm::ivec3() const noexcept
  {
    return glm::ivec3{ x, y, texture_page };
  }
  int x            = {};
  int y            = {};
  int texture_page = {};
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MOUSETOTILEPOS_H
