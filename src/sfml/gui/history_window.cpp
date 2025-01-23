#include "history_window.hpp"
#include "colors.hpp"
#include "format_imgui_text.hpp"
#include "gui/gui_labels.hpp"
#include "gui/push_pop_id.hpp"


void fme::history_window::render() const
{
     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("m_selections is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return;
     }
     if (!selections->display_history_window)
     {
          return;
     }
     const auto the_end = scope_guard([]() { ImGui::End(); });

     if (!ImGui::Begin(gui_labels::history.data()))
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
          spdlog::error("m_map_sprite is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return;
     }
     std::size_t                i             = {};

     std::optional<std::size_t> clicked_index = {};
     if (ImGui::CollapsingHeader("Undo", ImGuiTreeNodeFlags_DefaultOpen))
     {
          if (map_sprite->undo_enabled() && ImGui::BeginTable("MyTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
          {
               // Set up table headers
               ImGui::TableSetupColumn("##Index", ImGuiTableColumnFlags_WidthFixed, 50.0f);
               ImGui::TableSetupColumn("##Status", ImGuiTableColumnFlags_WidthFixed, 50.0f);
               ImGui::TableSetupColumn("##Text", ImGuiTableColumnFlags_WidthStretch);
               // ImGui::TableHeadersRow();
               std::ranges::for_each(map_sprite->undo_history() | std::ranges::views::reverse, [&](const auto &tuple) {
                    const auto &[index, status, text] = tuple;
                    ImGui::TableNextRow();
                    if (i % 2)
                    {
                         ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableDarkGray });// Dark gray
                    }
                    else
                    {
                         ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableLightDarkGray });// Slightly lighter gray
                    }
                    ++i;

                    // Column 1: Index
                    ImGui::TableSetColumnIndex(0);
                    ImVec2 const backup_pos = ImGui::GetCursorScreenPos();
                    const auto   pop_end    = scope_guard{ [&]() {
                         ImGui::PopStyleColor(2);
                         ImGui::SetCursorScreenPos(backup_pos);
                         format_imgui_text("{}", index);

                         // Column 2: Text
                         ImGui::TableSetColumnIndex(1);
                         format_imgui_text("{}", status);

                         // Column 3: Status
                         ImGui::TableSetColumnIndex(2);
                         format_imgui_text("{}", text);
                    } };

                    const auto pop_id = PushPopID();
                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colors::TableHoverDarkGray);
                    ImGui::PushStyleColor(ImGuiCol_HeaderActive, colors::TableActiveDarkGray);
                    if (ImGui::Selectable("##undo_history", false, ImGuiSelectableFlags_SpanAllColumns))
                    {
                         spdlog::info("Clicked on Index: {}", index);
                         clicked_index = index;
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
          if (map_sprite->redo_enabled() && ImGui::BeginTable("MyTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
          {
               // Set up table headers
               ImGui::TableSetupColumn("##Index", ImGuiTableColumnFlags_WidthFixed, 50.0f);
               ImGui::TableSetupColumn("##Status", ImGuiTableColumnFlags_WidthFixed, 50.0f);
               ImGui::TableSetupColumn("##Text", ImGuiTableColumnFlags_WidthStretch);

               std::ranges::for_each(map_sprite->redo_history() | std::ranges::views::reverse, [&](const auto &tuple) {
                    const auto &[index, status, text] = tuple;
                    ImGui::TableNextRow();

                    if (i % 2)
                    {
                         ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableDarkRed });// Dark red
                    }
                    else
                    {
                         ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableLightDarkRed });// Slightly lighter dark red
                    }
                    ++i;

                    // Column 1: Index
                    ImGui::TableSetColumnIndex(0);
                    ImVec2 const backup_pos = ImGui::GetCursorScreenPos();
                    const auto   pop_end    = scope_guard{ [&]() {
                         ImGui::PopStyleColor(2);
                         ImGui::SetCursorScreenPos(backup_pos);
                         format_imgui_text("{}", index);

                         // Column 2: Text
                         ImGui::TableSetColumnIndex(1);
                         format_imgui_text("{}", status);

                         // Column 3: Status
                         ImGui::TableSetColumnIndex(2);
                         format_imgui_text("{}", text);
                    } };
                    const auto pop_id = PushPopID();

                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colors::TableHoverDarkRed);
                    ImGui::PushStyleColor(ImGuiCol_HeaderActive, colors::TableActiveDarkRed);
                    if (ImGui::Selectable("##redo history", false, ImGuiSelectableFlags_SpanAllColumns))
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
void fme::history_window::update(const std::shared_ptr<map_sprite> &new_map_sprite) const
{
     m_map_sprite = new_map_sprite;
}
void fme::history_window::update(const std::shared_ptr<Selections> &new_selections) const
{
     m_selections = new_selections;
}