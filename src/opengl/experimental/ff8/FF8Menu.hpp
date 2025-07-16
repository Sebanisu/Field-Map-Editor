//
// Created by pcvii on 11/24/2021.
//

#ifndef FIELD_MAP_EDITOR_FF8MENU_HPP
#define FIELD_MAP_EDITOR_FF8MENU_HPP
#include "Application.hpp"
#include "FF8UpscaleMenu.hpp"
#include "Fields.hpp"
#include "Menu.hpp"

namespace ff_8
{
class Ff8Menu
{
   public:
     Ff8Menu();
     void on_update(float) const;
     void on_render() const;
     void on_im_gui_update() const;
     void on_im_gui_menu() const;
     void on_event(const glengine::event::Item &) const;
     template<glengine::Renderable T>
     void push_back(std::string name) const
     {
          m_menu.push_back(
            std::move(name), [this]() -> glengine::MenuItem { return glengine::MenuItem(std::in_place_type_t<T>{}, GetFields()); });
     }

   private:
     glengine::Menu m_menu     = { "FF8" };
     Ff8UpscaleMenu m_swizzles = {};
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_FF8MENU_HPP
