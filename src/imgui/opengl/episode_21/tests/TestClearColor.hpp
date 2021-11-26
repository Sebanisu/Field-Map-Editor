//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TESTCLEARCOLOR_HPP
#define MYPROJECT_TESTCLEARCOLOR_HPP
#include "Test.h"
#include <array>
namespace test
{
class TestClearColor
{
public:
  void
    OnUpdate(float) const
  {
  }
  void
    OnRender() const;
  void
    OnImGuiRender() const;

private:
  mutable std::array<float, 4U> m_clear_color = { 0.2F, 0.3F, 0.8F, 1.F };
};
static_assert(Test<TestClearColor>);
}// namespace test
#endif// MYPROJECT_TESTCLEARCOLOR_HPP
