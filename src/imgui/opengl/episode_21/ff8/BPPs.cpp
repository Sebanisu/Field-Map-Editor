//
// Created by pcvii on 12/1/2021.
//

#include "BPPs.hpp"
#include "imgui.h"
#include "scope_guard.hpp"
bool ff8::BPPs::OnImGuiUpdate() const
{
  bool changed = { false };
  if (ImGui::BeginCombo("BPP", String().data()))
  {
    int        id  = {};
    const auto end = glengine::scope_guard{ &ImGui::EndCombo };
    for (int i{}; const std::string_view &string : m_strings)
    {
      const bool is_selected = i == m_current;
      const auto pop         = glengine::scope_guard{ &ImGui::PopID };
      ImGui::PushID(++id);
      if (ImGui::Selectable(string.data(), is_selected))
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
