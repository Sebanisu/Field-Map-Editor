//
// Created by pcvii on 12/8/2021.
//

#include "LayerTests.hpp"

void layer::Tests::on_update(float ts) const
{
  test_menu.on_update(ts);
}
void layer::Tests::on_render() const
{
  test_menu.on_render();
}
void layer::Tests::on_im_gui_update() const
{
  if (ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    test_menu.on_im_gui_update();
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
void layer::Tests::on_event(const glengine::event::Item &e) const
{
  test_menu.on_event(e);
}