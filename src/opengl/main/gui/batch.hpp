//
// Created by pcvii on 3/7/2023.
//

#ifndef FIELD_MAP_EDITOR_BATCH_HPP
#define FIELD_MAP_EDITOR_BATCH_HPP
#include "archives_group.hpp"
#include "Configuration.hpp"
#include "cstdint"
#include "filebrowser.hpp"
#include "format_imgui_text.hpp"
#include "generic_combo.hpp"
#include "gui_labels.hpp"
#include "RangeConsumer.hpp"
#include "safedir.hpp"
#include "Selections.hpp"
#include <array>
#include <imgui.h>
#include <memory>
namespace fme
{
class batch
{
     enum struct directory_mode
     {
          input_mode,
          output_mode,
     };
     std::weak_ptr<Selections>       m_selections             = {};
     std::weak_ptr<archives_group>   m_archives_group         = {};
     bool                            m_input_path_valid       = { false };
     bool                            m_output_path_valid      = { false };
     static constexpr std::size_t    m_buffer_size            = { 256U };
     std::array<char, m_buffer_size> m_input_path             = {};
     std::array<char, m_buffer_size> m_output_path            = {};
     std::optional<open_viii::LangT> m_coo                    = {};
     std::string                     m_status                 = {};
     map_sprite                      m_map_sprite             = {};
     std::uint8_t                    m_num_columns            = { 5 };
     directory_mode                  m_directory_browser_mode = {};
     ImGui::FileBrowser              m_directory_browser{
          ImGuiFileBrowserFlags_SelectDirectory
          | ImGuiFileBrowserFlags_CreateNewDir
          | ImGuiFileBrowserFlags_EditPathString
          | ImGuiFileBrowserFlags_SkipItemsCausingError
     };

     RangeConsumer<open_viii::archive::FIFLFS<true>>    m_fields_consumer = {};
     std::shared_ptr<open_viii::archive::FIFLFS<false>> m_field = { nullptr };
     RangeConsumer<decltype(open_viii::LangCommon::to_array())> m_lang_consumer
       = RangeConsumer{ open_viii::LangCommon::to_array() };
     FutureConsumer<std::vector<std::future<void>>> m_future_consumer = {};
     FutureOfFutureConsumer<std::vector<std::future<std::future<void>>>>
                        m_future_of_future_consumer = {};

     mutable bool       m_update_delay              = false;
     mutable bool       m_force_loading             = true;

     mutable float      m_interval                  = 0.03f;
     mutable float      m_total_elapsed_time        = 0.f;

     void               combo_input_type();
     void               combo_output_type();
     void               combo_compact_type_ffnx();
     void               combo_compact_type();
     void               combo_flatten_type_bpp();
     void               combo_flatten_type();
     void               browse_input_path();
     void               browse_output_path();
     void               button_start();
     void               button_stop();
     void               checkbox_load_map();
     void               choose_field_and_coo();
     void               reset_for_next();
     void               generate_map_sprite();
     void               compact();
     void               flatten();
     bool               consume_one_future();
     void               open_directory_browser();
     void               button_input_browse();
     void               button_output_browse();
     void               example_input_paths();
     void               example_output_paths();
     // both use the output pattern currently
     const std::string &get_output_pattern(fme::input_types type);
     const std::string &get_output_pattern(fme::output_types type);
     const std::string &get_output_map_pattern(fme::input_types type);
     const std::string &get_output_map_pattern(fme::output_types type);
     void               save_input_path();
     void               save_output_path();
     //[[nodiscard]] std::filesystem::path append_file_structure(const
     // std::filesystem::path &path) const;
     [[nodiscard]] bool browse_path(
       std::string_view name,
       bool            &valid_path,
       std::array<
         char,
         m_buffer_size> &path_buffer);

     void checkmark_save_map();
     void checkmarks_save_masks();
     bool draw_multi_column_list_box(
       const std::string_view          name,
       const std::vector<std::string> &items,
       std::vector<bool>              &enabled);


   public:
     void update(float elapsed_time);
     batch() = default;
     explicit batch(
       std::weak_ptr<Selections>     existing_selections,
       std::weak_ptr<archives_group> existing_group);
     bool   in_progress() const;
     void   stop();
     batch &operator=(std::weak_ptr<archives_group> new_group);
     batch &operator=(std::weak_ptr<Selections> new_selections);
     void   draw_window();
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_BATCH_HPP
