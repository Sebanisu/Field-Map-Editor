//
// Created by pcvii on 12/2/2021.
//

#ifndef FIELD_MAP_EDITOR_KEYCODES_HPP
#define FIELD_MAP_EDITOR_KEYCODES_HPP
namespace glengine
{
// from glfw3.h
enum class Mods : int
{
  None     = 0x0000,
  Shift    = 0x0001,
  Control  = 0x0002,
  Alt      = 0x0004,
  Super    = 0x0008,
  CapsLock = 0x0010,
  NumLock  = 0x0020
};
enum class Key : int
{

  /* The unknown key */
  Unknown      = -1,

  /* Printable keys */
  Space        = 32,
  Apostrophe   = 39, /* ' */
  Comma        = 44, /* , */
  Minus        = 45, /* - */
  Period       = 46, /* . */
  Slash        = 47, /* / */
  Zero         = 48,
  One          = 49,
  Two          = 50,
  Three        = 51,
  Four         = 52,
  Five         = 53,
  Six          = 54,
  Seven        = 55,
  Eight        = 56,
  Nine         = 57,
  N1           = 49,
  N2           = 50,
  N3           = 51,
  N4           = 52,
  N5           = 53,
  N6           = 54,
  N7           = 55,
  N8           = 56,
  N9           = 57,
  Semicolon    = 59, /* ; */

  Equal        = 61, /* = */

  A            = 65,
  B            = 66,
  C            = 67,
  D            = 68,
  E            = 69,
  F            = 70,
  G            = 71,
  H            = 72,
  I            = 73,
  J            = 74,
  K            = 75,
  L            = 76,
  M            = 77,
  N            = 78,
  O            = 79,
  P            = 80,
  Q            = 81,
  R            = 82,
  S            = 83,
  T            = 84,
  U            = 85,
  V            = 86,
  W            = 87,
  X            = 88,
  Y            = 89,
  Z            = 90,
  LeftBracket  = 91, /* [ */

  Backslash    = 92, /* \ */

  RightBracket = 93, /* ] */

  GraveAccent  = 96, /* ` */

  World1       = 161, /* non-US #1 */

  World2       = 162, /* non-US #2 */

  /* Function keys */

  Escape       = 256,
  Enter        = 257,
  Tab          = 258,
  Backspace    = 259,
  Insert       = 260,
  Delete       = 261,
  Right        = 262,
  Left         = 263,
  Down         = 264,
  Up           = 265,
  PageUp       = 266,
  PageDown     = 267,
  Home         = 268,
  End          = 269,
  CapsLock     = 280,
  ScrollLock   = 281,
  NumLock      = 282,
  PrintScreen  = 283,
  Pause        = 284,
  F1           = 290,
  F2           = 291,
  F3           = 292,
  F4           = 293,
  F5           = 294,
  F6           = 295,
  F7           = 296,
  F8           = 297,
  F9           = 298,
  F10          = 299,
  F11          = 300,
  F12          = 301,
  F13          = 302,
  F14          = 303,
  F15          = 304,
  F16          = 305,
  F17          = 306,
  F18          = 307,
  F19          = 308,
  F20          = 309,
  F21          = 310,
  F22          = 311,
  F23          = 312,
  F24          = 313,
  F25          = 314,
  Kp0          = 320,
  Kp1          = 321,
  Kp2          = 322,
  Kp3          = 323,
  Kp4          = 324,
  Kp5          = 325,
  Kp6          = 326,
  Kp7          = 327,
  Kp8          = 328,
  Kp9          = 329,
  KpDecimal    = 330,
  KpDivide     = 331,
  KpMultiply   = 332,
  KpSubtract   = 333,
  KpAdd        = 334,
  KpEnter      = 335,
  KpEqual      = 336,
  LeftShift    = 340,
  LeftControl  = 341,
  LeftAlt      = 342,
  LeftSuper    = 343,
  RightShift   = 344,
  RightControl = 345,
  RightAlt     = 346,
  RightSuper   = 347,
  Menu         = 348,

  Last         = Menu,
  Release      = 0,
  Press        = 1,
  Repeat       = 2,
};
static constexpr auto operator+(Key e) noexcept
  -> std::enable_if_t<std::is_enum<Key>::value, std::underlying_type_t<Key>>
{
  return static_cast<std::underlying_type_t<Key>>(e);
}
static constexpr auto operator+(Mods e) noexcept
  -> std::enable_if_t<std::is_enum<Mods>::value, std::underlying_type_t<Mods>>
{
  return static_cast<std::underlying_type_t<Mods>>(e);
}
}// namespace glengine
#endif// FIELD_MAP_EDITOR_KEYCODES_HPP
