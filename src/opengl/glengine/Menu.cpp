//
// Created by pcvii on 11/29/2021.
//
#include "Menu.hpp"
#include "ImGuiIndent.hpp"
#include "ImGuiPushID.hpp"
namespace glengine
{
static_assert(Renderable<Menu>);
void Menu::OnRender() const
{
  m_current.OnRender();
}
void Menu::OnImGuiUpdate() const
{
  if (selected())
  {
    //ImGui::SetNextItemOpen(true);
    if(ImGui::CollapsingHeader(fmt::format("{}", m_current_string).c_str(),ImGuiTreeNodeFlags_DefaultOpen))
    {
      const auto un_indent = ImGuiIndent();
      m_current.OnImGuiUpdate();
    }
    {
      const auto pop = ImGuiPushID();
      if (ImGui::Button("Back"))
      {
        m_current        = MenuItem{};
        m_current_string = {};
        m_current_index  = {};
      }
    }
  }
  else
  {
    for (std::size_t i = {}; const auto &[name, funt] : m_list)
    {
      {
        const auto pop = ImGuiPushID();
        if (ImGui::Button(name.c_str()))
        {
          m_current        = funt();
          m_current_string = name;
          m_current_index  = i;
          break;
        }
      }
      ++i;
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

Menu::Menu()
  : m_current(MenuItem{})
{
}

void Menu::OnEvent(const Event::Item &e) const
{
  m_current.OnEvent(e);
}
}// namespace glengine