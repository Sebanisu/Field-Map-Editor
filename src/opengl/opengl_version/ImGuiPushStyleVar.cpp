//
// Created by pcvii on 5/25/2022.
//

#include "ImGuiPushStyleVar.hpp"

void (*const PopFunction)() = []() { ImGui::PopStyleVar(); };
glengine::ScopeGuard glengine::ImGuiPushStyleVar(ImGuiStyleVar style, float val) noexcept
{
     ImGui::PushStyleVar(style, val);
     return { PopFunction };
}
glengine::ScopeGuard glengine::ImGuiPushStyleVar(ImGuiStyleVar style, const ImVec2 &val) noexcept
{
     ImGui::PushStyleVar(style, val);
     return { PopFunction };
}
