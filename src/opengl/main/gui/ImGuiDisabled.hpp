//
// Created by pcvii on 1/25/2022.
#ifndef B99BEDA4_8B05_47FE_B03E_4AC0C8FF90EE
#define B99BEDA4_8B05_47FE_B03E_4AC0C8FF90EE
#include <imgui.h>
#include <ScopeGuard.hpp>
namespace fme
{
[[nodiscard]] inline auto ImGuiDisabled(bool disabled) noexcept
{

     ImGui::BeginDisabled(disabled);
     return glengine::ScopeGuard{ &ImGui::EndDisabled };
}
}// namespace fme
#endif /* B99BEDA4_8B05_47FE_B03E_4AC0C8FF90EE */
