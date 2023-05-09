//
// Created by pcvii on 3/7/2023.
//

#include "batch.hpp"
#include "Configuration.hpp"
struct AsString
{
     template<typename T>
     std::string operator()(const T &value) const
     {
          return fmt::format("{}", value);
     }
};
void batch::combo_input_type(int &imgui_id)
{
     static const auto values = std::array{ input_types::mim, input_types::deswizzle, input_types::swizzle };
     if (fme::generic_combo(
           imgui_id, "Input Type", []() { return values; }, []() { return values | std::views::transform(AsString{}); }, m_input_type))
     {
          Configuration config{};
          config->insert_or_assign("batch_input_type", static_cast<std::underlying_type_t<input_types>>(m_input_type));
          config.save();
     }
}
void batch::combo_output_type(int &imgui_id)
{
     static const auto values = std::array{ output_types::deswizzle, output_types::swizzle };
     if (fme::generic_combo(
           imgui_id, "Output Type", []() { return values; }, []() { return values | std::views::transform(AsString{}); }, m_output_type))
     {
          Configuration config{};
          config->insert_or_assign("batch_output_type", static_cast<std::underlying_type_t<output_types>>(m_output_type));
          config.save();
     }
}
void batch::combo_compact_type(int &imgui_id)
{
     using namespace std::string_view_literals;
     static const auto values = std::array{ compact_type::rows, compact_type::all, compact_type::map_order };
     static const auto tool_tips =
       std::array{ "Rows: sorts 8bit to 4bit, and separates conflicting palettes. Tries to apply sort to each row at a time."sv,
                   "All: sorts 8bit to 4bit, and separates conflicting palettes. Applies the sort to all the tiles "sv,
                   "Map Order: Creates a tile for each map entry. 16 cols, and 16 rows, per texture page."sv };
     if (!fme::generic_combo(
           imgui_id,
           gui_labels::compact,
           []() { return values; },
           []() { return values | std::views::transform(AsString{}); },
           []() { return tool_tips; },
           [&]() -> auto & { return m_compact_type; }))
          return;
     Configuration config{};
     config->insert_or_assign("batch_compact_type", static_cast<std::underlying_type_t<compact_type>>(m_compact_type.value()));
     config->insert_or_assign("batch_compact_enabled", m_compact_type.enabled());
     config.save();
}
void batch::combo_flatten_type(int &imgui_id)
{
     using namespace std::string_view_literals;
     const bool            all_or_only_palette = !m_compact_type.enabled() || m_compact_type.value() != compact_type::map_order;
     static constexpr auto values              = std::array{ flatten_type::bpp, flatten_type::palette, flatten_type::both };
     static constexpr auto palette_str         = "Palette: Changes all the palettes to 0. This might reduce reloading of textures."sv;
     static constexpr auto tool_tips           = std::array{
          "BPP: Changes all the bits per pixel to 4 to get the max number of tiles per texture page. Applied automatically by Map Order compacting."sv,
          palette_str,
          "Both."sv
     };
     static constexpr auto values_only_palette    = std::array{ flatten_type::palette };
     static constexpr auto tool_tips_only_palette = std::array{ palette_str };
     if (all_or_only_palette)
     {
          if (!fme::generic_combo(
                imgui_id,
                gui_labels::flatten,
                [&]() { return values; },
                [&]() { return values | std::views::transform(AsString{}); },
                [&]() { return tool_tips; },
                [&]() -> auto & { return m_flatten_type; }))
          {
               return;
          }
     }
     else
     {
          if (!fme::generic_combo(
                imgui_id,
                gui_labels::flatten,
                [&]() { return values_only_palette; },
                [&]() { return values_only_palette | std::views::transform(AsString{}); },
                [&]() { return tool_tips_only_palette; },
                [&]() -> auto & { return m_flatten_type; }))
          {
               return;
          }
     }
     Configuration config{};
     config->insert_or_assign("batch_flatten_type", static_cast<std::underlying_type_t<flatten_type>>(m_flatten_type.value()));
     config->insert_or_assign("batch_flatten_enabled", m_flatten_type.enabled());
     config.save();
     return;
}
void batch::browse_input_path(int &imgui_id)
{
     if (m_input_type == input_types::mim)
     {
          return;
     }
     if (!browse_path(imgui_id, "Input Path", m_input_path_valid, m_input_path))
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
void batch::browse_output_path(int &imgui_id)
{
     if (!browse_path(imgui_id, "Output Path", m_output_path_valid, m_output_path))
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
void batch::draw_multi_column_list_box(
  int                            &imgui_id,
  const std::string_view          name,
  const std::vector<std::string> &items,
  std::vector<bool>              &enabled)
{
     if (!ImGui::CollapsingHeader(name.data()))
     {
          return;
     }


     if (ImGui::Button("Select All"))
     {
          for (size_t i = 0; i < enabled.size(); ++i)
          {
               enabled[i] = true;
          }
     }

     ImGui::SameLine();

     if (ImGui::Button("Select None"))
     {
          for (size_t i = 0; i < enabled.size(); ++i)
          {
               enabled[i] = false;
          }
     }

     const auto pop_border = scope_guard{ []() { ImGui::PopStyleColor(); } };
     ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));// Set border alpha to 0

     ImGui::Columns(3, "multicol_listbox");
     //     ImGui::Separator();
     //     ImGui::Text("Toggle"); ImGui::NextColumn();
     //     ImGui::Text("Item"); ImGui::NextColumn();
     ImGui::Separator();

     ImVec4 enabledColor = ImVec4(0.4f, 0.8f, 0.4f, 1.0f);// Green

     for (size_t i = 0; i < items.size(); ++i)
     {
          const auto pop_id = scope_guard{ []() {
               ImGui::PopID();
               ImGui::NextColumn();
          } };
          ImGui::PushID(imgui_id++);
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
               ImGui::PushStyleColor(ImGuiCol_Text, enabledColor);

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
void batch::button_start(int &imgui_id)
{
     const auto pop_id_right = scope_guard{ &ImGui::PopID };
     const auto spacing      = ImGui::GetStyle().ItemInnerSpacing.x;
     ImGui::PushID(++imgui_id);
     ImGui::BeginDisabled(
       ((m_input_type == input_types::mim || (!m_input_path_valid && m_input_type != input_types::mim)) && !m_output_path_valid)
       || !m_archives_group.operator bool());
     ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0F, 0.5F, 0.0F, 1.0F));// Green
     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3F, 0.8F, 0.3F, 1.0F));// Light green hover
     ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1F, 0.3F, 0.1F, 1.0F));// Dark green active
     if (ImGui::Button("Begin Batch Operation...", ImVec2{ ImGui::GetContentRegionAvail().x * 0.75F - spacing, ImGui::GetFrameHeight() }))
     {
          m_fields_consumer = m_archives_group->fields();
          m_field.reset();
          m_lang_consumer.restart();
          m_coo.reset();
     }
     ImGui::PopStyleColor(3);
     ImGui::EndDisabled();
}

void batch::button_stop(int &imgui_id)
{
     const auto pop_id_right = scope_guard{ &ImGui::PopID };
     const auto spacing      = ImGui::GetStyle().ItemInnerSpacing.x;
     ImGui::SameLine(0, spacing);
     ImGui::PushID(++imgui_id);
     ImGui::BeginDisabled(
       ((m_input_type == input_types::mim || (!m_input_path_valid && m_input_type != input_types::mim)) && !m_output_path_valid)
       || !m_archives_group.operator bool());
     ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5F, 0.0F, 0.0F, 1.0F));// Red
     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8F, 0.3F, 0.3F, 1.0F));// Light red hover
     ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3F, 0.1F, 0.1F, 1.0F));// Dark red active
     if (ImGui::Button("Stop", ImVec2{ ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight() }))
     {
          stop();
     }
     ImGui::PopStyleColor(3);
     ImGui::EndDisabled();
}
void batch::button_input_browse()
{
     m_directory_browser.Open();
     m_directory_browser.SetTitle("Choose directory to load textures from...");
     m_directory_browser.SetPwd(m_input_path.data());
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_directory_browser_mode = directory_mode::input_mode;
};
void batch::button_output_browse()
{
     m_directory_browser.Open();
     m_directory_browser.SetTitle("Choose directory to save textures to...");
     m_directory_browser.SetPwd(m_output_path.data());
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_directory_browser_mode = directory_mode::output_mode;
};
bool batch::browse_path(int &imgui_id, std::string_view name, bool &valid_path, std::array<char, m_buffer_size> &path_buffer)
{
     bool              changed      = false;
     const ImGuiStyle &style        = ImGui::GetStyle();
     const float       spacing      = style.ItemInnerSpacing.x;
     const float       button_size  = ImGui::GetFrameHeight();
     const float       button_width = button_size * 2.60F;
     const auto        pop_id       = scope_guard{ &ImGui::PopID };
     ImGui::PushID(++imgui_id);
     // ImGui text box with browse button
     // Highlight text box red if the folder doesn't exist
     {
          const float width = ImGui::CalcItemWidth();
          ImGui::PushItemWidth(width - (spacing * 1.0F) - button_width);
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
     }
     ImGui::SameLine(0, spacing);
     {
          if (ImGui::Button("Browse", ImVec2{ button_width, button_size }))
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
     format_imgui_text("{}", name.data());
     return changed;
}
void batch::update(sf::Time elapsed_time)
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
          m_map_sprite.save_modified_map(append_file_structure(m_output_path.data()) / m_map_sprite.map_filename());
     }
     reset_for_next();
}
bool batch::consume_one_future()
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
void batch::generate_map_sprite()
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
void batch::compact()
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
          case compact_type::map_order:
               m_map_sprite.compact_map_order();
               break;
     }
}
void batch::flatten()
{
     if (!m_flatten_type.enabled())
     {
          return;
     }
     switch (m_flatten_type.value())
     {
          case flatten_type::bpp:
               if (!m_compact_type.enabled() || m_compact_type.value() != compact_type::map_order)
               {
                    m_map_sprite.flatten_bpp();
               }
               break;
          case flatten_type::palette:
               m_map_sprite.flatten_palette();
               break;
          case flatten_type::both:
               if (!m_compact_type.enabled() || m_compact_type.value() != compact_type::map_order)
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
void batch::reset_for_next()
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
void batch::choose_field_and_coo()
{
     while ((!m_field || !m_field->operator bool()) && !m_fields_consumer.done())
     {
          open_viii::archive::FIFLFSArchiveFetcher tmp         = *m_fields_consumer;
          const auto                              &map_data    = m_archives_group->mapdata();
          const auto                               find_result = std::ranges::find_if(
            map_data, [&](std::string_view map_name) -> bool { return open_viii::tools::i_find(map_name, tmp.map_name()); });

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
std::filesystem::path batch::append_file_structure(const std::filesystem::path &path) const
{
     std::string const      name   = m_map_sprite.get_base_name();
     std::string_view const prefix = std::string_view(name).substr(0, 2);
     return path / prefix / name;
}
void batch::open_directory_browser()
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
void batch::checkbox_load_map(int &imgui_id)
{
     if (!(m_input_type != input_types::mim))
     {
          return;
     }
     const auto pop_id = scope_guard{ &ImGui::PopID };
     ImGui::PushID(++imgui_id);
     if (!ImGui::Checkbox("Load Map", &m_input_load_map))
     {
          return;
     }
     Configuration config{};
     config->insert_or_assign("batch_input_load_map", m_input_load_map);
     config.save();
}
