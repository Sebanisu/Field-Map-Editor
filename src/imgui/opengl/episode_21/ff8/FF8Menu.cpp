//
// Created by pcvii on 11/29/2021.
//
#include "FF8Menu.hpp"
#include "Map.hpp"
#include "Mim.hpp"

static_assert(test::Test<ff8::FF8Menu>);
void ff8::FF8Menu::OnRender() const
{
  m_menu.OnRender();
}
void ff8::FF8Menu::OnImGuiUpdate() const
{
  bool fields_changed = m_fields.OnImGuiUpdate();
  if(fields_changed)
  {
    m_menu.reload();
  }
  m_menu.OnImGuiUpdate();
}
void ff8::FF8Menu::OnUpdate(float delta_time) const
{
  m_menu.OnUpdate(delta_time);
}
void ff8::FF8Menu::OnEvent(const Event::Item &e) const
{
  m_menu.OnEvent(e);
}
ff8::FF8Menu::FF8Menu()
{
  push_back<Map>("Test Map File");
  push_back<Mim>("Test Mim File");
}