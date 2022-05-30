//
// Created by pcvii on 5/30/2022.
//

#include "ImGuiTileDisplayWindow.hpp"
static constinit ff8::ImGuiTileDisplayWindow *current_window = nullptr;
void ff8::ImGuiTileDisplayWindow::OnUpdate(float) const
{
  m_drawn = false;
}
void ff8::ImGuiTileDisplayWindow::OnRender() const {}
void ff8::ImGuiTileDisplayWindow::OnImGuiUpdate() const
{
  OnImGuiUpdate(m_current_id, []() {});
}
void ff8::ImGuiTileDisplayWindow::OnEvent(const glengine::Event::Item &) const
{
}
void ff8::ImGuiTileDisplayWindow::TakeControl(
  const bool              has_hover,
  const glengine::Counter id)
{
  if (current_window && has_hover)
  {
    current_window->m_current_id = id;
  }
}
ff8::ImGuiTileDisplayWindow::~ImGuiTileDisplayWindow()
{
  current_window = nullptr;
}
ff8::ImGuiTileDisplayWindow::ImGuiTileDisplayWindow()
{
  current_window = this;
}
ff8::ImGuiTileDisplayWindow *ff8::ImGuiTileDisplayWindow::GetWindow()
{
  return current_window;
}
