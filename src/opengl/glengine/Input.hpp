//
// Created by pcvii on 12/2/2021.
//

#ifndef MYPROJECT_INPUT_HPP
#define MYPROJECT_INPUT_HPP
#include "KeyCodes.hpp"
#include "MouseButtonCodes.hpp"
namespace glengine
{
class Window;
class Input
{
public:
  static bool IsKeyPressed(KEY keycode);
  static bool IsMouseButtonPressed(MOUSE button);
  static std::optional<const std::array<float, 2U>> GetMousePosition();
  static std::optional<const float>                 GetMouseX();
  static std::optional<const float>                 GetMouseY();
  friend Window;

private:
  Input()                                      = default;
  inline constinit static GLFWwindow *m_window = nullptr;
};
}// namespace glengine
#endif// MYPROJECT_INPUT_HPP
