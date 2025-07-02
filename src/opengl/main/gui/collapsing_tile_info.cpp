#include "collapsing_tile_info.hpp"
#include "format_imgui_text.hpp"
#include "gui_labels.hpp"
#include "push_pop_id.hpp"
#include "tool_tip.hpp"
#include <IconsFontAwesome6.h>
#include <ScopeGuard.hpp>
#include <sstream>
#include <string>
namespace fme
{
bool collapsing_tile_info(
  std::weak_ptr<const map_sprite>                           map_ptr,
  const open_viii::graphics::background::Map::variant_tile &current_tile,
  const tile_button_options                                &options,
  const std::size_t                                         index)
{
     auto map = map_ptr.lock();
     if (!map)
     {
          return false;
     }
     using namespace open_viii::graphics::background;

     return std::visit(
       make_visitor(
         [&](const is_tile auto &tile) -> bool {
              std::string title      = index == std::numeric_limits<size_t>::max()
                                         ? fmt::format("{}", gui_labels::selected_tile_info)
                                         : fmt::format("{}: {}", gui_labels::selected_tile_info, index);
              const auto  pushpopid0 = glengine::ScopeGuard{ &ImGui::PopID };
              ImGui::PushID(title.data());
              // ImGui::SetItemAllowOverlap();
              const bool open = ImGui::CollapsingHeader(title.data(), ImGuiTreeNodeFlags_AllowOverlap);
              ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);// adjust for alignment
              if (ImGui::SmallButton(ICON_FA_TRASH))
              {
                   return true;
              }
              else
              {
                   tool_tip("Remove Selected Item");
              }
              if (!open)
              {
                   return false;
              }
              const auto   pushpopid1 = PushPopID();
              ImVec2 const table_pos  = ImGui::GetCursorScreenPos();
              if (ImGui::BeginTable("table_tile_info", 2))
              {
                   // Configure columns
                   ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed);
                   ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                   const auto         the_end_table = glengine::ScopeGuard([]() { ImGui::EndTable(); });
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
              auto options_with_size = options;
              options_with_size.size = { width * tile_scale, width * tile_scale };
              (void)create_tile_button(map, tile, options_with_size);
              ImGui::SetCursorScreenPos(backup_pos);
              return false;
         },
         [](const std::monostate &) -> bool { return false; }),
       current_tile);
}
}// namespace fme