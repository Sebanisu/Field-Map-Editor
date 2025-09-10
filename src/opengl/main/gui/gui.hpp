//
// Created by pcvii on 9/7/2021.
//
#ifndef FIELD_MAP_EDITOR_GUI_HPP
#define FIELD_MAP_EDITOR_GUI_HPP
// clang-format off
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "archives_group.hpp"
#include "as_string.hpp"
#include "batch.hpp"
#include "colors.hpp"
#include "compact_type.hpp"
#include "Configuration.hpp"
#include "create_tile_button.hpp"
#include "custom_paths_window.hpp"
#include "draw_window.hpp"
#include "fa_icons.hpp"
#include "field_file_window.hpp"
#include "file_dialog_mode.hpp"
#include "filebrowser.hpp"
#include "filter_window.hpp"
#include "formatters.hpp"
#include "generic_combo.hpp"
#include "gui/history_window.hpp"
#include "import.hpp"
#include "keyboard_shortcuts_window.hpp"
#include "map_directory_mode.hpp"
#include "mouse_positions.hpp"
#include "RangeConsumer.hpp"
#include "safedir.hpp"
#include "Selections.hpp"
#include <cstdint>
#include <fmt/chrono.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <memory>
#include <ScopeGuard.hpp>
#include <TimeStep.hpp>

namespace fme
{
struct gui
{
   public:
     gui(GLFWwindow *const window);
     void start(GLFWwindow *const window);
     ~gui();

   private:
     using variant_tile_t = std::variant<
       open_viii::graphics::background::Tile1,
       open_viii::graphics::background::Tile2,
       open_viii::graphics::background::Tile3,
       std::monostate>;
     std::shared_ptr<Selections>                        m_selections                = std::make_shared<Selections>();
     // todo replace with glengine::Shader
     // std::shared_ptr<sf::Shader>                        m_drag_sprite_shader  = {};
     static constexpr std::int8_t                       tile_size_px                = { 16 };
     static constexpr std::uint8_t                      tile_size_px_unsigned       = { 16U };
     int                                                m_field_index               = {};
     float                                              m_scale_width               = {};
     glengine::TimeStep                                 m_delta_clock               = {};
     float                                              m_elapsed_time              = {};///< seconds
     std::shared_ptr<archives_group>                    m_archives_group            = {};
     batch                                              m_batch_window              = {};
     std::shared_ptr<open_viii::archive::FIFLFS<false>> m_field                     = {};
     std::array<float, 2>                               xy                          = {};
     std::shared_ptr<mim_sprite>                        m_mim_sprite                = {};
     std::shared_ptr<map_sprite>                        m_map_sprite                = {};
     filter_window                                      m_filter_window             = { m_selections, m_map_sprite };
     draw_window                                        m_draw_window               = { m_selections, m_mim_sprite, m_map_sprite };
     custom_paths_window                                m_custom_paths_window       = { m_selections };
     field_file_window                                  m_field_file_window         = { m_field, m_selections };
     keyboard_shortcuts_window                          m_keyboard_shortcuts_window = { m_selections };
     struct PathsAndEnabled
     {
          std::vector<std::filesystem::path> path{};
          std::vector<std::vector<bool>>     enabled{};
          ConfigKey                          path_key{};
          std::vector<ConfigKey>             enabled_key{};
     };
     FutureOfFutureConsumer<std::vector<std::future<std::future<PathsAndEnabled>>>> m_future_of_future_paths_consumer = {};
     FutureConsumer<std::vector<std::future<PathsAndEnabled>>>                      m_future_paths_consumer           = {};
     FutureOfFutureConsumer<std::vector<std::future<std::future<void>>>>            m_future_of_future_consumer       = {};
     FutureConsumer<std::vector<std::future<void>>>                                 m_future_consumer                 = {};
     float                                                                          saved_window_width                = {};
     float                                                                          saved_window_height               = {};
     fme::import                                                                    m_import                          = {};
     fme::history_window                                                            m_history_window                  = {};

     // sf::RenderTexture                                                   m_shader_renderTexture        = {};

     bool                                                                           m_changed                         = { false };
     //  ImGuiStyle                  m_original_style  = {};
     // todo fix events
     // sf::Event                                                                      m_event                           = {};
     glm::vec2                                                                      m_cam_pos                         = {};
     // create a file browser instances
     ImGui::FileBrowser          m_save_file_browser{ ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir
                                             | ImGuiFileBrowserFlags_EditPathString };
     ImGui::FileBrowser          m_load_file_browser{ ImGuiFileBrowserFlags_EditPathString };
     ImGui::FileBrowser          m_directory_browser{ ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CreateNewDir
                                             | ImGuiFileBrowserFlags_EditPathString };


     std::array<float, 3U>       clear_color_f{};
     std::array<float, 3U>       clear_color_f2{};


     create_color_button         blue_color_button  = {};
     create_color_button         green_color_button = { { .button_color        = colors::ButtonGreen,
                                                          .button_hover_color  = colors::ButtonGreenHovered,
                                                          .button_active_color = colors::ButtonGreenActive } };

     create_color_button         pink_color_button  = { { .button_color        = colors::ButtonPink,
                                                          .button_hover_color  = colors::ButtonPinkHovered,
                                                          .button_active_color = colors::ButtonPinkActive } };

     // imgui doesn't support std::string or std::string_view or
     // std::filesystem::path, only const char *
     archives_group              get_archives_group() const;
     void                        update_path();
     void                        consume_one_future();
     std::shared_ptr<mim_sprite> get_mim_sprite() const;
     std::shared_ptr<map_sprite> get_map_sprite() const;
     bool                        combo_path();
     void                        combo_draw();
     void                        file_browser_display();
     void                        directory_browser_display();
     void                        menu_bar();
     void                        selected_tiles_panel();
     void                        tile_conflicts_panel();
     void                        hovered_tiles_panel();
     void                        combo_mim_palette();
     void                        combo_mim_bpp();
     void                        checkbox_mim_palette_texture();
     void                        combo_field();
     void                        combo_coo();
     std::string                 save_texture_path() const;
     void                        update_field();
     bool                        mim_test() const;
     bool                        map_test() const;
     void                        checkbox_map_swizzle();
     void                        checkbox_map_disable_blending();
     void                        menuitem_locate_ff8();
     void                        menuitem_save_swizzle_textures();
     void                        menuitem_save_swizzle_as_one_image_textures();
     void                        menuitem_save_deswizzle_textures();
     void                        menuitem_load_swizzle_textures();
     void                        menuitem_load_swizzle_as_one_image_textures();
     void                        menuitem_load_deswizzle_textures();
     void                        menuitem_save_texture(bool enabled = true);
     void                        menuitem_save_mim_file(bool enabled = true);
     void                        menuitem_save_map_file(bool enabled = true);
     void                        menuitem_save_map_file_modified(bool enabled = true);
     void                        menuitem_load_map_file(bool enabled = true);
     // void                      scale_window(float width = {}, float height = {});
     std::uint8_t                palette() const;
     open_viii::graphics::BPPT   bpp() const;
     std::shared_ptr<open_viii::archive::FIFLFS<false>> init_field();
     void                                               text_mouse_position() const;
     void                                               combo_swizzle_path();
     bool                                               combo_swizzle_path(ff_8::filter_old<ff_8::FilterTag::Swizzle> &filter) const;
     void                                               combo_swizzle_as_one_image_path();
     bool                    combo_swizzle_as_one_image_path(ff_8::filter_old<ff_8::FilterTag::SwizzleAsOneImage> &filter) const;
     void                    combo_deswizzle_path();
     bool                    combo_deswizzle_path(ff_8::filter_old<ff_8::FilterTag::Deswizzle> &filter) const;
     void                    combo_full_filename_path();
     bool                    combo_full_filename_path(ff_8::filter_old<ff_8::FilterTag::FullFileName> &filter) const;
     void                    combo_map_path();
     bool                    combo_map_path(ff_8::filter_old<ff_8::FilterTag::Map> &filter) const;
     const open_viii::LangT &get_coo() const;
     file_dialog_mode        m_file_dialog_mode       = {};
     map_directory_mode      m_modified_directory_map = {};
     // std::filesystem::path    m_loaded_swizzle_texture_path{};
     std::filesystem::path   m_loaded_deswizzle_texture_path{};
     //     void                                               popup_batch_reswizzle();
     //     void                                               popup_batch_deswizzle();
     // static void              popup_batch_common_filter_start(
     //                ff_8::filter_old<std::filesystem::path> &filter,
     //                std::string_view                         prefix,
     //                std::string_view                         base_name);
     // void popup_batch_embed();
     template<bool Nested = false>
     std::vector<std::filesystem::path> replace_entries(
       const open_viii::archive::FIFLFS<Nested> &field,
       const std::vector<std::filesystem::path> &paths) const;

     //     template<typename T, typename... argsT>
     //     void launch_async(T &&task, argsT &&...args)
     //     {
     //          std::invoke(std::forward<T>(task), std::forward<argsT>(args)...);
     //     }
     //     void                      batch_ops_ask_menu() const;
     void                                                    open_locate_ff8_filebrowser();
     variant_tile_t                                         &combo_selected_tile(bool &changed);
     [[nodiscard]] bool                                      browse_for_image_display_preview();
     [[nodiscard]] std::future<std::future<PathsAndEnabled>> generate_sort_paths();
     [[nodiscard]] std::future<std::future<PathsAndEnabled>> generate_external_texture_paths();
     [[nodiscard]] std::future<std::future<PathsAndEnabled>> generate_external_map_paths();
     void                                                    reset_imported_image();
     void                                                    control_panel_window();
     std::uint32_t                                           image_height() const;
     static std::vector<std::filesystem::path>               find_maps_in_directory(
                     const std::filesystem::path &src,
                     size_t                       reserve = {});
     float                 scaled_menubar_gap() const;
     void                  render_dockspace();
     void                  background_color_picker();
     void                  compact_flatten_buttons();
     void                  frame_rate();
     void                  control_panel_window_map();
     void                  control_panel_window_mim();
     std::filesystem::path path_with_prefix_and_base_name(std::filesystem::path selected_path) const;
     void                  filter_empty_import_tiles();
     void                  collapsing_header_generated_tiles() const;
     void                  adjust_source_xy_texture_page_for_import_map(
                        uint8_t       next_source_y,
                        const uint8_t next_texture_page);
     void                                            find_selected_tile_for_import(variant_tile_t &current_tile) const;
     // todo fix events.
     void                                            bind_shortcuts();
     // void event_type_key_pressed(const sf::Event::KeyEvent &key);
     // void event_type_mouse_button_pressed(const sf::Mouse::Button &button);
     // void event_type_mouse_button_released(const sf::Mouse::Button &button);
     void                                            file_menu();
     void                                            edit_menu();
     void                                            windows_menu();
     void                                            help_menu();
     void                                            refresh_map_swizzle();
     void                                            refresh_map_disable_blending();
     void                                            refresh_draw_mode();
     void                                            refresh_mim_palette_texture();
     void                                            refresh_path();
     void                                            browse_buttons();
     void                                            menu_swizzle_paths();
     void                                            menu_swizzle_as_one_image_paths();
     void                                            menu_deswizzle_paths();
     void                                            menu_full_filename_paths();
     void                                            menu_map_paths();
     void                                            refresh_field();
     void                                            refresh_bpp(open_viii::graphics::BPPT in_bpp);
     void                                            refresh_palette(std::uint8_t palette);
     void                                            refresh_render_texture(bool reload_textures = false);
     void                                            refresh_coo();
     [[nodiscard]] static std::optional<std::string> handle_path_deletion(
       std::vector<std::string> &ff8_directory_paths,
       std::ptrdiff_t            offset);
     [[nodiscard]] static std::string find_replacement_path_value(
       const std::vector<std::string> &paths,
       const std::vector<bool>        &paths_enabled);
     [[nodiscard]] static std::ptrdiff_t add_delete_button(
       const std::string              &path,
       const std::vector<std::string> &paths);

     [[nodiscard]] static std::ptrdiff_t add_delete_button(const std::ptrdiff_t index);


     static inline constinit bool        toggle_imgui_demo_window = { false };
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_GUI_HPP
