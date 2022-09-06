//
// Created by pcvii on 11/29/2021.
//
#include "FF8Menu.hpp"
#include "FF8UpscaleMenu.hpp"
#include "MapDeswizzle.hpp"
#include "MapSwizzle.hpp"
#include "MenuSaveToggles.hpp"
#include "Mim.hpp"
static_assert(glengine::Renderable<ff_8::Ff8Menu>);
void ff_8::Ff8Menu::on_render() const
{
  m_menu.on_render();
  m_upscales.on_render();
}
void ff_8::Ff8Menu::on_im_gui_update() const
{
  {
    auto const window_end = glengine::ScopeGuard{ []() { ImGui::End(); } };
    if (ImGui::Begin("FF8 Controls"))
    {
      if (GetFields().on_im_gui_update())
      {
        GetWindow().trigger_reload();
      }
      m_upscales.on_im_gui_update();
    }
  }
  m_menu.on_im_gui_update();
  m_upscales.on_im_gui_update_children();
}
void ff_8::Ff8Menu::on_im_gui_menu() const
{
  if(m_menu.on_im_gui_menu())
  {
    MenuSaveToggles(m_menu);
  }
  m_upscales.on_im_gui_menu();
}
void ff_8::Ff8Menu::on_update(float delta_time) const
{
  m_menu.on_update(delta_time);
  m_upscales.on_update(delta_time);
}
void ff_8::Ff8Menu::on_event(const glengine::event::Item &e) const
{
  const auto dispatcher = glengine::event::Dispatcher(e);
  dispatcher.Dispatch<glengine::event::Reload>(
    [this](const glengine::event::Reload &reload) -> bool {
      if (reload)
      {
        m_menu.reload();
        m_upscales.reload();
      }
      return true;
    });
  m_menu.on_event(e);
  m_upscales.on_event(e);
}
ff_8::Ff8Menu::Ff8Menu()
{
  push_back<Mim>("Mim File");
  push_back<MapSwizzle>("Map File (Swizzled)");
  push_back<MapDeswizzle>("Map File (Deswizzled)");
  MenuLoadToggles(m_menu);
}