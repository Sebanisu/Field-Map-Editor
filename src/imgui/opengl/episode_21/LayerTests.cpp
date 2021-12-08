//
// Created by pcvii on 12/8/2021.
//

#include "LayerTests.hpp"
#include <imgui.h>


void
  Layer::OnUpdate(const Tests &self, float ts)
{
  test::OnUpdate(self.test_menu, ts);
}
void
  Layer::OnRender(const Tests &self)
{
  test::OnRender(self.test_menu);
}
void
  Layer::OnImGuiUpdate(const Tests &self)
{
  if (ImGui::Begin("Test Window", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    test::OnImGuiUpdate(self.test_menu);
    ImGui::Text(
      "%s",
      fmt::format(
        "Application average {:.3f} ms/frame ({:.3f} FPS)",
        1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate)
        .c_str());
  }
  ImGui::End();
}
void
  Layer::OnEvent(const Tests &self, const Event::Item &e)
{
  test::OnEvent(self.test_menu, e);
}