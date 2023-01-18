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
void mouse_positions::update_sprite_pos([[maybe_unused]]bool swizzle, [[maybe_unused]]float spacing)
{
  float x = {};
  float y = {};
//  if (swizzle)
//  {
////    x = ((std::min)(static_cast<std::uint8_t>(pixel.x/16), max_tile_x) * 16.F)
////        + (texture_page * spacing);
//    x = static_cast<float>(pixel.x)+ (texture_page * spacing);
//  }
//  else
  {
    x = static_cast<float>(pixel.x);
  }
//  if (swizzle)
//  {
////    y = (static_cast<float>(pixel.y / 16)) * 16.F;
//    y = static_cast<float>(pixel.y);
//  }
//  else
  {
    y = static_cast<float>(pixel.y);
  }
  sprite.setPosition(x-24.F, y-24.F);
}