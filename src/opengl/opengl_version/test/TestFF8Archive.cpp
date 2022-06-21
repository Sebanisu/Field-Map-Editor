//
// Created by pcvii on 11/30/2021.
//
#include "TestFF8Archive.hpp"

void test::TestFF8Archive::on_update(float ts) const
{
  ff_8_menu.on_update(ts);
}
void test::TestFF8Archive::on_render() const
{
  ff_8_menu.on_render();
}
void test::TestFF8Archive::on_im_gui_update() const
{
  ff_8_menu.on_im_gui_update();
}
void test::TestFF8Archive::on_event(const glengine::event::Item &e) const
{
  ff_8_menu.on_event(e);
}