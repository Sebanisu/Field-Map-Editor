//
// Created by pcvii on 3/7/2023.
//

#ifndef FIELD_MAP_EDITOR_BATCH_HPP
#define FIELD_MAP_EDITOR_BATCH_HPP
#include "archives_group.hpp"
#include "compact_type.hpp"
#include "Configuration.hpp"
#include "cstdint"
#include "filebrowser.hpp"
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
     enum struct directory_mode
     {
          input_mode,
          output_mode,
     };
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
     map_sprite                                                 m_map_sprite        = {};
     directory_mode                                             m_directory_browser_mode             = {};
     bool                                                       m_input_load_map                     = { false };
     FutureOfFutureConsumer<std::vector<std::future<std::future<void>>>> m_future_of_future_consumer = {};
     FutureConsumer<std::vector<std::future<void>>>                      m_future_consumer           = {};
     ImGui::FileBrowser                                                  m_directory_browser{ static_cast<ImGuiFileBrowserFlags>(
       static_cast<std::uint32_t>(ImGuiFileBrowserFlags_SelectDirectory)
       | static_cast<std::uint32_t>(ImGuiFileBrowserFlags_CreateNewDir)) };

     void                                                                combo_input_type(int &imgui_id);
     void                                                                combo_output_type(int &imgui_id);
     void                                                                combo_compact_type(int &imgui_id);
     void                                                                combo_flatten_type(int &imgui_id);
     void                                                                browse_input_path(int &imgui_id);
     void                                                                browse_output_path(int &imgui_id);
     void                                                                button_begin(int &imgui_id);
     void                                                                checkbox_load_map(int &imgui_id);
     void                                                                choose_field_and_coo();
     void                                                                reset_for_next();
     void                                                                generate_map_sprite();
     void                                                                compact();
     void                                                                flatten();
     bool                                                                consume_one_future();
     void                                                                open_directory_browser();
     void                                                                button_input_browse();
     void                                                                button_output_browse();
     [[nodiscard]] std::filesystem::path                                 append_file_structure(const std::filesystem::path &path) const;
     [[nodiscard]] bool browse_path(int &imgui_id, std::string_view name, bool &valid_path, std::array<char, m_buffer_size> &path_buffer);

   public:
     void update(sf::Time elapsed_time);
     explicit batch(std::shared_ptr<archives_group> existing_group)
       : m_archives_group(std::move(existing_group))
     {
          Configuration const config{};
          m_input_type =
            static_cast<input_types>(config["batch_input_type"].value_or(static_cast<std::underlying_type_t<input_types>>(m_input_type)));
          {
               std::string str_tmp = config["batch_input_path"].value_or(std::string(m_input_path.data()));
               std::ranges::copy(str_tmp, m_input_path.data());
               m_input_path.at(str_tmp.size()) = '\0';
               const auto tmp                  = safedir(m_input_path.data());
               m_input_path_valid              = tmp.is_dir() && tmp.is_exists();
          }
          m_output_type = static_cast<output_types>(
            config["batch_output_type"].value_or(static_cast<std::underlying_type_t<output_types>>(m_output_type)));
          {
               std::string str_tmp = config["batch_output_path"].value_or(std::string(m_output_path.data()));
               std::ranges::copy(str_tmp, m_output_path.data());
               m_output_path.at(str_tmp.size()) = '\0';
               const auto tmp                   = safedir(m_output_path.data());
               m_output_path_valid              = tmp.is_dir() && tmp.is_exists();
          }
          m_compact_type.update(static_cast<compact_type>(
            config["batch_compact_type"].value_or(static_cast<std::underlying_type_t<compact_type>>(m_compact_type.value()))));
          if (config["batch_compact_enabled"].value_or(m_compact_type.enabled()))
          {
               m_compact_type.enable();
          }
          else
          {
               m_compact_type.disable();
          }
          m_flatten_type.update(static_cast<flatten_type>(
            config["batch_flatten_type"].value_or(static_cast<std::underlying_type_t<flatten_type>>(m_flatten_type.value()))));
          if (config["batch_flatten_enabled"].value_or(m_flatten_type.enabled()))
          {
               m_flatten_type.enable();
          }
          else
          {
               m_flatten_type.disable();
          }
          m_input_load_map = config["batch_input_load_map"].value_or(m_input_load_map);
     }
     batch &operator=(std::shared_ptr<archives_group> new_group)
     {
          m_archives_group = std::move(new_group);
          return *this;
     }
     void draw_window(int &imgui_id)
     {
          const auto end = scope_guard(&ImGui::End);
          if (!ImGui::Begin("Batch test"))
          {
               return;
          }
          const bool disabled = !m_fields_consumer.done() || m_field;
          open_directory_browser();
          ImGui::BeginDisabled(disabled);
          combo_input_type(imgui_id);
          browse_input_path(imgui_id);
          checkbox_load_map(imgui_id);
          combo_output_type(imgui_id);
          browse_output_path(imgui_id);
          combo_compact_type(imgui_id);
          combo_flatten_type(imgui_id);
          button_begin(imgui_id);
          ImGui::EndDisabled();
          if (!disabled)
          {
               return;
          }
          format_imgui_text("{}", m_status);
     }
};
#endif// FIELD_MAP_EDITOR_BATCH_HPP
