//
// Created by pcvii on 5/25/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIPUSHSTYLEVAR_HPP
#define FIELD_MAP_EDITOR_IMGUIPUSHSTYLEVAR_HPP
#include "ScopeGuard.hpp"
#include <imgui.h>
namespace glengine
{
[[maybe_unused]] [[maybe_unused]] [[nodiscard]] ScopeGuard ImGuiPushStyleVar(ImGuiStyleVar style, float val) noexcept;
[[maybe_unused]] [[nodiscard]] ScopeGuard                  ImGuiPushStyleVar(ImGuiStyleVar style, const ImVec2 &val) noexcept;

}// namespace glengine
#endif// FIELD_MAP_EDITOR_IMGUIPUSHSTYLEVAR_HPP
