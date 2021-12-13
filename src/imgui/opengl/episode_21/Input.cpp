//
// Created by pcvii on 12/2/2021.
//

#include "Input.hpp"

template<typename T>
static constexpr auto operator+(T e) noexcept
  -> std::enable_if_t<std::is_enum<T>::value, std::underlying_type_t<T>>
{
  return static_cast<std::underlying_type_t<T>>(e);
}
bool Input::IsKeyPressed(KEY keycode)
{
  if (!m_window || ImGui::GetIO().WantCaptureKeyboard)
    return false;
  const int state = glfwGetKey(m_window, +keycode);
  return state == GLFW_PRESS || state == GLFW_REPEAT;
}
bool Input::IsMouseButtonPressed(MOUSE button)
{

  if (!m_window || ImGui::GetIO().WantCaptureMouse)
    return false;
  const int state = glfwGetMouseButton(m_window, +button);
  return state == GLFW_PRESS;
}
std::optional<const std::array<float, 2U>> Input::GetMousePosition()
{
  if (!m_window || ImGui::GetIO().WantCaptureMouse)
    return std::nullopt;
  std::array<double, 2U> pos{};
  glfwGetCursorPos(m_window, &pos[0], &pos[1]);
  return std::array{ static_cast<float>(pos[0]), static_cast<float>(pos[1]) };
}
std::optional<const float> Input::GetMouseX()
{
  const auto pos = GetMousePosition();
  if (!pos)
    return std::nullopt;
  const auto [x, y] = *pos;
  return x;
}
std::optional<const float> Input::GetMouseY()
{
  const auto pos = GetMousePosition();
  if (!pos)
    return std::nullopt;
  const auto [x, y] = *pos;
  return y;
}
