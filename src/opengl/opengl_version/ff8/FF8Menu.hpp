//
// Created by pcvii on 11/24/2021.
//

#ifndef FIELD_MAP_EDITOR_FF8MENU_HPP
#define FIELD_MAP_EDITOR_FF8MENU_HPP
#include "Fields.hpp"
#include "Menu.hpp"
#include "Application.hpp"

namespace ff8
{
class FF8Menu
{
public:
  FF8Menu();
  void OnUpdate(float) const;
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const glengine::Event::Item &) const;
  template<glengine::Renderable T>
  void push_back(std::string name) const
  {
    m_menu.push_back(std::move(name), [this]() -> glengine::MenuItem {
      return glengine::MenuItem(std::in_place_type_t<T>{}, GetFields());
    });
  }

private:
  glengine::Menu          m_menu{};
};
}// namespace ff8
#endif// FIELD_MAP_EDITOR_FF8MENU_HPP
