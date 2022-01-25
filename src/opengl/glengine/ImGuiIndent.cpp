//
// Created by pcvii on 1/25/2022.
//

#include "ImGuiIndent.hpp"
glengine::scope_guard_expensive glengine::ImGuiIndent(float width) noexcept
{
  ImGui::Indent(width);
  return { [=]() { ImGui::Unindent(width); } };
}
