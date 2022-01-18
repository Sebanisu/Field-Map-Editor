//
// Created by pcvii on 12/1/2021.
//
#include "Palettes.hpp"
#include "imgui.h"
#include "scope_guard.hpp"
#include "ImGuiPushID.hpp"
bool ff8::Palettes::OnImGuiUpdate() const
{
  bool changed = { false };
  if (ImGui::BeginCombo("Palette", String().data()))
  {
    const auto end = glengine::scope_guard{ &ImGui::EndCombo };
    for (int i{}; const std::string_view &string : m_strings)
    {
      const bool is_selected = i == m_current;
      const auto pop         = glengine::ImGuiPushID();
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
std::uint8_t ff8::Palettes::Palette() const
{
  return m_values.at(static_cast<std::size_t>(m_current));
}
std::string_view ff8::Palettes::String() const
{
  return m_strings.at(static_cast<std::size_t>(m_current));
}
int ff8::Palettes::Index() const
{
  return m_current;
}
