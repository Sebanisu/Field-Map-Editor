//
// Created by pcvii on 11/29/2021.
//
#include "FF8UpscaleMenu.hpp"
#include "MapUpscaleDeswizzle.hpp"
#include "MapUpscaleSwizzle.hpp"
#include "Mim.hpp"
static_assert(glengine::Renderable<ff8::FF8UpscaleMenu>);
void ff8::FF8UpscaleMenu::OnRender() const
{
  m_menu.OnRender();
}
void ff8::FF8UpscaleMenu::OnImGuiUpdate() const
{
  bool fields_changed = m_upscales.OnImGuiUpdate();
  if (fields_changed)
  {
    m_menu.reload();
  }
  m_menu.OnImGuiUpdate();
}
void ff8::FF8UpscaleMenu::OnUpdate(float delta_time) const
{
  m_upscales.OnUpdate(delta_time);
  m_menu.OnUpdate(delta_time);
}
void ff8::FF8UpscaleMenu::OnEvent(const glengine::Event::Item &e) const
{
  m_upscales.OnEvent(e);
  m_menu.OnEvent(e);
}
ff8::FF8UpscaleMenu::FF8UpscaleMenu(const Fields &)
{
  push_back<MapUpscaleSwizzle>("Map File (Upscale Swizzled)");
  push_back<MapUpscaleDeswizzle>("Map File (Upscale Deswizzled)");
}