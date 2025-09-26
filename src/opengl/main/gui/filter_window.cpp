#include "filter_window.hpp"
#include "as_string.hpp"
#include "generic_combo.hpp"
#include "gui/ImGuiDisabled.hpp"
#include "gui_labels.hpp"
#include "push_pop_id.hpp"
#include <ctre.hpp>
#include <set>

fme::filter_window::filter_window(
  std::weak_ptr<Selections> in_selections,
  std::weak_ptr<map_sprite> in_map_sprite)
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
               spdlog::error(
                 "Failed to lock map_sprite: shared_ptr is expired.");
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
bool fme::filter_window::shortcut(const ImGuiKeyChord key_chord)
{
     if (!m_was_focused || !m_textures_map)
     {
          return false;
     }
     // Clear selection with Escape
     if (
       key_chord == ImGuiKey_Escape
       || key_chord == (ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_A))
     {
          m_last_selected      = {};
          m_selected_file_name = {};
          m_multi_select.clear();
          return true;
     }
     // Select All with Ctrl+A (only if no active edit)
     else if (
       m_selected_file_name.empty()
       && key_chord == (ImGuiMod_Ctrl | ImGuiKey_A))
     {
          m_multi_select.clear();
          m_multi_select.reserve(m_textures_map->size());
          for (const auto &[current_file_name, _] : *m_textures_map)
          {
               m_multi_select.push_back(current_file_name);
          }

          // Optionally set last_selected to the last file
          if (!m_multi_select.empty())
               m_last_selected = m_multi_select.back();
          else
               m_last_selected = {};

          return true;
     }
     // Inverse selection with Ctrl+I (only if no active edit)
     else if (
       m_selected_file_name.empty()
       && key_chord == (ImGuiMod_Ctrl | ImGuiKey_I))
     {
          std::vector<std::string> new_selection;
          new_selection.reserve(m_textures_map->size());

          for (const auto &[current_file_name, _] : *m_textures_map)
          {
               // Add file_name if it is NOT currently selected
               if (
                 std::ranges::find(m_multi_select, current_file_name)
                 == m_multi_select.end())
               {
                    new_selection.push_back(current_file_name);
               }
          }

          m_multi_select = std::move(new_selection);

          // Update last selected item if any are now selected
          if (!m_multi_select.empty())
               m_last_selected = m_multi_select.back();
          else
               m_last_selected = {};
     }
     return false;
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
     const auto pop_end = glengine::ScopeGuard(
       [&]()
       {
            m_was_focused
              = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
            ImGui::End();
       });
     if (lock_map_sprite->fail())
     {
          format_imgui_text(
            "The `.map` is in an invalid state.\nSo no filters are avalible.");
          return;
     }
     if (lock_selections->get<ConfigKey::DrawMode>() != draw_mode::draw_map)
     {
          format_imgui_text(
            "The draw mode is not set to `.map`.\nFilter changes won't show on "
            "draw window.");
     }

     if (m_search_open)
     {
          ImGui::Selectable(ICON_FA_CHEVRON_DOWN " Search", &m_search_open);
          static std::array<char, 128> filter_buf = {};
          if (!filter_buf[0])
          {
               m_search_field.clear();
          }
          const char *button_text = "Fix Name(s)";
          const char *input_text  = "Filter";
          float       buttonWidth = ImGui::CalcTextSize(button_text).x
                              + ImGui::GetStyle().FramePadding.x * 2.0f;
          float inputWidth = ImGui::GetContentRegionAvail().x - buttonWidth
                             - ImGui::GetStyle().ItemSpacing.x
                             - ImGui::CalcTextSize(input_text).x;

          ImGui::SetNextItemWidth(inputWidth);
          if (ImGui::InputText(
                input_text, filter_buf.data(), filter_buf.size()))
          {
               m_search_field = filter_buf.data();
          }

          ImGui::SameLine();

          toml::table *root_table = get_root_table(lock_selections);
          ImGui::BeginDisabled(m_select_for_fix_names.empty());
          if (ImGui::Button(button_text, ImVec2(buttonWidth, 0)))
          {
               for (auto &[key, tables] : m_select_for_fix_names)
               {
                    auto &[coo_table, file_table] = tables;
                    ff_8::filter_old<ff_8::FilterTag::MultiPupu> multi_pupu
                      = { ff_8::FilterSettings::All_Disabled };
                    multi_pupu.reload(*file_table);
                    const auto optional_field_coo = [&]()
                      -> std::optional<std::pair<std::string, std::string>>
                    {
                         using namespace std::ranges;

                         for (auto &&[field_name, field_val] :
                              *root_table
                                | views::filter(
                                  [](auto &p) { return p.second.is_table(); }))
                         {
                              auto &field_tbl = *field_val.as_table();

                              for (auto &&[coo, nested_val] :
                                   field_tbl
                                     | views::filter(
                                       [](auto &p)
                                       { return p.second.is_table(); }))
                              {
                                   if (
                                     nested_val.as_table()
                                     == coo_table)// pointer
                                                  // equality
                                   {
                                        return std::make_pair(
                                          std::string(field_name),
                                          std::string(coo));
                                   }
                              }
                         }
                         return std::nullopt;
                    }();
                    if (!optional_field_coo)
                    {
                         continue;
                    }
                    const auto &[field_name, coo_3_letter]
                      = *optional_field_coo;

                    open_viii::LangT coo
                      = open_viii::LangCommon::from_string_3_char(coo_3_letter);


                    key_value_data cpm
                      = { .field_name    = field_name,
                          .ext           = ".png",
                          .language_code = coo == open_viii::LangT::generic
                                             ? std::nullopt
                                             : std::optional{ coo } };

                    const auto opt_filename
                      = [&]() -> std::optional<std::string>
                    {
                         for (const std::uint32_t raw_pupu :
                              multi_pupu.value()
                                | std::views::transform(
                                  [](const auto &pupu_id)
                                  { return pupu_id.raw(); }))
                         {
                              cpm.pupu_id                    = raw_pupu;

                              std::filesystem::path out_path = cpm.replace_tags(
                                lock_selections
                                  ->get<ConfigKey::OutputDeswizzlePattern>(),
                                lock_selections,
                                {});

                              if (!coo_table->contains(
                                    out_path.filename().string()))
                              {
                                   return out_path.filename().string();
                              }
                         }
                         return std::nullopt;
                    }();
                    if (!opt_filename)
                    {
                         continue;
                    }


                    auto [it, success] = coo_table->insert(
                      *opt_filename, std::move(*file_table));

                    if (success)
                    {
                         // Now 'file_table' is in a moved-from state,
                         // but we don't care since it's owned by coo_table.
                         if (auto *stored_table = it->second.as_table())
                         {
                              stored_table->insert_or_assign("old_key", key);
                              // rename logic here...
                         }
                    }
                    else
                    {
                         continue;
                    }
                    coo_table->erase(key);
                    // todo we need to check if filer_window is accessing
                    // key or coo_table if so we may need to trigger a
                    // refresh of those values.
                    save_config(lock_selections);
               }
               m_select_for_fix_names.clear();


               // Do your fix logic here
          }
          ImGui::EndDisabled();
          root_table_to_imgui_tree(root_table);
     }
     else
     {
          ImGui::Selectable(ICON_FA_CHEVRON_RIGHT " Search", &m_search_open);
     }
     handle_remove_queue(lock_selections, lock_map_sprite);
     handle_rename_queue(lock_selections, lock_map_sprite);
     handle_regenerate(lock_selections, lock_map_sprite);
     cleanup_invalid_selections();
     handle_thumbnail_size_adjustment();

     if (const auto temp = lock_map_sprite->get_deswizzle_combined_textures();
         temp.has_value())
          m_textures_map = temp.value();
     else
     {
          m_textures_map = nullptr;
          spdlog::trace(
            "{}:{} m_textures_map is nullptr: {}",
            __FILE__,
            __LINE__,
            temp.error());
          return;
     }
     reload_thumbnails_if_needed(lock_map_sprite);

     if (
       m_selected_file_name.empty()
       || !m_textures_map->contains(m_selected_file_name))
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


toml::table *fme::filter_window::get_root_table(
  const std::shared_ptr<Selections> &lock_selections) const
{
     const key_value_data        config_path_values = { .ext = ".toml" };
     const std::filesystem::path config_path = config_path_values.replace_tags(
       lock_selections->get<ConfigKey::OutputTomlPattern>(), lock_selections);
     auto config = Configuration(config_path);
     return &(config.operator toml::table &());
}

void fme::filter_window::root_table_to_imgui_tree(toml::table *root_table) const
{
     std::vector<std::move_only_function<void()>> callbacks = {};
     root_table_to_imgui_tree(root_table, false, 0u, callbacks);
}

void fme::filter_window::root_table_to_imgui_tree(
  toml::table                                  *root_table,
  const bool                                    skip_search,
  const std::uint32_t                           current_depth,
  std::vector<std::move_only_function<void()>> &callbacks) const
{
     if (root_table == nullptr)
     {
          return;
     }

     for (auto &&[key, val] : *root_table)
     {
          const auto label = std::string{ key };
          if (val.is_table())
          {
               const auto pop_id = PushPopID();
               // if skip_search we already found a match.
               if (!skip_search && !contains_key_recursive(val.as_table()))
               {
                    continue;
               }
               bool open = ImGui::TreeNodeEx(
                 label.c_str(), ImGuiTreeNodeFlags_SpanLabelWidth);
               bool use_goto = false;
               if (current_depth == 0u && m_change_field_callback)
               {
                    // Push a lambda for this level
                    callbacks.push_back(
                      [this, label]
                      {
                           if (m_change_field_callback)
                                m_change_field_callback(label);
                      });
                    use_goto = true;
               }
               else if (
                 current_depth == 1u && m_change_coo_callback
                 && m_is_remaster_callback && m_is_remaster_callback())
               {
                    ImGui::SameLine();
                    bool selected = [&]()
                    {
                         for (auto &&[child_key, child_val] : *val.as_table())
                         {
                              if (
                                child_val.is_table()
                                && child_key != "unique_pupu_ids"
                                && !m_select_for_fix_names.contains(
                                  std::string{ child_key }))
                              {
                                   return false;
                                   break;
                              }
                         }
                         return true;
                    }();
                    if (ImGui::Checkbox("##fix_names_parent", &selected))
                    {
                         if (selected)
                         {
                              for (auto &&[child_key, child_val] :
                                   *val.as_table())
                              {
                                   if (
                                     child_val.is_table()
                                     && child_key != "unique_pupu_ids")
                                   {
                                        (void)
                                          m_select_for_fix_names.try_emplace(
                                            std::string{ child_key },
                                            val.as_table(),
                                            child_val.as_table());
                                   }
                              }
                         }
                         else
                         {
                              for (auto &&[child_key, child_val] :
                                   *val.as_table())
                              {
                                   if (child_val.is_table())
                                   {
                                        m_select_for_fix_names.erase(
                                          std::string{ child_key });
                                   }
                              }
                         }
                    }
                    // Push a lambda for this level
                    callbacks.push_back(
                      [this, label]
                      {
                           if (m_change_coo_callback)
                                m_change_coo_callback(label);
                      });
                    use_goto = true;
               }
               else if (current_depth == 2u && label != "unique_pupu_ids")
               {
                    ImGui::SameLine();
                    auto selected = m_select_for_fix_names.contains(label);
                    if (ImGui::Checkbox("##fix_names", &selected))
                    {
                         if (selected)
                         {
                              auto result = m_select_for_fix_names.try_emplace(
                                label, root_table, val.as_table());
                              if (!result.second)
                              {
                                   spdlog::warn(
                                     "Failed to insert '{}' into "
                                     "m_select_for_fix_names: key already "
                                     "exists",
                                     label);
                              }
                         }
                         else
                         {
                              m_select_for_fix_names.erase(label);
                         }
                    }
                    // Push a lambda for this level
                    callbacks.push_back(
                      [this, label]
                      {
                           auto lock_map_sprite = m_map_sprite.lock();
                           if (!lock_map_sprite)
                           {
                                spdlog::error(
                                  "Failed to lock map_sprite: shared_ptr "
                                  "is expired.");
                                return;
                           }
                           select_file(label, lock_map_sprite);
                      });
                    use_goto = true;
               }
               if (use_goto)
               {
                    ImGui::SameLine();
                    if (ImGui::Button("Goto"))
                    {
                         m_search_open = false;
                         for (auto &cb : callbacks)
                              cb();// run parent(s) first, then
                                   // current
                    }
               }
               if (open)
               {
                    root_table_to_imgui_tree(
                      val.as_table(), true, current_depth + 1u, callbacks);
                    ImGui::TreePop();
               }
               if (use_goto)
               {
                    callbacks.pop_back();
               }
          }
          else if (val.is_array())
          {
               bool open = ImGui::TreeNode(label.c_str());
               if (open)
               {
                    for (auto &&item : *val.as_array())
                    {
                         if (item.is_string())
                              format_imgui_bullet_text(
                                "{}", item.value<std::string>());
                         else if (item.is_integer())
                              format_imgui_bullet_text(
                                "{}", *item.value<int64_t>());
                         else if (item.is_floating_point())
                              format_imgui_bullet_text(
                                "{}", *item.value<double>());
                         else if (item.is_boolean())
                              format_imgui_bullet_text(
                                "{}", *item.value<bool>() ? "true" : "false");
                         else if (item.is_table())
                              root_table_to_imgui_tree(
                                item.as_table(),
                                true,
                                current_depth + 1u,
                                callbacks);
                         else
                              format_imgui_bullet_text("(unsupported type)");
                    }
                    ImGui::TreePop();
               }
          }
          else
          {
               // Scalars inside a table → show as "key = value"
               if (val.is_string())
                    format_imgui_bullet_text(
                      "{} = {}", label, val.value<std::string>().value_or(""));
               else if (val.is_integer())
                    format_imgui_bullet_text(
                      "{} = {}", label, *val.value<int64_t>());
               else if (val.is_floating_point())
                    format_imgui_bullet_text(
                      "{} = {}", label, *val.value<double>());
               else if (val.is_boolean())
                    format_imgui_bullet_text(
                      "{} = {}", label, *val.value<bool>() ? "true" : "false");
               else
                    format_imgui_bullet_text("{} = (unsupported type)", label);
          }
     }
}


bool fme::filter_window::contains_key_recursive(const toml::table *tbl) const
{
     // Return true immediately if search field is empty
     if (m_search_field.empty())
          return true;

     // Check current level keys
     if (std::ranges::any_of(
           *tbl,
           [&](auto const &node)
           {
                const auto &[key, _] = node;
                return open_viii::tools::i_find(key, m_search_field);
           }))
     {
          return true;
     }

     // Recurse into nested tables
     auto tables_view
       = std::ranges::subrange(tbl->begin(), tbl->end())
         | std::views::transform(
           [](auto const &node)
           {
                const auto &[_, value] = node;
                return value.as_table();
           })
         | std::views::filter([](const auto *t) { return t != nullptr; });

     return std::ranges::any_of(
       tables_view,
       [&](const auto *inner_table)
       { return contains_key_recursive(inner_table); });
}

bool fme::filter_window::begin_window(
  const std::shared_ptr<Selections> &lock_selections) const
{
     bool &visible = lock_selections->get<ConfigKey::DisplayFiltersWindow>();
     const auto pop_visible = glengine::ScopeGuard{
          [&lock_selections, &visible, was_visable = visible]
          {
               if (was_visable != visible)
               {
                    lock_selections->update<ConfigKey::DisplayFiltersWindow>();
               }
          }
     };
     if (!visible)
     {
          return false;
     }
     if (!ImGui::Begin(gui_labels::deswizzle_toml_editor.data(), &visible))
     {
          m_was_focused = false;
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
          (void)lock_map_sprite->remove_deswizzle_combined_toml_table(
            file_name);
     }
     save_config(lock_selections);
     m_remove_queue.clear();
}

void fme::filter_window::handle_rename_queue(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     if (m_rename_queue.empty())
          return;

     for (const auto &[old_name, new_name] : m_rename_queue)
     {
          (void)lock_map_sprite->rename_deswizzle_combined_toml_table(
            old_name, new_name);
     }
     save_config(lock_selections);
     m_rename_queue.clear();
}

void fme::filter_window::handle_regenerate(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     if (!m_regenerate_items)
     {
          return;
     }
     spdlog::debug("{}:{} Regenerate Started", __FILE__, __LINE__);
     m_regenerate_items     = false;
     toml::table *coo_table = lock_map_sprite->get_deswizzle_combined_coo_table(
       {}, lock_selections->get<ConfigKey::TOMLFailOverForEditor>());
     if (!coo_table)
     {
          return;
     }
     if (m_textures_map)
     {
          m_textures_map->clear();
     }
     coo_table->clear();// wipe old contents
     lock_map_sprite->save_deswizzle_generate_toml(
       lock_selections->get<ConfigKey::OutputDeswizzlePattern>(),
       {},
       *coo_table,
       lock_selections);
     save_config(lock_selections);
     spdlog::debug("{}:{} Regenerate Ended", __FILE__, __LINE__);
}

void fme::filter_window::cleanup_invalid_selections() const
{
     if (!m_textures_map || m_textures_map->empty())
     {
          // m_selected_file_name.clear();
          m_last_selected.clear();
          m_multi_select.clear();
          return;
     }
     if (!m_selected_file_name.empty())
     {
          if (!m_textures_map->contains(m_selected_file_name))
          {
               m_selected_file_name = {};
          }
          else if (
            m_textures_map->size() > 1u && !m_previous_file_name
            && !m_next_file_name)
          {
               m_previous_file_name = prev_key();
               m_next_file_name     = next_key();
          }
     }
     if (!m_last_selected.empty() && !m_textures_map->contains(m_last_selected))
     {
          m_last_selected = {};
     }
     if (!m_multi_select.empty())
     {
          std::erase_if(
            m_multi_select,
            [&](const std::string &filename)
            { return !m_textures_map->contains(filename); });
     }
}

void fme::filter_window::handle_thumbnail_size_adjustment() const
{
     ImGui::SliderFloat("Thumbnail Size", &m_thumb_size_width, 96.f, 3072.f);
     bool  ctrl  = ImGui::GetIO().KeyCtrl;
     float wheel = ImGui::GetIO().MouseWheel;

     if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ctrl)
     {
          static const constexpr auto speed = 20.f;
          if (wheel > 0.0f)
               m_thumb_size_width
                 = std::min(m_thumb_size_width + (wheel * speed), 3072.f);
          else if (wheel < 0.0f)
               m_thumb_size_width = std::max(
                 m_thumb_size_width + (wheel * speed),
                 96.f);// add because wheel is negative
     }
}

void fme::filter_window::reload_thumbnails_if_needed(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     if (m_reload_thumbnail)
     {
          m_reload_thumbnail = false;
          if (!m_textures_map)
          {
               m_reload_list.clear();
               return;
          }
          if (!m_reload_list.empty())
          {
               for (const std::string &current_file_name : m_reload_list)
               {
                    m_textures_map->erase(current_file_name);
               }
               m_reload_list.clear();
          }
          else if (!m_multi_select.empty())
          {
               for (const std::string &current_file_name : m_multi_select)
               {
                    m_textures_map->erase(current_file_name);
               }
          }
          else if (!m_selected_file_name.empty())
          {
               m_textures_map->erase(m_selected_file_name);
          }

          (void)lock_map_sprite->get_deswizzle_combined_textures();
          if (!m_selected_file_name.empty())
          {
               lock_map_sprite->consume_now();
          }
     }
}

void fme::filter_window::save_config(
  const std::shared_ptr<Selections> &lock_selections) const
{

     // TODO fill in common values here or else users can't use them. Like
     // Field names and coo
     const key_value_data        config_path_values = { .ext = ".toml" };
     const std::filesystem::path config_path = config_path_values.replace_tags(
       lock_selections->get<ConfigKey::OutputTomlPattern>(), lock_selections);
     auto config = Configuration(config_path);
     config.save();
}

void fme::filter_window::render_list_view(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     display_stats(lock_selections, lock_map_sprite);
     combo_failover(lock_selections);

     render_multi_select_toolbar(lock_selections, lock_map_sprite);
     popup_combo_filtered_pupu(lock_selections, lock_map_sprite);
     render_attribute_combine_controls(lock_selections, lock_map_sprite);

     render_thumbnails(lock_selections, lock_map_sprite);
}


void fme::filter_window::display_stats(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto unused_ids = get_unused_ids();
     const auto used_coo   = lock_map_sprite->get_used_coo(
       lock_selections->get<ConfigKey::TOMLFailOverForEditor>());
     format_imgui_text(
       "Unused Pupu IDs: {}\t Used Language Code: {}",
       unused_ids.size(),
       used_coo);
     if (!unused_ids.empty() && ImGui::IsItemHovered())
     {
          ImGui::BeginTooltip();
          for (const auto &id : unused_ids)
          {
               format_imgui_text("{}\n", id);
          }
          ImGui::EndTooltip();
     }
}

void fme::filter_window::combo_failover(
  const std::shared_ptr<Selections> &lock_selections) const
{
     static const constexpr auto FailOverLevelsArray = []()
     {
          constexpr auto first = std::to_underlying(FailOverLevels::Begin);
          constexpr auto last  = std::to_underlying(FailOverLevels::End);

          std::array<FailOverLevels, last - first + 1> arr = {};

          auto range = std::views::iota(first, last + 1)
                       | std::views::transform(
                         [](auto v) { return static_cast<FailOverLevels>(v); });

          std::ranges::copy(range, arr.begin());
          return arr;
     }();
     const auto gcc = GenericCombo(
       "Fail Over",
       [&]() { return FailOverLevelsArray; },
       [&]()
       { return FailOverLevelsArray | std::views::transform(AsString{}); },
       lock_selections->get<ConfigKey::TOMLFailOverForEditor>());
     if (gcc.render())
     {
          lock_selections->update<ConfigKey::TOMLFailOverForEditor>();
          if (m_textures_map)
          {
               m_textures_map->clear();
          }
     }
}

void fme::filter_window::render_multi_select_toolbar(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const float button_height
       = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2.0f;
     const ImVec2 button_size = { m_tool_button_size_width, button_height };
     ImGui::Columns(
       calc_column_count(m_tool_button_size_width),
       "##get_deswizzle_combined_tool_buttons",
       false);
     ImGui::BeginDisabled(std::ranges::empty(m_multi_select));
     format_imgui_wrapped_text(
       "Selected {} Items(s): ", std::ranges::size(m_multi_select));
     ImGui::NextColumn();
     // Combine into a new entry (keep originals)
     if (ImGui::Button(ICON_FA_LAYER_GROUP " Combine (New)", button_size))
     {
          (void)lock_map_sprite->add_combine_deswizzle_combined_toml_table(
            m_multi_select, generate_file_name(lock_map_sprite));
          save_config(lock_selections);
     }
     tool_tip(
       "Combine selected entries into a new entry without removing the "
       "originals.");
     ImGui::NextColumn();
     // Combine and replace originals
     if (ImGui::Button(ICON_FA_OBJECT_GROUP " Combine (Replace)", button_size))
     {
          std::string temp_name = generate_file_name(lock_map_sprite);
          (void)lock_map_sprite->add_combine_deswizzle_combined_toml_table(
            m_multi_select, temp_name);
          std::ranges::sort(m_multi_select);
          m_rename_queue.emplace_back(
            std::move(temp_name), m_multi_select.front());
          std::ranges::move(m_multi_select, std::back_inserter(m_remove_queue));
          m_multi_select.clear();
     }
     tool_tip(
       "Combine selected entries into one entry and remove the "
       "originals.");
     ImGui::NextColumn();
     // Copy
     if (ImGui::Button(ICON_FA_COPY " Copy", button_size))
     {
          lock_map_sprite->copy_deswizzle_combined_toml_table(
            m_multi_select,
            [&, index = int{}]() mutable
            { return generate_file_name(lock_map_sprite, index++); });
          save_config(lock_selections);
          // todo: copy create new entries with generated name
          // (prefix_timestamp_index.png).
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
     tool_tip(
       "Add a new entry.\nHold Ctrl to add multiple entries.\nWithout "
       "Ctrl, "
       "the mode will switch to editing the new entry.");
     ImGui::Columns(1);
     if (ImGui::Button(ICON_FA_REPEAT " Regenerate", button_size))
     {
          m_regenerate_items = true;
     }
     tool_tip("Clear and regenerate the TOML entries from PupuIDs.");
     ImGui::Columns(1);
}

void fme::filter_window::render_attribute_combine_controls(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{

     format_imgui_text("{}", "Combine some entries based on attributes:");
     ImGui::Columns(
       calc_column_count(m_tool_button_size_width),
       "##get_deswizzle_combined_based_on_attributes",
       false);
     ImGui::BeginDisabled(m_checkanimation || m_checkanimation_state);
     if (m_checkanimation || m_checkanimation_state)
     {
          bool is_true = true;
          (void)ImGui::Checkbox("Offset", &is_true);
     }
     else
     {
          (void)ImGui::Checkbox("Offset", &m_checkoffset);
     }
     ImGui::EndDisabled();
     tool_tip("mask 0xFFFF'FFF0U vs PupuID and combine all of those elements.");
     ImGui::NextColumn();
     if (ImGui::Checkbox("Animation", &m_checkanimation))
     {
          if (m_checkanimation)
          {
               m_checkoffset = true;
          }
     }
     tool_tip(
       "mask 0xFFF0'0000U vs PupuID and combine all of those elements. If "
       "one "
       "of the PupuIDs is has Animation ID 0xFF and Animation State "
       "0x00");
     ImGui::NextColumn();
     if (ImGui::Checkbox("ID", &m_checkanimation_id))
     {
     }

     tool_tip(
       "mask 0xFFFF'F000U vs PupuID and combine all of those elements. "
       "Join "
       "animations ids of not the same state.");

     ImGui::NextColumn();
     if (ImGui::Checkbox("State", &m_checkanimation_state))
     {
          if (m_checkanimation_state)
          {
               m_checkoffset = true;
          }
     }


     tool_tip(
       "mask 0xFFF0'0FF0U vs PupuID and combine all of those elements. "
       "Join "
       "animations of the same state because they usually don't overlap.");

     ImGui::NextColumn();
     (void)ImGui::Checkbox("Fill-in", &m_checkanimation_fill_in);
     tool_tip("Try to fill in blanks with Animation State 0x00U");

     ImGui::NextColumn();
     (void)ImGui::Checkbox("(Allow Blend)", &m_checkallow_same_blend);
     tool_tip("Allow combines on the same blend.");

     ImGui::NextColumn();
     (void)ImGui::Checkbox("Layer", &m_checklayer_id);
     tool_tip("Combine entries with different layer ids.");

     ImGui::NextColumn();
     combo_exclude_animation_id_from_state(lock_map_sprite);

     ImGui::Columns(1);
     process_combine(lock_selections, lock_map_sprite);
}


void fme::filter_window::render_thumbnails(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     ImGui::BeginChild(
       "##Scrolling",
       ImVec2(0, 0),// full remaining size
       false,       // no border
       ImGuiWindowFlags_HorizontalScrollbar);
     ImGui::Columns(
       calc_column_count(m_thumb_size_width),
       "##get_deswizzle_combined_textures",
       false);

     if (m_textures_map)
     {
          for (const auto &[file_name, framebuffer] : *m_textures_map)
          {
               draw_thumbnail(
                 lock_selections,
                 lock_map_sprite,
                 file_name,
                 framebuffer,
                 [&]() { select_file(file_name, lock_map_sprite); });
               draw_thumbnail_label(file_name);
               ImGui::NextColumn();
          }
     }

     draw_add_new_button(lock_selections, lock_map_sprite);

     ImGui::Columns(1);
     ImGui::EndChild();
}


int fme::filter_window::calc_column_count(float width) const
{
     const ImVec2 region_size = ImGui::GetContentRegionAvail();
     const float  padding     = ImGui::GetStyle().FramePadding.x * 2.0f
                           + ImGui::GetStyle().ItemSpacing.x;
     const int count = static_cast<int>(region_size.x / (width + padding));
     return count > 0 ? count : 1;
}


void fme::filter_window::select_file(
  const std::string                 &file_name,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     if (!m_textures_map)
     {
          return;
     }
     if (auto *ptr
         = lock_map_sprite->get_deswizzle_combined_toml_table(file_name);
         ptr)
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
          else if (
            ImGui::GetIO().KeyShift && std::ranges::empty(m_last_selected))
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
          else if (
            (ImGui::GetIO().KeyShift || ImGui::GetIO().KeyAlt)
            && !std::ranges::empty(m_last_selected))
          {
               auto last_it            = m_textures_map->find(m_last_selected);
               auto it                 = m_textures_map->find(file_name);
               m_last_selected         = file_name;
               const auto get_subrange = [&]()
               {
                    // Figure out order
                    auto begin_it = last_it;
                    auto end_it   = it;
                    // Walk forward until we either find end_it or hit the
                    // real end
                    for (auto tmp = last_it; tmp != m_textures_map->end();
                         ++tmp)
                    {
                         if (tmp == it)
                         {
                              // last_it comes before it
                              return std::ranges::subrange(
                                begin_it, std::ranges::next(end_it));
                         }
                    }

                    // If we didn’t find it going forward, then last_it
                    // must come after it
                    begin_it = it;
                    end_it   = last_it;
                    return std::ranges::subrange(
                      begin_it, std::ranges::next(end_it));
               };

               const auto add = [this](auto &&range)
               {
                    for (const auto &[current_file_name, _] : range)
                    {
                         m_multi_select.push_back(current_file_name);
                    }
                    std::ranges::sort(m_multi_select);
                    auto not_unique = std::ranges::unique(m_multi_select);
                    m_multi_select.erase(not_unique.begin(), not_unique.end());
               };

               const auto remove = [this](auto &&range)
               {
                    for (const auto &[current_file_name, _] : range)
                    {
                         auto found = std::ranges::find(
                           m_multi_select, current_file_name);
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
          const auto count
            = (std::min)(s_max_chars, m_selected_file_name.size());
          std::ranges::copy_n(
            m_selected_file_name.begin(),
            static_cast<std::ranges::range_difference_t<std::string>>(count),
            m_file_name_buffer.begin());
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
     if (!m_textures_map)
     {
          return std::nullopt;
     }
     if (std::ranges::empty(*m_textures_map))
     {
          return std::nullopt;
     }
     auto it = m_textures_map->find(m_selected_file_name);
     if (it == std::ranges::end(*m_textures_map))
     {
          return std::nullopt;// not found
     }
     if (it == std::ranges::begin(*m_textures_map))
     {
          // wrap around to last element
          auto last = std::ranges::prev(std::ranges::end(*m_textures_map));
          return last->first;
     }
     it = std::ranges::prev(it);
     return it->first;
}

std::optional<std::string> fme::filter_window::next_key() const
{
     if (!m_textures_map)
     {
          return std::nullopt;
     }
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
          return std::ranges::begin(*m_textures_map)->first;// loop
     }
     return it->first;
}

void fme::filter_window::draw_thumbnail_label(
  const std::string &file_name) const
{
     // Label under image (optional)
     const float  button_width = ImGui::GetFrameHeight();
     const ImVec2 button_size  = { button_width, button_width };
     const float  text_area_width
       = m_thumb_size_width - button_width + ImGui::GetStyle().FramePadding.x;
     // Remember the top-left of where we want to start
     const ImVec2 text_start_pos = ImGui::GetCursorScreenPos();

     ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + text_area_width);
     ImGui::Text("%s", file_name.c_str());
     ImGui::PopTextWrapPos();
     const ImVec2 backup_pos = ImGui::GetCursorScreenPos();
     // Position the button at top-right of this block (same Y as the start
     // of the text)
     ImGui::SetCursorScreenPos(ImVec2(
       text_start_pos.x + text_area_width + ImGui::GetStyle().FramePadding.x,
       text_start_pos.y));
     const auto pop_id = PushPopID();
     if (ImGui::Button(ICON_FA_TRASH, button_size))
     {
          m_remove_queue.push_back(file_name);
     }
     else
     {
          tool_tip("Remove");
     }
     ImGui::SetCursorScreenPos(ImVec2(
       backup_pos.x, (std::max)(ImGui::GetCursorScreenPos().y, backup_pos.y)));
}


void fme::filter_window::draw_add_new_button(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{


     ImTextureID tex_id
       = m_hovered_file_name == "##add"
           ? glengine::ConvertGliDtoImTextureId<ImTextureID>(
               lock_map_sprite->get_framebuffer().color_attachment_id(1))
           : glengine::ConvertGliDtoImTextureId<ImTextureID>(
               lock_map_sprite->get_framebuffer().color_attachment_id());
     m_aspect_ratio
       = static_cast<float>(lock_map_sprite->get_framebuffer().height())
         / static_cast<float>(lock_map_sprite->get_framebuffer().width());
     const ImVec2 thumb_size
       = { m_thumb_size_width, m_thumb_size_width * m_aspect_ratio };
     if (ImGui::ImageButton("add new item", tex_id, thumb_size))
     {
          add_new_entry(lock_selections, lock_map_sprite);
     }
     else
     {
          tool_tip(
            "Add a new entry.\nHold Ctrl to add multiple "
            "entries.\nWithout "
            "Ctrl, the mode will switch to editing the new entry.");
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
          m_selected_toml_table
            = lock_map_sprite->add_deswizzle_combined_toml_table(
              m_selected_file_name);

          std::ranges::copy_n(
            m_selected_file_name.begin(),
            static_cast<std::ranges::range_difference_t<std::string>>(
              (std::min)(s_max_chars, m_selected_file_name.size())),
            m_file_name_buffer.begin());
     }
     else
     {
          (void)lock_map_sprite->add_deswizzle_combined_toml_table(
            m_selected_file_name);
          m_selected_file_name.clear();
     }

     save_config(lock_selections);
}


std::string fme::filter_window::generate_file_name(
  const std::shared_ptr<map_sprite> &lock_map_sprite,
  const std::optional<int>           index) const
{
     auto now = std::chrono::system_clock::now();
     auto sec = std::chrono::time_point_cast<std::chrono::seconds>(now);
     auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(
                 now.time_since_epoch())
               % 1000;
     if (index.has_value())
     {
          return fmt::format(
            "{}_{:%Y%m%d_%H%M%S}_{:03d}_{}.png",
            lock_map_sprite->get_recommended_prefix(),
            sec,
            ms.count(),
            index.value());
     }
     return fmt::format(
       "{}_{:%Y%m%d_%H%M%S}_{:03d}.png",
       lock_map_sprite->get_recommended_prefix(),
       sec,
       ms.count());
}

void fme::filter_window::render_detail_view(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     if (!m_textures_map)
     {
          return;
     }
     const auto &framebuffer = m_textures_map->at(m_selected_file_name);
     draw_thumbnail(
       lock_selections,
       lock_map_sprite,
       m_selected_file_name,
       framebuffer,
       [&]() { unselect_file(); });
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
     if (ImGui::InputText(
           "##Empty",
           m_file_name_buffer.data(),
           m_file_name_buffer.size() - 1U))
     {
     }

     constexpr static auto pattern
       = CTRE_REGEX_INPUT_TYPE{ R".((?i)^[a-z0-9_\-\.]+\.png$)." };
     bool valid_fn = ctre::match<pattern>(
       m_file_name_buffer.data(),
       m_file_name_buffer.data()
         + static_cast<std::ranges::range_difference_t<std::array<char, 1>>>(
           strnlen(m_file_name_buffer.data(), m_file_name_buffer.size())));
     if (!valid_fn)
     {
          ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
            "Invalid filename (must be alphanumeric with .png extension)");
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
         m_selected_file_name,
         std::string_view(
           m_file_name_buffer.data(),
           strnlen(m_file_name_buffer.data(), m_file_name_buffer.size()))));
     ImGui::BeginDisabled(!valid_fn);
     if (ImGui::Button("Rename"))
     {
          auto new_file_name = std::string(m_file_name_buffer.data());
          (void)lock_map_sprite->rename_deswizzle_combined_toml_table(
            m_selected_file_name, new_file_name);
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
            static_cast<std::ranges::range_difference_t<std::string>>(
              (std::min)(s_max_chars, m_selected_file_name.size())),
            m_file_name_buffer.begin());
     }
     else
     {
          tool_tip(m_selected_file_name);
     }
     ImGui::EndDisabled();
     ImGui::SameLine();
     {
          const auto pop_id   = PushPopID();
          const bool has_prev = m_previous_file_name.has_value();
          const auto disabled = ImGuiDisabled(!has_prev);

          const bool activate_prev
            = ImGui::ArrowButton("##l", ImGuiDir_Left)
              || (has_prev && ImGui::IsKeyPressed(ImGuiKey_LeftArrow));
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
          const auto pop_id   = PushPopID();
          const bool has_next = m_next_file_name.has_value();
          const auto disabled = ImGuiDisabled(!has_next);

          const bool activate_next
            = ImGui::ArrowButton("##l", ImGuiDir_Right)
              || (has_next && ImGui::IsKeyPressed(ImGuiKey_RightArrow));
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
          if (m_next_file_name)
          {
               m_remove_queue.push_back(std::move(m_selected_file_name));
               select_file(m_next_file_name.value(), lock_map_sprite);
          }
          else
          {
               m_remove_queue.push_back(std::move(m_selected_file_name));
               unselect_file();
          }
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

void fme::filter_window::draw_filter_controls(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{

     combo_filtered_pupu(lock_map_sprite);
     ImGui::Separator();
     format_imgui_wrapped_text(
       "You may use these other filters in the export or testing process "
       "but "
       "we only can import via Pupu IDs. This may change in the "
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

std::vector<ff_8::PupuID> fme::filter_window::get_unused_ids() const
{
     if (!m_textures_map)
     {
          return {};
     }
     auto lock_map_sprite = m_map_sprite.lock();
     if (!lock_map_sprite)
     {
          spdlog::error("Failed to lock map_sprite: shared_ptr is expired.");
          return {};
     }
     std::set<ff_8::PupuID> used_pupu{};

     // collect used IDs from textures_map
     for (const auto &[current_file_name, _] : *m_textures_map)
     {
          const auto &pupu_map
            = lock_map_sprite->get_deswizzle_combined_textures_pupuids();

          if (auto it = pupu_map.find(current_file_name); it != pupu_map.end())
          {
               used_pupu.insert(it->second.begin(), it->second.end());
          }
     }

     // get all possible IDs
     const auto               &all_ids = lock_map_sprite->working_unique_pupu();

     // compute unused
     std::vector<ff_8::PupuID> unused_ids;
     unused_ids.reserve(all_ids.size());

     for (const auto &id : all_ids)
     {
          if (!used_pupu.contains(id))
               unused_ids.push_back(id);
     }
     std::ranges::sort(unused_ids);
     return unused_ids;
}

void fme::filter_window::popup_combo_filtered_pupu(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     if (ImGui::BeginPopupModal(
           "Pupu Filter Popup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
     {
          const auto gcc = GenericComboWithMultiFilter(
            gui_labels::pupu_id,
            [&]() { return lock_map_sprite->working_unique_pupu(); },
            [&]()
            {
                 return lock_map_sprite->working_unique_pupu()
                        | std::views::transform(AsString{});
            },
            [&]()
            {
                 return lock_map_sprite->working_unique_pupu()
                        | std::views::transform(
                          [](const ff_8::PupuID &pupu_id) -> decltype(auto)
                          { return pupu_id.create_summary(); });
            },
            [&]() -> auto & { return m_multi_select_filter; },
            generic_combo_settings{ .num_columns = 2 });
          (void)gcc.render();
          format_imgui_wrapped_text(
            "{}",
            m_multi_select_filter.enabled()
              ? "IDs will be " ICON_FA_LAYER_GROUP
                " added to the selected entries."
              : "IDs will be " ICON_FA_TRASH
                " removed from the selected entries.");
          if (ImGui::Button(ICON_FA_CHECK " Apply"))
          {
               for (const std::string &file_name : m_multi_select)
               {
                    lock_map_sprite
                      ->apply_multi_pupu_filter_deswizzle_combined_toml_table(
                        file_name, m_multi_select_filter);
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

void fme::filter_window::combo_filtered_pupu(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto gcc = GenericComboWithMultiFilter(
       gui_labels::pupu_id,
       [&]() { return lock_map_sprite->working_unique_pupu(); },
       [&]()
       {
            return lock_map_sprite->working_unique_pupu()
                   | std::views::transform(AsString{});
       },
       [&]()
       {
            return lock_map_sprite->working_unique_pupu()
                   | std::views::transform(
                     [](const ff_8::PupuID &pupu_id) -> decltype(auto)
                     { return pupu_id.create_summary(); });
       },
       [&]() -> auto & { return lock_map_sprite->filter().multi_pupu; },
       generic_combo_settings{ .num_columns = 3 });

     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

void fme::filter_window::combo_filtered_bpps(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
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

void fme::filter_window::combo_filtered_palettes(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &map         = lock_map_sprite->uniques().palette();
     const auto &keys        = lock_map_sprite->filter().multi_bpp.value();
     const auto  join_vector = [](auto &&pairs)
     {
          auto transform_pairs = pairs
                                 | std::views::transform(
                                   [](const auto &pair) { return pair.zip(); });
          auto join_pairs = std::ranges::join_view(transform_pairs);
          return join_pairs | std::ranges::to<std::vector>();
     };

     auto value_string_pairs = [&]()
     {
          if (keys.empty() || !lock_map_sprite->filter().multi_bpp.enabled())
          {
               return join_vector(map | std::views::values);
          }
          else
          {
               auto keys_filter
                 = keys
                   | std::views::filter([&](const auto &key)
                                        { return map.contains(key); });
               auto keys_transform
                 = keys_filter
                   | std::views::transform([&](const auto &key)
                                           { return map.at(key); })
                   | std::ranges::to<std::vector>();

               return join_vector(keys_transform);
          }
     }();


     // Deduplicate based on value
     std::ranges::sort(
       value_string_pairs,
       {},
       [](const auto &pair) { return std::get<0>(pair); });
     const auto unique_range = std::ranges::unique(
       value_string_pairs,
       {},
       [](const auto &pair) { return std::get<0>(pair); });
     value_string_pairs.erase(unique_range.begin(), unique_range.end());

     // Extract values and strings into separate views
     auto values = value_string_pairs
                   | std::views::transform([](const auto &pair)
                                           { return std::get<0>(pair); });
     auto strings = value_string_pairs
                    | std::views::transform([](const auto &pair)
                                            { return std::get<1>(pair); });

     const auto gcc = fme::GenericComboWithMultiFilter(
       gui_labels::palette,
       [&values]() { return values; },
       [&strings]() { return strings; },
       [&strings]() { return strings; },
       [&]() -> auto & { return lock_map_sprite->filter().multi_palette; });

     if (!gcc.render())
     {
          return;
     }

     lock_map_sprite->update_render_texture();
     m_changed = true;
}


void fme::filter_window::combo_filtered_blend_modes(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
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

void fme::filter_window::combo_filtered_blend_other(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
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

void fme::filter_window::combo_filtered_layers(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
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

void fme::filter_window::combo_filtered_texture_pages(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().texture_page_id();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::texture_page,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto   &
     { return lock_map_sprite->filter().multi_texture_page_id; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}


void fme::filter_window::combo_exclude_animation_id_from_state(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().animation_id();
     // ImGui::BeginDisabled(std::ranges::size(pair.values()) <= 1U ||
     // std::ranges::size(pair.strings()) <= 1U); const auto pop_disabled =
     // glengine::ScopeGuard(&ImGui::EndDisabled);
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       "Exclude",
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return m_excluded_animation_id_from_state; });
     if (!gcc.render())
     {
          return;
     }
     tool_tip("Exclude Animation IDs from State Combine");
     m_changed = true;
}

void fme::filter_window::combo_filtered_animation_ids(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().animation_id();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::animation_id,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       //       filter<FilterTag::MultiAnimationId> multi_animation_id;
       [&]() -> auto   &
     { return lock_map_sprite->filter().multi_animation_id; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

void fme::filter_window::combo_filtered_animation_states(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &map  = lock_map_sprite->uniques().animation_state();
     const auto &keys = lock_map_sprite->filter().multi_animation_id.value();
     const auto  join_vector = [](auto &&pairs)
     {
          auto transform_pairs = pairs
                                 | std::views::transform(
                                   [](const auto &pair) { return pair.zip(); });
          auto join_pairs = std::ranges::join_view(transform_pairs);
          return join_pairs | std::ranges::to<std::vector>();
     };

     auto value_string_pairs = [&]()
     {
          if (
            keys.empty()
            || !lock_map_sprite->filter().multi_animation_id.enabled())
          {
               return join_vector(map | std::views::values);
          }
          else
          {
               auto keys_filter
                 = keys
                   | std::views::filter([&](const auto &key)
                                        { return map.contains(key); });
               auto keys_transform
                 = keys_filter
                   | std::views::transform([&](const auto &key)
                                           { return map.at(key); })
                   | std::ranges::to<std::vector>();

               return join_vector(keys_transform);
          }
     }();

     std::ranges::sort(
       value_string_pairs,
       {},
       [](const auto &pair) { return std::get<0>(pair); });
     const auto unique_range = std::ranges::unique(
       value_string_pairs,
       {},
       [](const auto &pair) { return std::get<0>(pair); });
     value_string_pairs.erase(unique_range.begin(), unique_range.end());

     auto values = value_string_pairs
                   | std::views::transform([&](const auto &pair)
                                           { return std::get<0>(pair); });
     auto strings = value_string_pairs
                    | std::views::transform([&](const auto &pair)
                                            { return std::get<1>(pair); });
     const auto gcc = fme::GenericComboWithMultiFilter(
       gui_labels::animation_state,
       [&values]() { return values; },
       [&strings]() { return strings; },
       [&strings]() { return strings; },
       [&]() -> auto &
       { return lock_map_sprite->filter().multi_animation_state; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

void fme::filter_window::combo_filtered_z(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
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

void fme::filter_window::combo_filtered_draw_bit(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     using namespace std::string_view_literals;
     static constexpr auto values
       = std::array{ ff_8::draw_bitT::all, ff_8::draw_bitT::enabled,
                     ff_8::draw_bitT::disabled };
     const auto gcc = fme::GenericComboWithFilter(
       gui_labels::draw_bit,
       []() { return values; },
       []() { return values | std::views::transform(AsString{}); },
       []()
       {
            return std::array{ gui_labels::draw_bit_all_tooltip,
                               gui_labels::draw_bit_enabled_tooltip,
                               gui_labels::draw_bit_disabled_tooltip };
       },
       [&]() -> auto & { return lock_map_sprite->filter().draw_bit; });
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
          return m_map_sprite->working_unique_pupu()
                 | std::views::transform(fme::AsString{});
     }
     auto tooltips() const
     {
          return m_map_sprite->working_unique_pupu()
                 | std::views::transform(
                   [](const ff_8::PupuID &pupu_id) -> decltype(auto)
                   { return pupu_id.create_summary(); });
     }
     auto zip() const
     {
          return std::ranges::views::zip(values(), strings(), tooltips());
     }
};

void fme::filter_window::menu_filtered_pupu(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::pupu_id,
       map_pupu_id{ lock_map_sprite },
       lock_map_sprite->filter().multi_pupu,
       [&]()
       {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_bpps(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::bpp,
       lock_map_sprite->uniques().bpp(),
       lock_map_sprite->filter().multi_bpp,
       [&]()
       {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_palettes(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &map         = lock_map_sprite->uniques().palette();
     const auto &keys        = lock_map_sprite->filter().multi_bpp.value();

     const auto  join_vector = [](auto &&pairs)
     {
          auto transform_pairs = pairs
                                 | std::views::transform(
                                   [](const auto &pair) { return pair.zip(); });
          auto join_pairs = std::ranges::join_view(transform_pairs);
          return join_pairs | std::ranges::to<std::vector>();
     };

     auto value_string_pairs = [&]()
     {
          if (keys.empty() || !lock_map_sprite->filter().multi_bpp.enabled())
          {
               return join_vector(map | std::views::values);
          }
          else
          {
               auto keys_filter
                 = keys
                   | std::views::filter([&](const auto &key)
                                        { return map.contains(key); });
               auto keys_transform
                 = keys_filter
                   | std::views::transform([&](const auto &key)
                                           { return map.at(key); })
                   | std::ranges::to<std::vector>();

               return join_vector(keys_transform);
          }
     }();

     std::ranges::sort(
       value_string_pairs,
       {},
       [](const auto &pair) { return std::get<0>(pair); });
     const auto unique_range = std::ranges::unique(
       value_string_pairs,
       {},
       [](const auto &pair) { return std::get<0>(pair); });
     value_string_pairs.erase(unique_range.begin(), unique_range.end());

     const auto unique_palettes = ff_8::unique_values_and_strings<std::uint8_t>(
       value_string_pairs
         | std::views::transform([&](const auto &pair)
                                 { return std::get<0>(pair); })
         | std::ranges::to<std::vector>(),
       value_string_pairs
         | std::views::transform([&](const auto &pair)
                                 { return std::get<1>(pair); })
         | std::ranges::to<std::vector>());

     GenericMenuWithMultiFilter(
       gui_labels::palette,
       unique_palettes,
       lock_map_sprite->filter().multi_palette,
       [&]()
       {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_blend_modes(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::blend_mode,
       lock_map_sprite->uniques().blend_mode(),
       lock_map_sprite->filter().multi_blend_mode,
       [&]()
       {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_blend_other(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::blend_other,
       lock_map_sprite->uniques().blend_other(),
       lock_map_sprite->filter().multi_blend_other,
       [&]()
       {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_layers(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::layer_id,
       lock_map_sprite->uniques().layer_id(),
       lock_map_sprite->filter().multi_layer_id,
       [&]()
       {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_texture_pages(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::texture_page,
       lock_map_sprite->uniques().texture_page_id(),
       lock_map_sprite->filter().multi_texture_page_id,
       [&]()
       {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_animation_ids(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::animation_id,
       lock_map_sprite->uniques().animation_id(),
       lock_map_sprite->filter().multi_animation_id,
       [&]()
       {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_animation_states(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &map  = lock_map_sprite->uniques().animation_state();
     const auto &keys = lock_map_sprite->filter().multi_animation_id.value();

     const auto  join_vector = [](auto &&pairs)
     {
          auto transform_pairs = pairs
                                 | std::views::transform(
                                   [](const auto &pair) { return pair.zip(); });
          auto join_pairs = std::ranges::join_view(transform_pairs);
          return join_pairs | std::ranges::to<std::vector>();
     };

     auto value_string_pairs = [&]()
     {
          if (
            keys.empty()
            || !lock_map_sprite->filter().multi_animation_id.enabled())
          {
               return join_vector(map | std::views::values);
          }
          else
          {
               auto keys_filter
                 = keys
                   | std::views::filter([&](const auto &key)
                                        { return map.contains(key); });
               auto keys_transform
                 = keys_filter
                   | std::views::transform([&](const auto &key)
                                           { return map.at(key); })
                   | std::ranges::to<std::vector>();

               return join_vector(keys_transform);
          }
     }();

     std::ranges::sort(
       value_string_pairs,
       {},
       [](const auto &pair) { return std::get<0>(pair); });
     const auto unique_range = std::ranges::unique(
       value_string_pairs,
       {},
       [](const auto &pair) { return std::get<0>(pair); });
     value_string_pairs.erase(unique_range.begin(), unique_range.end());

     const auto unique_animation_state
       = ff_8::unique_values_and_strings<std::uint8_t>(
         value_string_pairs
           | std::views::transform([&](const auto &pair)
                                   { return std::get<0>(pair); })
           | std::ranges::to<std::vector>(),
         value_string_pairs
           | std::views::transform([&](const auto &pair)
                                   { return std::get<1>(pair); })
           | std::ranges::to<std::vector>());

     GenericMenuWithMultiFilter(
       gui_labels::animation_state,
       unique_animation_state,
       lock_map_sprite->filter().multi_animation_state,
       [&]()
       {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_z(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::z,
       lock_map_sprite->uniques().z(),
       lock_map_sprite->filter().multi_z,
       [&]()
       {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

struct map_draw_bit
{
   private:
     static constexpr auto m_values
       = std::array{ ff_8::draw_bitT::all, ff_8::draw_bitT::enabled,
                     ff_8::draw_bitT::disabled };
     static constexpr auto m_tooltips
       = std::array{ fme::gui_labels::draw_bit_all_tooltip,
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

void fme::filter_window::menu_filtered_draw_bit(
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithFilter(
       gui_labels::draw_bit,
       map_draw_bit{},
       lock_map_sprite->filter().draw_bit,
       [&]()
       {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::draw_thumbnail(
  const std::shared_ptr<Selections>          &lock_selections,
  const std::shared_ptr<map_sprite>          &lock_map_sprite,
  const std::string                          &file_name,
  const std::optional<glengine::FrameBuffer> &framebuffer,
  std::move_only_function<void()>             on_click) const
{
     const std::string &tooltip_str
       = get_thumbnail_tooltip(lock_map_sprite, file_name);

     const auto it       = std::ranges::find(m_multi_select, file_name);
     bool       selected = it != std::ranges::end(m_multi_select);
     if (framebuffer.has_value())
     {
          render_thumbnail_button(
            file_name, framebuffer, selected, std::move(on_click));
          tool_tip(tooltip_str);
          render_thumbnail_popup(lock_selections, lock_map_sprite, file_name);
     }
     else
     {
          const auto pop_id = PushPopID();
          ImVec2     padded_thumb_size
            = { m_thumb_size_width + ImGui::GetStyle().FramePadding.x * 2.0f,
                m_thumb_size_width * m_aspect_ratio
                  + ImGui::GetStyle().FramePadding.y * 2.0f };
          if (ImGui::Button("##Empty", padded_thumb_size))
          {
               on_click();
          }
          else
          {
               tool_tip(tooltip_str);
          }
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


const std::string &fme::filter_window::get_thumbnail_tooltip(
  const std::shared_ptr<map_sprite> &lock_map_sprite,
  const std::string                 &file_name) const
{
     static const std::string empty_msg = "No filters are enabled...";
     if (lock_map_sprite->get_deswizzle_combined_textures_tooltips().contains(
           file_name))
     {
          const std::string &tmp
            = lock_map_sprite->get_deswizzle_combined_textures_tooltips().at(
              file_name);
          return tmp.empty() ? empty_msg : tmp;
     }
     return empty_msg;
}

void fme::filter_window::render_thumbnail_button(
  const std::string                          &file_name,
  const std::optional<glengine::FrameBuffer> &framebuffer,
  const bool                                  selected,
  std::move_only_function<void()>             on_click) const
{
     ImTextureID tex_id = (m_hovered_file_name == file_name)
                            ? glengine::ConvertGliDtoImTextureId<ImTextureID>(
                                framebuffer.value().color_attachment_id(1))
                            : glengine::ConvertGliDtoImTextureId<ImTextureID>(
                                framebuffer.value().color_attachment_id());
     m_aspect_ratio     = static_cast<float>(framebuffer.value().height())
                      / static_cast<float>(framebuffer.value().width());
     const ImVec2 thumb_size
       = { m_thumb_size_width, m_thumb_size_width * m_aspect_ratio };

     const auto pop_style_color
       = glengine::ScopeGuard{ [selected]()
                               {
                                    if (selected)
                                    {
                                         ImGui::PopStyleColor(3);
                                    }
                               } };
     if (selected)
     {
          ImGui::PushStyleColor(ImGuiCol_Button, colors::ButtonGreen);
          ImGui::PushStyleColor(
            ImGuiCol_ButtonHovered, colors::ButtonGreenHovered);
          ImGui::PushStyleColor(
            ImGuiCol_ButtonActive, colors::ButtonGreenActive);
     }

     if (ImGui::ImageButton(file_name.c_str(), tex_id, thumb_size))
     {
          std::invoke(on_click);
     }
}

void fme::filter_window::render_thumbnail_popup(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite,
  const std::string                 &file_name) const
{
     const auto pop_id = PushPopID();
     if (!ImGui::BeginPopupContextItem(
           "FilterOptions"))// right-click menu for this button
     {
          return;
     }
     if (ImGui::MenuItem(
           ICON_FA_SQUARE_PLUS " Add to selected",
           nullptr,
           nullptr,
           !m_multi_select.empty()))
     {
          ff_8::filter_old<ff_8::FilterTag::MultiPupu> temp_filter
            = { ff_8::FilterSettings::All_Disabled };
          const toml::table *const file_table
            = lock_map_sprite->get_deswizzle_combined_toml_table(file_name);
          if (file_table)
          {
               temp_filter.reload(*file_table);
               temp_filter.enable();
               for (const std::string &update_file_name : m_multi_select)
               {
                    lock_map_sprite
                      ->apply_multi_pupu_filter_deswizzle_combined_toml_table(
                        update_file_name, temp_filter);
               }
               m_reload_thumbnail = true;
               save_config(lock_selections);
          }
     }
     tool_tip("Add hovered values to selected items.");
     if (ImGui::MenuItem(
           ICON_FA_SQUARE_MINUS " Remove from selected",
           nullptr,
           nullptr,
           !m_multi_select.empty()))
     {
          ff_8::filter_old<ff_8::FilterTag::MultiPupu> temp_filter
            = { ff_8::FilterSettings::All_Disabled };
          const toml::table *const file_table
            = lock_map_sprite->get_deswizzle_combined_toml_table(file_name);
          if (file_table)
          {
               temp_filter.reload(*file_table);
               temp_filter.disable();
               for (const std::string &update_file_name : m_multi_select)
               {
                    lock_map_sprite
                      ->apply_multi_pupu_filter_deswizzle_combined_toml_table(
                        update_file_name, temp_filter);
               }
               m_reload_thumbnail = true;
               save_config(lock_selections);
          }
     }
     tool_tip("Remove hovered values to selected items.");
     ImGui::Separator();
     if (ImGui::MenuItem(
           ICON_FA_LAYER_GROUP " Combine (New)",
           nullptr,
           nullptr,
           !m_multi_select.empty()))
     {
          (void)lock_map_sprite->add_combine_deswizzle_combined_toml_table(
            m_multi_select, generate_file_name(lock_map_sprite));
          save_config(lock_selections);
     }

     tool_tip(
       "Combine selected entries into a new entry without "
       "removing the originals.");
     if (ImGui::MenuItem(
           ICON_FA_OBJECT_GROUP " Combine (Replace)",
           nullptr,
           nullptr,
           !m_multi_select.empty()))
     {
          std::string temp_name = generate_file_name(lock_map_sprite);
          (void)lock_map_sprite->add_combine_deswizzle_combined_toml_table(
            m_multi_select, temp_name);
          std::ranges::sort(m_multi_select);
          m_rename_queue.emplace_back(
            std::move(temp_name), m_multi_select.front());
          std::ranges::move(m_multi_select, std::back_inserter(m_remove_queue));
          m_multi_select.clear();
     }
     tool_tip(
       "Combine selected entries into one entry and remove the "
       "originals.");
     ImGui::Separator();

     if (ImGui::MenuItem(
           ICON_FA_COPY " Copy", nullptr, nullptr, !m_multi_select.empty()))
     {
          lock_map_sprite->copy_deswizzle_combined_toml_table(
            m_multi_select,
            [&, index = int{}]() mutable
            { return generate_file_name(lock_map_sprite, index++); });
          save_config(lock_selections);
          // todo: copy create new entries with generated name
          // (prefix_timestamp_index.png).
     }
     tool_tip(
       "Copy selected entries into new entries with generated "
       "names.");
     if (ImGui::MenuItem(
           ICON_FA_TRASH " Remove", nullptr, nullptr, !m_multi_select.empty()))
     {
          std::ranges::move(m_multi_select, std::back_inserter(m_remove_queue));
          m_multi_select.clear();
     }
     tool_tip("Remove selected entries.");

     if (ImGui::MenuItem(
           ICON_FA_FILTER " Pupu Filter",
           nullptr,
           nullptr,
           !m_multi_select.empty()))
     {
          ImGui::OpenPopup("Pupu Filter Popup");
     }
     tool_tip("Bulk enable or disable pupu.");
     ImGui::Separator();
     if (ImGui::MenuItem(
           ICON_FA_BROOM " Clear Selection",
           nullptr,
           nullptr,
           !m_multi_select.empty()))
     {
          m_multi_select.clear();
     }
     tool_tip("Clear the current selection.");
     ImGui::EndPopup();
}

bool fme::filter_window::is_excluded(const ff_8::PupuID &pupu_id) const
{
     return m_excluded_animation_id_from_state.enabled()
            && std::ranges::any_of(
              m_excluded_animation_id_from_state.value(),
              [&](const auto &id) { return pupu_id.animation_id() == id; });
}

void fme::filter_window::process_combine(
  const std::shared_ptr<Selections> &lock_selections,
  const std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     ImGui::BeginDisabled(
       !m_checkoffset && !m_checkanimation && !m_checkanimation_id
       && !m_checkanimation_state && !m_checklayer_id
       && !m_checkanimation_fill_in);
     if (ImGui::Button("Combine (w/attribute)"))
     {
          [&]()
          {
               const auto &unique_pupu_ids
                 = lock_map_sprite->working_unique_pupu();
               toml::table *coo_table
                 = lock_map_sprite->get_deswizzle_combined_coo_table(
                   {},
                   lock_selections->get<ConfigKey::TOMLFailOverForEditor>());
               if (m_checkoffset)
               {
                    process_combine(
                      coo_table, unique_pupu_ids,
                      [](auto &&...) { return true; },
                      [](
                        const ff_8::PupuID &u_pupu_id,
                        const ff_8::PupuID &i_pupu_id)
                      { return u_pupu_id.same_base(i_pupu_id); });
               }
               if (m_checkanimation_id)
               {
                    process_combine(
                      coo_table, unique_pupu_ids,
                      [](auto &&...) { return true; },
                      [](
                        const ff_8::PupuID &u_pupu_id,
                        const ff_8::PupuID &i_pupu_id)
                      { return u_pupu_id.same_animation_id_base(i_pupu_id); });
               }
               if (m_checkanimation_state)
               {
                    process_combine(
                      coo_table, unique_pupu_ids,
                      [](auto &&...) { return true; },
                      [](
                        const ff_8::PupuID &u_pupu_id,
                        const ff_8::PupuID &i_pupu_id)
                      {
                           return u_pupu_id.same_animation_state_base(i_pupu_id)
                                  && i_pupu_id.animation_id() != 0xFFU
                                  && u_pupu_id.animation_id() != 0xFFU;
                      });
               }
               if (m_checkanimation_fill_in)
               {
                    process_combine(
                      coo_table, unique_pupu_ids,
                      [](
                        const ff_8::PupuID                 &u_pupu_id,
                        const std::span<const ff_8::PupuID> temp_pupus)
                      {
                           return u_pupu_id.animation_state() != 0u
                                  || std::ranges::any_of(
                                    temp_pupus,
                                    [&](const ff_8::PupuID &pupu_id)
                                    {
                                         return u_pupu_id
                                           .same_animation_id_base(pupu_id);
                                    });
                      },
                      [](const ff_8::PupuID &, const ff_8::PupuID &)
                      { return true; });
               }
               if (m_checklayer_id)
               {
                    process_combine(
                      coo_table, unique_pupu_ids,
                      [](auto &&...) { return true; },
                      [](
                        const ff_8::PupuID &u_pupu_id,
                        const ff_8::PupuID &i_pupu_id)
                      { return u_pupu_id.same_layer_base(i_pupu_id); });
               }


               auto cmp = [](
                            std::vector<ff_8::PupuID> const &a,
                            std::vector<ff_8::PupuID> const &b)
               {
                    if (a.size() != b.size())
                         return a.size() < b.size();// size first
                    return std::ranges::lexicographical_compare(
                      a, b);// then lexicographically
               };
               // Assuming value() is hashable/comparable
               std::set<std::vector<ff_8::PupuID>, decltype(cmp)> seen(cmp);


               for (auto &&[key, value] : *coo_table)
               {
                    if (value.is_table())
                    {
                         ff_8::filter_old<ff_8::FilterTag::MultiPupu>
                           temp_filter = { ff_8::FilterSettings::All_Disabled };
                         toml::table &file_table = *value.as_table();
                         temp_filter.reload(file_table);

                         if (!seen.insert(temp_filter.value()).second)
                         {
                              // Duplicate → queue for removal
                              m_remove_queue.push_back(
                                std::string{ key.str() });
                         }
                    }
               }


               if (m_checkanimation)
               {
                    process_combine(
                      coo_table, unique_pupu_ids,
                      [](auto &&...) { return true; },
                      [](
                        const ff_8::PupuID &u_pupu_id,
                        const ff_8::PupuID &i_pupu_id)
                      {
                           return u_pupu_id.same_animation_base(i_pupu_id)
                                  && i_pupu_id.animation_id() != 0xFFU
                                  && u_pupu_id.animation_id() == 0xFFU
                                  && u_pupu_id.animation_state() == 0x00U;
                      });
               }
               save_config(lock_selections);
          }();
     }
     ImGui::EndDisabled();
     tool_tip(
       "Automaticly combine with attributes selected. Replacing entries.");
     ImGui::Columns(1);
}

void fme::filter_window::process_combine(
  toml::table                           *coo_table,
  const std::vector<ff_8::PupuID>       &unique_pupu_ids,
  fme::filter_window::OuterFilter        outer_filter,
  fme::filter_window::PupuMatchPredicate match_pred) const
{
     if (!coo_table)
     {
          return;
     }

     for (auto &&[key, value] : *coo_table)
     {
          if (!value.is_table())
          {
               continue;
          }

          ff_8::filter_old<ff_8::FilterTag::MultiPupu> temp_filter{
               ff_8::FilterSettings::All_Disabled
          };
          toml::table &file_table = *value.as_table();
          temp_filter.reload(file_table);

          if (!temp_filter.enabled())
          {
               continue;
          }

          if (
            !m_checkallow_same_blend
            && std::ranges::any_of(
              temp_filter.value(),
              [](const ff_8::PupuID &pupu_id)
              {
                   return pupu_id.blend_mode()
                          != open_viii::graphics::background::BlendModeT::none;
              }))
          {
               continue;
          }

          // debating on this being here.
          if (std::ranges::all_of(
                temp_filter.value(),
                [&](const ff_8::PupuID &pupu_id)
                {
                     if (is_excluded(pupu_id))
                     {
                          return false;
                     }
                     return pupu_id.offset() > 0;
                }))
          {
               // mark for deletion
               m_remove_queue.emplace_back(key);
               continue;
          }

          auto copy_values = temp_filter.value();
          bool reload      = false;

          for (const auto &u : unique_pupu_ids)
          {
               if ((is_excluded(u)) || (!outer_filter(u, temp_filter.value())))
               {
                    continue;
               }

               for (const auto &i : temp_filter.value())
               {
                    if (
                      is_excluded(i) || u == i
                      || u.blend_mode() != i.blend_mode() || !match_pred(u, i))
                    {
                         continue;
                    }
                    copy_values.push_back(u);
                    reload = true;
               }
          }

          if (reload)
          {
               m_reload_list.emplace_back(key);
               m_reload_thumbnail = true;

               std::ranges::sort(copy_values);
               const auto remove_range = std::ranges::unique(copy_values);
               copy_values.erase(remove_range.begin(), remove_range.end());

               temp_filter.update(std::move(copy_values));
               temp_filter.update(file_table);
          }
     }
}


void fme::filter_window::register_change_field_callback(
  std::move_only_function<void(const std::string &)> in_callback)
{
     m_change_field_callback = std::move(in_callback);
}

void fme::filter_window::clear_change_field_callback()
{
     m_change_field_callback = nullptr;
}

void fme::filter_window::register_change_coo_callback(
  std::move_only_function<void(const std::string &)> in_callback)
{
     m_change_coo_callback = std::move(in_callback);
}

void fme::filter_window::clear_change_coo_callback()
{
     m_change_coo_callback = nullptr;
}

void fme::filter_window::register_is_remaster_callback(
  std::move_only_function<bool()> in_callback)
{
     m_is_remaster_callback = std::move(in_callback);
}

void fme::filter_window::clear_is_remaster_callback()
{
     m_is_remaster_callback = nullptr;
}