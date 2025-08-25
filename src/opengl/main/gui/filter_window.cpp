#include "filter_window.hpp"
#include "as_string.hpp"
#include "generic_combo.hpp"
#include "gui/ImGuiDisabled.hpp"
#include "gui_labels.hpp"
#include "push_pop_id.hpp"
#include <ctre.hpp>

fme::filter_window::filter_window(std::weak_ptr<Selections> in_selections, std::weak_ptr<map_sprite> in_map_sprite)
  : m_selections(std::move(in_selections))
  , m_map_sprite(std::move(in_map_sprite))

{
}


void fme::filter_window::collapsing_header_filters() const
{
     m_changed = false;
     if (ImGui::CollapsingHeader(gui_labels::filters.data()))
     {
          auto lock_map_sprite = m_map_sprite.lock();
          if (!lock_map_sprite)
          {
               spdlog::error("Failed to lock map_sprite: shared_ptr is expired.");
               return;
          }
          combo_filtered_pupu(lock_map_sprite);
          combo_filtered_bpps(lock_map_sprite);
          combo_filtered_palettes(lock_map_sprite);
          combo_filtered_blend_modes(lock_map_sprite);
          combo_filtered_blend_other(lock_map_sprite);
          combo_filtered_layers(lock_map_sprite);
          combo_filtered_texture_pages(lock_map_sprite);
          combo_filtered_animation_ids(lock_map_sprite);
          combo_filtered_animation_states(lock_map_sprite);
          combo_filtered_z(lock_map_sprite);
          combo_filtered_draw_bit(lock_map_sprite);
     }
}

void fme::filter_window::render() const
{
     m_changed            = false;
     auto lock_selections = m_selections.lock();
     if (!lock_selections)
     {
          spdlog::error("Failed to lock selections: shared_ptr is expired.");
          return;
     }
     auto lock_map_sprite = m_map_sprite.lock();
     if (!lock_map_sprite)
     {
          spdlog::error("Failed to lock map_sprite: shared_ptr is expired.");
          return;
     }
     if (!begin_window(lock_selections))
     {
          return;
     }
     const auto pop_end = glengine::ScopeGuard(&ImGui::End);
     if (lock_map_sprite->fail())
     {
          format_imgui_text("The `.map` is in an invalid state.\nSo no filters are avalible.");
          return;
     }
     if (lock_selections->get<ConfigKey::DrawMode>() != draw_mode::draw_map)
     {
          format_imgui_text("The draw mode is not set to `.map`.\nFilter changes won't show on draw window.");
     }
     
     handle_remove_queue(lock_selections, lock_map_sprite);
     constexpr auto flags = ImGuiInputFlags_RouteOverFocused;
     if (ImGui::Shortcut(ImGuiKey_Escape, flags))
     {
          m_last_selected      = {};
          m_selected_file_name = {};
          m_multi_select.clear();
     }
     ImGui::SliderFloat("Thumbnail Size", &m_thumb_size_width, 96.f, 1024.f);
     bool  ctrl  = ImGui::GetIO().KeyCtrl;
     float wheel = ImGui::GetIO().MouseWheel;

     if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ctrl)
     {
          static const constexpr auto speed = 20.f;
          if (wheel > 0.0f)
               m_thumb_size_width = std::min(m_thumb_size_width + (wheel * speed), 1024.f);
          else if (wheel < 0.0f)
               m_thumb_size_width = std::max(m_thumb_size_width + (wheel * speed), 96.f);// add because wheel is negative
     }

     m_textures_map = &lock_map_sprite->get_deswizzle_combined_textures();

     if (m_reload_thumbnail)
     {
          m_reload_thumbnail = false;
          if (!m_multi_select.empty())
          {
               for (const std::string &current_file_name : m_multi_select)
               {
                    m_textures_map->erase(current_file_name);
               }
          }
          else
          {
               m_textures_map->erase(m_selected_file_name);
          }
          (void)lock_map_sprite->get_deswizzle_combined_textures();
     }

     if (m_selected_file_name.empty() || !m_textures_map->contains(m_selected_file_name))
     {
          render_list_view(lock_selections, lock_map_sprite);
     }
     else if (m_textures_map->contains(m_selected_file_name))
     {
          render_detail_view(lock_selections, lock_map_sprite);
     }
}

void fme::filter_window::menu() const
{
     m_changed            = false;
     auto lock_map_sprite = m_map_sprite.lock();
     if (!lock_map_sprite)
     {
          spdlog::error("Failed to lock map_sprite: shared_ptr is expired.");
          return;
     }
     menu_filtered_pupu(lock_map_sprite);
     menu_filtered_bpps(lock_map_sprite);
     menu_filtered_palettes(lock_map_sprite);
     menu_filtered_blend_modes(lock_map_sprite);
     menu_filtered_blend_other(lock_map_sprite);
     menu_filtered_layers(lock_map_sprite);
     menu_filtered_texture_pages(lock_map_sprite);
     menu_filtered_animation_ids(lock_map_sprite);
     menu_filtered_animation_states(lock_map_sprite);
     menu_filtered_z(lock_map_sprite);
     menu_filtered_draw_bit(lock_map_sprite);
}

void fme::filter_window::update(std::weak_ptr<Selections> in_selections)
{
     m_selections = std::move(in_selections);
}
void fme::filter_window::update(std::weak_ptr<map_sprite> in_map_sprite)
{
     m_map_sprite = std::move(in_map_sprite);
}

bool fme::filter_window::begin_window(const std::shared_ptr<Selections> &lock_selections) const
{
     bool      &visible     = lock_selections->get<ConfigKey::DisplayFiltersWindow>();
     const auto pop_visible = glengine::ScopeGuard{ [&lock_selections, &visible, was_visable = visible] {
          if (was_visable != visible)
          {
               lock_selections->update<ConfigKey::DisplayFiltersWindow>();
          }
     } };
     if (!visible)
     {
          return false;
     }
     if (!ImGui::Begin(gui_labels::deswizzle_toml_editor.data(), &visible))
     {
          ImGui::End();
          return false;
     }
     return true;
}

void fme::filter_window::handle_remove_queue(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     if (m_remove_queue.empty())
          return;

     for (const auto &file_name : m_remove_queue)
     {
          (void)lock_map_sprite->remove_deswizzle_combined_toml_table(file_name);
     }
     save_config(lock_selections);
     m_remove_queue.clear();
}

void fme::filter_window::save_config(const std::shared_ptr<Selections> &lock_selections) const
{

     // TODO fill in common values here or else users can't use them. Like Field names and coo
     const key_value_data        config_path_values = { .ext = ".toml" };
     const std::filesystem::path config_path =
       config_path_values.replace_tags(lock_selections->get<ConfigKey::OutputTomlPattern>(), lock_selections);
     auto config = Configuration(config_path);
     config.save();
}

void fme::filter_window::render_list_view(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     
     const float  button_height = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2.0f;
     const ImVec2 button_size   = { m_tool_button_size_width, button_height };
     ImGui::Columns(calc_column_count(m_tool_button_size_width), "##get_deswizzle_combined_tool_buttons", false);
     ImGui::BeginDisabled(std::ranges::empty(m_multi_select));
     format_imgui_wrapped_text("Selected {} Items(s): ", std::ranges::size(m_multi_select));
     ImGui::NextColumn();
     // Combine into a new entry (keep originals)
     if (ImGui::Button(ICON_FA_LAYER_GROUP " Combine (New)", button_size))
     {
          (void)lock_map_sprite->add_combine_deswizzle_combined_toml_table(m_multi_select, generate_file_name(lock_map_sprite));
          save_config(lock_selections);
     }
     tool_tip("Combine selected entries into a new entry without removing the originals.");
     ImGui::NextColumn();
     // Combine and replace originals
     if (ImGui::Button(ICON_FA_OBJECT_GROUP " Combine (Replace)", button_size))
     {
          (void)lock_map_sprite->add_combine_deswizzle_combined_toml_table(m_multi_select, generate_file_name(lock_map_sprite));
          std::ranges::move(m_multi_select, std::back_inserter(m_remove_queue));
          m_multi_select.clear();
     }
     tool_tip("Combine selected entries into one entry and remove the originals.");
     ImGui::NextColumn();
     // Copy
     if (ImGui::Button(ICON_FA_COPY " Copy", button_size))
     {
          (void)lock_map_sprite->copy_deswizzle_combined_toml_table(
            m_multi_select, [&, index = int{}]() mutable { return generate_file_name(lock_map_sprite, index++); });
          save_config(lock_selections);
          // todo: copy create new entries with generated name (prefix_timestamp_index.png).
     }
     tool_tip("Copy selected entries into new entries with generated names.");
     ImGui::NextColumn();
     if (ImGui::Button(ICON_FA_TRASH " Remove", button_size))
     {
          std::ranges::move(m_multi_select, std::back_inserter(m_remove_queue));
          m_multi_select.clear();
     }
     tool_tip("Remove selected entries.");
     ImGui::NextColumn();
     if (ImGui::Button(ICON_FA_FILTER " Pupu Filter", button_size))
     {
          ImGui::OpenPopup("Pupu Filter Popup");
     }
     tool_tip("Bulk enable or disable pupu.");
     popup_combo_filtered_pupu(lock_selections, lock_map_sprite);
     ImGui::NextColumn();
     if (ImGui::Button(ICON_FA_BROOM " Clear Selection", button_size))
     {
          m_multi_select.clear();
     }
     tool_tip("Clear the current selection.");
     ImGui::EndDisabled();
     ImGui::NextColumn();
     if (ImGui::Button(ICON_FA_PLUS " Add New", button_size))
     {
          add_new_entry(lock_selections, lock_map_sprite);
     }
     tool_tip("Add a new entry.\nHold Ctrl to add multiple entries.\nWithout Ctrl, the mode will switch to editing the new entry.");
     ImGui::Columns(1);
     ImGui::BeginChild("##Scrolling");
     ImGui::Columns(calc_column_count(m_thumb_size_width), "##get_deswizzle_combined_textures", false);

     for (const auto &[file_name, framebuffer] : *m_textures_map)
     {
          draw_thumbnail(lock_map_sprite, file_name, framebuffer, [&]() { select_file(file_name, lock_map_sprite); });
          draw_thumbnail_label(file_name);
          ImGui::NextColumn();
     }

     draw_add_new_button(lock_selections, lock_map_sprite);

     ImGui::Columns(1);
     ImGui::EndChild();
}

int fme::filter_window::calc_column_count(float width) const
{
     const ImVec2 region_size = ImGui::GetContentRegionAvail();
     const float  padding     = ImGui::GetStyle().FramePadding.x * 2.0f + ImGui::GetStyle().ItemSpacing.x;
     const int    count       = static_cast<int>(region_size.x / (width + padding));
     return count > 0 ? count : 1;
}


void fme::filter_window::select_file(const std::string &file_name, const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     if (auto *ptr = lock_map_sprite->get_deswizzle_combined_toml_table(file_name); ptr)
     {
          if (ImGui::GetIO().KeyCtrl)
          {
               const auto it = std::ranges::find(m_multi_select, file_name);
               if (it == std::ranges::end(m_multi_select))
               {
                    m_multi_select.push_back(file_name);
                    m_last_selected = file_name;
               }
               else
               {
                    m_last_selected = {};
                    m_multi_select.erase(it);
               }
               return;
          }
          else if (ImGui::GetIO().KeyShift && std::ranges::empty(m_last_selected))
          {
               // No anchor set yet — just select this one and set anchor
               m_multi_select.clear();
               m_multi_select.push_back(file_name);
               m_last_selected = file_name;
               return;
          }
          else if (ImGui::GetIO().KeyAlt && std::ranges::empty(m_last_selected))
          {
               m_last_selected = file_name;
               return;
          }
          else if ((ImGui::GetIO().KeyShift || ImGui::GetIO().KeyAlt) && !std::ranges::empty(m_last_selected))
          {
               auto last_it            = m_textures_map->find(m_last_selected);
               auto it                 = m_textures_map->find(file_name);
               m_last_selected         = file_name;
               const auto get_subrange = [&]() {
                    // Figure out order
                    auto begin_it = last_it;
                    auto end_it   = it;
                    // Walk forward until we either find end_it or hit the real end
                    for (auto tmp = last_it; tmp != m_textures_map->end(); ++tmp)
                    {
                         if (tmp == it)
                         {
                              // last_it comes before it
                              return std::ranges::subrange(begin_it, std::ranges::next(end_it));
                         }
                    }

                    // If we didn’t find it going forward, then last_it must come after it
                    begin_it = it;
                    end_it   = last_it;
                    return std::ranges::subrange(begin_it, std::ranges::next(end_it));
               };

               const auto add = [this](auto &&range) {
                    for (const auto &[file_name, _] : range)
                    {
                         m_multi_select.push_back(file_name);
                    }
                    std::ranges::sort(m_multi_select);
                    auto not_unique = std::ranges::unique(m_multi_select);
                    m_multi_select.erase(not_unique.begin(), not_unique.end());
               };

               const auto remove = [this](auto &&range) {
                    for (const auto &[file_name, _] : range)
                    {
                         auto found = std::ranges::find(m_multi_select, file_name);
                         if (found != m_multi_select.end())
                         {
                              m_multi_select.erase(found);
                         }
                    }
               };
               if (ImGui::GetIO().KeyShift)
               {
                    add(get_subrange());
               }
               else
               {
                    remove(get_subrange());
               }
               return;
          }
          m_multi_select.clear();
          m_selected_file_name = file_name;
          const auto count     = (std::min)(s_max_chars, static_cast<size_t>(m_selected_file_name.size()));
          std::ranges::copy_n(m_selected_file_name.begin(), count, m_file_name_buffer.begin());
          m_file_name_buffer[count] = '\0';
          m_selected_toml_table     = ptr;
          lock_map_sprite->filter().reload(*ptr);
          lock_map_sprite->update_render_texture();
          m_previous_file_name = prev_key();
          m_next_file_name     = next_key();
     }
}


std::optional<std::string> fme::filter_window::prev_key() const
{
     auto it = m_textures_map->find(m_selected_file_name);
     if (std::ranges::end(*m_textures_map) == it || std::ranges::begin(*m_textures_map) == it)
     {
          return std::nullopt;
     }
     it = std::ranges::prev(it);
     return it->first;
}

std::optional<std::string> fme::filter_window::next_key() const
{
     if (std::ranges::empty(*m_textures_map))
     {
          return std::nullopt;
     }
     auto it = m_textures_map->find(m_selected_file_name);
     if (std::ranges::end(*m_textures_map) == it)
     {
          return std::nullopt;
     }
     it = std::ranges::next(it);
     if (std::ranges::end(*m_textures_map) == it)
     {
          // return std::nullopt;
          return std::ranges::begin(*m_textures_map)->first;// loop
     }
     return it->first;
}

void fme::filter_window::draw_thumbnail_label(const std::string &file_name) const
{
     // Label under image (optional)
     const float  button_width    = ImGui::GetFrameHeight();
     const ImVec2 button_size     = { button_width, button_width };
     const float  text_area_width = m_thumb_size_width - button_width + ImGui::GetStyle().FramePadding.x;
     // Remember the top-left of where we want to start
     const ImVec2 text_start_pos  = ImGui::GetCursorScreenPos();

     ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + text_area_width);
     ImGui::Text("%s", file_name.c_str());
     ImGui::PopTextWrapPos();
     const ImVec2 backup_pos = ImGui::GetCursorScreenPos();
     // Position the button at top-right of this block (same Y as the start of the text)
     ImGui::SetCursorScreenPos(ImVec2(text_start_pos.x + text_area_width + ImGui::GetStyle().FramePadding.x, text_start_pos.y));
     const auto pop_id = PushPopID();
     if (ImGui::Button(ICON_FA_TRASH, button_size))
     {
          m_remove_queue.push_back(file_name);
     }
     else
     {
          tool_tip("Remove");
     }
     ImGui::SetCursorScreenPos(ImVec2(backup_pos.x, (std::max)(ImGui::GetCursorScreenPos().y, backup_pos.y)));
}


void fme::filter_window::draw_add_new_button(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{


     ImTextureID tex_id = m_hovered_file_name == "##add"
                            ? glengine::ConvertGliDtoImTextureId<ImTextureID>(lock_map_sprite->get_framebuffer().color_attachment_id(1))
                            : glengine::ConvertGliDtoImTextureId<ImTextureID>(lock_map_sprite->get_framebuffer().color_attachment_id());
     m_aspect_ratio =
       static_cast<float>(lock_map_sprite->get_framebuffer().height()) / static_cast<float>(lock_map_sprite->get_framebuffer().width());
     const ImVec2 thumb_size = { m_thumb_size_width, m_thumb_size_width * m_aspect_ratio };
     if (ImGui::ImageButton("add new item", tex_id, thumb_size))
     {
          add_new_entry(lock_selections, lock_map_sprite);
     }
     else
     {
          tool_tip("Add a new entry.\nHold Ctrl to add multiple entries.\nWithout Ctrl, the mode will switch to editing the new entry.");
     }

     if (ImGui::IsItemHovered())
     {
          m_hovered_file_name = "##add";
     }
     else if (m_hovered_file_name == "##add")
     {
          m_hovered_file_name = {};
     }
     format_imgui_wrapped_text("Add new entry...");
}

void fme::filter_window::add_new_entry(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{

     m_selected_file_name = generate_file_name(lock_map_sprite);

     if (!ImGui::GetIO().KeyCtrl)
     {
          m_multi_select.clear();
          m_selected_toml_table = lock_map_sprite->add_deswizzle_combined_toml_table(m_selected_file_name);

          std::ranges::copy_n(
            m_selected_file_name.begin(),
            (std::min)(s_max_chars, static_cast<size_t>(m_selected_file_name.size())),
            m_file_name_buffer.begin());
     }
     else
     {
          (void)lock_map_sprite->add_deswizzle_combined_toml_table(m_selected_file_name);
          m_selected_file_name.clear();
     }

     save_config(lock_selections);
}


std::string fme::filter_window::generate_file_name(const std::shared_ptr<map_sprite> &lock_map_sprite, const std::optional<int> index) const
{
     auto now = std::chrono::system_clock::now();
     auto sec = std::chrono::time_point_cast<std::chrono::seconds>(now);
     auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
     if (index.has_value())
     {
          return fmt::format(
            "{}_{:%Y%m%d_%H%M%S}_{:03d}_{}.png", lock_map_sprite->get_recommended_prefix(), sec, ms.count(), index.value());
     }
     return fmt::format("{}_{:%Y%m%d_%H%M%S}_{:03d}.png", lock_map_sprite->get_recommended_prefix(), sec, ms.count());
}

void fme::filter_window::render_detail_view(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &framebuffer = m_textures_map->at(m_selected_file_name);
     draw_thumbnail(lock_map_sprite, m_selected_file_name, framebuffer, [&]() { unselect_file(); });
     draw_filename_controls(lock_selections, lock_map_sprite);
     ImGui::Separator();
     draw_filter_controls(lock_map_sprite);

     if (m_changed)
     {
          m_reload_thumbnail = true;
          lock_map_sprite->filter().update(*m_selected_toml_table);
          save_config(lock_selections);
     }
}

void fme::filter_window::draw_filename_controls(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     if (ImGui::InputText("##Empty", m_file_name_buffer.data(), m_file_name_buffer.size() - 1U))
     {
     }

     constexpr static auto pattern  = CTRE_REGEX_INPUT_TYPE{ R".((?i)^[a-z0-9_\-\.]+\.png$)." };
     bool                  valid_fn = ctre::match<pattern>(
       m_file_name_buffer.data(),
       m_file_name_buffer.data()
         + static_cast<std::ranges::range_difference_t<std::array<char, 1>>>(
           strnlen(m_file_name_buffer.data(), m_file_name_buffer.size())));
     if (!valid_fn)
     {
          ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Invalid filename (must be alphanumeric with .png extension)");
     }
     const std::string prefix = lock_map_sprite->get_recommended_prefix();
     if (ImGui::Button("Copy Prefix"))
     {
          ImGui::SetClipboardText(prefix.c_str());
     }
     else
     {
          tool_tip(prefix);
     }
     ImGui::SameLine();
     ImGui::BeginDisabled(
       std::ranges::equal(
         m_selected_file_name, std::string_view(m_file_name_buffer.data(), strnlen(m_file_name_buffer.data(), m_file_name_buffer.size()))));
     ImGui::BeginDisabled(!valid_fn);
     if (ImGui::Button("Rename"))
     {
          auto new_file_name = std::string(m_file_name_buffer.data());
          (void)lock_map_sprite->rename_deswizzle_combined_toml_table(m_selected_file_name, new_file_name);
          m_selected_file_name = std::move(new_file_name);
          save_config(lock_selections);
     }
     ImGui::EndDisabled();
     ImGui::SameLine();
     if (ImGui::Button("Reset"))
     {
          m_file_name_buffer = {};
          std::ranges::copy_n(
            m_selected_file_name.begin(),
            (std::min)(s_max_chars, static_cast<size_t>(m_selected_file_name.size())),
            m_file_name_buffer.begin());
     }
     else
     {
          tool_tip(m_selected_file_name);
     }
     ImGui::EndDisabled();
     ImGui::SameLine();
     {
          const auto pop_id        = PushPopID();
          const bool has_prev      = m_previous_file_name.has_value();
          const auto disabled      = ImGuiDisabled(!has_prev);

          const bool activate_prev = ImGui::ArrowButton("##l", ImGuiDir_Left) || (has_prev && ImGui::IsKeyPressed(ImGuiKey_LeftArrow));
          if (activate_prev)
          {
               select_file(m_previous_file_name.value(), lock_map_sprite);
          }
          else if (has_prev)
          {
               tool_tip(m_previous_file_name.value());
          }
     }
     ImGui::SameLine();
     {
          const auto pop_id        = PushPopID();
          const bool has_next      = m_next_file_name.has_value();
          const auto disabled      = ImGuiDisabled(!has_next);

          const bool activate_next = ImGui::ArrowButton("##l", ImGuiDir_Right) || (has_next && ImGui::IsKeyPressed(ImGuiKey_RightArrow));
          if (activate_next)
          {
               select_file(m_next_file_name.value(), lock_map_sprite);
          }
          else if (has_next)
          {
               tool_tip(m_next_file_name.value());
          }
     }
     ImGui::SameLine();
     if (ImGui::Button(ICON_FA_TRASH))
     {
          m_remove_queue.push_back(std::move(m_selected_file_name));
          unselect_file();
     }
     else
     {
          tool_tip("Remove");
     }
}

void fme::filter_window::unselect_file() const
{
     m_selected_file_name = {};
     m_last_selected      = {};
     m_file_name_buffer   = {};
}

void fme::filter_window::draw_filter_controls(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{

     combo_filtered_pupu(lock_map_sprite);
     ImGui::Separator();
     format_imgui_wrapped_text(
       "You may use these other filters in the export or testing process but we only can import via Pupu IDs. This may change in the "
       "future once we figure out how.");
     ImGui::Separator();
     combo_filtered_bpps(lock_map_sprite);
     combo_filtered_palettes(lock_map_sprite);
     combo_filtered_blend_modes(lock_map_sprite);
     combo_filtered_blend_other(lock_map_sprite);
     combo_filtered_layers(lock_map_sprite);
     combo_filtered_texture_pages(lock_map_sprite);
     combo_filtered_animation_ids(lock_map_sprite);
     combo_filtered_animation_states(lock_map_sprite);
     combo_filtered_z(lock_map_sprite);
     combo_filtered_draw_bit(lock_map_sprite);
}


void fme::filter_window::popup_combo_filtered_pupu(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     if (ImGui::BeginPopupModal("Pupu Filter Popup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
     {
          const auto gcc = GenericComboWithMultiFilter(
            gui_labels::pupu_id,
            [&]() { return lock_map_sprite->working_unique_pupu(); },
            [&]() { return lock_map_sprite->working_unique_pupu() | std::views::transform(AsString{}); },
            [&]() {
                 return lock_map_sprite->working_unique_pupu()
                        | std::views::transform([](const ff_8::PupuID &pupu_id) -> decltype(auto) { return pupu_id.create_summary(); });
            },
            [&]() -> auto & { return m_multi_select_filter; });
          (void)gcc.render();
          format_imgui_wrapped_text(
            "{}",
            m_multi_select_filter.enabled() ? "IDs will be " ICON_FA_LAYER_GROUP " added to the selected entries."
                                            : "IDs will be " ICON_FA_TRASH " removed from the selected entries.");
          if (ImGui::Button(ICON_FA_CHECK " Apply"))
          {
               for (const std::string &file_name : m_multi_select)
               {
                    lock_map_sprite->apply_multi_pupu_filter_deswizzle_combined_toml_table(file_name, m_multi_select_filter);
               }
               m_reload_thumbnail = true;
               ImGui::CloseCurrentPopup();
               save_config(lock_selections);
          }
          ImGui::SameLine();
          if (ImGui::Button(ICON_FA_XMARK " Cancel"))
          {
               ImGui::CloseCurrentPopup();
          }

          ImGui::EndPopup();
     }
}

void fme::filter_window::combo_filtered_pupu(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto gcc = GenericComboWithMultiFilter(
       gui_labels::pupu_id,
       [&]() { return lock_map_sprite->working_unique_pupu(); },
       [&]() { return lock_map_sprite->working_unique_pupu() | std::views::transform(AsString{}); },
       [&]() {
            return lock_map_sprite->working_unique_pupu()
                   | std::views::transform([](const ff_8::PupuID &pupu_id) -> decltype(auto) { return pupu_id.create_summary(); });
       },
       [&]() -> auto & { return lock_map_sprite->filter().multi_pupu; });

     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

void fme::filter_window::combo_filtered_bpps(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().bpp();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::bpp,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_bpp; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

void fme::filter_window::combo_filtered_palettes(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &map         = lock_map_sprite->uniques().palette();
     const auto &keys        = lock_map_sprite->filter().multi_bpp.value();
     const auto  join_vector = [](auto &&pairs) {
          auto transform_pairs = pairs | std::views::transform([](const auto &pair) { return pair.zip(); });
          auto join_pairs      = std::ranges::join_view(transform_pairs);
          return join_pairs | std::ranges::to<std::vector>();
     };

     auto value_string_pairs = [&]() {
          if (keys.empty() || !lock_map_sprite->filter().multi_bpp.enabled())
          {
               return join_vector(map | std::views::values);
          }
          else
          {
               auto keys_filter = keys | std::views::filter([&](const auto &key) { return map.contains(key); });
               auto keys_transform =
                 keys_filter | std::views::transform([&](const auto &key) { return map.at(key); }) | std::ranges::to<std::vector>();

               return join_vector(keys_transform);
          }
     }();


     // Deduplicate based on value
     std::ranges::sort(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     const auto unique_range = std::ranges::unique(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     value_string_pairs.erase(unique_range.begin(), unique_range.end());

     // Extract values and strings into separate views
     auto       values  = value_string_pairs | std::views::transform([](const auto &pair) { return std::get<0>(pair); });
     auto       strings = value_string_pairs | std::views::transform([](const auto &pair) { return std::get<1>(pair); });

     const auto gcc     = fme::GenericComboWithMultiFilter(
       gui_labels::palette,
       [&values]() { return values; },
       [&strings]() { return strings; },
       [&strings]() { return strings; },
       [&]() -> auto     &{ return lock_map_sprite->filter().multi_palette; });

     if (!gcc.render())
     {
          return;
     }

     lock_map_sprite->update_render_texture();
     m_changed = true;
}


void fme::filter_window::combo_filtered_blend_modes(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().blend_mode();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::blend_mode,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_blend_mode; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

void fme::filter_window::combo_filtered_blend_other(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().blend_other();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::blend_other,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_blend_other; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

void fme::filter_window::combo_filtered_layers(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().layer_id();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::layer_id,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_layer_id; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

void fme::filter_window::combo_filtered_texture_pages(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().texture_page_id();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::texture_page,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_texture_page_id; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

void fme::filter_window::combo_filtered_animation_ids(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().animation_id();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::animation_id,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_animation_id; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

void fme::filter_window::combo_filtered_animation_states(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &map         = lock_map_sprite->uniques().animation_state();
     const auto &keys        = lock_map_sprite->filter().multi_animation_id.value();
     const auto  join_vector = [](auto &&pairs) {
          auto transform_pairs = pairs | std::views::transform([](const auto &pair) { return pair.zip(); });
          auto join_pairs      = std::ranges::join_view(transform_pairs);
          return join_pairs | std::ranges::to<std::vector>();
     };

     auto value_string_pairs = [&]() {
          if (keys.empty() || !lock_map_sprite->filter().multi_animation_id.enabled())
          {
               return join_vector(map | std::views::values);
          }
          else
          {
               auto keys_filter = keys | std::views::filter([&](const auto &key) { return map.contains(key); });
               auto keys_transform =
                 keys_filter | std::views::transform([&](const auto &key) { return map.at(key); }) | std::ranges::to<std::vector>();

               return join_vector(keys_transform);
          }
     }();

     std::ranges::sort(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     const auto unique_range = std::ranges::unique(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     value_string_pairs.erase(unique_range.begin(), unique_range.end());

     auto       values  = value_string_pairs | std::views::transform([&](const auto &pair) { return std::get<0>(pair); });
     auto       strings = value_string_pairs | std::views::transform([&](const auto &pair) { return std::get<1>(pair); });
     const auto gcc     = fme::GenericComboWithMultiFilter(
       gui_labels::animation_state,
       [&values]() { return values; },
       [&strings]() { return strings; },
       [&strings]() { return strings; },
       [&]() -> auto     &{ return lock_map_sprite->filter().multi_animation_state; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

void fme::filter_window::combo_filtered_z(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().z();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::z,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_z; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

void fme::filter_window::combo_filtered_draw_bit(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     using namespace std::string_view_literals;
     static constexpr auto values = std::array{ ff_8::draw_bitT::all, ff_8::draw_bitT::enabled, ff_8::draw_bitT::disabled };
     const auto            gcc    = fme::GenericComboWithFilter(
       gui_labels::draw_bit,
       []() { return values; },
       []() { return values | std::views::transform(AsString{}); },
       []() {
            return std::array{ gui_labels::draw_bit_all_tooltip,
                               gui_labels::draw_bit_enabled_tooltip,
                               gui_labels::draw_bit_disabled_tooltip };
       },
       [&]() -> auto               &{ return lock_map_sprite->filter().draw_bit; });
     if (!gcc.render())
          return;
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

struct map_pupu_id
{
     std::shared_ptr<fme::map_sprite> m_map_sprite = {};
     const auto                      &values() const
     {
          return m_map_sprite->working_unique_pupu();
     }
     auto strings() const
     {
          return m_map_sprite->working_unique_pupu() | std::views::transform(fme::AsString{});
     }
     auto tooltips() const
     {
          return m_map_sprite->working_unique_pupu()
                 | std::views::transform([](const ff_8::PupuID &pupu_id) -> decltype(auto) { return pupu_id.create_summary(); });
     }
     auto zip() const
     {
          return std::ranges::views::zip(values(), strings(), tooltips());
     }
};

void fme::filter_window::menu_filtered_pupu(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(gui_labels::pupu_id, map_pupu_id{ lock_map_sprite }, lock_map_sprite->filter().multi_pupu, [&]() {
          lock_map_sprite->update_render_texture();
          m_changed = true;
     })();
}

void fme::filter_window::menu_filtered_bpps(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(gui_labels::bpp, lock_map_sprite->uniques().bpp(), lock_map_sprite->filter().multi_bpp, [&]() {
          lock_map_sprite->update_render_texture();
          m_changed = true;
     })();
}

void fme::filter_window::menu_filtered_palettes(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &map         = lock_map_sprite->uniques().palette();
     const auto &keys        = lock_map_sprite->filter().multi_bpp.value();

     const auto  join_vector = [](auto &&pairs) {
          auto transform_pairs = pairs | std::views::transform([](const auto &pair) { return pair.zip(); });
          auto join_pairs      = std::ranges::join_view(transform_pairs);
          return join_pairs | std::ranges::to<std::vector>();
     };

     auto value_string_pairs = [&]() {
          if (keys.empty() || !lock_map_sprite->filter().multi_bpp.enabled())
          {
               return join_vector(map | std::views::values);
          }
          else
          {
               auto keys_filter = keys | std::views::filter([&](const auto &key) { return map.contains(key); });
               auto keys_transform =
                 keys_filter | std::views::transform([&](const auto &key) { return map.at(key); }) | std::ranges::to<std::vector>();

               return join_vector(keys_transform);
          }
     }();

     std::ranges::sort(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     const auto unique_range = std::ranges::unique(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     value_string_pairs.erase(unique_range.begin(), unique_range.end());

     const auto unique_palettes = ff_8::unique_values_and_strings<std::uint8_t>(
       value_string_pairs | std::views::transform([&](const auto &pair) { return std::get<0>(pair); }) | std::ranges::to<std::vector>(),
       value_string_pairs | std::views::transform([&](const auto &pair) { return std::get<1>(pair); }) | std::ranges::to<std::vector>());

     GenericMenuWithMultiFilter(gui_labels::palette, unique_palettes, lock_map_sprite->filter().multi_palette, [&]() {
          lock_map_sprite->update_render_texture();
          m_changed = true;
     })();
}

void fme::filter_window::menu_filtered_blend_modes(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::blend_mode, lock_map_sprite->uniques().blend_mode(), lock_map_sprite->filter().multi_blend_mode, [&]() {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_blend_other(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::blend_other, lock_map_sprite->uniques().blend_other(), lock_map_sprite->filter().multi_blend_other, [&]() {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_layers(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::layer_id, lock_map_sprite->uniques().layer_id(), lock_map_sprite->filter().multi_layer_id, [&]() {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_texture_pages(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::texture_page, lock_map_sprite->uniques().texture_page_id(), lock_map_sprite->filter().multi_texture_page_id, [&]() {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_animation_ids(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::animation_id, lock_map_sprite->uniques().animation_id(), lock_map_sprite->filter().multi_animation_id, [&]() {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_animation_states(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &map         = lock_map_sprite->uniques().animation_state();
     const auto &keys        = lock_map_sprite->filter().multi_animation_id.value();

     const auto  join_vector = [](auto &&pairs) {
          auto transform_pairs = pairs | std::views::transform([](const auto &pair) { return pair.zip(); });
          auto join_pairs      = std::ranges::join_view(transform_pairs);
          return join_pairs | std::ranges::to<std::vector>();
     };

     auto value_string_pairs = [&]() {
          if (keys.empty() || !lock_map_sprite->filter().multi_animation_id.enabled())
          {
               return join_vector(map | std::views::values);
          }
          else
          {
               auto keys_filter = keys | std::views::filter([&](const auto &key) { return map.contains(key); });
               auto keys_transform =
                 keys_filter | std::views::transform([&](const auto &key) { return map.at(key); }) | std::ranges::to<std::vector>();

               return join_vector(keys_transform);
          }
     }();

     std::ranges::sort(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     const auto unique_range = std::ranges::unique(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     value_string_pairs.erase(unique_range.begin(), unique_range.end());

     const auto unique_animation_state = ff_8::unique_values_and_strings<std::uint8_t>(
       value_string_pairs | std::views::transform([&](const auto &pair) { return std::get<0>(pair); }) | std::ranges::to<std::vector>(),
       value_string_pairs | std::views::transform([&](const auto &pair) { return std::get<1>(pair); }) | std::ranges::to<std::vector>());

     GenericMenuWithMultiFilter(
       gui_labels::animation_state, unique_animation_state, lock_map_sprite->filter().multi_animation_state, [&]() {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_z(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(gui_labels::z, lock_map_sprite->uniques().z(), lock_map_sprite->filter().multi_z, [&]() {
          lock_map_sprite->update_render_texture();
          m_changed = true;
     })();
}

struct map_draw_bit
{
   private:
     static constexpr auto m_values   = std::array{ ff_8::draw_bitT::all, ff_8::draw_bitT::enabled, ff_8::draw_bitT::disabled };
     static constexpr auto m_tooltips = std::array{ fme::gui_labels::draw_bit_all_tooltip,
                                                    fme::gui_labels::draw_bit_enabled_tooltip,
                                                    fme::gui_labels::draw_bit_disabled_tooltip };

   public:
     auto values() const
     {
          return m_values;
     }
     auto strings() const
     {
          return m_values | std::views::transform(fme::AsString{});
     }
     auto tooltips() const
     {
          return m_tooltips;
     }
     auto zip() const
     {
          return std::ranges::views::zip(values(), strings(), tooltips());
     }
};

void fme::filter_window::menu_filtered_draw_bit(const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithFilter(gui_labels::draw_bit, map_draw_bit{}, lock_map_sprite->filter().draw_bit, [&]() {
          lock_map_sprite->update_render_texture();
          m_changed = true;
     })();
}

void fme::filter_window::draw_thumbnail(
  const std::shared_ptr<map_sprite>          &lock_map_sprite,
  const std::string                          &file_name,
  const std::optional<glengine::FrameBuffer> &framebuffer,
  std::move_only_function<void()>             on_click) const
{
     const std::string &tooltip_str = [&]() -> const std::string & {
          static const std::string empty_msg = "No filters are enabled...";
          if (lock_map_sprite->get_deswizzle_combined_textures_tooltips().contains(file_name))
          {
               const std::string &tmp = lock_map_sprite->get_deswizzle_combined_textures_tooltips().at(file_name);
               return tmp.empty() ? empty_msg : tmp;
          }
          return empty_msg;
     }();
     if (framebuffer.has_value())
     {
          ImTextureID tex_id           = (m_hovered_file_name == file_name)
                                           ? glengine::ConvertGliDtoImTextureId<ImTextureID>(framebuffer.value().color_attachment_id(1))
                                           : glengine::ConvertGliDtoImTextureId<ImTextureID>(framebuffer.value().color_attachment_id());
          m_aspect_ratio               = static_cast<float>(framebuffer.value().height()) / static_cast<float>(framebuffer.value().width());
          const ImVec2 thumb_size      = { m_thumb_size_width, m_thumb_size_width * m_aspect_ratio };

          const auto   it              = std::ranges::find(m_multi_select, file_name);
          bool         selected        = it != std::ranges::end(m_multi_select);
          const auto   pop_style_color = glengine::ScopeGuard{ [selected]() {
               if (selected)
               {
                    ImGui::PopStyleColor(3);
               }
          } };
          if (selected)
          {
               ImGui::PushStyleColor(ImGuiCol_Button, colors::ButtonGreen);
               ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors::ButtonGreenHovered);
               ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors::ButtonGreenActive);
          }

          if (ImGui::ImageButton(file_name.c_str(), tex_id, thumb_size))
          {
               std::invoke(on_click);
          }
          else
          {
               tool_tip(tooltip_str);
          }
          if (ImGui::IsItemHovered())
          {
               m_hovered_file_name = file_name;
          }
          else if (m_hovered_file_name == file_name)
          {
               m_hovered_file_name = {};
          }
     }
     else
     {
          const auto pop_id            = PushPopID();
          ImVec2     padded_thumb_size = { m_thumb_size_width + ImGui::GetStyle().FramePadding.x * 2.0f,
                                           m_thumb_size_width * m_aspect_ratio + ImGui::GetStyle().FramePadding.y * 2.0f };
          if (ImGui::Button("##Empty", padded_thumb_size))
          {
               on_click();
          }
          else
          {
               tool_tip(tooltip_str);
          }
     }
}