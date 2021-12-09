//
// Created by pcvii on 11/29/2021.
//
#include "TestMenuItem.hpp"

void test::TestMenuItem::OnUpdate(float ts)
{
  if (m_impl)
  {
    return m_impl->OnUpdate(ts);
  }
}
void test::TestMenuItem::OnRender()
{
  if (m_impl)
  {
    return m_impl->OnRender();
  }
}
void test::TestMenuItem::OnImGuiUpdate()
{
  if (m_impl)
  {
    return m_impl->OnImGuiUpdate();
  }
}
test::TestMenuItem::operator bool() const
{
  return bool{ m_impl };
}
void test::TestMenuItem::OnEvent(const Event::Item &e)
{
  if (m_impl)
  {
    return m_impl->OnEvent(e);
  }
}