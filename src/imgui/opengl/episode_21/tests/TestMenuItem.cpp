//
// Created by pcvii on 11/29/2021.
//
#include "TestMenuItem.hpp"

void
  test::OnUpdate(const TestMenuItem &menu_item, float ts)
{
  if (menu_item)
  {
    return menu_item.m_impl->OnUpdate(ts);
  }
}
void
  test::OnRender(const TestMenuItem &menu_item)
{
  if (menu_item)
  {
    return menu_item.m_impl->OnRender();
  }
}
void
  test::OnImGuiUpdate(const TestMenuItem &menu_item)
{
  if (menu_item)
  {
    return menu_item.m_impl->OnImGuiUpdate();
  }
}
test::TestMenuItem::operator bool() const
{
  return bool{ m_impl };
}
void
  test::OnEvent(const TestMenuItem &menu_item, const Event::Item &e)
{
  if (menu_item)
  {
    return menu_item.m_impl->OnEvent(e);
  }
}