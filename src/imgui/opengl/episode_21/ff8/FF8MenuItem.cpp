//
// Created by pcvii on 11/29/2021.
//
#include "FF8MenuItem.hpp"

void ff8::FF8MenuItem::OnUpdate(float ts) const
{
  if (m_impl)
  {
    return m_impl->OnUpdate(ts);
  }
}
void ff8::FF8MenuItem::OnRender() const
{
  if (m_impl)
  {
    return m_impl->OnRender();
  }
}
void ff8::FF8MenuItem::OnImGuiUpdate() const
{
  if (m_impl)
  {
    return m_impl->OnImGuiUpdate();
  }
}
void ff8::FF8MenuItem::OnEvent(const Event::Item &e) const
{
  if (m_impl)
  {
    return m_impl->OnEvent(e);
  }
}
ff8::FF8MenuItem::operator bool() const
{
  return bool{ m_impl };
}