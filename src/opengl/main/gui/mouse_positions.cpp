//
// Created by pcvii on 12/21/2022.
//
#include "mouse_positions.hpp"
#include <spdlog/spdlog.h>
namespace fme
{
void MousePositions::update()
{
     old_left    = left;
     mouse_moved = false;
}
bool MousePositions::left_changed() const
{
     const auto condition = old_left != left;
     if (!mouse_enabled && condition)
     {
          spdlog::trace("Warning! mouse up off screen!");
     }
     return condition;
}
void MousePositions::update_sprite_pos([[maybe_unused]] bool swizzle, [[maybe_unused]] int spacing)
{
     static constexpr std::uint8_t TILE_SIZE      = 16;
     static constexpr auto         TEXTURE_OFFSET = static_cast<std::uint8_t>(TILE_SIZE * 1.5);
     if (swizzle && max_tile_x > 0U)// snap to grid.
     {
          const auto x_offset = down_pixel.x % TILE_SIZE;
          pixel.x =
            (std::min)(((pixel.x % spacing) / TILE_SIZE) * TILE_SIZE, static_cast<int>(max_tile_x)) + x_offset + (texture_page * spacing);

          const auto y_offset = down_pixel.y % TILE_SIZE;
          pixel.y             = ((pixel.y / TILE_SIZE) * TILE_SIZE) + y_offset;
     }
     // sprite.setPosition(
     //   static_cast<float>(pixel.x - TEXTURE_OFFSET),
     //   static_cast<float>(pixel.y - TEXTURE_OFFSET));
}
}