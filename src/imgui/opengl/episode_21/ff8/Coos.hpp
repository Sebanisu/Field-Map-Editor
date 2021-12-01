//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_COOS_HPP
#define MYPROJECT_COOS_HPP
#include "scope_guard.hpp"
#include "tests/Test.hpp"
#include <imgui.h>
#include <open_viii/strings/LangCommon.hpp>
namespace ff8
{
class Coos
{
public:
  friend void
    OnUpdate(const Coos &, float);
  friend void
    OnRender(const Coos &);
  friend bool
    OnImGuiRender(const Coos &);
  [[nodiscard]] std::string_view
    Coo() const
  {
    return m_coos.at(static_cast<std::size_t>(m_current));
  }

private:
  static constexpr auto m_coos = open_viii::LangCommon::to_string_array();
  mutable int           m_current{};
};
static_assert(test::Test<Coos>);

inline void
  OnUpdate(const Coos &, float)
{
}
inline void
  OnRender(const Coos &)
{
}
inline bool
  OnImGuiRender(const Coos &self)
{
  bool changed = { false };
  if (ImGui::BeginCombo("Language", self.Coo().data()))
  {
    int        id  = {};
    const auto end = scope_guard{ &ImGui::EndCombo };
    for (int i{}; const std::string_view &coo : self.m_coos)
    {
      const bool is_selected = i == self.m_current;
      const auto pop         = scope_guard{ &ImGui::PopID };
      ImGui::PushID(++id);
      if (ImGui::Selectable(coo.data(), is_selected))
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
}// namespace ff8
#endif// MYPROJECT_COOS_HPP
