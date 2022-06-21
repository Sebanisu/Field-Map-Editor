//
// Created by pcvii on 12/8/2021.
//
#include "LayerItem.hpp"
namespace glengine
{
void Layer::Item::on_update(float ts) const
{
  if (m_impl)
  {
    return m_impl->on_update(ts);
  }
}
void Layer::Item::on_render() const
{
  if (m_impl)
  {
    return m_impl->on_render();
  }
}
void Layer::Item::on_im_gui_update() const
{
  if (m_impl)
  {
    return m_impl->on_im_gui_update();
  }
}
void Layer::Item::on_event(const event::Item &e) const
{
  if (m_impl)
  {
    return m_impl->on_event(e);
  }
}
Layer::Item::operator bool() const
{
  return bool{ m_impl };
}
}// namespace glengine