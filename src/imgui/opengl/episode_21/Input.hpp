//
// Created by pcvii on 12/2/2021.
//

#ifndef MYPROJECT_INPUT_HPP
#define MYPROJECT_INPUT_HPP
#include "KeyCodes.hpp"
#include "MouseButtonCodes.hpp"
#include <array>
#include <GLFW/glfw3.h>
#include <memory>

class Input
{
public:
  Input(std::shared_ptr<GLFWwindow> window)
    : m_window(std::move(window))
  {
  }
  bool
    IsKeyPressed(KEY keycode);
  bool
    IsMouseButtonPressed(MOUSE button);
  std::array<float, 2U>
    GetMousePosition();
  float
    GetMouseX();

  float
    GetMouseY();

private:
  std::shared_ptr<GLFWwindow> m_window{};
};
#endif// MYPROJECT_INPUT_HPP
