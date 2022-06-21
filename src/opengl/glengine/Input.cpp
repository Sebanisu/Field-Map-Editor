//
// Created by pcvii on 12/2/2021.
//

#include "Input.hpp"

namespace glengine
{
template<typename T>
static constexpr auto operator+(T e) noexcept
  -> std::enable_if_t<std::is_enum<T>::value, std::underlying_type_t<T>>
{
  return static_cast<std::underlying_type_t<T>>(e);
}
bool Input::is_key_pressed(Key keycode)
{
  if (!m_window)
    return false;
  const int state = glfwGetKey(m_window, +keycode);
  return state == GLFW_PRESS || state == GLFW_REPEAT;
}
bool Input::is_mouse_button_pressed(Mouse button)
{
  if (!m_window)
    return false;
  const int state = glfwGetMouseButton(m_window, +button);
  return state == GLFW_PRESS;
}
std::optional<const std::array<float, 2U>> Input::get_mouse_position()
{
  if (!m_window)
    return std::nullopt;
  std::array<double, 2U> pos{};
  glfwGetCursorPos(m_window, &pos[0], &pos[1]);
  return std::array{ static_cast<float>(pos[0]), static_cast<float>(pos[1]) };
}
std::optional<const float> Input::get_mouse_x()
{
  const auto pos = get_mouse_position();
  if (!pos)
    return std::nullopt;
  const auto [x, y] = *pos;
  return x;
}
std::optional<const float> Input::get_mouse_y()
{
  const auto pos = get_mouse_position();
  if (!pos)
    return std::nullopt;
  const auto [x, y] = *pos;
  return y;
}
}// namespace glengine
