//
// Created by pcvii on 12/21/2022.
//

#ifndef FIELD_MAP_EDITOR_SCROLLING_HPP
#define FIELD_MAP_EDITOR_SCROLLING_HPP
#include <array>
#include <SFML/Graphics.hpp>
struct scrolling
{
  std::array<float, 2U> total_scroll_time = { 1000.F, 1000.F };
  bool                  left{};
  bool                  right{};
  bool                  up{};
  bool                  down{};
  void                  reset() noexcept;
  bool scroll(std::array<float, 2U> &in_xy, const sf::Time &time);
};
#endif// FIELD_MAP_EDITOR_SCROLLING_HPP
