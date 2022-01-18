//
// Created by pcvii on 11/30/2021.
//

#include "Coos.hpp"
#include "ImGuiPushID.hpp"
bool ff8::Coos::OnImGuiUpdate() const
{
  bool changed = { false };
  if (ImGui::BeginCombo("Language", Coo().data()))
  {
    const auto end = glengine::scope_guard{ &ImGui::EndCombo };
    for (int i{}; const std::string_view &coo : m_coos)
    {
      const bool is_selected = i == m_current;
      const auto pop         = glengine::ImGuiPushID();
      if (ImGui::Selectable(coo.data(), is_selected))
      {
        m_current = i;
        changed   = true;
      }
      if (is_selected)
      {
        ImGui::SetItemDefaultFocus();
      }
      ++i;
    }
  }
  return changed;
}
