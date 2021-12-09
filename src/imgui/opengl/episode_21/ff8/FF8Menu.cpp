//
// Created by pcvii on 11/29/2021.
//
#include "FF8Menu.hpp"
#include "Map.hpp"
#include "Mim.hpp"
#include <imgui.h>
static_assert(test::Test<ff8::FF8Menu>);
void ff8::FF8Menu::OnRender() const
{
  m_current.OnRender();
}

void ff8::FF8Menu::OnImGuiUpdate() const
{
  bool fields_changed = m_fields.OnImGuiUpdate();
  if (m_current)
  {
    if (fields_changed)
    {
      m_current = m_list.at(m_current_pos).second();
    }
    ImGui::Separator();
    m_current.OnImGuiUpdate();
    ImGui::Separator();
    if (ImGui::Button("Back"))
    {
      m_current = FF8MenuItem{};
    }
    ImGui::Separator();
  }
  else
  {
    for (std::size_t i{}; const auto &[name, funt] : m_list)
    {
      if (ImGui::Button(name.c_str()))
      {
        m_current     = funt();
        m_current_pos = i;
        break;
      }
      ++i;
    }
  }
}

void ff8::FF8Menu::OnUpdate(float delta_time) const
{
  m_current.OnUpdate(delta_time);
}

void ff8::FF8Menu::push_back(std::string name, std::function<test_types()> funt)
  const
{
  m_list.emplace_back(std::move(name), std::move(funt));
}

ff8::FF8Menu::FF8Menu(ff8::FF8Menu::test_types current)
  : m_current(std::move(current))
{
  push_back<Map>("Test Map File");
  push_back<Mim>("Test Mim File");
}

ff8::FF8Menu::FF8Menu()
  : FF8Menu(FF8MenuItem{})
{
}

void ff8::FF8Menu::OnEvent(const Event::Item &e) const
{
  m_current.OnEvent(e);
}
