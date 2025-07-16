//
// Created by pcvii on 11/29/2021.
//
#include "FF8UpscaleMenu.hpp"
#include "MapUpscaleDeswizzle.hpp"
#include "MapUpscaleSwizzle.hpp"
#include "MenuSaveToggles.hpp"
static_assert(glengine::Renderable<ff_8::Ff8UpscaleMenu>);
void ff_8::Ff8UpscaleMenu::on_render() const
{
     m_menu.on_render();
}
void ff_8::Ff8UpscaleMenu::on_im_gui_update() const
{
     if (m_swizzles.on_im_gui_update())// upscale_path_changed
     {
          m_menu.reload();
     }
}
void ff_8::Ff8UpscaleMenu::on_im_gui_update_children() const
{
     m_menu.on_im_gui_update();
}
void ff_8::Ff8UpscaleMenu::on_im_gui_menu() const
{
     if (m_menu.on_im_gui_menu())
     {
          MenuSaveToggles(m_menu);
     }
}
void ff_8::Ff8UpscaleMenu::on_update(float delta_time) const
{
     m_swizzles.on_update(delta_time);
     m_menu.on_update(delta_time);
}
void ff_8::Ff8UpscaleMenu::on_event(const glengine::event::Item &e) const
{
     m_swizzles.on_event(e);
     m_menu.on_event(e);
}
ff_8::Ff8UpscaleMenu::Ff8UpscaleMenu(const Fields &)
  : Ff8UpscaleMenu()
{
}
ff_8::Ff8UpscaleMenu::Ff8UpscaleMenu()
{
     push_back<MapUpscaleSwizzle>(TileFunctionsUpscaleSwizzle::label);
     push_back<MapUpscaleDeswizzle>(TileFunctionsUpscaleDeswizzle::label);
     MenuLoadToggles(m_menu);
}
void ff_8::Ff8UpscaleMenu::reload() const
{
     m_menu.reload();
}
