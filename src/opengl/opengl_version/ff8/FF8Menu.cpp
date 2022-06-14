//
// Created by pcvii on 11/29/2021.
//
#include "FF8Menu.hpp"
#include "FF8UpscaleMenu.hpp"
#include "MapDeswizzle.hpp"
#include "MapSwizzle.hpp"
#include "Mim.hpp"
static_assert(glengine::Renderable<ff8::FF8Menu>);
void ff8::FF8Menu::OnRender() const
{
  m_menu.OnRender();
}
void ff8::FF8Menu::OnImGuiUpdate() const
{
  bool fields_changed = GetFields().OnImGuiUpdate();
  if (fields_changed)
  {
    m_menu.reload();
  }
  m_menu.OnImGuiUpdate();
}
void ff8::FF8Menu::OnUpdate(float delta_time) const
{
  m_menu.OnUpdate(delta_time);
}
void ff8::FF8Menu::OnEvent(const glengine::Event::Item &e) const
{
  m_menu.OnEvent(e);
}
ff8::FF8Menu::FF8Menu()
{
  push_back<FF8UpscaleMenu>("Upscales");
  push_back<Mim>("Mim File");
  push_back<MapSwizzle>("Map File (Swizzled)");
  push_back<MapDeswizzle>("Map File (Deswizzled)");
}