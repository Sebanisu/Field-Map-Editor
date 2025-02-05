//
// Created by pcvii on 3/7/2023.
//

#include "batch.hpp"
#include "as_string.hpp"
#include "Configuration.hpp"
#include "open_file_explorer.hpp"
#include "push_pop_id.hpp"
#include "tool_tip.hpp"
void fme::batch::combo_input_type()
{
     static constexpr auto values = std::array{ input_types::mim, input_types::deswizzle, input_types::swizzle };
     static constexpr auto tooltips =
       std::array{ gui_labels::input_mim_tooltip, gui_labels::input_deswizzle_tooltip, gui_labels::input_swizzle_tooltip };
     const auto gcc = fme::GenericComboClass(
       gui_labels::input_type,
       []() { return values; },
       []() { return values | std::views::transform(AsString{}); },
       []() { return tooltips; },
       m_input_type);
     if (gcc.render())
     {
          Configuration config{};
          config->insert_or_assign("batch_input_type", static_cast<std::underlying_type_t<input_types>>(m_input_type));
          config.save();
     }
}
void fme::batch::combo_output_type()
{
     static constexpr auto values = std::array{ output_types::deswizzle, output_types::swizzle };
     const auto            gcc    = fme::GenericComboClass(
       gui_labels::output_type, []() { return values; }, []() { return values | std::views::transform(AsString{}); }, m_output_type);
     if (gcc.render())
     {
          Configuration config{};
          config->insert_or_assign("batch_output_type", static_cast<std::underlying_type_t<output_types>>(m_output_type));
          config.save();
     }
}
void fme::batch::combo_compact_type()
{
     const auto        tool_tip_pop = scope_guard{ [&]() { tool_tip(gui_labels::compact_tooltip); } };

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
       [this]() -> auto              &{ return m_compact_type; });

     if (!gcc.render())
     {
          return;
     }

     Configuration config{};
     config->insert_or_assign("batch_compact_type", static_cast<std::underlying_type_t<compact_type>>(m_compact_type.value()));
     config->insert_or_assign("batch_compact_enabled", m_compact_type.enabled());
     config.save();
}
void fme::batch::combo_flatten_type()
{
     const auto tool_tip_pop        = scope_guard{ [&]() { tool_tip(gui_labels::flatten_tooltip); } };
     const bool all_or_only_palette = !m_compact_type.enabled() || (m_compact_type.value() != compact_type::map_order)
                                      || (m_compact_type.value() != compact_type::map_order_ffnx);
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
            [this]() -> auto & { return m_flatten_type; });
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
            [this]() -> auto & { return m_flatten_type; });
          if (!gcc.render())
          {
               return;
          }
     }
     Configuration config{};
     config->insert_or_assign("batch_flatten_type", static_cast<std::underlying_type_t<flatten_type>>(m_flatten_type.value()));
     config->insert_or_assign("batch_flatten_enabled", m_flatten_type.enabled());
     config.save();
}
void fme::batch::browse_input_path()
{
     if (m_input_type == input_types::mim)
     {
          return;
     }
     if (!browse_path(gui_labels::input_path, m_input_path_valid, m_input_path))
     {
          return;
     }
     if (!m_input_path_valid)
     {
          return;
     }
     Configuration config{};
     config->insert_or_assign("batch_input_path", std::string(m_input_path.data()));
     config.save();
}
void fme::batch::browse_output_path()
{
     if (!browse_path(gui_labels::output_path, m_output_path_valid, m_output_path))
     {
          return;
     }
     if (!m_output_path_valid)
     {
          return;
     }
     Configuration config{};
     config->insert_or_assign("batch_output_path", std::string(m_output_path.data()));
     config.save();
}
void fme::batch::draw_multi_column_list_box(const std::string_view name, const std::vector<std::string> &items, std::vector<bool> &enabled)
{
     if (!ImGui::CollapsingHeader(name.data()))
     {
          return;
     }


     if (ImGui::Button(gui_labels::select_all.data()))
     {
          std::ranges::transform(enabled, enabled.begin(), [](auto &&) { return true; });
     }

     ImGui::SameLine();

     if (ImGui::Button(gui_labels::select_none.data()))
     {
          std::ranges::transform(enabled, enabled.begin(), [](auto &&) { return false; });
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
     for (size_t i = 0; i != items.size(); ++i)
     {
          const auto pop_id     = PushPopID();
          const auto pop_column = scope_guard{ &ImGui::NextColumn };
          const auto selectable = [&]() {
               if (ImGui::Selectable(items[i].c_str(), enabled[i]))
               {
                    enabled[i] = !enabled[i];
               }
          };

          if (enabled[i])
          {
               // Revert text color to default
               const auto pop_text_color = scope_guard{ []() { ImGui::PopStyleColor(); } };
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
}
void fme::batch::button_start()
{
     const auto pop_id_right = PushPopID();
     const auto spacing      = ImGui::GetStyle().ItemInnerSpacing.x;
     ImGui::BeginDisabled(
       ((m_input_type == input_types::mim || !m_input_path_valid) && !m_output_path_valid) || !m_archives_group.operator bool());
     ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0F, 0.5F, 0.0F, 1.0F));// Green
     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3F, 0.8F, 0.3F, 1.0F));// Light green hover
     ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1F, 0.3F, 0.1F, 1.0F));// Dark green active
     if (ImGui::Button(
           gui_labels::begin_batch_operation.data(), ImVec2{ ImGui::GetContentRegionAvail().x * 0.75F - spacing, ImGui::GetFrameHeight() }))
     {
          m_fields_consumer = m_archives_group->fields();
          m_field.reset();
          m_lang_consumer.restart();
          m_coo.reset();
     }
     ImGui::PopStyleColor(3);
     ImGui::EndDisabled();
}

void fme::batch::button_stop()
{
     const auto pop_id_right = PushPopID();
     const auto spacing      = ImGui::GetStyle().ItemInnerSpacing.x;
     ImGui::SameLine(0, spacing);
     ImGui::BeginDisabled(
       ((m_input_type == input_types::mim || !m_input_path_valid) && !m_output_path_valid) || !m_archives_group.operator bool());
     ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5F, 0.0F, 0.0F, 1.0F));// Red
     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8F, 0.3F, 0.3F, 1.0F));// Light red hover
     ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3F, 0.1F, 0.1F, 1.0F));// Dark red active
     if (ImGui::Button(gui_labels::stop.data(), ImVec2{ ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight() }))
     {
          stop();
     }
     ImGui::PopStyleColor(3);
     ImGui::EndDisabled();
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
          const auto pop_item_width = scope_guard(&ImGui::PopItemWidth);
          if (!valid_path)
          {
               ImGui::PushStyleColor(ImGuiCol_FrameBg, static_cast<ImVec4>(ImColor::HSV(0.0F, 0.5F, 0.5F)));
               ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, static_cast<ImVec4>(ImColor::HSV(0.0F, 0.8F, 0.8F)));// lighter red on hover
               ImGui::PushStyleColor(ImGuiCol_FrameBgActive, static_cast<ImVec4>(ImColor::HSV(0.0F, 0.5F, 0.5F)));
          }
          const auto pop_color = scope_guard([valid = valid_path]() {
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
          tool_tip({ path_buffer.begin(), path_buffer.end() });
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

     format_imgui_text("{}", name.data());
     return changed;
}
void fme::batch::update(sf::Time elapsed_time)
{
     static constexpr int interval           = 30;// the interval in milliseconds
     static int           total_elapsed_time = 0;// keep track of the elapsed time using a static variable

     total_elapsed_time += elapsed_time.asMilliseconds();// add the elapsed time since last update

     if (total_elapsed_time < interval)
     {
          return;
     }
     total_elapsed_time = 0;// reset the elapsed time
     if (consume_one_future())
     {
          return;
     }
     choose_field_and_coo();
     if (!m_field || !m_coo || !m_field->operator bool())
     {
          return;
     }
     m_status = fmt::format("Processing {}:{}", m_field->get_base_name(), *m_coo);
     generate_map_sprite();

     if (!m_map_sprite.fail() && (m_map_sprite.using_coo() || m_coo.value() == open_viii::LangT::generic))
     {
          if (m_input_load_map && m_input_type != input_types::mim)
          {
               m_map_sprite.load_map(append_file_structure(m_input_path.data()) / m_map_sprite.map_filename());
          }
          compact();
          flatten();
          switch (m_output_type)
          {
               case output_types::deswizzle:
                    m_future_of_future_consumer = m_map_sprite.save_pupu_textures(append_file_structure(m_output_path.data()));
                    break;
               case output_types::swizzle:
                    m_future_of_future_consumer = m_map_sprite.save_swizzle_textures(append_file_structure(m_output_path.data()));
                    break;
          }
          if (m_save_map)
          {
               m_map_sprite.save_modified_map(append_file_structure(m_output_path.data()) / m_map_sprite.map_filename());
          }
     }
     reset_for_next();
}
bool fme::batch::consume_one_future()
{
     // perform your operation here
     if (!m_future_of_future_consumer.done())
     {
          ++m_future_of_future_consumer;
          return true;
     }
     else if (!m_future_of_future_consumer.output_empty())
     {
          m_future_consumer = m_future_of_future_consumer.get_consumer();
          return true;
     }
     else if (!m_future_consumer.done())
     {
          ++m_future_consumer;
          return true;
     }
     return false;
}
void fme::batch::generate_map_sprite()
{
     assert(m_field);
     assert(m_coo);
     ff_8::filters filters = {};
     switch (m_input_type)
     {
          case input_types::mim:
               // do nothing
               break;
          case input_types::deswizzle:
               filters.deswizzle.update(append_file_structure(m_input_path.data())).enable();
               break;
          case input_types::swizzle:
               filters.upscale.update(append_file_structure(m_input_path.data())).enable();
               break;
     }
     m_map_sprite = map_sprite{ ff_8::map_group{ m_field, *m_coo },
                                m_output_type == output_types::swizzle,
                                filters,
                                true,
                                m_coo && m_coo.value() != open_viii::LangT::generic };
}
void fme::batch::compact()
{
     if (!m_compact_type.enabled())
     {
          return;
     }
     switch (m_compact_type.value())
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
void fme::batch::flatten()
{
     if (!m_flatten_type.enabled())
     {
          return;
     }
     switch (m_flatten_type.value())
     {
          case flatten_type::bpp:
               if (
                 !m_compact_type.enabled() || (m_compact_type.value() != compact_type::map_order)
                 || (m_compact_type.value() != compact_type::map_order_ffnx))
               {
                    m_map_sprite.flatten_bpp();
               }
               break;
          case flatten_type::palette:
               m_map_sprite.flatten_palette();
               break;
          case flatten_type::both:
               if (
                 !m_compact_type.enabled() || (m_compact_type.value() != compact_type::map_order)
                 || (m_compact_type.value() != compact_type::map_order_ffnx))
               {
                    m_map_sprite.flatten_bpp();
               }
               m_map_sprite.flatten_palette();
               break;
     }
     if (m_compact_type.value() != compact_type::map_order)
     {
          compact();
     }
}
void fme::batch::reset_for_next()
{
     m_coo.reset();
     if (m_lang_consumer.done())
     {
          m_field.reset();
          if (!m_fields_consumer.done())
          {
               m_lang_consumer.restart();
          }
     }
}
void fme::batch::choose_field_and_coo()
{
     while ((!m_field || !m_field->operator bool()) && !m_fields_consumer.done())
     {
          open_viii::archive::FIFLFSArchiveFetcher tmp         = *m_fields_consumer;
          const auto                              &map_data    = m_archives_group->mapdata();
          const auto                               find_result = std::ranges::find_if(
            map_data, [&](std::string_view map_name) -> bool { return open_viii::tools::i_equals(map_name, tmp.map_name()); });

          if (find_result != std::ranges::end(map_data))
          {
               const auto offset = std::ranges::distance(std::ranges::begin(map_data), find_result);
               if (m_maps_enabled.at(static_cast<std::size_t>(offset)))
               {
                    m_field = std::make_shared<open_viii::archive::FIFLFS<false>>(tmp.get());
                    ++m_fields_consumer;
                    break;
               }
          }

          ++m_fields_consumer;
     }

     while (!m_coo && !m_lang_consumer.done())
     {
          m_coo = *m_lang_consumer;
          ++m_lang_consumer;
     }
}
std::filesystem::path fme::batch::append_file_structure(const std::filesystem::path &path) const
{
     std::string const      name   = m_map_sprite.get_base_name();
     std::string_view const prefix = std::string_view(name).substr(0, 2);
     return path / prefix / name;
}
void fme::batch::open_directory_browser()
{
     m_directory_browser.Display();
     if (!m_directory_browser.HasSelected())
     {
          return;
     }
     const auto         clear_browser = scope_guard([this]() { m_directory_browser.ClearSelected(); });
     const std::string &selected_path = m_directory_browser.GetPwd().string();
     const auto         tmp           = safedir(selected_path);
     switch (m_directory_browser_mode)
     {
          case directory_mode::input_mode: {
               Configuration config{};
               config->insert_or_assign("batch_input_path", selected_path);
               config.save();
               std::ranges::copy(selected_path, m_input_path.data());
               m_input_path.at(selected_path.size()) = '\0';
               m_input_path_valid                    = tmp.is_dir() && tmp.is_exists();
          }
          break;
          case directory_mode::output_mode: {
               Configuration config{};
               config->insert_or_assign("batch_output_path", selected_path);
               config.save();
               std::ranges::copy(selected_path, m_output_path.data());
               m_output_path.at(selected_path.size()) = '\0';
               m_output_path_valid                    = tmp.is_dir() && tmp.is_exists();
          }
          break;
     }
}
void fme::batch::checkbox_load_map()
{
     if (!(m_input_type != input_types::mim))
     {
          return;
     }
     const auto pop_id = PushPopID();
     if (!ImGui::Checkbox(gui_labels::batch_load_map.data(), &m_input_load_map))
     {
          return;
     }
     Configuration config{};
     config->insert_or_assign("batch_input_load_map", m_input_load_map);
     config.save();
}

void fme::batch::draw_window()
{
     const auto end = scope_guard(&ImGui::End);
     if (!ImGui::Begin(gui_labels::batch_operation_window.data()))
     {
          return;
     }
     const bool disabled = in_progress();
     open_directory_browser();
     ImGui::BeginDisabled(disabled);
     combo_input_type();
     browse_input_path();
     checkbox_load_map();


     combo_output_type();
     browse_output_path();
     checkmark_save_map();
     if (ImGui::CollapsingHeader(gui_labels::compact_flatten.data()))
     {
          format_imgui_wrapped_text("{}", gui_labels::compact_flatten_warning);
          combo_compact_type();
          combo_flatten_type();
     }
     if (m_archives_group)
     {
          draw_multi_column_list_box("Map List", m_archives_group->mapdata(), m_maps_enabled);
     }
     button_start();
     ImGui::EndDisabled();
     ImGui::BeginDisabled(!disabled);
     button_stop();
     ImGui::EndDisabled();
     if (!disabled)
     {
          return;
     }
     format_imgui_text("{}", m_status);
}

void fme::batch::checkmark_save_map()
{
     bool changed = false;
     bool forced  = (m_compact_type.enabled() || m_flatten_type.enabled());
     if (!m_save_map && forced)
     {
          m_save_map = true;
          changed    = true;
     }
     ImGui::BeginDisabled(forced);
     if (ImGui::Checkbox(gui_labels::save_map_files.data(), &m_save_map) || changed)
     {
          Configuration config{};
          config->insert_or_assign("batch_save_map", static_cast<std::underlying_type_t<input_types>>(m_save_map));
          config.save();
     }
     ImGui::EndDisabled();
}

fme::batch &fme::batch::operator=(std::shared_ptr<archives_group> new_group)
{
     stop();
     m_archives_group = std::move(new_group);
     if (m_archives_group && m_archives_group->mapdata().size() != m_maps_enabled.size())
     {
          m_maps_enabled.resize(m_archives_group->mapdata().size(), true);
     }
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

fme::batch::batch(std::shared_ptr<archives_group> existing_group)
  : m_archives_group(std::move(existing_group))
{
     fme::Configuration const config{};

     m_compact_type =
       decltype(m_compact_type){ static_cast<compact_type>(config[ff_8::ConfigKeys<ff_8::FilterTag::Compact>::key_name].value_or(
                                   std::to_underlying(m_compact_type.value()))),
                                 config[ff_8::ConfigKeys<ff_8::FilterTag::Compact>::enabled_key_name].value_or(false) };

     m_flatten_type =
       decltype(m_flatten_type){ static_cast<flatten_type>(config[ff_8::ConfigKeys<ff_8::FilterTag::Flatten>::key_name].value_or(
                                   std::to_underlying(m_flatten_type.value()))),
                                 config[ff_8::ConfigKeys<ff_8::FilterTag::Flatten>::enabled_key_name].value_or(false) };

     if (m_archives_group && m_archives_group->mapdata().size() != m_maps_enabled.size())
     {
          m_maps_enabled.resize(m_archives_group->mapdata().size(), true);
     }
     m_input_type =
       static_cast<input_types>(config["batch_input_type"].value_or(static_cast<std::underlying_type_t<input_types>>(m_input_type)));
     {
          std::string str_tmp = config["batch_input_path"].value_or(std::string(m_input_path.data()));
          std::ranges::copy(str_tmp, m_input_path.data());
          m_input_path.at(str_tmp.size()) = '\0';
          const auto tmp                  = safedir(m_input_path.data());
          m_input_path_valid              = tmp.is_dir() && tmp.is_exists();
     }
     m_output_type =
       static_cast<output_types>(config["batch_output_type"].value_or(static_cast<std::underlying_type_t<output_types>>(m_output_type)));
     {
          std::string str_tmp = config["batch_output_path"].value_or(std::string(m_output_path.data()));
          std::ranges::copy(str_tmp, m_output_path.data());
          m_output_path.at(str_tmp.size()) = '\0';
          const auto tmp                   = safedir(m_output_path.data());
          m_output_path_valid              = tmp.is_dir() && tmp.is_exists();
     }
     m_input_load_map = config["batch_input_load_map"].value_or(m_input_load_map);

     m_save_map       = config["batch_save_map"].value_or(m_save_map);
}
