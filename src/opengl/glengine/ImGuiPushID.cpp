//
// Created by pcvii on 1/18/2022.
//
#include "ImGuiPushID.hpp"
#include <imgui.h>
glengine::scope_guard glengine::ImGuiPushID_impl::operator()() const noexcept
{
  ImGui::PushID(++s_id);
  return { &ImGui::PopID };
}
void glengine::ImGuiPushID_impl::reset() const noexcept
{
  s_id = {};
}
