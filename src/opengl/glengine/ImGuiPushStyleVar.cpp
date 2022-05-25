//
// Created by pcvii on 5/25/2022.
//

#include "ImGuiPushStyleVar.hpp"

void(*const pop_function)()  = []() { ImGui::PopStyleVar(); };
glengine::scope_guard
  glengine::ImGuiPushStyleVar(ImGuiStyleVar style, float val) noexcept
{
  ImGui::PushStyleVar(style, val);
  return { pop_function };
}
glengine::scope_guard
  glengine::ImGuiPushStyleVar(ImGuiStyleVar style, const ImVec2 &val) noexcept
{
  ImGui::PushStyleVar(style, val);
  return { pop_function };
}
