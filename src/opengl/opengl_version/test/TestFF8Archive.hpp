//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_TESTFF8ARCHIVE_HPP
#define MYPROJECT_TESTFF8ARCHIVE_HPP
#include "ff8/FF8Menu.hpp"
namespace test
{
class TestFF8Archive
{
public:
  void OnUpdate(float) const;
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const glengine::Event::Item &) const;

private:
  ff8::FF8Menu ff8_menu = {};
};
}// namespace test
#endif// MYPROJECT_TESTFF8ARCHIVE_HPP
