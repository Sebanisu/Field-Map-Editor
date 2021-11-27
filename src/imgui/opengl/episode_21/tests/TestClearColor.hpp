//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TESTCLEARCOLOR_HPP
#define MYPROJECT_TESTCLEARCOLOR_HPP
#include "Renderer.hpp"
#include "Test.h"
#include <array>
#include <imgui.h>
namespace test
{
class TestClearColor
{
public:
  friend void
    OnUpdate(const TestClearColor &, float);
  friend void
    OnRender(const TestClearColor &);
  friend void
    OnImGuiRender(const TestClearColor &);

private:
  mutable std::array<float, 4U> m_clear_color = { 0.2F, 0.3F, 0.8F, 1.F };
};
static_assert(Test<TestClearColor>);
void
  OnRender(const TestClearColor &self)
{
  GLCall{ glClearColor,
          self.m_clear_color[0],
          self.m_clear_color[1],
          self.m_clear_color[2],
          self.m_clear_color[3] };
  GLCall{ glClear, GL_COLOR_BUFFER_BIT };
}
void
  OnImGuiRender(const TestClearColor &self)
{
  ImGui::ColorEdit4("Clear Color", std::data(self.m_clear_color));
}

void
  OnUpdate(const TestClearColor &, float)
{
}
}// namespace test
#endif// MYPROJECT_TESTCLEARCOLOR_HPP
