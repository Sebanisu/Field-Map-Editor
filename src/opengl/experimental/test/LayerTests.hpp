//
// Created by pcvii on 12/8/2021.
//

#ifndef FIELD_MAP_EDITOR_LAYERTESTS_HPP
#define FIELD_MAP_EDITOR_LAYERTESTS_HPP

#include <glengine/Event/EventItem.hpp>

#include "ff8/FF8Menu.hpp"
#include "test/TestMenu.hpp"
namespace layer
{
// todo rename.
class Tests
{
   public:
     void on_update(float) const;
     void on_render() const;
     void on_im_gui_update() const;
     void on_event(const glengine::event::Item &) const;

   private:
     test::TestMenu test_menu = {};
     ff_8::Ff8Menu  ff_8_menu = {};
     void           main_menu() const;
     void           fps() const;
     //  void           dock_window() const;
};

static_assert(glengine::Renderable<test::TestMenu>);
static_assert(glengine::Renderable<Tests>);
}// namespace layer
#endif// FIELD_MAP_EDITOR_LAYERTESTS_HPP
