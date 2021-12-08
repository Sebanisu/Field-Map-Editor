//
// Created by pcvii on 11/29/2021.
//
#include "FF8Menu.hpp"
#include "Map.hpp"
#include "Mim.hpp"
#include <imgui.h>
static_assert(test::Test<ff8::FF8Menu>);
void
  ff8::OnRender(const FF8Menu &self)
{
  OnRender(self.m_current);
}
void
  ff8::OnImGuiUpdate(const FF8Menu &self)
{
  bool fields_changed = ff8::OnImGuiUpdate(self.m_fields);
  if (self.m_current)
  {
    if (fields_changed)
    {
      self.m_current = self.m_list.at(self.m_current_pos).second();
    }
    ImGui::Separator();
    OnImGuiUpdate(self.m_current);
    ImGui::Separator();
    if (ImGui::Button("Back"))
    {
      self.m_current = FF8MenuItem{};
    }
    ImGui::Separator();
  }
  else
  {
    for (std::size_t i{}; const auto &[name, funt] : self.m_list)
    {
      if (ImGui::Button(name.c_str()))
      {
        self.m_current     = funt();
        self.m_current_pos = i;
        break;
      }
      ++i;
    }
  }
}

void
  ff8::OnUpdate(const FF8Menu &self, float delta_time)
{
  OnUpdate(self.m_current, delta_time);
}
void
  ff8::FF8Menu::push_back(std::string name, std::function<test_types()> funt)
    const
{
  m_list.emplace_back(std::move(name), std::move(funt));
}
ff8::FF8Menu::FF8Menu(ff8::FF8Menu::test_types current)
  : m_current(std::move(current))
{
  push_back<Map>("Test Map File");
  push_back<Mim>("Test Mim File");
  m_fields.init();
}
ff8::FF8Menu::FF8Menu()
  : FF8Menu(FF8MenuItem{})
{
}
void
  ff8::OnEvent(const FF8Menu &self, const Event::Item &e)
{
  OnEvent(self.m_current, e);
}
