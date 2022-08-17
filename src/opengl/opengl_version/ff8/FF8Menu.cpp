//
// Created by pcvii on 11/29/2021.
//
#include "FF8Menu.hpp"
#include "FF8UpscaleMenu.hpp"
#include "MapDeswizzle.hpp"
#include "MapSwizzle.hpp"
#include "Mim.hpp"
static_assert(glengine::Renderable<ff_8::Ff8Menu>);
void ff_8::Ff8Menu::on_render() const
{
  m_menu.on_render();
  m_upscales.on_render();
}
void ff_8::Ff8Menu::on_im_gui_update() const
{
  if (ImGui::Begin("FF8 Controls"))
  {
    if (GetFields().on_im_gui_update())
    {
      m_menu.reload();
      m_upscales.reload();
    }
    m_upscales.on_im_gui_update();
  }
  ImGui::End();
  m_menu.on_im_gui_update();
  m_upscales.on_im_gui_update_children();
}
void ff_8::Ff8Menu::on_im_gui_menu() const
{
  m_menu.on_im_gui_menu();
  m_upscales.on_im_gui_menu();
}
void ff_8::Ff8Menu::on_update(float delta_time) const
{
  m_menu.on_update(delta_time);
  m_upscales.on_update(delta_time);
}
void ff_8::Ff8Menu::on_event(const glengine::event::Item &e) const
{
  m_menu.on_event(e);
  m_upscales.on_update(e);
}
ff_8::Ff8Menu::Ff8Menu()
{
  push_back<Mim>("Mim File");
  push_back<MapSwizzle>("Map File (Swizzled)");
  push_back<MapDeswizzle>("Map File (Deswizzled)");
}