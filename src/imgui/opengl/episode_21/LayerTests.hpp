//
// Created by pcvii on 12/8/2021.
//

#ifndef MYPROJECT_LAYERTESTS_HPP
#define MYPROJECT_LAYERTESTS_HPP

#include "tests/Test.hpp"
#include "tests/TestMenu.hpp"
#include <EventItem.hpp>
namespace Layer
{
class Tests
{
public:
  Tests()              = default;
  Tests(const Tests &) = delete;
  Tests &
    operator=(const Tests &) = delete;
  Tests(Tests &&) noexcept   = default;
  Tests &
    operator=(Tests &&) noexcept = default;
  friend void
    OnUpdate(const Tests &, float);
  friend void
    OnRender(const Tests &);
  friend void
    OnImGuiUpdate(const Tests &);
  friend void
    OnEvent(const Tests &, const Event::Item &);

private:
  test::TestMenu test_menu = {};
};

static_assert(test::Test<test::TestMenu>);
static_assert(test::Test<Tests>);
void
  OnUpdate(const Tests &, float);
void
  OnRender(const Tests &);
void
  OnImGuiUpdate(const Tests &);
void
  OnEvent(const Tests &, const Event::Item &);
}// namespace Layer
#endif// MYPROJECT_LAYERTESTS_HPP
