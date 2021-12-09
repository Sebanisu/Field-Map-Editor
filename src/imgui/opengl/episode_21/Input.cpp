//
// Created by pcvii on 12/2/2021.
//

#include "Input.hpp"
#include <type_traits>
template<typename T>
static constexpr auto
  operator+(T e) noexcept
  -> std::enable_if_t<std::is_enum<T>::value, std::underlying_type_t<T>>
{
  return static_cast<std::underlying_type_t<T>>(e);
}
bool
  Input::IsKeyPressed(KEY keycode)
{
  if (!m_window)
    return false;
  const int state = glfwGetKey(m_window, +keycode);
  return state == GLFW_PRESS || state == GLFW_REPEAT;
}
bool
  Input::IsMouseButtonPressed(MOUSE button)
{

  if (!m_window)
    return false;
  const int state = glfwGetMouseButton(m_window, +button);
  return state == GLFW_PRESS;
}
std::array<float, 2U>
  Input::GetMousePosition()
{
  std::array<double, 2U> pos{};
  if (!m_window)
    return {};
  glfwGetCursorPos(m_window, &pos[0], &pos[1]);
  return { static_cast<float>(pos[0]), static_cast<float>(pos[1]) };
}
float
  Input::GetMouseX()
{
  const auto [x, y] = GetMousePosition();
  return x;
}
float
  Input::GetMouseY()
{
  const auto [x, y] = GetMousePosition();
  return y;
}
