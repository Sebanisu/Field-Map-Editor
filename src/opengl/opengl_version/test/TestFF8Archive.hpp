//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_TESTFF8ARCHIVE_HPP
#define FIELD_MAP_EDITOR_TESTFF8ARCHIVE_HPP
#include "ff8/FF8Menu.hpp"
namespace test
{
class TestFF8Archive
{
public:
  void on_update(float) const;
  void on_render() const;
  void on_im_gui_update() const;
  void on_event(const glengine::event::Item &) const;

private:
  ff_8::FF8Menu ff_8_menu = {};
};
}// namespace test
#endif// FIELD_MAP_EDITOR_TESTFF8ARCHIVE_HPP
