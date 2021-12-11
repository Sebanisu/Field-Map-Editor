//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_MENU_HPP
#define MYPROJECT_MENU_HPP
#include "EventItem.hpp"
#include "MenuItem.hpp"
#include "tests/Test.hpp"


class Menu
{
public:
  Menu();
  Menu(MenuItem current);

  Menu(const Menu &other) noexcept = delete;
  Menu &operator=(const Menu &other) noexcept = delete;
  Menu(Menu &&other) noexcept                 = default;
  Menu &operator=(Menu &&other) noexcept = default;
  void  OnUpdate(float) const;
  void  OnRender() const;
  void  OnImGuiUpdate() const;
  void  OnEvent(const Event::Item &) const;
  template<test::Test T>
  void push_back(std::string name) const
  {
    push_back(
      std::move(name), []() -> MenuItem { return std::in_place_type_t<T>{}; });
  }
  void push_back(std::string name, std::function<MenuItem()> funt) const;

private:
  mutable MenuItem m_current = {};
  mutable std::vector<std::pair<std::string, std::function<MenuItem()>>>
    m_list = {};
};
#endif// MYPROJECT_MENU_HPP
