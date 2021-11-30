//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TESTMENU_HPP
#define MYPROJECT_TESTMENU_HPP
#include "MenuItem.hpp"
#include <functional>
#include <string>
#include <vector>
namespace test
{
class TestMenu
{
public:
  using test_types = MenuItem;
  TestMenu();
  TestMenu(test_types current);
  TestMenu(TestMenu &&other) noexcept;
  TestMenu &
    operator=(TestMenu &&other) noexcept;
  friend void
    OnUpdate(const TestMenu &, float);
  friend void
    OnRender(const TestMenu &);
  friend void
    OnImGuiRender(const TestMenu &);
  template<Test T>
  void
    push_back(std::string name) const
  {
    push_back(std::move(name), []() -> test_types { return MenuItem(T{}); });
  }
  void
    push_back(std::string name, std::function<test_types()> funt) const;

  friend void
    swap(TestMenu &left, TestMenu &right) noexcept;

private:
  mutable test_types m_current = {};
  mutable std::vector<std::pair<std::string, std::function<test_types()>>>
    m_list = {};
};
void
  OnRender(const TestMenu &self);
void
  OnImGuiRender(const TestMenu &self);
void
  OnUpdate(const TestMenu &self, float delta_time);
void
  swap(TestMenu &left, TestMenu &right) noexcept;
}// namespace test
#endif// MYPROJECT_TESTMENU_HPP
