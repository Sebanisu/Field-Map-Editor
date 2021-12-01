//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_FF8MENU_HPP
#define MYPROJECT_FF8MENU_HPP
#include "FF8MenuItem.hpp"
#include "Fields.hpp"
#include <functional>
#include <string>
#include <vector>
namespace ff8
{
class FF8Menu
{
public:
  using test_types = FF8MenuItem;
  FF8Menu();
  FF8Menu(test_types current);
  FF8Menu(FF8Menu &&other) noexcept;
  FF8Menu &
    operator=(FF8Menu &&other) noexcept;
  friend void
    OnUpdate(const FF8Menu &, float);
  friend void
    OnRender(const FF8Menu &);
  friend void
    OnImGuiRender(const FF8Menu &);
  template<test::Test T>
  void
    push_back(std::string name) const
  {
    push_back(
      std::move(name),
      [this]() -> test_types
      { return FF8MenuItem(test::test_dummy<T>{}, m_fields); });
  }
  void
    push_back(std::string name, std::function<test_types()> funt) const;

  friend void
    swap(FF8Menu &left, FF8Menu &right) noexcept;

private:
  mutable test_types m_current = {};
  mutable std::vector<std::pair<std::string, std::function<test_types()>>>
                      m_list        = {};
  Fields              m_fields      = {};
  mutable std::size_t m_current_pos = {};
};
void
  OnRender(const FF8Menu &self);
void
  OnImGuiRender(const FF8Menu &self);
void
  OnUpdate(const FF8Menu &self, float delta_time);
void
  swap(FF8Menu &left, FF8Menu &right) noexcept;
}// namespace ff8
#endif// MYPROJECT_FF8MENU_HPP
