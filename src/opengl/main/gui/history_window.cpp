#include "history_window.hpp"
#include "format_imgui_text.hpp"
#include "gui/ColorConversions.hpp"
#include "gui/gui_labels.hpp"
#include "gui/push_pop_id.hpp"
#include "tool_tip.hpp"

void fme::history_window::render() const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error(
            "m_selections is no longer valid. File: {}, Line: {}",
            __FILE__,
            __LINE__);
          return;
     }
     if (!selections->get<ConfigKey::DisplayHistoryWindow>())
     {
          return;
     }

     bool      &visible = selections->get<ConfigKey::DisplayHistoryWindow>();
     const auto pop_visible = glengine::ScopeGuard{
          [&selections, &visible, was_visable = visible]
          {
               if (was_visable != visible)
               {
                    selections->update<ConfigKey::DisplayHistoryWindow>();
               }
          }
     };
     const auto the_end = glengine::ScopeGuard([]() { ImGui::End(); });

     if (!ImGui::Begin(gui_labels::history.data(), &visible))
     {
          return;
     }
     // start adding elements here.
     draw_table();
}


void fme::history_window::draw_table() const
{

     auto map_sprite = m_map_sprite.lock();

     if (!map_sprite)
     {
          spdlog::error(
            "m_map_sprite is no longer valid. File: {}, Line: {}",
            __FILE__,
            __LINE__);
          return;
     }
     std::size_t                i             = {};

     std::optional<std::size_t> clicked_index = {};
     if (ImGui::CollapsingHeader("Undo", ImGuiTreeNodeFlags_DefaultOpen))
     {
          if (
            map_sprite->undo_enabled()
            && ImGui::BeginTable(
              "MyTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
          {
               // Set up table headers
               ImGui::TableSetupColumn(
                 "##Index", ImGuiTableColumnFlags_WidthFixed, 50.0f);
               ImGui::TableSetupColumn(
                 "##Status", ImGuiTableColumnFlags_WidthFixed, 50.0f);
               ImGui::TableSetupColumn(
                 "##Text", ImGuiTableColumnFlags_WidthStretch);
               // ImGui::TableHeadersRow();
               std::ranges::for_each(
                 map_sprite->undo_history() | std::ranges::views::reverse,
                 [&](const auto &tuple)
                 {
                      const auto &[index, status, text] = tuple;
                      ImGui::TableNextRow();
                      if (i % 2)
                      {
                           ImGui::TableSetBgColor(
                             ImGuiTableBgTarget_RowBg0,
                             ff_8::Colors::to_imU32(
                               ff_8::Colors::TableDarkGray));// Dark gray
                      }
                      else
                      {
                           ImGui::TableSetBgColor(
                             ImGuiTableBgTarget_RowBg0,
                             ff_8::Colors::to_imU32(
                               ff_8::Colors::TableLightDarkGray));// Slightly
                                                                  // lighter
                                                                  // gray
                      }
                      ++i;

                      // Column 1: Index
                      ImGui::TableSetColumnIndex(0);
                      ImVec2 const backup_pos = ImGui::GetCursorScreenPos();
                      const auto   pop_end    = glengine::ScopeGuard{
                           [&]()
                           {
                                ImGui::PopStyleColor(2);
                                ImGui::SetCursorScreenPos(backup_pos);
                                format_imgui_text("{}", index);

                                // Column 2: Text
                                ImGui::TableSetColumnIndex(1);
                                format_imgui_text("{}", status);

                                // Column 3: Status
                                ImGui::TableSetColumnIndex(2);
                                format_imgui_text("{}", text);
                           }
                      };

                      const auto pop_id = PushPopID();
                      ImGui::PushStyleColor(
                        ImGuiCol_HeaderHovered,
                        ff_8::Colors::to_imvec4(
                          ff_8::Colors::TableDarkGrayHovered));
                      ImGui::PushStyleColor(
                        ImGuiCol_HeaderActive,
                        ff_8::Colors::to_imvec4(
                          ff_8::Colors::TableDarkGrayActive));
                      if (ImGui::Selectable(
                            "##undo_history",
                            false,
                            ImGuiSelectableFlags_SpanAllColumns))
                      {
                           spdlog::info("Clicked on Index: {}", index);
                           clicked_index = index;
                      }
                      else
                      {
                           tool_tip(text);
                      }
                 });
               if (clicked_index)
               {
                    for (std::size_t j = {}; j <= clicked_index.value(); ++j)
                    {
                         map_sprite->undo();
                    }
                    clicked_index.reset();
               }
               ImGui::EndTable();
          }
          else
          {
               format_imgui_text("{}", "No states to undo...");
          }
     }
     if (ImGui::CollapsingHeader("Redo", ImGuiTreeNodeFlags_DefaultOpen))
     {
          if (
            map_sprite->redo_enabled()
            && ImGui::BeginTable(
              "MyTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
          {
               // Set up table headers
               ImGui::TableSetupColumn(
                 "##Index", ImGuiTableColumnFlags_WidthFixed, 50.0f);
               ImGui::TableSetupColumn(
                 "##Status", ImGuiTableColumnFlags_WidthFixed, 50.0f);
               ImGui::TableSetupColumn(
                 "##Text", ImGuiTableColumnFlags_WidthStretch);

               std::ranges::for_each(
                 map_sprite->redo_history() | std::ranges::views::reverse,
                 [&](const auto &tuple)
                 {
                      const auto &[index, status, text] = tuple;
                      ImGui::TableNextRow();

                      if (i % 2)
                      {
                           ImGui::TableSetBgColor(
                             ImGuiTableBgTarget_RowBg0,
                             ff_8::Colors::to_imU32(
                               ff_8::Colors::TableDarkRed));// Dark red
                      }
                      else
                      {
                           ImGui::TableSetBgColor(
                             ImGuiTableBgTarget_RowBg0,
                             ff_8::Colors::to_imU32(
                               ff_8::Colors::TableLightDarkRed));// Slightly
                                                                 // lighter dark
                                                                 // red
                      }
                      ++i;

                      // Column 1: Index
                      ImGui::TableSetColumnIndex(0);
                      ImVec2 const backup_pos = ImGui::GetCursorScreenPos();
                      const auto   pop_end    = glengine::ScopeGuard{
                           [&]()
                           {
                                ImGui::PopStyleColor(2);
                                ImGui::SetCursorScreenPos(backup_pos);
                                format_imgui_text("{}", index);

                                // Column 2: Text
                                ImGui::TableSetColumnIndex(1);
                                format_imgui_text("{}", status);

                                // Column 3: Status
                                ImGui::TableSetColumnIndex(2);
                                format_imgui_text("{}", text);
                           }
                      };
                      const auto pop_id = PushPopID();

                      ImGui::PushStyleColor(
                        ImGuiCol_HeaderHovered,
                        ff_8::Colors::to_imvec4(
                          ff_8::Colors::TableDarkRedHovered));
                      ImGui::PushStyleColor(
                        ImGuiCol_HeaderActive,
                        ff_8::Colors::to_imvec4(
                          ff_8::Colors::TableDarkRedActive));
                      if (ImGui::Selectable(
                            "##redo history",
                            false,
                            ImGuiSelectableFlags_SpanAllColumns))
                      {
                           spdlog::info("Clicked on Index: {}", index);
                           clicked_index = index;
                      }
                 });

               if (clicked_index)
               {
                    for (std::size_t j = {}; j <= clicked_index.value(); ++j)
                    {
                         map_sprite->redo();
                    }
                    clicked_index.reset();
               }

               ImGui::EndTable();
          }
          else
          {
               format_imgui_text("{}", "No states to redo...");
          }
     }
}
void fme::history_window::update(
  const std::shared_ptr<map_sprite> &new_map_sprite) const
{
     m_map_sprite = new_map_sprite;
}
void fme::history_window::update(
  const std::shared_ptr<Selections> &new_selections) const
{
     m_selections = new_selections;
}