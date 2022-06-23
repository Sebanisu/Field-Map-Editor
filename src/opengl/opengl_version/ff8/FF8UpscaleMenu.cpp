//
// Created by pcvii on 11/29/2021.
//
#include "FF8UpscaleMenu.hpp"
#include "MapUpscaleDeswizzle.hpp"
#include "MapUpscaleSwizzle.hpp"
static_assert(glengine::Renderable<ff_8::Ff8UpscaleMenu>);
void ff_8::Ff8UpscaleMenu::on_render() const
{
  m_menu.on_render();
}
void ff_8::Ff8UpscaleMenu::on_im_gui_update() const
{
  bool fields_changed = m_upscales.on_im_gui_update();
  if (fields_changed)
  {
    m_menu.reload();
  }
  m_menu.on_im_gui_update();
}
void ff_8::Ff8UpscaleMenu::on_update(float delta_time) const
{
  m_upscales.on_update(delta_time);
  m_menu.on_update(delta_time);
}
void ff_8::Ff8UpscaleMenu::on_event(const glengine::event::Item &e) const
{
  m_upscales.on_event(e);
  m_menu.on_event(e);
}
ff_8::Ff8UpscaleMenu::Ff8UpscaleMenu(const Fields &)
{
  push_back<MapUpscaleSwizzle>("Map File (Upscale Swizzled)");
  push_back<MapUpscaleDeswizzle>("Map File (Upscale Deswizzled)");
}