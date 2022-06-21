//
// Created by pcvii on 11/24/2021.
//

#ifndef FIELD_MAP_EDITOR_TESTMENU_HPP
#define FIELD_MAP_EDITOR_TESTMENU_HPP
#include "Event/EventItem.hpp"
#include "Menu.hpp"

namespace test
{
class TestMenu
{
public:
  TestMenu();
  void on_update(float) const;
  void on_render() const;
  void on_im_gui_update() const;
  void on_event(const glengine::event::Item &) const;

private:
  glengine::Menu m_menu{};
};
}// namespace test
#endif// FIELD_MAP_EDITOR_TESTMENU_HPP
