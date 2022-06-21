//
// Created by pcvii on 11/29/2021.
//
#include "MenuItem.hpp"
namespace glengine
{
void MenuItem::on_update(float ts)
{
  if (m_impl)
  {
    return m_impl->on_update(ts);
  }
}
void MenuItem::on_render()
{
  if (m_impl)
  {
    return m_impl->on_render();
  }
}
void MenuItem::on_im_gui_update()
{
  if (m_impl)
  {
    return m_impl->on_im_gui_update();
  }
}
MenuItem::operator bool() const
{
  return bool{ m_impl };
}
void MenuItem::on_event(const event::Item &e)
{
  if (m_impl)
  {
    return m_impl->on_event(e);
  }
}
}// namespace glengine