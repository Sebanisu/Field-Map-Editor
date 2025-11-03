//
// Created by pcvii on 1/25/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIDISABLED_HPP
#define FIELD_MAP_EDITOR_IMGUIDISABLED_HPP
#include <glengine/ScopeGuard.hpp>
#include <imgui.h>
namespace glengine
{
[[nodiscard]] inline auto ImGuiDisabled(bool disabled) noexcept
{

     ImGui::BeginDisabled(disabled);
     return ScopeGuard{ &ImGui::EndDisabled };
}
}// namespace glengine
#endif// FIELD_MAP_EDITOR_IMGUIDISABLED_HPP
