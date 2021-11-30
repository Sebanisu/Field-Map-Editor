//
// Created by pcvii on 11/29/2021.
//

#include "TestClearColor.hpp"
#include "Renderer.hpp"
#include "Test.hpp"
#include <imgui.h>
static_assert(test::Test<test::TestClearColor>);

namespace test
{
static const Renderer renderer{};
}

test::TestClearColor::TestClearColor()
{
  renderer.ClearColor(m_clear_color);
}
void
  test::OnRender(const TestClearColor &)
{
  renderer.Clear();
}
void
  test::OnImGuiRender(const TestClearColor &self)
{
  if (ImGui::ColorEdit4("Clear Color", &self.m_clear_color.r))
  {
    renderer.ClearColor(self.m_clear_color);
  }
}

void
  test::OnUpdate(const TestClearColor &, float)
{
}