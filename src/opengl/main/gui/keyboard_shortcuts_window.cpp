#include "keyboard_shortcuts_window.hpp"
#include <ScopeGuard.hpp>
fme::keyboard_shortcuts_window::keyboard_shortcuts_window(std::weak_ptr<Selections> selections)
  : m_selections(selections)
{
}

void fme::keyboard_shortcuts_window::render() const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          return;
     }
     bool       show_keyboard_shortcuts = selections->get<ConfigKey::DisplayKeyboardShortcutsWindow>();
     const auto pop_enabled             = glengine::ScopeGuard(
       [&]()
       {
            if (show_keyboard_shortcuts != selections->get<ConfigKey::DisplayKeyboardShortcutsWindow>())
            {
                 selections->get<ConfigKey::DisplayKeyboardShortcutsWindow>() = show_keyboard_shortcuts;
                 selections->update<ConfigKey::DisplayKeyboardShortcutsWindow>();
            }
       });
     if (!show_keyboard_shortcuts)
     {
          return;
     }
     const auto pop_end = glengine::ScopeGuard(&ImGui::End);
     ImGui::Begin("Keyboard Shortcuts", &show_keyboard_shortcuts);

     ImGui::TextWrapped("A quick reference for keyboard shortcuts and mouse + modifier behaviors.");

     if (ImGui::CollapsingHeader("Global Hotkeys"))
     {
          ImGui::Text("Navigation:");
          ImGui::BulletText("Esc - Context sensitive: clear clicked tiles or file selection");
          ImGui::BulletText("Page Up / Page Down - Switch field maps");
          ImGui::BulletText("Ctrl + Page Up/Down - Cycle COO (remaster only)");

          ImGui::Separator();

          ImGui::Text("Undo / Redo (Tile Editing Only):");
          ImGui::BulletText("Ctrl + Z - Undo");
          ImGui::BulletText("Ctrl + Y - Redo");
          ImGui::BulletText("Ctrl + Shift + Z - Undo all");
          ImGui::BulletText("Ctrl + Shift + Y - Redo all");
     }

     if (ImGui::CollapsingHeader("Window Toggles"))
     {
          ImGui::BulletText("Ctrl + H - History");
          ImGui::BulletText("Ctrl + B - Batch");
          ImGui::BulletText("Ctrl + D - Draw");
          ImGui::BulletText("Ctrl + U - Custom Paths");
          ImGui::BulletText("Ctrl + F - Field File");
          ImGui::BulletText("Ctrl + P - Control Panel");
     }

     if (ImGui::CollapsingHeader("File List"))
     {
          ImGui::Text("Keyboard:");
          ImGui::BulletText("Esc or Ctrl+Shift+A - Clear selection");
          ImGui::BulletText("Ctrl + A - Select all");
          ImGui::BulletText("Ctrl + I - Invert selection");

          ImGui::Text("Mouse + Modifiers:");
          ImGui::BulletText("Click - Select single file");
          ImGui::BulletText("Ctrl + Click - Toggle file in selection");
          ImGui::BulletText("Shift + Click - Add range (with/without anchor)");
          ImGui::BulletText("Alt + Click - Set/remove anchor ranges");
     }
}