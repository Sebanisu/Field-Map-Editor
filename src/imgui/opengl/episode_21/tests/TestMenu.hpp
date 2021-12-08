//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TESTMENU_HPP
#define MYPROJECT_TESTMENU_HPP
#include "EventItem.hpp"
#include "TestMenuItem.hpp"
#include <functional>
#include <string>
#include <vector>
namespace test
{
class TestMenu
{
public:
  using test_types = TestMenuItem;
  TestMenu();
  TestMenu(test_types current);

  TestMenu(const TestMenu &other) noexcept = delete;
  TestMenu &
    operator=(const TestMenu &other) noexcept = delete;
  TestMenu(TestMenu &&other) noexcept         = default;
  TestMenu &
    operator=(TestMenu &&other) noexcept = default;
  friend void
    OnUpdate(const TestMenu &, float);
  friend void
    OnRender(const TestMenu &);
  friend void
    OnImGuiUpdate(const TestMenu &);
  friend void
    OnEvent(const TestMenu &, const Event::Item &);
  template<Test T>
  void
    push_back(std::string name) const
  {
    push_back(
      std::move(name),
      []() -> test_types { return std::in_place_type_t<T>{}; });
  }
  void
    push_back(std::string name, std::function<test_types()> funt) const;

private:
  mutable test_types m_current = {};
  mutable std::vector<std::pair<std::string, std::function<test_types()>>>
    m_list = {};
};
void
  OnRender(const TestMenu &self);
void
  OnImGuiUpdate(const TestMenu &self);
void
  OnUpdate(const TestMenu &self, float delta_time);
void
  OnEvent(const TestMenu &, const Event::Item &);
}// namespace test
#endif// MYPROJECT_TESTMENU_HPP
