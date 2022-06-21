//
// Created by pcvii on 11/29/2021.
//
#include "Menu.hpp"
#include "ImGuiIndent.hpp"
namespace glengine
{
static_assert(Renderable<Menu>);
void Menu::on_render() const
{
  for (auto &current : m_current)
  {
    if (current)
    {
      current.on_render();
    }
  }
}
void Menu::on_im_gui_update() const
{
  for (std::size_t i = {}; const auto &[name, funt] : m_list)
  {
    if (ImGui::CollapsingHeader(
          fmt::format("{}", name).c_str(), ImGuiTreeNodeFlags_None))
    {
      if (!m_current[i])
        m_current[i] = funt();
      const auto un_indent = ImGuiIndent();
      m_current[i].on_im_gui_update();
    }
    else if (m_current[i])
    {
      m_current[i] = MenuItem{};
    }
    ++i;
  }
}

void Menu::on_update(float delta_time) const
{
  for (auto &current : m_current)
  {
    if (current)
    {
      current.on_update(delta_time);
    }
  }
}

void Menu::push_back(std::string name, std::function<MenuItem()> funt) const
{
  m_list.emplace_back(std::move(name), std::move(funt));
  m_current.push_back({});
}

Menu::Menu()
  : m_current(MenuItem{})
{
}

void Menu::on_event(const event::Item &e) const
{
  for (auto &current : m_current)
  {
    if (current)
    {
      current.on_event(e);
    }
  }
}
void Menu::reload() const
{
  for (size_t current_index = 0U; auto &current : m_current)
  {
    if (current)
    {
      current = m_list[current_index].second();
    }
    ++current_index;
  }
}
}// namespace glengine