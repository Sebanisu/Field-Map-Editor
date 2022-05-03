//
// Created by pcvii on 11/24/2021.
//

#ifndef FIELD_MAP_EDITOR_FF8UPSCALEMENU_HPP
#define FIELD_MAP_EDITOR_FF8UPSCALEMENU_HPP
#include "Application.hpp"
#include "Fields.hpp"
#include "Menu.hpp"
#include "Upscales.hpp"

namespace ff8
{
class FF8UpscaleMenu
{
public:
  FF8UpscaleMenu() = default;
  FF8UpscaleMenu(const Fields &);// discards the value passed in.
  void OnUpdate(float) const;
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const glengine::Event::Item &) const;
  template<glengine::Renderable T>
  void push_back(std::string name) const
  {
    m_menu.push_back(std::move(name), [this]() -> glengine::MenuItem {
      return glengine::MenuItem(std::in_place_type_t<T>{}, GetFields(), m_upscales.Path());
    });
  }

private:
  glengine::Menu m_menu     = {};
  Upscales       m_upscales = {};
};
}// namespace ff8
#endif// FIELD_MAP_EDITOR_FF8UPSCALEMENU_HPP
