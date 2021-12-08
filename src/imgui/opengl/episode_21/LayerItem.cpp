//
// Created by pcvii on 12/8/2021.
//
#include "LayerItem.hpp"
void
  Layer::OnUpdate(const Item &menu_item, float ts)
{
  if (menu_item)
  {
    return menu_item.m_impl->OnUpdate(ts);
  }
}
void
  Layer::OnRender(const Item &menu_item)
{
  if (menu_item)
  {
    return menu_item.m_impl->OnRender();
  }
}
void
  Layer::OnImGuiUpdate(const Item &menu_item)
{
  if (menu_item)
  {
    return menu_item.m_impl->OnImGuiUpdate();
  }
}
void
  Layer::OnEvent(const Item &menu_item, const Event::Item &e)
{
  if (menu_item)
  {
    return menu_item.m_impl->OnEvent(e);
  }
}
Layer::Item::operator bool() const
{
  return bool{ m_impl };
}