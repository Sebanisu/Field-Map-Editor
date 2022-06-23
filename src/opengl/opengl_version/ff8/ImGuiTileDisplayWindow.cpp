//
// Created by pcvii on 5/30/2022.
//

#include "ImGuiTileDisplayWindow.hpp"
static constinit ff_8::ImGuiTileDisplayWindow *CurrentWindow = nullptr;
void ff_8::ImGuiTileDisplayWindow::on_update(float) const
{
  m_drawn = false;
}
void ff_8::ImGuiTileDisplayWindow::on_render() const {}
void ff_8::ImGuiTileDisplayWindow::on_im_gui_update() const
{
  on_im_gui_update(m_current_id, []() {});
}
void ff_8::ImGuiTileDisplayWindow::on_event(const glengine::event::Item &) const
{
}
void ff_8::ImGuiTileDisplayWindow::take_control(
  const bool              has_hover,
  const glengine::Counter id)
{
  if (CurrentWindow && has_hover)
  {
    CurrentWindow->m_current_id = id;
  }
}
ff_8::ImGuiTileDisplayWindow::~ImGuiTileDisplayWindow()
{
  CurrentWindow = nullptr;
}
ff_8::ImGuiTileDisplayWindow::ImGuiTileDisplayWindow()
{
  CurrentWindow = this;
}
ff_8::ImGuiTileDisplayWindow *ff_8::ImGuiTileDisplayWindow::get_window()
{
  return CurrentWindow;
}
