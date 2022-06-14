//
// Created by pcvii on 5/25/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIPUSHSTYLEVAR_HPP
#define FIELD_MAP_EDITOR_IMGUIPUSHSTYLEVAR_HPP
#include "scope_guard.hpp"
#include <imgui.h>
namespace glengine
{
[[maybe_unused]] [[nodiscard]] scope_guard
  ImGuiPushStyleVar(ImGuiStyleVar style, float val) noexcept;
[[maybe_unused]] [[nodiscard]] scope_guard
  ImGuiPushStyleVar(ImGuiStyleVar style, const ImVec2 &val) noexcept;

}// namespace glengine
#endif// FIELD_MAP_EDITOR_IMGUIPUSHSTYLEVAR_HPP
