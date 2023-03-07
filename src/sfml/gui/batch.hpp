//
// Created by pcvii on 3/7/2023.
//

#ifndef FIELD_MAP_EDITOR_BATCH_HPP
#define FIELD_MAP_EDITOR_BATCH_HPP
#include "compact_type.hpp"
#include "cstdint"
#include "filter.hpp"
#include "generic_combo.hpp"
#include "gui_labels.hpp"
#include "safedir.hpp"
#include <array>
#include <imgui.h>
class batch
{
     input_types                    m_input_type;
     output_types                   m_output_type;
     ff_8::filter_old<compact_type> m_compact_type;
     ff_8::filter_old<flatten_type> m_flatten_type;
     std::array<char, 256U>         m_input_path;
     bool                           m_input_path_valid = false;

     void                           combo_input_type(int &imgui_id);
     void                           combo_output_type(int &imgui_id);
     void                           combo_compact_type(int &imgui_id);
     void                           combo_flatten_type(int &imgui_id);
     void                           browse_input_path(int &imgui_id)
     {
          const auto pop_id = scope_guard{ &ImGui::PopID };
          ImGui::PushID(++imgui_id);
          // ImGui text box with browse button
          // Highlight text box red if the folder doesn't exist
          {
               if (!m_input_path_valid)
               {
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.0F, 0.6F, 0.6F));
               }
               const auto pop_color = scope_guard([valid = m_input_path_valid]() {
                    if (!valid)
                    {
                         ImGui::PopStyleColor();
                    }
               });
               if (ImGui::InputText("Folder Path", m_input_path.data(), m_input_path.size()))
               {
                    // Check if the folder path exists when the text box changes
                    const auto tmp     = safedir(m_input_path.data());
                    m_input_path_valid = tmp.is_exists() && tmp.is_dir();
               }
          }
          ImGui::SameLine();
          if (ImGui::Button("Browse"))
          {
               // Trigger the chooseFolder function when the button is clicked
               // chooseFolder();
          }
     }

   public:
     void draw_window(int &imgui_id)
     {
          if (ImGui::Begin("Batch test"))
          {
               combo_input_type(imgui_id);
               combo_output_type(imgui_id);
               combo_compact_type(imgui_id);
               combo_flatten_type(imgui_id);
               browse_input_path(imgui_id);
          }
          ImGui::End();
     }
};
#endif// FIELD_MAP_EDITOR_BATCH_HPP
