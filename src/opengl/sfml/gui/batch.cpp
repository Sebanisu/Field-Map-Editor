//
// Created by pcvii on 3/7/2023.
//

#include "batch.hpp"
#include "as_string.hpp"
#include "Configuration.hpp"
#include "custom_paths_window.hpp"
#include "open_file_explorer.hpp"
#include "push_pop_id.hpp"
#include "tool_tip.hpp"
#include <optional>
namespace fme
{
static ImGuiTreeNodeFlags flags{};
}
/**
 * @brief Safely copies a null-terminated string from one contiguous range to another.
 *
 * This function copies characters from the source range into the destination range,
 * ensuring that no buffer overflows occur. The destination is null-terminated.
 *
 * @tparam Src Source contiguous range type (e.g., std::string, std::array<char>)
 * @tparam Dst Destination contiguous range type (e.g., std::array<char>)
 * @param src The source string-like range.
 * @param dst The destination character buffer.
 * @return true if the copy succeeded and the resulting path exists and is a directory.
 * @return false if the destination was too small or the path is not a valid directory.
 */
template<std::ranges::contiguous_range Src, std::ranges::contiguous_range Dst>
     requires std::indirectly_copyable<std::ranges::iterator_t<Src>, std::ranges::iterator_t<Dst>>
static bool safe_copy_string(const Src &src, Dst &dst)
{
     const auto src_size = std::ranges::size(src);
     const auto dst_size = std::ranges::size(dst);

     if (src_size + 1 > dst_size)
     {
          spdlog::error("safe_copy_string: destination buffer too small ({} < {}).", dst_size, src_size + 1);
          return false;
     }

     std::ranges::copy_n(std::ranges::begin(src), src_size, std::ranges::data(dst));
     dst[src_size]  = '\0';

     const auto tmp = safedir(std::ranges::data(dst));
     return tmp.is_dir() && tmp.is_exists();
}

void fme::batch::draw_window()
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     bool      &visible     = selections->display_batch_window;
     const auto pop_visible = glengine::ScopeGuard{ [&selections, &visible, was_visable = visible] {
          if (was_visable != visible)
          {
               selections->update_configuration_key(ConfigKey::DisplayBatchWindow);
          }
     } };
     const auto end = glengine::ScopeGuard(&ImGui::End);
     if (!ImGui::Begin(gui_labels::batch_operation_window.data(), &visible))
     {
          return;
     }
     open_directory_browser();
     // while in_progress disable changing the values.
     const bool disabled = in_progress();
     ImGui::BeginDisabled(disabled);
     // input values
     combo_input_type();
     browse_input_path();
     checkbox_load_map();
     // output values
     combo_output_type();
     browse_output_path();
     checkmark_save_map();
     // automatic processing via compact or flatten

     if (ImGui::CollapsingHeader(gui_labels::compact_flatten.data(), flags))
     {
          format_imgui_wrapped_text("{}", gui_labels::compact_flatten_warning);
          combo_compact_type();
          combo_flatten_type();
     }
     // toggle maps to be processed.

     const auto archives_group = m_archives_group.lock();
     if (archives_group)
     {
          if (draw_multi_column_list_box("Map List", archives_group->mapdata(), selections->batch_map_list_enabled))
          {
               selections->update_configuration_key(ConfigKey::BatchMapListEnabled);
          }
     }
     else
     {
          spdlog::error("Failed to lock m_archives_group: shared_ptr is expired.");
     }
     // click to start processing
     button_start();
     ImGui::EndDisabled();
     ImGui::BeginDisabled(!disabled);
     // click to stop processsing.
     button_stop();
     ImGui::EndDisabled();
     if (!disabled)
     {
          return;
     }
     format_imgui_text("{}", m_status);
}

void fme::batch::combo_input_type()
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     static constexpr auto values = std::array{ input_types::mim, input_types::deswizzle, input_types::swizzle };
     static constexpr auto tooltips =
       std::array{ gui_labels::input_mim_tooltip, gui_labels::input_deswizzle_tooltip, gui_labels::input_swizzle_tooltip };
     const auto gcc = fme::GenericComboClass(
       gui_labels::input_type,
       []() { return values; },
       []() { return values | std::views::transform(AsString{}); },
       []() { return tooltips; },
       selections->batch_input_type);
     if (gcc.render())
     {
          selections->update_configuration_key(ConfigKey::BatchInputType);
     }
}


static void
  render_output_example_table(const char *table_id, const std::string &main_pattern, const std::string &map_pattern, bool show_map)
{
     if (ImGui::BeginTable(table_id, 1, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchSame))
     {
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          format_imgui_wrapped_text("{}", main_pattern);

          if (show_map)
          {
               ImGui::TableNextRow();
               ImGui::TableNextColumn();
               format_imgui_wrapped_text("{}", map_pattern);
          }

          ImGui::EndTable();
     }
};
// remember to pass in a path string that won't go poof.
static const std::string &get_selected_path(const std::string &custom_path, fme::root_path_types path_type)
{
     static const std::string ff8_path     = "{ff8_path}";
     static const std::string current_path = "{current_path}";

     switch (path_type)
     {
          case fme::root_path_types::ff8_path:
               return ff8_path;
          case fme::root_path_types::current_path:
               return current_path;
          default:
               return custom_path;
     }
}

void fme::batch::example_input_paths()
{
     if (!ImGui::CollapsingHeader("Example Input Paths", ImGuiTreeNodeFlags_DefaultOpen))
     {
          return;
     }
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     static const key_value_data png_example     = { .field_name    = "field01",
                                                     .ext           = ".png",
                                                     .language_code = open_viii::LangT::en,
                                                     .palette       = std::uint8_t{ 0 },
                                                     .texture_page  = std::uint8_t{ 5 },
                                                     .pupu_id       = 9999U };
     static const key_value_data map_example     = { .field_name = "field01", .ext = ".map", .language_code = open_viii::LangT::en };
     static const std::string    ff8_path        = "{ff8_path}";
     static const std::string    current_path    = "{current_path}";

     const std::string          &selected_string = get_selected_path(selections->batch_input_path, selections->batch_input_root_path_type);

     // currently input and output use the same patterns this might change later.
     render_output_example_table(
       "DeSwizzleOutputExampleTable",
       png_example.replace_tags(fme::batch::get_output_pattern(selections->batch_input_type), selections, selected_string),
       map_example.replace_tags(fme::batch::get_output_map_pattern(selections->batch_input_type), selections, selected_string),
       selections->batch_output_save_map);
}

const std::string &fme::batch::get_output_pattern(fme::input_types type)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          static const std::string empty{};
          return empty;
     }
     switch (type)
     {
          default:
          case fme::input_types::swizzle:
               return selections->output_swizzle_pattern;
          case fme::input_types::deswizzle:
               return selections->output_deswizzle_pattern;
     }
}

const std::string &fme::batch::get_output_pattern(fme::output_types type)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          static const std::string empty{};
          return empty;
     }
     switch (type)
     {
          case fme::output_types::swizzle:
          case fme::output_types::swizzle_as_one_image:
               return selections->output_swizzle_pattern;
          default:
          case fme::output_types::deswizzle:
               return selections->output_deswizzle_pattern;
     }
}

const std::string &fme::batch::get_output_map_pattern(fme::input_types type)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          static const std::string empty{};
          return empty;
     }
     switch (type)
     {
          default:
          case fme::input_types::swizzle:
               return selections->output_map_pattern_for_swizzle;
          case fme::input_types::deswizzle:
               return selections->output_map_pattern_for_deswizzle;
     }
}

const std::string &fme::batch::get_output_map_pattern(fme::output_types type)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          static const std::string empty{};
          return empty;
     }
     switch (type)
     {
          case fme::output_types::swizzle:
          case fme::output_types::swizzle_as_one_image:
               return selections->output_map_pattern_for_swizzle;
          default:
          case fme::output_types::deswizzle:
               return selections->output_map_pattern_for_deswizzle;
     }
}

void fme::batch::example_output_paths()
{

     if (!ImGui::CollapsingHeader("Example Output Paths", ImGuiTreeNodeFlags_DefaultOpen))
     {
          return;
     }
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }

     static const key_value_data png_example = { .field_name    = "field01",
                                                 .ext           = ".png",
                                                 .language_code = open_viii::LangT::en,
                                                 .palette       = std::uint8_t{ 0 },
                                                 .texture_page  = std::uint8_t{ 5 },
                                                 .pupu_id       = 9999U };
     static const key_value_data map_example = { .field_name = "field01", .ext = ".map", .language_code = open_viii::LangT::en };

     const std::string &selected_string      = get_selected_path(selections->batch_output_path, selections->batch_output_root_path_type);

     render_output_example_table(
       "DeSwizzleOutputExampleTable",
       png_example.replace_tags(fme::batch::get_output_pattern(selections->batch_output_type), selections, selected_string),
       map_example.replace_tags(fme::batch::get_output_map_pattern(selections->batch_output_type), selections, selected_string),
       selections->batch_output_save_map);
}

void fme::batch::save_input_path()
{
     if (!m_input_path_valid)
     {
          return;
     }
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     selections->batch_input_path = m_input_path.data();
     spdlog::info("batch_input_path: {}", selections->batch_input_path);
     selections->update_configuration_key(ConfigKey::BatchInputPath);
}

void fme::batch::save_output_path()
{
     if (!m_output_path_valid)
     {
          return;
     }
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     selections->batch_output_path = m_output_path.data();
     spdlog::info("batch_output_path: {}", selections->batch_input_path);
     selections->update_configuration_key(ConfigKey::BatchOutputPath);
}

void fme::batch::checkbox_load_map()
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     if (!(selections->batch_input_type != input_types::mim))
     {
          return;
     }
     const auto pop_id = PushPopID();
     if (!ImGui::Checkbox(gui_labels::batch_load_map.data(), &selections->batch_input_load_map))
     {
          return;
     }
     spdlog::info("batch_input_load_map: {}", selections->batch_input_load_map);
     selections->update_configuration_key(ConfigKey::BatchInputLoadMap);
}

void fme::batch::combo_output_type()
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     static constexpr auto values = std::array{ output_types::deswizzle, output_types::swizzle, output_types::swizzle_as_one_image };
     const auto            gcc    = fme::GenericComboClass(
       gui_labels::output_type,
       []() { return values; },
       []() { return values | std::views::transform(AsString{}); },
       selections->batch_output_type);
     if (!gcc.render())
     {
          return;
     }
     selections->update_configuration_key(ConfigKey::BatchOutputType);
}


void fme::batch::browse_input_path()
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     if (selections->batch_input_type == input_types::mim)
     {
          // using embedded images from ff8.
          return;
     }

     static constexpr auto values = std::array{ root_path_types::selected_path, root_path_types::ff8_path, root_path_types::current_path };
     static constexpr auto tooltips =
       std::array{ gui_labels::selected_path_tooltip, gui_labels::ff8_path_tooltip, gui_labels::current_path_tooltip };
     const auto gcc = fme::GenericComboClass(
       gui_labels::input_root_path_type,
       []() { return values; },
       []() { return values | std::views::transform(AsString{}); },
       []() { return tooltips; },
       selections->batch_input_root_path_type);
     if (gcc.render())
     {
          selections->update_configuration_key(ConfigKey::BatchInputRootPathType);
     }
     if (selections->batch_input_root_path_type != root_path_types::selected_path)
     {
          const float        button_size      = ImGui::GetFrameHeight();
          const float        button_width     = button_size * 3.0F;
          const auto         pop_id           = PushPopID();
          const std::string &selected_string  = get_selected_path(selections->batch_input_path, selections->batch_input_root_path_type);
          std::string        processed_string = key_value_data::static_replace_tags(selected_string, selections);
          if (ImGui::Button(gui_labels::explore.data(), ImVec2{ button_width, button_size }))
          {
               open_directory(processed_string);
          }
          else
          {
               tool_tip(processed_string);
          }
     }

     const bool path_changed = selections->batch_input_root_path_type == root_path_types::selected_path
                               && browse_path(gui_labels::input_path, m_input_path_valid, m_input_path);
     example_input_paths();
     if (!path_changed)
     {
          return;
     }
     save_input_path();
}

void fme::batch::browse_output_path()
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     static constexpr auto values = std::array{ root_path_types::selected_path, root_path_types::ff8_path, root_path_types::current_path };
     static constexpr auto tooltips =
       std::array{ gui_labels::selected_path_tooltip, gui_labels::ff8_path_tooltip, gui_labels::current_path_tooltip };
     const auto gcc = fme::GenericComboClass(
       gui_labels::output_root_path_type,
       []() { return values; },
       []() { return values | std::views::transform(AsString{}); },
       []() { return tooltips; },
       selections->batch_output_root_path_type);
     if (gcc.render())
     {
          selections->update_configuration_key(ConfigKey::BatchOutputRootPathType);
     }
     if (selections->batch_output_root_path_type != root_path_types::selected_path)
     {
          const float        button_size      = ImGui::GetFrameHeight();
          const float        button_width     = button_size * 3.0F;
          const auto         pop_id           = PushPopID();
          const std::string &selected_string  = get_selected_path(selections->batch_output_path, selections->batch_output_root_path_type);
          std::string        processed_string = key_value_data::static_replace_tags(selected_string, selections);
          if (ImGui::Button(gui_labels::explore.data(), ImVec2{ button_width, button_size }))
          {
               open_directory(processed_string);
          }
          else
          {
               tool_tip(processed_string);
          }
     }
     const bool path_changed = selections->batch_output_root_path_type == root_path_types::selected_path
                               && browse_path(gui_labels::output_path, m_output_path_valid, m_output_path);
     example_output_paths();
     if (!path_changed)
     {
          return;
     }
     save_output_path();
}

void fme::batch::checkmark_save_map()
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     bool changed = false;
     bool forced =
       (selections->batch_compact_type.enabled() || selections->batch_flatten_type.enabled() || selections->batch_input_load_map);
     if (!selections->batch_output_save_map && forced)
     {
          selections->batch_output_save_map = true;
          changed                           = true;
     }
     ImGui::BeginDisabled(forced);
     if (ImGui::Checkbox(gui_labels::save_map_files.data(), &selections->batch_output_save_map) || changed)
     {
          spdlog::info("batch_output_save_map: {}", selections->batch_output_save_map);
          selections->update_configuration_key(ConfigKey::BatchOutputSaveMap);
     }
     ImGui::EndDisabled();
}

void fme::batch::combo_compact_type()
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     const auto        tool_tip_pop = glengine::ScopeGuard{ [&]() { tool_tip(gui_labels::compact_tooltip); } };

     static const auto values       = std::array{
          compact_type::rows, compact_type::all, compact_type::move_only_conflicts, compact_type::map_order, compact_type::map_order_ffnx
     };
     static const auto tool_tips = std::array{ gui_labels::compact_rows_tooltip,
                                               gui_labels::compact_all_tooltip,
                                               gui_labels::move_conflicts_only_tooltip,
                                               gui_labels::compact_map_order_tooltip,
                                               gui_labels::compact_map_order_ffnx_tooltip };

     const auto        gcc       = fme::GenericComboClassWithFilter(
       gui_labels::compact,
       []() { return values; },
       []() { return values | std::views::transform(AsString{}); },
       []() { return tool_tips; },
       [&]() -> auto              &{ return selections->batch_compact_type; });

     if (!gcc.render())
     {
          return;
     }
     // selections->update_configuration_key(ConfigKey::BatchCompact);
}
void fme::batch::combo_flatten_type()
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     const auto tool_tip_pop        = glengine::ScopeGuard{ [&]() { tool_tip(gui_labels::flatten_tooltip); } };
     const bool all_or_only_palette = !selections->batch_compact_type.enabled()
                                      || (selections->batch_compact_type.value() != compact_type::map_order)
                                      || (selections->batch_compact_type.value() != compact_type::map_order_ffnx);
     static constexpr auto values = std::array{ flatten_type::bpp, flatten_type::palette, flatten_type::both };
     static constexpr auto tool_tips =
       std::array{ gui_labels::flatten_bpp_tooltip, gui_labels::flatten_palette_tooltip, gui_labels::flatten_both_tooltip };
     static constexpr auto values_only_palette    = std::array{ flatten_type::palette };
     static constexpr auto tool_tips_only_palette = std::array{ gui_labels::flatten_palette_tooltip };
     if (all_or_only_palette)
     {
          const auto gcc = fme::GenericComboClassWithFilter(
            gui_labels::flatten,
            [&]() { return values; },
            [&]() { return values | std::views::transform(AsString{}); },
            [&]() { return tool_tips; },
            [&]() -> auto & { return selections->batch_flatten_type; });
          if (!gcc.render())
          {
               return;
          }
     }
     else
     {
          const auto gcc = fme::GenericComboClassWithFilter(
            gui_labels::flatten,
            [&]() { return values_only_palette; },
            [&]() { return values_only_palette | std::views::transform(AsString{}); },
            [&]() { return tool_tips_only_palette; },
            [&]() -> auto & { return selections->batch_flatten_type; });
          if (!gcc.render())
          {
               return;
          }
     }
     // selections->update_configuration_key(ConfigKey::BatchFlatten);
}
bool fme::batch::draw_multi_column_list_box(const std::string_view name, const std::vector<std::string> &items, std::vector<bool> &enabled)
{
     bool changed = false;
     if (!ImGui::CollapsingHeader(name.data()))
     {
          return changed;
     }


     if (ImGui::Button(gui_labels::select_all.data()))
     {
          std::ranges::transform(enabled, enabled.begin(), [](auto &&) { return true; });
          changed = true;
     }

     ImGui::SameLine();

     if (ImGui::Button(gui_labels::select_none.data()))
     {
          std::ranges::transform(enabled, enabled.begin(), [](auto &&) { return false; });
          changed = true;
     }

     ImGui::SameLine();

     {
          ImGui::BeginDisabled(m_num_columns <= 1);
          if (ImGui::ArrowButton("num_columns_dec", ImGuiDir_Left) && m_num_columns > 1)
          {
               --m_num_columns;
          }
          ImGui::EndDisabled();
     }

     ImGui::SameLine();
     {
          ImGui::BeginDisabled(m_num_columns == (std::numeric_limits<std::uint8_t>::max)());
          if (ImGui::ArrowButton("num_columns_inc", ImGuiDir_Right))
          {
               ++m_num_columns;
          }
          ImGui::EndDisabled();
     }

     ImGui::Columns(m_num_columns, "multicol_listbox", false);
     ImGui::Separator();

     ImVec4 const enabled_color = ImVec4(0.4F, 0.8F, 0.4F, 1.0F);// Green
     assert(items.size() == enabled.size());
     for (auto &&zipped : std::ranges::views::zip(items, enabled))
     {
          const auto &item       = std::get<0>(zipped);// const auto& or auto& depending on the range
          auto       &enable     = std::get<1>(zipped);// mutable reference

          const auto  pop_id     = PushPopID();
          const auto  pop_column = glengine::ScopeGuard{ &ImGui::NextColumn };
          const auto  selectable = [&]() {
               if (ImGui::Selectable(item.c_str(), enable))
               {
                    enable  = !enable;
                    changed = true;
               }
          };

          if (enable)
          {
               // Revert text color to default
               const auto pop_text_color = glengine::ScopeGuard{ []() { ImGui::PopStyleColor(); } };
               // Change text color and show the current enabled status
               ImGui::PushStyleColor(ImGuiCol_Text, enabled_color);

               selectable();
          }
          else
          {
               selectable();
          }
     }

     ImGui::Columns(1);
     ImGui::Separator();
     return changed;
}
void fme::batch::button_start()
{
     const auto archives_group = m_archives_group.lock();
     if (!archives_group)
     {
          spdlog::error("Failed to lock m_archives_group: shared_ptr is expired.");
          return;
     }
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     const auto pop_id_right = PushPopID();
     const auto spacing      = ImGui::GetStyle().ItemInnerSpacing.x;
     const auto end_function = glengine::ScopeGuard{ []() {
          ImGui::PopStyleColor(3);
          ImGui::EndDisabled();
     } };
     ImGui::BeginDisabled(
       ((selections->batch_input_type == input_types::mim
         || (!m_input_path_valid && selections->batch_input_root_path_type == root_path_types::selected_path))
        && (!m_output_path_valid && selections->batch_output_root_path_type == root_path_types::selected_path))
       || !archives_group);
     ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0F, 0.5F, 0.0F, 1.0F));// Green
     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3F, 0.8F, 0.3F, 1.0F));// Light green hover
     ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1F, 0.3F, 0.1F, 1.0F));// Dark green active
     if (!ImGui::Button(
           gui_labels::begin_batch_operation.data(), ImVec2{ ImGui::GetContentRegionAvail().x * 0.75F - spacing, ImGui::GetFrameHeight() }))
     {
          return;
     }

     m_fields_consumer = archives_group->fields();
     m_field.reset();
     m_lang_consumer.restart();
     m_coo.reset();
}

void fme::batch::button_stop()
{
     const auto archives_group = m_archives_group.lock();
     if (!archives_group)
     {
          spdlog::error("Failed to lock m_archives_group: shared_ptr is expired.");
          return;
     }
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     const auto pop_id_right = PushPopID();
     const auto spacing      = ImGui::GetStyle().ItemInnerSpacing.x;
     ImGui::SameLine(0, spacing);
     const auto end_function = glengine::ScopeGuard{ []() {
          ImGui::PopStyleColor(3);
          ImGui::EndDisabled();
     } };
     ImGui::BeginDisabled(
       ((selections->batch_input_type == input_types::mim
         || (!m_input_path_valid && selections->batch_input_root_path_type == root_path_types::selected_path))
        && (!m_output_path_valid && selections->batch_output_root_path_type == root_path_types::selected_path))
       || !archives_group);
     ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5F, 0.0F, 0.0F, 1.0F));// Red
     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8F, 0.3F, 0.3F, 1.0F));// Light red hover
     ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3F, 0.1F, 0.1F, 1.0F));// Dark red active
     if (!ImGui::Button(gui_labels::stop.data(), ImVec2{ ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight() }))
     {
          return;
     }

     stop();
}
void fme::batch::button_input_browse()
{
     m_directory_browser.Open();
     m_directory_browser.SetTitle(gui_labels::choose_input_directory.data());
     m_directory_browser.SetPwd(m_input_path.data());
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_directory_browser_mode = directory_mode::input_mode;
};
void fme::batch::button_output_browse()
{
     m_directory_browser.Open();
     m_directory_browser.SetTitle(gui_labels::choose_output_directory.data());
     m_directory_browser.SetPwd(m_output_path.data());
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_directory_browser_mode = directory_mode::output_mode;
};
bool fme::batch::browse_path(std::string_view name, bool &valid_path, std::array<char, m_buffer_size> &path_buffer)
{
     bool              changed      = false;
     const ImGuiStyle &style        = ImGui::GetStyle();
     const float       spacing      = style.ItemInnerSpacing.x;
     const float       button_size  = ImGui::GetFrameHeight();
     const float       button_width = button_size * 3.0F;
     const auto        pop_id       = PushPopID();
     // ImGui text box with browse button
     // Highlight text box red if the folder doesn't exist
     {
          const float width = ImGui::CalcItemWidth();
          ImGui::PushItemWidth(width - (spacing * 2.0F) - button_width * 2.0F);
          const auto pop_item_width = glengine::ScopeGuard(&ImGui::PopItemWidth);
          if (!valid_path)
          {
               ImGui::PushStyleColor(ImGuiCol_FrameBg, static_cast<ImVec4>(ImColor::HSV(0.0F, 0.5F, 0.5F)));
               ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, static_cast<ImVec4>(ImColor::HSV(0.0F, 0.8F, 0.8F)));// lighter red on hover
               ImGui::PushStyleColor(ImGuiCol_FrameBgActive, static_cast<ImVec4>(ImColor::HSV(0.0F, 0.5F, 0.5F)));
          }
          const auto pop_color = glengine::ScopeGuard([valid = valid_path]() {
               if (!valid)
               {
                    ImGui::PopStyleColor(3);
               }
          });
          if (ImGui::InputText("##Empty", path_buffer.data(), path_buffer.size()))
          {
               // Check if the folder path exists when the text box changes
               const auto tmp = safedir(path_buffer.data());
               valid_path     = tmp.is_exists() && tmp.is_dir();
               changed        = true;
          }
          tool_tip(std::string_view{ path_buffer.begin(), path_buffer.end() });
     }
     ImGui::SameLine(0, spacing);
     {
          if (ImGui::Button(gui_labels::browse.data(), ImVec2{ button_width, button_size }))
          {
               // Trigger the chooseFolder function when the button is clicked
               // chooseFolder();
               if (std::addressof(path_buffer) == std::addressof(m_input_path))
               {
                    button_input_browse();
               }
               else if (std::addressof(path_buffer) == std::addressof(m_output_path))
               {
                    button_output_browse();
               }
          }
     }
     ImGui::SameLine(0, spacing);
     if (ImGui::Button(gui_labels::explore.data(), ImVec2{ button_width, button_size }))
     {
          open_directory(path_buffer.data());
     }
     tool_tip(gui_labels::explore_tooltip);
     ImGui::SameLine(0, spacing);

     format_imgui_wrapped_text("{}", name.data());
     return changed;
}

/**
 * @brief Updates the batch processing state based on the elapsed time.
 *
 * This function runs periodically and is responsible for:
 * - Throttling execution to one update every 30ms.
 * - Attempting to consume a future if available.
 * - Selecting a field and COO if needed.
 * - Processing the map sprite if all required data is valid.
 * - Compacting, flattening, and saving textures/maps according to user-selected options.
 * - Resetting internal state for the next processing cycle.
 *
 * @param elapsed_time The time elapsed since the last update call.
 */
void fme::batch::update(float elapsed_time)
{
     // Attempt to acquire a shared_ptr to the selections structure
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }

     // Interval between updates in seconds
     static constexpr float interval           = 0.03f;

     // Accumulated elapsed time (preserved across calls)
     static float           total_elapsed_time = 0.f;

     // Add elapsed time from this frame to the total
     total_elapsed_time += elapsed_time;

     // Skip update if interval threshold hasn't been reached
     if (total_elapsed_time < interval)
     {
          return;
     }

     // Reset accumulated time after reaching the threshold
     total_elapsed_time = 0.f;

     // Attempt to process any pending futures first
     if (consume_one_future())
     {
          return;
     }

     // Select the next valid field and COO if needed
     choose_field_and_coo();

     // Exit if required data is missing or invalid
     if (!m_field || !m_coo || !m_field->operator bool())
     {
          return;
     }

     // Update status with the field and language info
     m_status = fmt::format("Processing {}:{}", m_field->get_base_name(), *m_coo);

     // Generate the visual representation of the map
     generate_map_sprite();

     // Proceed only if map sprite generation succeeded
     // and it either uses the COO or COO is 'generic'
     if (!m_map_sprite.fail() && (m_map_sprite.using_coo() || m_coo.value() == open_viii::LangT::generic))
     {
          compact();
          flatten();

          // Choose output method based on batch output type
          const std::string &selected_string = get_selected_path(selections->batch_output_path, selections->batch_output_root_path_type);
          switch (selections->batch_output_type)
          {
               case output_types::deswizzle:
                    m_future_consumer += m_map_sprite.save_pupu_textures(selections->output_deswizzle_pattern, selected_string);
                    break;
               case output_types::swizzle:
                    m_future_consumer += m_map_sprite.save_swizzle_textures(selections->output_swizzle_pattern, selected_string);
                    break;
               case output_types::swizzle_as_one_image:
                    m_future_consumer += m_map_sprite.save_combined_swizzle_texture(selections->output_swizzle_pattern, selected_string);
                    break;
          }

          // Optionally save the modified map
          if (selections->batch_output_save_map)
          {
               const key_value_data cpm2 = {
                    .field_name    = m_map_sprite.get_base_name(),
                    .ext           = ".map",
                    .language_code = m_coo.has_value() && m_coo.value() != open_viii::LangT::generic ? m_coo : std::nullopt,
               };
               m_map_sprite.save_modified_map(
                 cpm2.replace_tags(get_output_map_pattern(selections->batch_output_type), selections, selected_string));
          }
     }

     // Clean up and prepare for next processing cycle
     reset_for_next();
}


/**
 * @brief Attempts to advance or retrieve results from future consumers.
 *
 * This function checks the state of asynchronous operations (represented by
 * `m_future_of_future_consumer` and `m_future_consumer`) and progresses them
 * if needed. It ensures that any pending output is consumed before continuing.
 *
 * @return true if any future was consumed or progressed, false otherwise.
 */
bool fme::batch::consume_one_future()
{
     // If the outer future is still processing, advance it
     if (!m_future_of_future_consumer.done())
     {
          ++m_future_of_future_consumer;
          return true;
     }
     // If the outer future is done but has output, consume it
     else if (m_future_of_future_consumer.consumer_ready())
     {
          m_future_consumer += m_future_of_future_consumer.get_consumer();
          return true;
     }
     // If the inner future is still processing, advance it
     else if (!m_future_consumer.done())
     {
          ++m_future_consumer;
          return true;
     }

     // Nothing to do, both futures are done and consumed
     return false;
}

/**
 * @brief Generates the map sprite based on the current field, language, and input type.
 *
 * This function builds a `map_sprite` instance using the current field and language (COO),
 * applying filters depending on the input type selected by the user (e.g., deswizzle, swizzle).
 * The resulting sprite is stored in `m_map_sprite`.
 *
 * It uses input settings from `m_selections` and applies different filters accordingly.
 * If the shared_ptr to selections is expired, the function logs an error and aborts.
 */
void fme::batch::generate_map_sprite()
{
     // Try to acquire a shared pointer to user selections
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }

     // Ensure preconditions are met
     assert(m_field);
     assert(m_coo);

     // Initialize filters with default disabled state
     ff_8::filters      filters         = { false };

     // Enable specific filters depending on the input type
     const std::string &selected_string = get_selected_path(selections->batch_input_path, selections->batch_input_root_path_type);
     switch (selections->batch_input_type)
     {
          case input_types::mim:
               // No filters applied for MIM input and no .map files are loaded automaticly.
               break;

          case input_types::deswizzle:
               // Enable deswizzle filter using the input path
               filters.deswizzle.update(selected_string).enable();
               if (selections->batch_input_load_map)
                    filters.deswizzle_map.update(selected_string).enable();
               break;

          case input_types::swizzle:
               // Enable upscale filter using the input path
               filters.upscale.update(selected_string).enable();
               if (selections->batch_input_load_map)
                    filters.upscale_map.update(selected_string).enable();
               break;
     }

     // Create the map sprite with appropriate settings
     m_map_sprite = map_sprite{ ff_8::map_group{ m_field, *m_coo },// field and language
                                selections->batch_output_type == output_types::swizzle,// use swizzle format?
                                filters,// filters to apply
                                true,// always include image data?
                                m_coo && m_coo.value() != open_viii::LangT::generic// use localized COO?
                                ,
                                m_selections };
}

/**
 * @brief Applies a compaction strategy to the current map sprite based on user selection.
 *
 * This function checks whether a compact type is enabled in the batch selection.
 * If it is, it applies the appropriate compaction method to `m_map_sprite`.
 *
 * Compaction can rearrange tiles, resolve overlaps, or adjust layout according to the selected strategy.
 */
void fme::batch::compact()
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }

     // Skip if compact type is not enabled
     if (!selections->batch_compact_type.enabled())
     {
          return;
     }

     // Apply the appropriate compaction strategy based on the selected type
     switch (selections->batch_compact_type.value())
     {
          case compact_type::rows:
               m_map_sprite.compact_rows();
               break;
          case compact_type::all:
               m_map_sprite.compact_all();
               break;
          case compact_type::move_only_conflicts:
               m_map_sprite.compact_move_conflicts_only();
               break;
          case compact_type::map_order:
               m_map_sprite.compact_map_order();
               break;
          case compact_type::map_order_ffnx:
               m_map_sprite.compact_map_order_ffnx();
               break;
     }
}

/**
 * @brief Applies flattening operations to the current map sprite based on user selection.
 *
 * Flattening can reduce BPP or palette complexity, depending on the type selected.
 * Certain flattening operations are skipped if incompatible compaction types are selected.
 *
 * If flattening is applied and the compact type is not map-order based,
 * the function will additionally call `compact()` to optimize layout.
 */
void fme::batch::flatten()
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }

     // Skip if flattening is not enabled
     if (!selections->batch_flatten_type.enabled())
     {
          return;
     }

     // Determine and apply flattening strategy
     switch (selections->batch_flatten_type.value())
     {
          case flatten_type::bpp:
               // Only flatten BPP if compact type isn't using map order
               if (
                 !selections->batch_compact_type.enabled()
                 || (selections->batch_compact_type.value() != compact_type::map_order && selections->batch_compact_type.value() != compact_type::map_order_ffnx))
               {
                    m_map_sprite.flatten_bpp();
               }
               break;

          case flatten_type::palette:
               m_map_sprite.flatten_palette();
               break;

          case flatten_type::both:
               // Only flatten BPP if not using map order
               if (
                 !selections->batch_compact_type.enabled()
                 || (selections->batch_compact_type.value() != compact_type::map_order && selections->batch_compact_type.value() != compact_type::map_order_ffnx))
               {
                    m_map_sprite.flatten_bpp();
               }
               m_map_sprite.flatten_palette();
               break;
     }

     // If the compact strategy is not map-order-based, re-apply compaction
     if (selections->batch_compact_type.value() != compact_type::map_order)
     {
          compact();
     }
}


/**
 * @brief Resets the current language and field state in preparation for the next item in batch processing.
 *
 * If the language consumer is done, the field is reset and language consumer is restarted if more fields remain.
 * This helps coordinate iteration through fields and their associated languages.
 */
void fme::batch::reset_for_next()
{
     // Clear the current language
     m_coo.reset();

     // If the current language has been fully processed
     if (m_lang_consumer.done())
     {
          // Clear the field
          m_field.reset();

          // If more fields are available, restart the language consumer for the next field
          if (!m_fields_consumer.done())
          {
               m_lang_consumer.restart();
          }
     }
}

/**
 * @brief Chooses and initializes a field and COO (Language Archive) from consumers.
 *
 * This function attempts to initialize `m_field` from the `m_fields_consumer`
 * and `m_coo` from the `m_lang_consumer`. It ensures the field is valid and
 * corresponds to a map marked as enabled in `m_maps_enabled`.
 *
 * Requirements:
 * - `m_archives_group` must be valid (not expired).
 * - `m_fields_consumer` and `m_lang_consumer` must support dereferencing and iteration.
 * - `m_maps_enabled` should have a size matching the number of map entries in `mapdata()`.
 *
 * If a valid field and COO are found, they are stored in `m_field` and `m_coo`, respectively.
 */
void fme::batch::choose_field_and_coo()
{
     // Attempt to acquire a shared_ptr to the archives group
     const auto archives_group = m_archives_group.lock();
     if (!archives_group)
     {
          spdlog::error("Failed to lock m_archives_group: shared_ptr is expired.");
          return;
     }

     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }

     // Attempt to choose a valid field from m_fields_consumer
     while ((!m_field || !m_field->operator bool()) && !m_fields_consumer.done())
     {
          // Retrieve the next archive from the consumer
          open_viii::archive::FIFLFSArchiveFetcher tmp         = *m_fields_consumer;

          // Reference to the list of map names in the archive group
          const auto                              &map_data    = archives_group->mapdata();

          // Attempt to find a case-insensitive match for the current archive's map name
          const auto                               find_result = std::ranges::find_if(
            map_data, [&](std::string_view map_name) -> bool { return open_viii::tools::i_equals(map_name, tmp.map_name()); });

          // If a matching map name is found
          if (find_result != std::ranges::end(map_data))
          {
               const auto offset = std::ranges::distance(std::ranges::begin(map_data), find_result);

               // Check if the map at this offset is enabled
               if (selections->batch_map_list_enabled.at(static_cast<std::size_t>(offset)))
               {
                    // Create the field object from the archive and store it
                    m_field = std::make_shared<open_viii::archive::FIFLFS<false>>(tmp.get());

                    // Move to the next consumer item
                    ++m_fields_consumer;

                    break;// Exit the loop after successfully choosing a field
               }
          }

          // Skip to next field archive if current one is invalid or disabled
          ++m_fields_consumer;
     }

     // Attempt to choose the first available language archive
     while (!m_coo && !m_lang_consumer.done())
     {
          m_coo = *m_lang_consumer;
          ++m_lang_consumer;
     }
}

// std::filesystem::path fme::batch::append_file_structure(const std::filesystem::path &path) const
// {
//      std::string const      name   = m_map_sprite.get_base_name();
//      std::string_view const prefix = std::string_view(name).substr(0, 2);
//      return path / prefix / name;
// }

void fme::batch::open_directory_browser()
{
     m_directory_browser.Display();
     if (!m_directory_browser.HasSelected())
     {
          return;
     }
     const auto         clear_browser = glengine::ScopeGuard([this]() { m_directory_browser.ClearSelected(); });
     const std::string &selected_path = m_directory_browser.GetPwd().string();
     const auto         tmp           = safedir(selected_path);
     switch (m_directory_browser_mode)
     {
          case directory_mode::input_mode: {
               m_input_path_valid = safe_copy_string(selected_path, m_input_path);
               save_input_path();
          }
          break;
          case directory_mode::output_mode: {
               m_output_path_valid = safe_copy_string(selected_path, m_output_path);
               save_output_path();
          }
          break;
     }
}

fme::batch &fme::batch::operator=(std::weak_ptr<archives_group> new_group)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return *this;
     }
     stop();
     m_archives_group    = std::move(new_group);
     auto archives_group = m_archives_group.lock();
     if (archives_group && archives_group->mapdata().size() != selections->batch_map_list_enabled.size())
     {
          selections->batch_map_list_enabled.resize(archives_group->mapdata().size(), true);
          selections->update_configuration_key(ConfigKey::BatchMapListEnabled);
     }
     return *this;
}
fme::batch &fme::batch::operator=(std::weak_ptr<Selections> new_selections)
{
     stop();
     m_selections          = std::move(new_selections);
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return *this;
     }
     m_input_path_valid  = safe_copy_string(selections->batch_input_path, m_input_path);
     m_output_path_valid = safe_copy_string(selections->batch_output_path, m_output_path);
     return *this;
}

bool fme::batch::in_progress() const
{
     return !m_fields_consumer.done() || m_field;
}
void fme::batch::stop()
{
     m_fields_consumer.stop();
     m_lang_consumer.stop();
     m_field.reset();
}

fme::batch::batch(std::weak_ptr<Selections> existing_selections, std::weak_ptr<archives_group> existing_group)
{
     operator=(std::move(existing_selections));
     operator=(std::move(existing_group));


     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     flags = selections->batch_compact_type.enabled() || selections->batch_flatten_type.enabled() ? ImGuiTreeNodeFlags_DefaultOpen
                                                                                                  : ImGuiTreeNodeFlags{};
}
