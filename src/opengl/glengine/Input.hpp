//
// Created by pcvii on 12/2/2021.
//

#ifndef FIELD_MAP_EDITOR_INPUT_HPP
#define FIELD_MAP_EDITOR_INPUT_HPP
#include "KeyCodes.hpp"
#include "MouseButtonCodes.hpp"
#include <GLFW/glfw3.h>
#include <optional>
namespace glengine
{
class Window;
class Input
{
   public:
     static bool                                       is_key_pressed(Key keycode);
     static bool                                       is_mouse_button_pressed(Mouse button);
     static std::optional<const std::array<float, 2U>> get_mouse_position();
     static std::optional<const float>                 get_mouse_x();
     static std::optional<const float>                 get_mouse_y();
     friend Window;

   private:
     Input()                                      = default;
     inline constinit static GLFWwindow *m_window = nullptr;
};
}// namespace glengine
#endif// FIELD_MAP_EDITOR_INPUT_HPP
