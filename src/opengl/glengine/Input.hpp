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


  static void SetViewPortFocused() noexcept
  {
    s_viewport_focused = true;
  }
  static void SetViewPortNotFocused() noexcept
  {
    s_viewport_focused = false;
  }
  static bool ViewPortFocused() noexcept
  {
    return s_viewport_focused;
  }

  static void SetViewPortHovered() noexcept
  {
    s_viewport_hovered = true;
  }
  static void SetViewPortNotHovered() noexcept
  {
    s_viewport_hovered = false;
  }
  static bool ViewPortHovered() noexcept
  {
    return s_viewport_hovered;
  }


private:
  Input()                                                = default;
  inline constinit static GLFWwindow *m_window           = nullptr;
  inline constinit static bool        s_viewport_focused = true;
  inline constinit static bool        s_viewport_hovered = false;
};
}// namespace glengine
#endif// MYPROJECT_INPUT_HPP
