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
#include <array>
#include <imgui.h>
class batch
{
     input_types                    m_input_type;
     output_types                   m_output_type;
     ff_8::filter_old<compact_type> m_compact_type;
     ff_8::filter_old<flatten_type> m_flatten_type;

     void                           combo_input_type(int &imgui_id);
     void                           combo_output_type(int &imgui_id);
     void                           combo_compact_type(int &imgui_id);
     void                           combo_flatten_type(int &imgui_id);

   public:
     void draw_window(int &imgui_id)
     {
          if (ImGui::Begin("Batch test"))
          {
               combo_input_type(imgui_id);
               combo_output_type(imgui_id);
               combo_compact_type(imgui_id);
               combo_flatten_type(imgui_id);
          }
          ImGui::End();
     }
};
#endif// FIELD_MAP_EDITOR_BATCH_HPP
