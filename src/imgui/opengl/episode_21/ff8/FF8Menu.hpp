//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_FF8MENU_HPP
#define MYPROJECT_FF8MENU_HPP
#include "Fields.hpp"
#include "Menu.hpp"


namespace ff8
{
class FF8Menu
{
public:
  FF8Menu();
  void OnUpdate(float) const;
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const Event::Item &) const;
  template<Renderable T>
  void push_back(std::string name) const
  {
    m_menu.push_back(std::move(name), [this]() -> MenuItem {
      return MenuItem(std::in_place_type_t<T>{}, m_fields);
    });
  }

private:
  Menu   m_menu{};
  Fields m_fields = {};
};
}// namespace ff8
#endif// MYPROJECT_FF8MENU_HPP
