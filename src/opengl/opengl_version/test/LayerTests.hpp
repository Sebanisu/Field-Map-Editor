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
  void on_update(float) const;
  void on_render() const;
  void on_im_gui_update() const;
  void on_event(const glengine::event::Item &) const;

private:
  test::TestMenu test_menu = {};
};

static_assert(glengine::Renderable<test::TestMenu>);
static_assert(glengine::Renderable<Tests>);
}// namespace Layer
#endif// FIELD_MAP_EDITOR_LAYERTESTS_HPP
