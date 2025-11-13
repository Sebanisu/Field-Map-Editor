#ifndef FFE291E8_6AE6_403A_A4F4_7173F0D98EE5
#define FFE291E8_6AE6_403A_A4F4_7173F0D98EE5
#include "format_imgui_text.hpp"
#include "gui_labels.hpp"
#include "Selections.hpp"
#include <glengine/ScopeGuard.hpp>
#include <imgui_utils/ImGuiPushID.hpp>
#include <open_viii/archive/FIFLFS.hpp>
namespace fme
{
struct field_file_window
{
     using Field          = open_viii::archive::FIFLFS<false>;
     using WeakField      = std::weak_ptr<Field>;
     using WeakSelections = std::weak_ptr<Selections>;

     field_file_window(
       WeakField      in_field,
       WeakSelections in_selections)
       : m_field{ in_field }
       , m_selections{ in_selections }
     {
     }

     field_file_window &refresh(WeakSelections in_selections)
     {
          m_selections = in_selections;
          m_changed    = true;
          return *this;
     }

     field_file_window &refresh(WeakField in_field)
     {
          m_field   = in_field;
          m_changed = true;
          return *this;
     }

     field_file_window &refresh()
     {
          m_changed = true;
          return *this;
     }

     void render() const
     {
          // escape out if weak pointers aren't valid.
          const auto selections = m_selections.lock();
          if (!selections)
          {
               spdlog::error(
                 "Failed to lock m_selections: shared_ptr is expired.");
               return;
          }
          if (!selections->get<ConfigKey::DisplayFieldFileWindow>())
          {
               return;
          }

          const auto field = m_field.lock();
          if (!field)
          {
               spdlog::error("Failed to lock m_field: shared_ptr is expired.");
               return;
          }
          bool &visible = selections->get<ConfigKey::DisplayFieldFileWindow>();
          const auto pop_visible = glengine::ScopeGuard{
               [&selections, &visible, was_visable = visible]
               {
                    if (was_visable != visible)
                    {
                         selections
                           ->update<ConfigKey::DisplayFieldFileWindow>();
                    }
               }
          };
          const auto pop_end = glengine::ScopeGuard(&ImGui::End);
          if (!ImGui::Begin(gui_labels::field_file_window.data(), &visible))
          {
               return;
          }
          const auto pop_changed
            = glengine::ScopeGuard([this]() { m_changed = false; });
          static std::vector<std::string> paths = {};

          if (std::ranges::empty(paths) || m_changed)
          {
               paths = field->get_all_paths_from_fl({});
          }
          for (const std::string_view path : paths)
          {
               const auto pop_id = imgui_utils::ImGuiPushId();
               (void)ImGui::Selectable(path.data());
               if (ImGui::BeginPopupContextItem())// <-- use last item id as
                                                  // popup id
               {
                    if (ImGui::Selectable("Copy Path"))
                    {
                         ImGui::SetClipboardText(path.data());
                    }
                    if (ImGui::Selectable("Copy All Paths"))
                    {
                         using namespace std::string_literals;
                         auto combined_paths
                           = paths | std::ranges::views::join_with("\n"s)
                             | std::ranges::to<std::string>();
                         ImGui::SetClipboardText(combined_paths.data());
                    }
                    if (ImGui::Button("Close"))
                         ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
               }
               else
               {
                    ImGui::SetItemTooltip("Right-click to open popup");
               }
          }
     }

   private:
     WeakField      m_field      = {};
     WeakSelections m_selections = {};
     mutable bool   m_changed    = { false };
};
}// namespace fme

#endif /* FFE291E8_6AE6_403A_A4F4_7173F0D98EE5 */