#include "tool_tip.hpp"
#include "format_imgui_text.hpp"
#include "push_pop_id.hpp"
#include <imgui.h>
namespace fme
{
void tool_tip(const std::string_view str, bool override)
{
     if (!ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !override)
     {
          return;
     }

     //const auto pop_id       = PushPopID();
     const auto pop_tool_tip = scope_guard{ &ImGui::EndTooltip };

     ImGui::BeginTooltip();
     const auto pop_textwrappos = scope_guard{ &ImGui::PopTextWrapPos };
     ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);// Adjust wrap width as needed
     format_imgui_text("{}", str);
}
}// namespace fme