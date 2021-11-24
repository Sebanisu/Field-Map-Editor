//
// Created by pcvii on 11/24/2021.
//

#include "TestClearColor.hpp"
#include "Renderer.hpp"
#include <imgui.h>
namespace test
{
void
  TestClearColor::OnRender() const
{
  GLCall{ glClearColor,
          m_clear_color[0],
          m_clear_color[1],
          m_clear_color[2],
          m_clear_color[3] };
  GLCall{ glClear, GL_COLOR_BUFFER_BIT };
}
void
  TestClearColor::OnImGuiRender() const
{
  ImGui::ColorEdit4("Clear Color", std::data(m_clear_color));
}

}// namespace test