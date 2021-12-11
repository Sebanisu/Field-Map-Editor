//
// Created by pcvii on 12/2/2021.
//

#ifndef MYPROJECT_INPUT_HPP
#define MYPROJECT_INPUT_HPP
#include "KeyCodes.hpp"
#include "MouseButtonCodes.hpp"


class Window;
class Input
{
public:
  static bool                  IsKeyPressed(KEY keycode);
  static bool                  IsMouseButtonPressed(MOUSE button);
  static std::array<float, 2U> GetMousePosition();
  static float                 GetMouseX();
  static float                 GetMouseY();
  friend Window;

private:
  Input()                            = default;
  inline static GLFWwindow *m_window = nullptr;
};
#endif// MYPROJECT_INPUT_HPP
