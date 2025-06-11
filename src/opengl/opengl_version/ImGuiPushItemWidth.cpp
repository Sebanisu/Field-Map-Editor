//
// Created by pcvii on 1/25/2022.
//

#include "ImGuiPushItemWidth.hpp"
#include <imgui.h>
glengine::ScopeGuard glengine::ImGuiPushItemWidth(float item_width) noexcept
{
     ImGui::PushItemWidth(item_width);
     return { ImGui::PopItemWidth };
}
