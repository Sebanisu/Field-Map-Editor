//
// Created by pcvii on 1/18/2022.
//
#include "ImGuiPushID.hpp"
glengine::ScopeGuard glengine::ImGuiPushIdImpl::operator()() const noexcept
{
  ImGui::PushID(++s_id);
  return { &ImGui::PopID };
}
void glengine::ImGuiPushIdImpl::reset() const noexcept
{
  s_id = {};
}
