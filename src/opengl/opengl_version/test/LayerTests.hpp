//
// Created by pcvii on 12/8/2021.
//

#ifndef FIELD_MAP_EDITOR_LAYERTESTS_HPP
#define FIELD_MAP_EDITOR_LAYERTESTS_HPP

#include "Event/EventItem.hpp"

#include "test/TestMenu.hpp"
namespace Layer
{
class Tests
{
public:
  void OnUpdate(float) const;
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const glengine::Event::Item &) const;

private:
  test::TestMenu test_menu = {};
};

static_assert(glengine::Renderable<test::TestMenu>);
static_assert(glengine::Renderable<Tests>);
}// namespace Layer
#endif// FIELD_MAP_EDITOR_LAYERTESTS_HPP
