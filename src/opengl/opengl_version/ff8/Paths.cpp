//
// Created by pcvii on 11/30/2021.
//
#include "Paths.hpp"
#include "ImGuiPushID.hpp"
bool ff8::Paths::OnImGuiUpdate() const
{
  bool changed = { false };
  if (ImGui::BeginCombo("Path", Path().c_str()))
  {
    const auto end = glengine::scope_guard{ &ImGui::EndCombo };
    for (int i{}; const std::string &path : m_paths)
    {
      const bool is_selected = i == m_current;
      const auto pop         = glengine::ImGuiPushID();
      if (ImGui::Selectable(path.c_str(), is_selected))
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
const std::string &ff8::Paths::Path() const
{
  if (std::cmp_less(m_current, std::ranges::size(m_paths)))
  {
    return m_paths.at(static_cast<std::size_t>(m_current));
  }
  const static auto empty = std::string("");
  return empty;
}
ff8::Paths::Paths()
{
  m_paths = open_viii::Paths::get();
}
