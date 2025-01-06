#include "collapsing_tile_info.hpp"
#include "create_tile_button.hpp"
#include "events.hpp"
#include "format_imgui_text.hpp"
#include "gui_labels.hpp"
#include "push_pop_id.hpp"
#include "scope_guard.hpp"
#include <sstream>
#include <string>
void collapsing_tile_info(
  const map_sprite &map,
  const std::variant<
    open_viii::graphics::background::Tile1,
    open_viii::graphics::background::Tile2,
    open_viii::graphics::background::Tile3,
    std::monostate> &current_tile,
  const std::size_t  index)
{
     using namespace open_viii::graphics::background;
     std::visit(
       events::make_visitor(
         [&](const is_tile auto &tile) {
              std::string title      = index == std::numeric_limits<size_t>::max()
                                         ? fmt::format("{}", gui_labels::selected_tile_info)
                                         : fmt::format("{}: {}", gui_labels::selected_tile_info, index);
              const auto  pushpopid0 = scope_guard{ &ImGui::PopID };
              ImGui::PushID(title.data());
              if (!ImGui::CollapsingHeader(title.data()))
              {
                   return;
              }
              const auto   pushpopid1 = PushPopID();
              ImVec2 const table_pos  = ImGui::GetCursorScreenPos();
              if (ImGui::BeginTable("table_tile_info", 2))
              {
                   // Configure columns
                   ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed);
                   ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                   const auto         the_end_table = scope_guard([]() { ImGui::EndTable(); });
                   const auto         tile_string   = fmt::format("{}", tile);
                   std::istringstream string_stream(tile_string);
                   std::string        line;
                   std::string        key;
                   std::string        value;
                   while (std::getline(string_stream, line, '\n'))
                   {
                        std::istringstream string_stream_line(line);
                        if (std::getline(string_stream_line, key, ':') && std::getline(string_stream_line, value))
                        {
                             ImGui::TableNextRow();
                             ImGui::TableNextColumn();
                             format_imgui_text("{}", std::string_view(key));
                             ImGui::TableNextColumn();
                             format_imgui_text("{}", std::string_view(value).substr(1));
                        }
                   }
              }

              ImVec2 const           backup_pos            = ImGui::GetCursorScreenPos();
              const float            width_max             = backup_pos.y - table_pos.y;
              const auto             width                 = width_max * 0.75F;
              ImGuiStyle const      &style                 = ImGui::GetStyle();
              static constexpr float position_width_scale  = 1.1F;
              static constexpr float position_height_scale = 0.9F;
              static constexpr float padding_height_scale  = 4.0F;
              static constexpr float tile_scale            = 0.9F;
              ImGui::SetCursorScreenPos(ImVec2(
                backup_pos.x + width * position_width_scale,
                backup_pos.y - width * position_height_scale - style.FramePadding.y * padding_height_scale));
              const auto pushpopid2 = PushPopID();
              (void)create_tile_button(map, tile, { width * tile_scale, width * tile_scale });
              ImGui::SetCursorScreenPos(backup_pos);
         },
         [](const std::monostate &) {}),
       current_tile);
}