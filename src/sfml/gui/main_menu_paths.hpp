#ifndef E3DD6F5B_A89D_486E_AB39_F93FAD249002
#define E3DD6F5B_A89D_486E_AB39_F93FAD249002
#include "fa_icons.hpp"
#include "gui_labels.hpp"
#include "map_directory_mode.hpp"
#include "open_file_explorer.hpp"
#include "scope_guard.hpp"
#include "tool_tip.hpp"
#include <algorithm>
#include <filesystem>
#include <functional>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>
namespace fme
{
struct main_menu_paths_settings
{
     std::reference_wrapper<std::vector<std::string>> user_paths;
     std::reference_wrapper<std::vector<std::string>> generated_paths;
     std::reference_wrapper<std::vector<bool>>        generated_paths_enabled;
     std::string_view                                 main_label;
     std::string_view                                 browse_tooltip;
};
template<typename main_filter_t, typename other_filter_t>
struct main_menu_paths
{

   private:
     mutable std::reference_wrapper<main_filter_t>  m_main_filter;
     mutable std::reference_wrapper<other_filter_t> m_other_filter;
     main_menu_paths_settings                       m_settings;


   public:
     main_menu_paths(main_filter_t &main_filter, other_filter_t &other_filter, main_menu_paths_settings settings)
       : m_main_filter(main_filter)
       , m_other_filter(other_filter)
       , m_settings(settings)
     {
     }

     void render(const std::invocable auto &generate, const std::invocable auto &refresh, const std::invocable auto &open_browser) const
     {
          if (!ImGui::BeginMenu(m_settings.main_label.data()))
          {
               return;
          }
          const auto end_menu1 = scope_guard(&ImGui::EndMenu);
          [&]() {
               if (!ImGui::MenuItem(gui_labels::browse.data(), nullptr, false, true))
               {
                    tool_tip(m_settings.browse_tooltip);
                    return;
               }
               std::invoke(open_browser);
          }();
          if (ImGui::MenuItem(
                gui_labels::explore.data(),
                nullptr,
                nullptr,
                !std::ranges::empty(m_main_filter.get().value()) && m_main_filter.get().enabled()))
          {
               open_directory(m_main_filter.get().value());
          }
          else
          {
               tool_tip(gui_labels::explore_tooltip);
               tool_tip(m_main_filter.get().value().string());
          }

          const auto     transformed_paths = m_settings.user_paths.get() | std::ranges::views::enumerate;

          std::ptrdiff_t delete_me         = -1;
          static float   elapsed_time      = 0.0f;// Track elapsed time

          elapsed_time += ImGui::GetIO().DeltaTime;// Increment with frame delta time
          static constexpr size_t max_display_chars = 50;
          static constexpr float  chars_per_second  = 8.0f;
          [&]() {
               if (std::ranges::empty(m_settings.user_paths.get()))
               {
                    return;
               }
               ImGui::Separator();
               if (ImGui::BeginTable("##path_table", 2))
               {
                    const auto end_table = scope_guard(&ImGui::EndTable);
                    auto zip_path = std::ranges::views::zip(m_settings.generated_paths.get(), m_settings.generated_paths_enabled.get());
                    for (const auto &[index, path] : transformed_paths)
                    {
                         bool is_checked = path == m_main_filter.get().value() && m_main_filter.get().enabled();
                         ImGui::TableNextColumn();
                         ImGui::SetNextItemAllowOverlap();
                         auto it      = std::ranges::find_if(zip_path, [&path](const auto &pair) {
                              const auto &[t_path, t_enabled] = pair;
                              return std::ranges::equal(path, t_path);
                         });
                         bool enabled = [&]() -> bool {
                              if (it != std::ranges::end(zip_path))
                              {
                                   const auto &[t_path, t_enabled] = *it;
                                   return t_enabled;
                              }
                              return false;
                         }();
                         {
                              ImGui::BeginDisabled(!enabled);
                              const auto pop_disabled = scope_guard{ &ImGui::EndDisabled };
                              if (ImGui::MenuItem(path.data(), nullptr, &is_checked, true))
                              {
                                   if (m_main_filter.get().value() != path)
                                   {
                                        m_main_filter.get().update(path);
                                        m_other_filter.get().disable();
                                        m_main_filter.get().enable();
                                   }
                                   else
                                   {
                                        if (m_main_filter.get().enabled())
                                        {
                                             m_main_filter.get().disable();
                                        }
                                        else
                                        {
                                             m_other_filter.get().disable();
                                             m_main_filter.get().enable();
                                        }
                                   }
                                   // refresh_render_texture(true);
                                   std::invoke(refresh);
                              }
                         }
                         ImGui::TableNextColumn();
                         add_explore_button(path);
                         delete_me = add_delete_button(index, delete_me);
                    }
               }
          }();
          [&]() {
               if (std::ranges::empty(m_settings.generated_paths.get()))
               {
                    return;
               }
               ImGui::Separator();

               if (ImGui::BeginTable("##path_table", 2))
               {
                    const auto end_table = scope_guard(&ImGui::EndTable);
                    for (const auto &[path, enabled] :
                         std::ranges::views::zip(m_settings.generated_paths.get(), m_settings.generated_paths_enabled.get()))
                    {
                         bool is_checked = path == m_main_filter.get().value() && m_main_filter.get().enabled();
                         ImGui::TableNextColumn();
                         ImGui::SetNextItemAllowOverlap();
                         {
                              ImGui::BeginDisabled(!enabled);
                              const auto pop_disabled = scope_guard{ &ImGui::EndDisabled };
                              const auto path_padded  = path + "  -  ";
                              size_t offset = static_cast<size_t>(elapsed_time * chars_per_second) % (path_padded.size());// Sliding offset
                              std::string display_text = path_padded.substr(offset, max_display_chars);
                              if (display_text.size() < max_display_chars && offset > 0)
                              {
                                   // Wrap-around to show the start of the string
                                   display_text += path_padded.substr(0, max_display_chars - display_text.size());
                              }
                              const auto pop_id_menu_item = PushPopID();
                              ImVec2     cursor_pos       = ImGui::GetCursorScreenPos();
                              bool       selected         = ImGui::MenuItem("##menu_item", nullptr, &is_checked);
                              if (!selected)
                              {
                                   tool_tip(path);
                              }
                              ImGui::SetCursorScreenPos(cursor_pos);
                              ImGui::TextUnformatted(display_text.c_str());// Draw the scrolling text separately
                              ImGui::SameLine();
                              float sz = ImGui::GetTextLineHeight();
                              ImGui::Dummy(ImVec2(sz, sz));
                              if (selected)
                              {
                                   if (m_main_filter.get().value() != path)
                                   {
                                        m_main_filter.get().update(path);
                                        m_other_filter.get().disable();
                                        m_main_filter.get().enable();
                                   }
                                   else
                                   {
                                        if (m_main_filter.get().enabled())
                                        {
                                             m_main_filter.get().disable();
                                        }
                                        else
                                        {
                                             m_other_filter.get().disable();
                                             m_main_filter.get().enable();
                                        }
                                   }
                                   // refresh_render_texture(true);
                                   std::invoke(refresh);
                              }
                         }
                         ImGui::TableNextColumn();
                         add_explore_button(path);
                         delete_me = add_delete_button(path, m_settings.user_paths.get(), delete_me);
                    }
               }
               if (std::cmp_greater_equal(delete_me, 0))
               {
                    spdlog::info("delete {}", delete_me);
               }
               if (const auto found = handle_path_deletion(m_settings.user_paths, delete_me); found.has_value())
               {
                    std::invoke(generate);
                    // generate_deswizzle_map_paths();
                    // handle update config
                    if (found.value() == m_main_filter.get().value())
                    {
                         m_main_filter.get().update(
                           find_replacement_path_value(m_settings.generated_paths, m_settings.generated_paths_enabled));
                         if (std::ranges::empty(m_main_filter.get().value()))
                         {
                              m_main_filter.get().disable();
                         }
                         // refresh_render_texture(true);
                         std::invoke(refresh);
                    }
               }
          }();
     }

     std::optional<std::string> handle_path_deletion(std::vector<std::string> &paths_vector, std::ptrdiff_t offset) const
     {
          if (std::cmp_less(offset, 0))
          {
               return std::nullopt;
          }
          auto it = std::ranges::begin(paths_vector);
          std::ranges::advance(it, offset);
          if (it != std::ranges::end(paths_vector))
          {
               auto return_value = std::optional<std::string>(std::move(*it));
               paths_vector.erase(it);
               return return_value;
          }
          return std::nullopt;
     }

     std::string find_replacement_path_value(const std::vector<std::string> &paths, const std::vector<bool> &paths_enabled) const
     {
          if (std::ranges::empty(paths))
          {
               return "";
          }
          if (std::ranges::size(paths) == std::ranges::size(paths_enabled))
          {
               return "";
          }

          auto zip_paths = std::ranges::views::zip(paths, paths_enabled);
          auto it        = std::ranges::find_if(zip_paths, [](const auto &pair) { return std::get<1>(pair); });
          if (it != std::ranges::end(zip_paths))
          {
               return std::get<0>(*it);
          }
          return "";
     }

     std::ptrdiff_t add_delete_button(const std::ptrdiff_t index, const std::ptrdiff_t delete_me) const
     {
          const auto  pop_id      = PushPopID();
          const float button_size = ImGui::GetFrameHeight();
          ImGui::SameLine();
          if (ImGui::Button(ICON_FA_TRASH, ImVec2{ button_size, button_size }))
          {
               // ImGui::CloseCurrentPopup();
               return static_cast<std::ptrdiff_t>(index);
          }
          else
          {
               tool_tip("delete me");
          }
          return delete_me;
     }

     std::ptrdiff_t add_delete_button(const std::string &path, const std::vector<std::string> &paths, const std::ptrdiff_t delete_me) const
     {
          auto       transformed_paths = paths | std::ranges::views::enumerate;
          const auto it =
            std::ranges::find_if(transformed_paths, [&path](const auto &pair) { return path.starts_with(std::get<1>(pair)); });
          if (it != std::ranges::end(transformed_paths))

          {
               const auto  pop_id      = PushPopID();
               const float button_size = ImGui::GetFrameHeight();
               ImGui::SameLine();
               if (ImGui::Button(ICON_FA_TRASH, ImVec2{ button_size, button_size }))
               {
                    const auto &index = std::get<0>(*it);

                    // ImGui::CloseCurrentPopup();
                    return static_cast<std::ptrdiff_t>(index);
               }
               else
               {
                    tool_tip("delete me");
               }
          }
          return delete_me;
     }
     void add_explore_button(const std::string &path) const
     {
          const float button_size = ImGui::GetFrameHeight();
          const auto  _           = PushPopID();
          if (ImGui::Button(ICON_FA_FOLDER_OPEN, ImVec2{ button_size, button_size }))
          {
               open_directory(path);
          }
          tool_tip(gui_labels::explore_tooltip);
     }
};
}// namespace fme

#endif /* E3DD6F5B_A89D_486E_AB39_F93FAD249002 */
