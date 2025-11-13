//
// Created by pcvii on 5/25/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIPUSHSTYLEVAR_HPP
#define FIELD_MAP_EDITOR_IMGUIPUSHSTYLEVAR_HPP
#include <glengine/ScopeGuard.hpp>
#include <imgui.h>
namespace imgui_utils
{

[[maybe_unused]] [[nodiscard]] inline auto ImGuiPushStyleVar(
  ImGuiStyleVar style,
  float         val) noexcept
{
     ImGui::PushStyleVar(style, val);
     return glengine::ScopeGuard{ []() { ImGui::PopStyleVar(); } };
}
[[maybe_unused]] [[nodiscard]] inline auto ImGuiPushStyleVar(
  ImGuiStyleVar style,
  const ImVec2 &val) noexcept
{
     ImGui::PushStyleVar(style, val);
     return glengine::ScopeGuard{ []() { ImGui::PopStyleVar(); } };
}

}// namespace imgui_utils
#endif// FIELD_MAP_EDITOR_IMGUIPUSHSTYLEVAR_HPP
