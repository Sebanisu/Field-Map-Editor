//
// Created by pcvii on 1/25/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIPUSHITEMWIDTH_HPP
#define FIELD_MAP_EDITOR_IMGUIPUSHITEMWIDTH_HPP
#include "ScopeGuard.hpp"
#include <imgui.h>
namespace glengine
{
[[nodiscard]] inline auto ImGuiPushItemWidth(float item_width) noexcept
{
     ImGui::PushItemWidth(item_width);
     return ScopeGuard{ &ImGui::PopItemWidth };
}
}// namespace glengine
#endif// FIELD_MAP_EDITOR_IMGUIPUSHITEMWIDTH_HPP
