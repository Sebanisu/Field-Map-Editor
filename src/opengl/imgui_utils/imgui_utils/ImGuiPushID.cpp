//
// Created by pcvii on 1/18/2022.
//
#include "ImGuiPushID.hpp"

[[nodiscard]] static int &get_imgui_id()
{
     static int imgui_id = {};
     return imgui_id;
}
void imgui_utils::impl::ImGuiPushId::reset() const noexcept
{
     get_imgui_id() = {};
}

int imgui_utils::impl::ImGuiPushId::id() const noexcept
{
     return get_imgui_id();
}

glengine::ScopeGuard<void (*)()>
  imgui_utils::impl::ImGuiPushId::operator()() const noexcept
{
     ImGui::PushID(++get_imgui_id());
     return glengine::ScopeGuard{ &ImGui::PopID };
}
