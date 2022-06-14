//
// Created by pcvii on 12/7/2021.
//

#ifndef FIELD_MAP_EDITOR_EVENTENUMS_HPP
#define FIELD_MAP_EDITOR_EVENTENUMS_HPP
#include <cstdint>
namespace glengine
{
namespace Event
{
  // enum class Type : std::uint32_t
  //{
  //   None,
  //   WindowClose,
  //   WindowResize,
  //   WindowFocus,
  //   WindowLostFocus,
  //   WindowMoved,
  //   KeyPressed,
  //   KeyReleased,
  //   MouseButtonPressed,
  //   MouseButtonReleased,
  //   MouseMoved,
  //   MouseScrolled
  // };
  enum class Category : std::uint16_t
  {
    None        = 0U,
    Application = 1U << 0U,
    Input       = 1U << 1U,
    Keyboard    = 1U << 2U,
    Mouse       = 1U << 3U,
    MouseButton = 1U << 4U,
  };

  constexpr std::uint16_t operator+(Category ec)
  {
    return static_cast<std::uint16_t>(ec);
  }
  constexpr Category operator|(Category l, Category r)
  {
    return static_cast<Category>(+l | +r);
  }
  constexpr Category operator&(Category l, Category r)
  {
    return static_cast<Category>(+l & +r);
  }

  constexpr bool HasFlag(Category l, Category r)
  {
    return (l & r) != Category::None;
  }
}// namespace Event
}// namespace glengine
#endif// FIELD_MAP_EDITOR_EVENTENUMS_HPP
