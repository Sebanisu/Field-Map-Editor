//
// Created by pcvii on 1/18/2022.
//
#include "ImGuiPushID.hpp"
namespace glengine
{
static std::atomic_int s_id = {};
glengine::scope_guard ImGuiPushID_impl::operator()() const noexcept
{
  ImGui::PushID(++s_id);
  return { &ImGui::PopID };
}
void ImGuiPushID_impl::reset() const noexcept
{
  s_id = {};
}
}// namespace glengine
