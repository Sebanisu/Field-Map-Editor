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
  const open_viii::graphics::background::Map::variant_tile &in_original_tile,
  const open_viii::graphics::background::Map::variant_tile &in_working_tile,
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
         [&](const is_tile auto &original_tile, const is_tile auto &working_tile) -> bool {
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
                   auto split_lines = [](const std::string &str) {
                        return std::views::split(str, '\n')
                               | std::views::transform([](auto &&r) { return std::string_view(&*r.begin(), std::ranges::distance(r)); });
                   };

                   // Configure columns
                   ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed);
                   ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                   const auto        the_end_table        = glengine::ScopeGuard([]() { ImGui::EndTable(); });
                   const std::string original_tile_string = fmt::format("{}", original_tile);
                   const std::string working_tile_string  = fmt::format("{}", working_tile);
                   auto              split_original_tile  = split_lines(original_tile_string);
                   auto              split_working_tile   = split_lines(working_tile_string);
                   auto              zipped_tile_info     = std::views::zip(split_working_tile, split_original_tile);
                   for (auto [new_line, old_line] : zipped_tile_info)
                   {
                        auto split_kv = [](std::string_view line) -> std::pair<std::string_view, std::string_view> {
                             if (auto pos = line.find(':'); pos != std::string_view::npos)
                             {
                                  return { line.substr(0, pos), line.substr(pos + 1) };
                             }
                             return { line, {} };
                        };
                        auto [new_key, new_val] = split_kv(new_line);
                        auto [old_key, old_val] = split_kv(old_line);

                        assert(new_key == old_key);

                        new_val = new_val.starts_with(' ') ? new_val.substr(1) : new_val;
                        old_val = old_val.starts_with(' ') ? old_val.substr(1) : old_val;

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        format_imgui_text("{}", new_key);

                        ImGui::TableNextColumn();

                        const bool changed = new_val != old_val;

                        if (changed)
                             ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colors::Peach));

                        format_imgui_text("{}", new_val);

                        if (changed)
                        {

                             ImGui::PopStyleColor();
                             tool_tip(old_val);
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
              (void)create_tile_button(map, original_tile, options_with_size);
              ImGui::SetCursorScreenPos(backup_pos);
              return false;
         },
         [](...) -> bool { return false; }),
       in_original_tile,
       in_working_tile);
}
}// namespace fme