//
// Created by pcvii on 12/8/2021.
//

#include "LayerTests.hpp"
#include <Event/EventDispatcher.hpp>
#include <ScopeGuard.hpp>

void layer::Tests::on_update(float ts) const
{
  test_menu.on_update(ts);
  ff_8_menu.on_update(ts);
}
void layer::Tests::on_render() const
{
  test_menu.on_render();
  ff_8_menu.on_render();
}
void layer::Tests::on_im_gui_update() const
{
  ImVec2 vWindowSize = ImGui::GetMainViewport()->Size;
  ImVec2 vPos0       = ImGui::GetMainViewport()->Pos;

  ImGui::SetNextWindowPos(
    ImVec2((float)vPos0.x, (float)vPos0.y), ImGuiCond_Always);
  ImGui::SetNextWindowSize(
    ImVec2((float)vWindowSize.x, (float)vWindowSize.y), ImGuiCond_Always);
  {
    const auto pop_style = glengine::ImGuiPushStyleVar(
      ImGuiStyleVar_WindowPadding, ImVec2(0.F, 0.F));
    if (!ImGui::Begin(
          "Control Panel",
          nullptr,
          ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoBringToFrontOnFocus
            | ImGuiWindowFlags_NoTitleBar))
    {
      return;
    }
    static const ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
    ImGuiID dockSpace = ImGui::GetID("MainWindowDockspace");
    ImGui::DockSpace(dockSpace, ImVec2(0.0f, 0.0f), dockspaceFlags);
  }
  auto const window_end = glengine::ScopeGuard{ []() { ImGui::End(); } };
  if (ImGui::BeginMainMenuBar())
  {
    const auto end_menubar =
      glengine::ScopeGuard{ []() { ImGui::EndMainMenuBar(); } };
    if (ImGui::BeginMenu("Edit"))
    {
      auto const menu_end = glengine::ScopeGuard{ []() { ImGui::End(); } };
      if (ImGui::MenuItem(
            "Undo", "CTRL+Z", false, GetMapHistory()->undo_enabled()))
      {
        (void)GetMapHistory()->undo();
        GetWindow().trigger_refresh_image();
      }
      if (ImGui::MenuItem(
            "Redo", "CTRL+Y", false, GetMapHistory()->redo_enabled()))
      {
        (void)GetMapHistory()->redo();
        GetWindow().trigger_refresh_image();
      }
      if (ImGui::MenuItem(
            fmt::format("Undo All ({})", GetMapHistory()->count()).c_str(),
            "SHIFT+CTRL+Z",
            false,
            GetMapHistory()->undo_enabled()))
      {
        GetMapHistory()->undo_all();
        GetWindow().trigger_refresh_image();
      }
      if (ImGui::MenuItem(
            fmt::format("Redo All ({})", GetMapHistory()->redo_count()).c_str(),
            "SHIFT+CTRL+Y",
            false,
            GetMapHistory()->redo_enabled()))
      {
        (void)GetMapHistory()->redo_all();
        GetWindow().trigger_refresh_image();
      }
    }
    ff_8_menu.on_im_gui_menu();
    test_menu.on_im_gui_menu();
  }
  if (ImGui::Begin("FPS"))
  {
    ImGui::Text(
      "%s",
      fmt::format(
        "Application average {:.3f} ms/frame ({:.3f} FPS)",
        1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate)
        .c_str());
  }
  test_menu.on_im_gui_update();
  ff_8_menu.on_im_gui_update();
}
void layer::Tests::on_event(const glengine::event::Item &e) const
{
  test_menu.on_event(e);
  ff_8_menu.on_event(e);
  const auto dispatcher = glengine::event::Dispatcher(e);
  dispatcher.Dispatch<glengine::event::KeyPressed>(
    [](glengine::event::KeyPressed key_pressed) -> bool {
      if (
        (key_pressed.key() == glengine::Key::Z)
        && (+key_pressed.mods() & +glengine::Mods::Control) != 0)
      {
        if ((+key_pressed.mods() & +glengine::Mods::Shift) != 0)
        {
          GetMapHistory()->undo_all();
          GetWindow().trigger_refresh_image();
        }
        else
        {
          (void)GetMapHistory()->undo();
          GetWindow().trigger_refresh_image();
        }
      }
      if (
        (key_pressed.key() == glengine::Key::Y)
        && (+key_pressed.mods() & +glengine::Mods::Control) != 0)
      {
        if ((+key_pressed.mods() & +glengine::Mods::Shift) != 0)
        {
          GetMapHistory()->redo_all();
          GetWindow().trigger_refresh_image();
        }
        else
        {
          (void)GetMapHistory()->redo();
          GetWindow().trigger_refresh_image();
        }
      }

      return true;
    });
}