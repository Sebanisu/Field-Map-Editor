//
// Created by pcvii on 12/8/2021.
//

#ifndef MYPROJECT_LAYERTESTS_HPP
#define MYPROJECT_LAYERTESTS_HPP

#include "EventItem.hpp"
#include "tests/Test.hpp"
#include "tests/TestMenu.hpp"
namespace Layer
{
class Tests
{
public:
  void OnUpdate(float) const;
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const Event::Item &) const;

private:
  test::TestMenu test_menu = {};
};

static_assert(test::Test<test::TestMenu>);
static_assert(test::Test<Tests>);
}// namespace Layer
#endif// MYPROJECT_LAYERTESTS_HPP
