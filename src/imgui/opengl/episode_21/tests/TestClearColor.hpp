//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TESTCLEARCOLOR_HPP
#define MYPROJECT_TESTCLEARCOLOR_HPP
#include <array>
#include <glm/glm.hpp>
namespace test
{
class TestClearColor
{
public:
  TestClearColor();
  friend void
    OnUpdate(const TestClearColor &, float);
  friend void
    OnRender(const TestClearColor &);
  friend void
    OnImGuiUpdate(const TestClearColor &);

private:
  mutable glm::vec4 m_clear_color = { 0.2F, 0.3F, 0.8F, 1.F };
};
void
  OnUpdate(const TestClearColor &, float);
void
  OnRender(const TestClearColor &);
void
  OnImGuiUpdate(const TestClearColor &);
}// namespace test
#endif// MYPROJECT_TESTCLEARCOLOR_HPP
