//
// Created by pcvii on 11/24/2021.
//

#ifndef FIELD_MAP_EDITOR_FF8UPSCALEMENU_HPP
#define FIELD_MAP_EDITOR_FF8UPSCALEMENU_HPP
#include "Application.hpp"
#include "Fields.hpp"
#include "Menu.hpp"
#include "Upscales.hpp"

namespace ff_8
{
class FF8UpscaleMenu
{
public:
  FF8UpscaleMenu() = default;
  FF8UpscaleMenu(const Fields &);// discards the value passed in.
  void on_update(float) const;
  void on_render() const;
  void on_im_gui_update() const;
  void on_event(const glengine::event::Item &) const;
  template<glengine::Renderable T>
  void push_back(std::string name) const
  {
    m_menu.push_back(std::move(name), [this]() -> glengine::MenuItem {
      return glengine::MenuItem(
        std::in_place_type_t<T>{}, GetFields(), m_upscales.Path());
    });
  }

private:
  glengine::Menu m_menu     = {};
  Upscales       m_upscales = {};
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_FF8UPSCALEMENU_HPP
