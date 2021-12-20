//
// Created by pcvii on 11/29/2021.
//

#include "TestClearColor.hpp"
#include "Renderer.hpp"

static_assert(Renderable<test::TestClearColor>);
namespace test
{
static constinit Renderer::Clear_impl LocalClear = {};
}
test::TestClearColor::TestClearColor()
{
  LocalClear.Color(m_clear_color);
}
void test::TestClearColor::OnRender() const
{
  LocalClear();
}
void test::TestClearColor::OnImGuiUpdate() const
{
  if (ImGui::ColorEdit4("Clear Color", &m_clear_color.r))
  {
    LocalClear.Color(m_clear_color);
  }
}
