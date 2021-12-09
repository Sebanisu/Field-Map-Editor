//
// Created by pcvii on 11/29/2021.
//

#include "TestClearColor.hpp"
#include "Renderer.hpp"
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
void test::TestClearColor::OnRender() const
{
  renderer.Clear();
}
void test::TestClearColor::OnImGuiUpdate() const
{
  if (ImGui::ColorEdit4("Clear Color", &m_clear_color.r))
  {
    renderer.ClearColor(m_clear_color);
  }
}
