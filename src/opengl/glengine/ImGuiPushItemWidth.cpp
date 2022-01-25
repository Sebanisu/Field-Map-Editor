//
// Created by pcvii on 1/25/2022.
//

#include "ImGuiPushItemWidth.hpp"
glengine::scope_guard glengine::ImGuiPushItemWidth(float item_width) noexcept
{
  ImGui::PushItemWidth(item_width);
  return { ImGui::PopItemWidth };
}
