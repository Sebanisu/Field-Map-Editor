//
// Created by pcvii on 1/25/2022.
//

#include "ImGuiDisabled.hpp"
#include <imgui.h>
glengine::scope_guard glengine::ImGuiDisabled(bool disabled) noexcept
{
  if (disabled)
  {
    ImGui::BeginDisabled(disabled);
    return { &ImGui::EndDisabled };
  }
  return {};
}
