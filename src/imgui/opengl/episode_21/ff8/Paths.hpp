//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_PATHS_HPP
#define MYPROJECT_PATHS_HPP
#include "scope_guard.hpp"
#include <imgui.h>
#include <open_viii/paths/Paths.hpp>
#include <tests/Test.hpp>
namespace ff8
{
class Paths
{
public:
  friend void
    OnUpdate(const Paths &, float);
  friend void
    OnRender(const Paths &);
  friend bool
    OnImGuiRender(const Paths &);
  [[nodiscard]] const std::string &
    Path() const
  {
    if (std::cmp_less(m_current, std::ranges::size(m_paths)))
    {
      return m_paths.at(static_cast<std::size_t>(m_current));
    }
    const static auto empty = std::string("");
    return empty;
  }
  void
    init() const
  {
    m_paths = open_viii::Paths::get();
  }

private:
  mutable std::vector<std::string> m_paths   = {};
  mutable int                      m_current = {};
};
inline void
  OnUpdate(const Paths &, float)
{
}
inline void
  OnRender(const Paths &)
{
}
inline bool
  OnImGuiRender(const Paths &self)
{
  bool changed = { false };
  if (ImGui::BeginCombo("Path", self.Path().c_str()))
  {
    int        id  = {};
    const auto end = scope_guard{ &ImGui::EndCombo };
    for (int i{}; const std::string &path : self.m_paths)
    {
      const bool is_selected = i == self.m_current;
      const auto pop         = scope_guard{ &ImGui::PopID };
      ImGui::PushID(++id);
      if (ImGui::Selectable(path.c_str(), is_selected))
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
static_assert(test::Test<Paths>);
}// namespace ff8
#endif// MYPROJECT_PATHS_HPP
