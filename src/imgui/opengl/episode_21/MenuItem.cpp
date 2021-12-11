//
// Created by pcvii on 11/29/2021.
//
#include "MenuItem.hpp"

void MenuItem::OnUpdate(float ts)
{
  if (m_impl)
  {
    return m_impl->OnUpdate(ts);
  }
}
void MenuItem::OnRender()
{
  if (m_impl)
  {
    return m_impl->OnRender();
  }
}
void MenuItem::OnImGuiUpdate()
{
  if (m_impl)
  {
    return m_impl->OnImGuiUpdate();
  }
}
MenuItem::operator bool() const
{
  return bool{ m_impl };
}
void MenuItem::OnEvent(const Event::Item &e)
{
  if (m_impl)
  {
    return m_impl->OnEvent(e);
  }
}