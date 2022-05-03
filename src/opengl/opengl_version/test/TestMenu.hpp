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
  void OnUpdate(float) const;
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const glengine::Event::Item &) const;

private:
  glengine::Menu m_menu{};
};
}// namespace test
#endif// FIELD_MAP_EDITOR_TESTMENU_HPP
