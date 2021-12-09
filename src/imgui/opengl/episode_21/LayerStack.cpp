//
// Created by pcvii on 12/8/2021.
//

#include "LayerStack.hpp"
#include <ranges>
static_assert(test::Test<Layer::Stack>);
void Layer::Stack::OnRender() const
{
  for (const Item &layer : m_layers)
  {
    layer.OnRender();
  }
}
void Layer::Stack::OnImGuiUpdate() const
{
  for (const Item &layer : m_layers)
  {
    layer.OnImGuiUpdate();
  }
}
void Layer::Stack::OnUpdate(float delta_time) const
{
  for (const Item &layer : m_layers)
  {
    layer.OnUpdate(delta_time);
  }
}
void Layer::Stack::OnEvent(const Event::Item &e) const
{
  auto reversed = m_layers | std::views::reverse;
  for (const Item &layer : reversed)
  {
    layer.OnEvent(e);
  }
}