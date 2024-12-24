//
// Created by pcvii on 12/21/2022.
//

#ifndef FIELD_MAP_EDITOR_MOUSE_POSITIONS_HPP
#define FIELD_MAP_EDITOR_MOUSE_POSITIONS_HPP
#include <SFML/Graphics.hpp>
#include <cstdint>
struct mouse_positions
{
  sf::Vector2i       pixel         = {};
  sf::Vector2i       down_pixel    = {};
  std::uint8_t       texture_page  = {};
  bool               left          = { false };
  bool               mouse_enabled = { false };
  bool               mouse_moved   = { false };
  sf::Sprite         sprite        = {};
  std::uint8_t       max_tile_x    = {};
  // sf::Sprite   cover         = {};
  void               update();
  [[nodiscard]] bool left_changed() const;
  void               update_sprite_pos(bool swizzle, int spacing = 256);

private:
  bool old_left = { false };
};
#endif// FIELD_MAP_EDITOR_MOUSE_POSITIONS_HPP
