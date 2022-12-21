//
// Created by pcvii on 12/21/2022.
//
#include <spdlog/spdlog.h>
#include "mouse_positions.hpp"
void mouse_positions::update()
{
  old_left    = left;
  mouse_moved = false;
}
bool mouse_positions::left_changed() const
{
  const auto condition = old_left != left;
  if (!mouse_enabled && condition)
  {
    spdlog::trace("Warning! mouse up off screen!");
  }
  return condition;
}
void mouse_positions::update_sprite_pos(bool swizzle, float spacing)
{
  float x = {};
  if (swizzle && max_tile_x != 0U)
  {
    x = ((std::min)(static_cast<std::uint8_t>(tile.x), max_tile_x) * 16.F)
        + (texture_page * spacing);
  }
  else
  {
    x = (static_cast<float>(pixel.x / 16)) * 16.F;
  }
  float y = (static_cast<float>(pixel.y / 16)) * 16.F;
  sprite.setPosition(x, y);
}