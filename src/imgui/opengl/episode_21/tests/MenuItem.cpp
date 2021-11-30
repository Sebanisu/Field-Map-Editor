//
// Created by pcvii on 11/29/2021.
//
#include "MenuItem.hpp"

void
  test::OnUpdate(const MenuItem &menu_item, float ts)
{
  if (menu_item)
  {
    return menu_item.m_impl->OnUpdate(ts);
  }
}
void
  test::OnRender(const MenuItem &menu_item)
{
  if (menu_item)
  {
    return menu_item.m_impl->OnRender();
  }
}
void
  test::OnImGuiRender(const MenuItem &menu_item)
{
  if (menu_item)
  {
    return menu_item.m_impl->OnImGuiRender();
  }
}
test::MenuItem::MenuItem(test::MenuItem &&other) noexcept
  : MenuItem()
{
  swap(*this, other);
}
test::MenuItem &
  test::MenuItem::operator=(test::MenuItem &&other) noexcept
{
  swap(*this, other);
  return *this;
}
test::MenuItem::operator bool() const
{
  return bool{ m_impl };
}
void
  test::swap(MenuItem &left, MenuItem &right) noexcept
{
  using std::swap;
  swap(left.m_impl, right.m_impl);
}