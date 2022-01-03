//
// Created by pcvii on 12/8/2021.
//

#include "LayerTests.hpp"

void Layer::Tests::OnUpdate(float ts) const
{
  test_menu.OnUpdate(ts);
}
void Layer::Tests::OnRender() const
{
  test_menu.OnRender();
}
void Layer::Tests::OnImGuiUpdate() const
{
  if (ImGui::Begin("Test Window", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    test_menu.OnImGuiUpdate();
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
void Layer::Tests::OnEvent(const glengine::Event::Item &e) const
{
  test_menu.OnEvent(e);
}