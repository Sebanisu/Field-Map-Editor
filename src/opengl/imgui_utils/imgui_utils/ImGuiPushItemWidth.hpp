//
// Created by pcvii on 1/25/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIPUSHITEMWIDTH_HPP
#define FIELD_MAP_EDITOR_IMGUIPUSHITEMWIDTH_HPP
#include <glengine/ScopeGuard.hpp>
#include <imgui.h>
namespace imgui_utils
{
[[nodiscard]] inline auto ImGuiPushItemWidth(float item_width) noexcept
{
     ImGui::PushItemWidth(item_width);
     return glengine::ScopeGuard{ &ImGui::PopItemWidth };
}
}// namespace imgui_utils
#endif// FIELD_MAP_EDITOR_IMGUIPUSHITEMWIDTH_HPP
