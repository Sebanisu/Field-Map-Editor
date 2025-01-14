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
// template<typename... B>
//[[nodiscard]] inline auto make_scope_guard(B &&...b) noexcept
//{
//   struct scope_gaurd : std::remove_cvref_t<B>...
//   {// disreguard rule of 5.
//     //    scope_gaurd(B &&...t) : B(std::forward<B>(t))...{}
//     //    scope_gaurd()                        = delete;
//     //    scope_gaurd(const scope_gaurd &)     = delete;
//     //    scope_gaurd(scope_gaurd &&) noexcept = delete;
//     //    auto &operator=(const scope_gaurd &) = delete;
//     //    auto &operator=(scope_gaurd &&) noexcept = delete;
//     ~scope_gaurd() { ((void)B::operator()(), ...); }
//   };
//   return scope_gaurd{ std::forward<B>(b)... };
// }
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
