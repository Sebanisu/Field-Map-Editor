#ifndef FFE291E8_6AE6_403A_A4F4_7173F0D98EE5
#define FFE291E8_6AE6_403A_A4F4_7173F0D98EE5
#include "format_imgui_text.hpp"
#include "gui_labels.hpp"
#include "scope_guard.hpp"
#include "Selections.hpp"
#include <open_viii/archive/FIFLFS.hpp>
namespace fme
{
struct field_file_window
{
     using Field          = open_viii::archive::FIFLFS<false>;
     using WeakField      = std::weak_ptr<Field>;
     using WeakSelections = std::weak_ptr<Selections>;

     field_file_window(WeakField in_field, WeakSelections in_selections)
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
          if (!selections || !selections->display_field_file_window)
          {
               return;
          }

          const auto field = m_field.lock();
          if (!field)
          {
               return;
          }


          const auto pop_end = scope_guard(&ImGui::End);
          if (!ImGui::Begin(gui_labels::field_file_window.data()))
          {
               return;
          }
          const auto                      pop_changed = scope_guard([this]() { m_changed = false; });
          static std::vector<std::string> paths       = {};

          if (std::ranges::empty(paths) || m_changed)
          {
               paths = field->get_all_paths_from_fl({});
          }
          for (const std::string_view path : paths)
          {
               format_imgui_text("{}", path);
          }
     }

   private:
     WeakField      m_field      = {};
     WeakSelections m_selections = {};
     mutable bool   m_changed    = { false };
};
}// namespace fme

#endif /* FFE291E8_6AE6_403A_A4F4_7173F0D98EE5 */