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
     std::vector<bool>                                          m_maps_enabled      = {};
     std::uint8_t                                               m_num_columns       = { 5 };
     bool                                                       m_save_map          = {};
     directory_mode                                             m_directory_browser_mode             = {};
     bool                                                       m_input_load_map                     = { false };
     FutureOfFutureConsumer<std::vector<std::future<std::future<void>>>> m_future_of_future_consumer = {};
     FutureConsumer<std::vector<std::future<void>>>                      m_future_consumer           = {};
     ImGui::FileBrowser                  m_directory_browser{ ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CreateNewDir
                                             | ImGuiFileBrowserFlags_EditPathString };

     void                                combo_input_type(int &imgui_id);
     void                                combo_output_type(int &imgui_id);
     void                                combo_compact_type(int &imgui_id);
     void                                combo_flatten_type(int &imgui_id);
     void                                browse_input_path(int &imgui_id);
     void                                browse_output_path(int &imgui_id);
     void                                button_start(int &imgui_id);
     void                                button_stop(int &imgui_id);
     void                                checkbox_load_map(int &imgui_id);
     void                                choose_field_and_coo();
     void                                reset_for_next();
     void                                generate_map_sprite();
     void                                compact();
     void                                flatten();
     bool                                consume_one_future();
     void                                open_directory_browser();
     void                                button_input_browse();
     void                                button_output_browse();
     [[nodiscard]] std::filesystem::path append_file_structure(const std::filesystem::path &path) const;
     [[nodiscard]] bool browse_path(int &imgui_id, std::string_view name, bool &valid_path, std::array<char, m_buffer_size> &path_buffer);

   public:
     void update(sf::Time elapsed_time);
     explicit batch(std::shared_ptr<archives_group> existing_group);
     bool   in_progress() const;
     void   stop();
     batch &operator=(std::shared_ptr<archives_group> new_group);
     void   draw_window(int &imgui_id);
     void   checkmark_save_map();
     void   draw_multi_column_list_box(
         int                            &imgui_id,
         const std::string_view          name,
         const std::vector<std::string> &items,
         std::vector<bool>              &enabled);

     void tool_tip(const std::string_view str, int &imgui_id);
};
#endif// FIELD_MAP_EDITOR_BATCH_HPP
