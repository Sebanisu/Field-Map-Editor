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
     for (std::size_t i = {}; const auto &[name, function] : m_list)
     {
          if (m_current[i])
          {
               auto const window_end = glengine::ScopeGuard{ []() { ImGui::End(); } };
               if (ImGui::Begin(fmt::format("{} - Controls", name).c_str()))
               {
                    m_current[i].on_im_gui_update();
               }
          }
          ++i;
     }
}
bool Menu::on_im_gui_menu() const
{
     bool changed = false;
     if (ImGui::BeginMenu(m_title))
     {
          const auto end_menu = ScopeGuard{ []() { ImGui::EndMenu(); } };
          for (std::size_t i = {}; const auto &[name, function] : m_list)
          {
               bool current_enabled = static_cast<bool>(m_current[i]);
               if (ImGui::MenuItem(fmt::format("{}", name).c_str(), nullptr, current_enabled))
               {
                    if (!m_current[i])
                    {
                         m_current[i] = function();
                    }
                    else
                    {
                         m_current[i] = MenuItem{};
                    }
                    changed = true;
               }
               ++i;
          }
     }
     return changed;
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