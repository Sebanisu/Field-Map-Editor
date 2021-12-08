//
// Created by pcvii on 12/2/2021.
//

#ifndef MYPROJECT_MOUSEBUTTONCODES_HPP
#define MYPROJECT_MOUSEBUTTONCODES_HPP
#include <utility>
enum class MOUSE : int
{
  BUTTON_1      = 0,
  BUTTON_2      = 1,
  BUTTON_3      = 2,
  BUTTON_4      = 3,
  BUTTON_5      = 4,
  BUTTON_6      = 5,
  BUTTON_7      = 6,
  BUTTON_8      = 7,
  BUTTON_LAST   = BUTTON_8,
  BUTTON_LEFT   = BUTTON_1,
  BUTTON_RIGHT  = BUTTON_2,
  BUTTON_MIDDLE = BUTTON_3,
};
static constexpr auto
  operator+(MOUSE e) noexcept
  -> std::enable_if_t<std::is_enum<MOUSE>::value, std::underlying_type_t<MOUSE>>
{
  return static_cast<std::underlying_type_t<MOUSE>>(e);
}
#endif// MYPROJECT_MOUSEBUTTONCODES_HPP
