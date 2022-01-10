//
// Created by pcvii on 12/8/2021.
//
#include "LayerItem.hpp"
namespace glengine
{
void Layer::Item::OnUpdate(float ts) const
{
  if (m_impl)
  {
    return m_impl->OnUpdate(ts);
  }
}
void Layer::Item::OnRender() const
{
  if (m_impl)
  {
    return m_impl->OnRender();
  }
}
void Layer::Item::OnImGuiUpdate() const
{
  if (m_impl)
  {
    return m_impl->OnImGuiUpdate();
  }
}
void Layer::Item::OnEvent(const Event::Item &e) const
{
  if (m_impl)
  {
    return m_impl->OnEvent(e);
  }
}
Layer::Item::operator bool() const
{
  return bool{ m_impl };
}
}// namespace glengine