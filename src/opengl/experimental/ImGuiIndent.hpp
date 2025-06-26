//
// Created by pcvii on 1/25/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIINDENT_HPP
#define FIELD_MAP_EDITOR_IMGUIINDENT_HPP
#include "ScopeGuard.hpp"
#include <imgui.h>
namespace glengine
{
[[nodiscard]] inline auto ImGuiIndent(float width = 0.F) noexcept

{
     ImGui::Indent(width);
     return glengine::ScopeGuard{ [=]() { ImGui::Unindent(width); } };
}
}// namespace glengine
#endif// FIELD_MAP_EDITOR_IMGUIINDENT_HPP
