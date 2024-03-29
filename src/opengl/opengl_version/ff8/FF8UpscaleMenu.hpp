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
class Ff8UpscaleMenu
{
public:
  Ff8UpscaleMenu();
  Ff8UpscaleMenu(const Fields &);// discards the value passed in.
  void on_update(float) const;
  void on_render() const;
  void on_im_gui_update() const;
  void on_im_gui_update_children() const;
  void reload() const;
  void on_im_gui_menu() const;
  void on_event(const glengine::event::Item &) const;
  template<glengine::Renderable T>
  void push_back(std::string name) const
  {
    m_menu.push_back(std::move(name), [this]() -> glengine::MenuItem {
      return glengine::MenuItem(
        std::in_place_type_t<T>{}, GetFields(), m_upscales);
    });
  }

private:
  Upscales       m_upscales = {};
  glengine::Menu m_menu     = {"Upscales"};
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_FF8UPSCALEMENU_HPP
