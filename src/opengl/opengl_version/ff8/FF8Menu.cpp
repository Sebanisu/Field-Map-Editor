//
// Created by pcvii on 11/29/2021.
//
#include "FF8Menu.hpp"
#include "FF8UpscaleMenu.hpp"
#include "MapDeswizzle.hpp"
#include "MapSwizzle.hpp"
#include "Mim.hpp"
static_assert(glengine::Renderable<ff_8::FF8Menu>);
void ff_8::FF8Menu::on_render() const
{
  m_menu.on_render();
}
void ff_8::FF8Menu::on_im_gui_update() const
{
  bool fields_changed = GetFields().on_im_gui_update();
  if (fields_changed)
  {
    m_menu.reload();
  }
  m_menu.on_im_gui_update();
}
void ff_8::FF8Menu::on_update(float delta_time) const
{
  m_menu.on_update(delta_time);
}
void ff_8::FF8Menu::on_event(const glengine::event::Item &e) const
{
  m_menu.on_event(e);
}
ff_8::FF8Menu::FF8Menu()
{
  push_back<FF8UpscaleMenu>("Upscales");
  push_back<Mim>("Mim File");
  push_back<MapSwizzle>("Map File (Swizzled)");
  push_back<MapDeswizzle>("Map File (Deswizzled)");
}