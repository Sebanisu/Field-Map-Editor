//
// Created by pcvii on 12/1/2021.
//
#include "Palettes.hpp"
#include "imgui.h"
#include "scope_guard.hpp"
void
  ff8::OnUpdate(const Palettes &, float)
{
}
void
  ff8::OnRender(const Palettes &)
{
}
bool
  ff8::OnImGuiUpdate(const Palettes &self)
{
  bool changed = { false };
  if (ImGui::BeginCombo("Palette", self.String().data()))
  {
    int        id  = {};
    const auto end = scope_guard{ &ImGui::EndCombo };
    for (int i{}; const std::string_view &string : self.m_strings)
    {
      const bool is_selected = i == self.m_current;
      const auto pop         = scope_guard{ &ImGui::PopID };
      ImGui::PushID(++id);
      if (ImGui::Selectable(string.data(), is_selected))
      {
        self.m_current = i;
        changed        = true;
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