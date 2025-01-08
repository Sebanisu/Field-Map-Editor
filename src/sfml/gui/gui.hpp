//
// Created by pcvii on 9/7/2021.
//
#ifndef FIELD_MAP_EDITOR_GUI_HPP
#define FIELD_MAP_EDITOR_GUI_HPP
#include "archives_group.hpp"
#include "batch.hpp"
#include "compact_type.hpp"
#include "Configuration.hpp"
#include "events.hpp"
#include "file_dialog_mode.hpp"
#include "filebrowser.hpp"
#include "formatters.hpp"
#include "generic_combo.hpp"
#include "grid.hpp"
#include "gui/history_window.hpp"
#include "import.hpp"
#include "map_directory_mode.hpp"
#include "mouse_positions.hpp"
#include "RangeConsumer.hpp"
#include "safedir.hpp"
#include "scope_guard.hpp"
#include "scrolling.hpp"
#include "Selections.hpp"
#include "upscales.hpp"
#include <cstdint>
#include <fmt/chrono.h>
#include <GL/glew.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>

namespace fme
{
struct gui
{
   public:
     gui();
     void start();

   private:
     using variant_tile_t = std::variant<
       open_viii::graphics::background::Tile1,
       open_viii::graphics::background::Tile2,
       open_viii::graphics::background::Tile3,
       std::monostate>;
     sf::Color                                                           clear_color                 = sf::Color::Black;
     std::mutex                                                          append_results_mutex        = {};
     std::vector<std::filesystem::path>                                  append_results              = {};
     std::shared_ptr<sf::Shader>                                         m_drag_sprite_shader        = {};
     std::shared_ptr<Selections>                                         m_selections                = std::make_shared<Selections>();
     scrolling                                                           m_scrolling                 = {};
     static constexpr std::int8_t                                        tile_size_px                = { 16 };
     static constexpr std::uint8_t                                       tile_size_px_unsigned       = { 16U };
     mouse_positions                                                     m_mouse_positions           = {};
     float                                                               m_scale_width               = {};
     sf::RenderWindow                                                    m_window                    = {};
     sf::Clock                                                           m_delta_clock               = {};
     sf::Time                                                            m_elapsed_time              = {};
     toml::array                                                         m_paths                     = {};
     toml::array                                                         m_custom_upscale_paths      = {};
     std::shared_ptr<archives_group>                                     m_archives_group            = {};
     batch                                                               m_batch                     = batch{ m_archives_group };
     std::vector<std::string>                                            m_upscale_paths             = {};
     std::shared_ptr<open_viii::archive::FIFLFS<false>>                  m_field                     = {};
     std::array<float, 2>                                                xy                          = {};
     mim_sprite                                                          m_mim_sprite                = {};
     std::shared_ptr<map_sprite>                                         m_map_sprite                = {};
     FutureOfFutureConsumer<std::vector<std::future<std::future<void>>>> m_future_of_future_consumer = {};
     FutureConsumer<std::vector<std::future<void>>>                      m_future_consumer           = {};
     float                                                               saved_window_width          = {};
     float                                                               saved_window_height         = {};
     fme::import                                                         m_import                    = {};
     fme::history_window                                                 m_history_window            = {};

     sf::RenderTexture                                                   m_shader_renderTexture      = {};

     bool                                                                m_changed                   = { false };
     //  ImGuiStyle                  m_original_style  = {};
     sf::Event                                                           m_event                     = {};
     sf::Vector2f                                                        m_cam_pos                   = {};
     // create a file browser instances
     ImGui::FileBrowser          m_save_file_browser{ ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir
                                             | ImGuiFileBrowserFlags_EditPathString };
     ImGui::FileBrowser          m_load_file_browser{ ImGuiFileBrowserFlags_EditPathString };
     ImGui::FileBrowser          m_directory_browser{ ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CreateNewDir
                                             | ImGuiFileBrowserFlags_EditPathString };

     static toml::array          get_paths();
     // imgui doesn't support std::string or std::string_view or
     // std::filesystem::path, only const char *
     archives_group              get_archives_group() const;
     sf::RenderWindow            get_render_window() const;
     void                        update_path();
     void                        consume_one_future();
     mim_sprite                  get_mim_sprite() const;
     std::shared_ptr<map_sprite> get_map_sprite() const;

     void                        init_and_get_style();
     void                        loop_events();
     void                        loop();
     void                        draw_window();
     void                        update_hover_and_mouse_button_status_for_map(const ImVec2 &img_start, const float scale);
     void         draw_map_grid_lines_for_tiles(const ImVec2 &screen_pos, const sf::Vector2f &scaled_size, const float scale);
     void         draw_map_grid_lines_for_texture_page(const ImVec2 &screen_pos, const sf::Vector2f &scaled_size, const float scale);
     void         draw_mim_grid_lines_for_tiles(const ImVec2 &screen_pos, const sf::Vector2f &scaled_size, const float scale);
     void         draw_mim_grid_lines_for_texture_page(const ImVec2 &screen_pos, const sf::Vector2f &scaled_size, const float scale);
     void         draw_mouse_positions_sprite(const float scale, const ImVec2 &screen_pos);
     bool         combo_path();
     void         combo_draw();
     void         file_browser_save_texture();
     void         directory_browser_display();
     void         menu_bar();
     void         combo_pupu();
     void         combo_palette();
     void         combo_bpp();
     void         checkbox_mim_palette_texture();
     void         combo_field();
     void         combo_coo();
     void         combo_draw_bit();
     std::string  save_texture_path() const;
     void         update_field();
     bool         mim_test() const;
     bool         map_test() const;
     void         checkbox_map_swizzle();
     void         checkbox_map_disable_blending();
     void         menuitem_locate_ff8();
     void         menuitem_save_swizzle_textures();
     void         menuitem_save_deswizzle_textures();
     void         menuitem_load_swizzle_textures();
     void         menuitem_load_deswizzle_textures();
     void         menuitem_save_texture(bool enabled = true);
     void         menuitem_save_mim_file(bool enabled = true);
     void         menuitem_save_map_file(bool enabled = true);
     void         menuitem_save_map_file_modified(bool enabled = true);
     void         menuitem_load_map_file(bool enabled = true);
     void         scale_window(float width = {}, float height = {});
     int          get_selected_field();
     std::uint8_t palette() const;
     open_viii::graphics::BPPT                          bpp() const;
     void                                               combo_blend_modes();
     void                                               combo_layers();
     void                                               combo_texture_pages();
     void                                               combo_animation_ids();
     void                                               combo_animation_frames();
     void                                               combo_filtered_palettes();
     void                                               combo_filtered_bpps();
     void                                               combo_blend_other();
     void                                               combo_z();
     bool                                               handle_mouse_cursor();
     std::shared_ptr<open_viii::archive::FIFLFS<false>> init_field();
     void                                               text_mouse_position() const;
     void                                               on_click_not_imgui();
     void                                               combo_upscale_path();
     bool                                               combo_upscale_path(ff_8::filter_old<std::filesystem::path> &filter) const;
     void                                               combo_deswizzle_path();
     const open_viii::LangT                            &get_coo() const;
     file_dialog_mode                                   m_file_dialog_mode       = {};
     map_directory_mode                                 m_modified_directory_map = {};
     std::filesystem::path                              m_loaded_swizzle_texture_path{};
     std::filesystem::path                              m_loaded_deswizzle_texture_path{};
     std::vector<std::size_t>                           m_clicked_tile_indices{};
     //     void                                               popup_batch_reswizzle();
     //     void                                               popup_batch_deswizzle();
     static std::string                                 starter_field();
     static void                                        popup_batch_common_filter_start(
                                              ff_8::filter_old<std::filesystem::path> &filter,
                                              std::string_view                         prefix,
                                              std::string_view                         base_name);
     // void popup_batch_embed();
     template<bool Nested = false>
     std::vector<std::filesystem::path>
          replace_entries(const open_viii::archive::FIFLFS<Nested> &field, const std::vector<std::filesystem::path> &paths) const;

     //     template<typename T, typename... argsT>
     //     void launch_async(T &&task, argsT &&...args)
     //     {
     //          std::invoke(std::forward<T>(task), std::forward<argsT>(args)...);
     //     }
     //     void                      batch_ops_ask_menu() const;
     bool combo_upscale_path(std::filesystem::path &path, const std::string &field_name, open_viii::LangT coo) const;
     void open_locate_ff8_filebrowser();
     variant_tile_t                           &combo_selected_tile(bool &changed);
     [[nodiscard]] bool                        browse_for_image_display_preview();
     void                                      generate_upscale_paths(const std::string &field_name, open_viii::LangT coo);
     void                                      menuitem_locate_custom_upscale();
     static toml::array                        get_custom_upscale_paths_vector();
     void                                      save_swizzle_textures();
     void                                      reset_imported_image();
     void                                      sort_paths();
     void                                      control_panel_window();
     std::uint32_t                             image_height() const;
     static std::vector<std::filesystem::path> find_maps_in_directory(const std::filesystem::path &src, size_t reserve = {});
     float                                     scaled_menubar_gap() const;
     void                                      render_dockspace();
     void                                      background_color_picker();
     void                                      collapsing_header_filters();
     void                                      compact_flatten_buttons();
     void                                      frame_rate();
     void                                      control_panel_window_map();
     void                                      control_panel_window_mim();
     std::filesystem::path                     path_with_prefix_and_base_name(std::filesystem::path selected_path) const;
     template<typename batch_opT, typename filterT, typename askT, typename processT>
     void popup_batch_common(batch_opT &&batch_op, filterT &&filter, askT &&ask, processT &&process) const
     {
          if (!m_archives_group)
          {
               return;
          }
          if (batch_op(
                m_archives_group->mapdata(),
                [&](const int &pos, std::filesystem::path selected_path, ff_8::filters filters, auto &&...rest) {
                     const auto field = m_archives_group->field(pos);
                     if (!field)
                     {
                          return;
                     }
                     const auto map_pairs = field->get_vector_of_indexes_and_files({ open_viii::graphics::background::Map::EXT });
                     if (map_pairs.empty())
                     {
                          return;
                     }
                     std::string const      base_name = map_sprite::str_to_lower(field->get_base_name());
                     std::string_view const prefix    = std::string_view{ base_name }.substr(0U, 2U);
                     popup_batch_common_filter_start(filter(filters), prefix, base_name);

                     auto map = m_map_sprite->with_field(field, open_viii::LangT::generic).with_filters(filters);
                     if (map.fail())
                     {
                          return;
                     }
                     if (filter(filters).enabled())
                     {
                          auto    map_path      = filter(filters).value() / map.map_filename();
                          safedir safe_map_path = map_path;
                          if (safe_map_path.is_exists())
                          {
                               map.load_map(map_path);
                          }
                     }
                     selected_path = selected_path / prefix / base_name;
                     std::error_code error_code{};
                     const bool      create_directories_result = std::filesystem::create_directories(selected_path, error_code);
                     if (error_code)
                     {
                          spdlog::error(
                            "{}:{} - {}: {} - path: {}",
                            __FILE__,
                            __LINE__,
                            error_code.value(),
                            error_code.message(),
                            selected_path.string());
                          error_code.clear();
                     }
                     if (create_directories_result)
                     {
                          format_imgui_text("{} {}", gui_labels::directory_created, selected_path.string());
                     }
                     else
                     {
                          format_imgui_text("{} {}", gui_labels::directory_exists, selected_path.string());
                     }
                     format_imgui_text(gui_labels::saving_textures);

                     if (map_pairs.size() > 1U)
                     {
                          spdlog::debug(
                            "{}:{} - {}: {}\t {}: {}",
                            __FILE__,
                            __LINE__,
                            gui_labels::count_of_maps,
                            map_pairs.size(),
                            gui_labels::field,
                            base_name);
                          for (const auto &[i, file_path] : map_pairs)
                          {
                               const auto             filename      = std::filesystem::path(file_path).filename().stem().string();
                               std::string_view const filename_view = { filename };
                               std::string_view const basename_view = { base_name };
                               if (
                                 filename_view.substr(0, std::min(std::size(filename_view), std::size(basename_view)))
                                 != basename_view.substr(0, std::min(std::size(filename_view), std::size(basename_view))))
                               {
                                    continue;
                               }
                               if (filename.size() == base_name.size())
                               {
                                    process(selected_path, map, rest...);
                                    continue;
                               }
                               const auto coo_view = filename_view.substr(std::size(basename_view) + 1U, 2U);
                               spdlog::info("Filename and coo: {}\t{}", filename, coo_view);
                               map = map.with_coo(open_viii::LangCommon::from_string(coo_view));
                               process(selected_path, map, rest...);
                          }
                     }
                     else
                     {
                          process(selected_path, map, rest...);
                     }
                },
                ask))
          {
          }
     }
     void filter_empty_import_tiles();
     void collapsing_header_generated_tiles() const;
     void adjust_source_xy_texture_page_for_import_map(uint8_t next_source_y, const uint8_t next_texture_page);
     void find_selected_tile_for_import(variant_tile_t &current_tile) const;
     void event_type_key_released(const sf::Event::KeyEvent &key);
     void event_type_key_pressed(const sf::Event::KeyEvent &key);
     void event_type_mouse_button_pressed(const sf::Mouse::Button &button);
     void event_type_mouse_button_released(const sf::Mouse::Button &button);
     void file_menu();
     void edit_menu();
     void batch_operation_test_menu();
     void import_menu();
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_GUI_HPP
