//
// Created by pcvii on 12/8/2021.
//

#include "LayerStack.hpp"
#include <ranges>
static_assert(test::Test<Layer::Stack>);
void
  Layer::OnRender(const Stack &self)
{
  for (const auto &layer : self.m_layers)
    OnRender(layer);
}
void
  Layer::OnImGuiUpdate(const Stack &self)
{
  for (const auto &layer : self.m_layers)
    OnImGuiUpdate(layer);
}
void
  Layer::OnUpdate(const Stack &self, float delta_time)
{
  for (const auto &layer : self.m_layers)
    OnUpdate(layer, delta_time);
}
void
  Layer::OnEvent(const Stack &self, const Event::Item &e)
{
  auto reversed = self.m_layers | std::views::reverse;
  for (const auto &layer : reversed)
    OnEvent(layer, e);
}