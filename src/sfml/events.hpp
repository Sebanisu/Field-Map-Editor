//
// Created by pcvii on 9/6/2021.
//

#ifndef FIELD_MAP_EDITOR_EVENTS_HPP
#define FIELD_MAP_EDITOR_EVENTS_HPP
#include <SFML/Window/Event.hpp>
#include <utility>
#include <variant>

namespace events
{
template<typename... B>
[[nodiscard]] inline auto make_visitor(B &&...b) noexcept
{
     struct visitor : public std::remove_cvref_t<B>...
     {
          using B::operator()...;
     };
     return visitor{ std::forward<B>(b)... };
}
using sf_event_variant = std::variant<
  std::monostate,
  sf::Event::SizeEvent,
  sf::Event::TextEvent,
  sf::Event::KeyEvent,
  sf::Event::MouseWheelEvent,
  sf::Event::MouseWheelScrollEvent,
  sf::Event::MouseButtonEvent,
  sf::Event::MouseMoveEvent,
  sf::Event::JoystickButtonEvent,
  sf::Event::JoystickMoveEvent,
  sf::Event::JoystickConnectEvent,
  sf::Event::TouchEvent,
  sf::Event::SensorEvent>;
[[nodiscard]] sf_event_variant get(const sf::Event &event) noexcept;
}// namespace events
#endif// FIELD_MAP_EDITOR_EVENTS_HPP
