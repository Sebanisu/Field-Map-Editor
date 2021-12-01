//
// Created by pcvii on 11/29/2021.
//
#include "FF8MenuItem.hpp"

void
  ff8::OnUpdate(const FF8MenuItem &menu_item, float ts)
{
  if (menu_item)
  {
    return menu_item.m_impl->OnUpdate(ts);
  }
}
void
  ff8::OnRender(const FF8MenuItem &menu_item)
{
  if (menu_item)
  {
    return menu_item.m_impl->OnRender();
  }
}
void
  ff8::OnImGuiRender(const FF8MenuItem &menu_item)
{
  if (menu_item)
  {
    return menu_item.m_impl->OnImGuiRender();
  }
}
ff8::FF8MenuItem::operator bool() const
{
  return bool{ m_impl };
}