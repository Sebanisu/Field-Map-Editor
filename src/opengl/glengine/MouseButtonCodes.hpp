//
// Created by pcvii on 12/2/2021.
//

#ifndef FIELD_MAP_EDITOR_MOUSEBUTTONCODES_HPP
#define FIELD_MAP_EDITOR_MOUSEBUTTONCODES_HPP
namespace glengine
{
enum class Mouse : int
{
  Button1      = 0,
  Button2      = 1,
  Button3      = 2,
  Button4      = 3,
  Button5      = 4,
  Button6      = 5,
  Button7      = 6,
  Button8      = 7,
  ButtonLast   = Button8,
  ButtonLeft   = Button1,
  ButtonRight  = Button2,
  ButtonMiddle = Button3,
  Release      = 0,
  Press        = 1
};
static constexpr auto operator+(Mouse e) noexcept
  -> std::enable_if_t<std::is_enum<Mouse>::value, std::underlying_type_t<Mouse>>
{
  return static_cast<std::underlying_type_t<Mouse>>(e);
}
}// namespace glengine
#endif// FIELD_MAP_EDITOR_MOUSEBUTTONCODES_HPP
