//
// Created by pcvii on 3/7/2023.
//

#ifndef FIELD_MAP_EDITOR_BATCH_HPP
#define FIELD_MAP_EDITOR_BATCH_HPP
#include "archives_group.hpp"
#include "compact_type.hpp"
#include "cstdint"
#include "filter.hpp"
#include "format_imgui_text.hpp"
#include "generic_combo.hpp"
#include "gui_labels.hpp"
#include "RangeConsumer.hpp"
#include "safedir.hpp"
#include <array>
#include <imgui.h>
#include <memory>
#include <SFML/System/Time.hpp>
class batch
{
     std::shared_ptr<archives_group>                            m_archives_group    = {};
     input_types                                                m_input_type        = {};
     output_types                                               m_output_type       = {};
     ff_8::filter_old<compact_type>                             m_compact_type      = {};
     ff_8::filter_old<flatten_type>                             m_flatten_type      = {};
     bool                                                       m_input_path_valid  = { false };
     bool                                                       m_output_path_valid = { false };
     static constexpr std::size_t                               m_buffer_size       = { 256U };
     std::array<char, m_buffer_size>                            m_input_path        = {};
     std::array<char, m_buffer_size>                            m_output_path       = {};
     RangeConsumer<open_viii::archive::FIFLFS<true>>            m_fields_consumer   = {};
     std::shared_ptr<open_viii::archive::FIFLFS<false>>         m_field             = { nullptr };
     RangeConsumer<decltype(open_viii::LangCommon::to_array())> m_lang_consumer     = RangeConsumer{ open_viii::LangCommon::to_array() };
     std::optional<open_viii::LangT>                            m_coo               = {};
     std::string                                                m_status            = {};

     void                                                       combo_input_type(int &imgui_id);
     void                                                       combo_output_type(int &imgui_id);
     void                                                       combo_compact_type(int &imgui_id);
     void                                                       combo_flatten_type(int &imgui_id);
     void                                                       browse_input_path(int &imgui_id);
     void                                                       browse_output_path(int &imgui_id);
     void                                                       button_begin(int &imgui_id);
     static void browse_path(int &imgui_id, std::string_view name, bool &valid_path, std::array<char, m_buffer_size> &path_buffer);

   public:
     void update(sf::Time /*elsapsed_ms*/);
     explicit batch(std::shared_ptr<archives_group> existing_group)
       : m_archives_group(std::move(existing_group))
     {
     }
     batch &operator=(std::shared_ptr<archives_group> new_group)
     {
          m_archives_group = std::move(new_group);
          return *this;
     }
     void draw_window(int &imgui_id)
     {
          if (ImGui::Begin("Batch test"))
          {
               const bool disabled = !m_fields_consumer.done() || m_field;
               ImGui::BeginDisabled(disabled);
               combo_input_type(imgui_id);
               if (m_input_type != input_types::mim)
               {
                    browse_input_path(imgui_id);
               }
               combo_output_type(imgui_id);
               browse_output_path(imgui_id);
               combo_compact_type(imgui_id);
               combo_flatten_type(imgui_id);
               button_begin(imgui_id);
               ImGui::EndDisabled();
               if (disabled)
               {
                    format_imgui_text("{}", m_status);
               }
          }
          ImGui::End();
     }
     void choose_field_and_coo();
     void reset_for_next();
};
#endif// FIELD_MAP_EDITOR_BATCH_HPP
