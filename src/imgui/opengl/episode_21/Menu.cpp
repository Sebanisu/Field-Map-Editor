//
// Created by pcvii on 11/29/2021.
//

#include "Menu.hpp"
static_assert(test::Test<Menu>);
void Menu::OnRender() const
{
  m_current.OnRender();
}
void Menu::OnImGuiUpdate() const
{
  if (m_current)
  {
    m_current.OnImGuiUpdate();
    if (ImGui::Button("Back"))
    {
      m_current = MenuItem{};
    }
  }
  else
  {
    for (const auto &[name, funt] : m_list)
    {
      if (ImGui::Button(name.c_str()))
      {
        m_current = funt();
        break;
      }
    }
  }
}

void Menu::OnUpdate(float delta_time) const
{
  m_current.OnUpdate(delta_time);
}

void Menu::push_back(std::string name, std::function<MenuItem()> funt) const
{
  m_list.emplace_back(std::move(name), std::move(funt));
}

Menu::Menu(MenuItem current)
  : m_current(std::move(current))
{
}
Menu::Menu()
  : Menu(MenuItem{})
{
}

void Menu::OnEvent(const Event::Item &e) const
{
  m_current.OnEvent(e);
}
