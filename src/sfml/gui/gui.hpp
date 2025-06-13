//
// Created by pcvii on 9/7/2021.
//
#ifndef FIELD_MAP_EDITOR_GUI_HPP
#define FIELD_MAP_EDITOR_GUI_HPP
#include "archives_group.hpp"
#include "as_string.hpp"
#include "batch.hpp"
#include "colors.hpp"
#include "compact_type.hpp"
#include "Configuration.hpp"
#include "custom_paths_window.hpp"
#include "events.hpp"
#include "fa_icons.hpp"
#include "field_file_window.hpp"
#include "file_dialog_mode.hpp"
#include "filebrowser.hpp"
#include "formatters.hpp"
#include "generic_combo.hpp"
#include "gui/history_window.hpp"
#include "import.hpp"
#include "map_directory_mode.hpp"
#include "mouse_positions.hpp"
#include "RangeConsumer.hpp"
#include "safedir.hpp"
#include "scope_guard.hpp"
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
     gui(sf::RenderWindow &window);
     void start(sf::RenderWindow &window);

   private:
     using variant_tile_t = std::variant<
       open_viii::graphics::background::Tile1,
       open_viii::graphics::background::Tile2,
       open_viii::graphics::background::Tile3,
       std::monostate>;
     std::shared_ptr<Selections>                                         m_selections          = std::make_shared<Selections>();
     std::shared_ptr<sf::Shader>                                         m_drag_sprite_shader  = {};
     static constexpr std::int8_t                                        tile_size_px          = { 16 };
     static constexpr std::uint8_t                                       tile_size_px_unsigned = { 16U };
     mouse_positions                                                     m_mouse_positions     = {};
     int                                                                 m_field_index         = {};
     float                                                               m_scale_width         = {};
     sf::Clock                                                           m_delta_clock         = {};
     sf::Time                                                            m_elapsed_time        = {};
     std::shared_ptr<archives_group>                                     m_archives_group      = {};
     batch                                                               m_batch;
     std::vector<std::string>                                            m_upscale_paths               = {};
     std::vector<bool>                                                   m_upscale_paths_enabled       = {};
     std::vector<std::string>                                            m_deswizzle_paths             = {};
     std::vector<bool>                                                   m_deswizzle_paths_enabled     = {};
     std::vector<std::string>                                            m_upscale_map_paths           = {};
     std::vector<bool>                                                   m_upscale_map_paths_enabled   = {};
     std::vector<std::string>                                            m_deswizzle_map_paths         = {};
     std::vector<bool>                                                   m_deswizzle_map_paths_enabled = {};
     std::shared_ptr<open_viii::archive::FIFLFS<false>>                  m_field                       = {};
     std::array<float, 2>                                                xy                            = {};
     mim_sprite                                                          m_mim_sprite                  = {};
     std::shared_ptr<map_sprite>                                         m_map_sprite                  = {};
     FutureOfFutureConsumer<std::vector<std::future<std::future<void>>>> m_future_of_future_consumer   = {};
     FutureConsumer<std::vector<std::future<void>>>                      m_future_consumer             = {};
     float                                                               saved_window_width            = {};
     float                                                               saved_window_height           = {};
     fme::import                                                         m_import                      = {};
     fme::history_window                                                 m_history_window              = {};

     sf::RenderTexture                                                   m_shader_renderTexture        = {};

     bool                                                                m_changed                     = { false };
     //  ImGuiStyle                  m_original_style  = {};
     sf::Event                                                           m_event                       = {};
     sf::Vector2f                                                        m_cam_pos                     = {};
     std::vector<std::size_t>                                            m_hovered_tiles_indices       = {};
     std::ptrdiff_t                                                      m_hovered_index               = { -1 };
     custom_paths_window                                                 m_custom_paths_window         = { m_selections };
     field_file_window                                                   m_field_file_window           = { m_field, m_selections };
     // create a file browser instances
     ImGui::FileBrowser          m_save_file_browser{ ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir
                                             | ImGuiFileBrowserFlags_EditPathString };
     ImGui::FileBrowser          m_load_file_browser{ ImGuiFileBrowserFlags_EditPathString };
     ImGui::FileBrowser          m_directory_browser{ ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CreateNewDir
                                             | ImGuiFileBrowserFlags_EditPathString };


     std::array<float, 3U>       clear_color_f{};

     // imgui doesn't support std::string or std::string_view or
     // std::filesystem::path, only const char *
     archives_group              get_archives_group() const;
     sf::RenderWindow            get_render_window() const;
     void                        update_path();
     void                        consume_one_future();
     mim_sprite                  get_mim_sprite() const;
     std::shared_ptr<map_sprite> get_map_sprite() const;
     void                        draw_window();
     void                        update_hover_and_mouse_button_status_for_map(const ImVec2 &img_start, const float scale);
     void                        draw_map_grid_lines_for_tiles(const ImVec2 &screen_pos, const ImVec2 &scaled_size, const float scale);
     void                        draw_map_grid_for_conflict_tiles(const ImVec2 &screen_pos, const float scale);
     void                      draw_map_grid_lines_for_texture_page(const ImVec2 &screen_pos, const ImVec2 &scaled_size, const float scale);
     void                      draw_mim_grid_lines_for_tiles(const ImVec2 &screen_pos, const ImVec2 &scaled_size, const float scale);
     void                      draw_mim_grid_lines_for_texture_page(const ImVec2 &screen_pos, const ImVec2 &scaled_size, const float scale);
     void                      draw_mouse_positions_sprite(const float scale, const ImVec2 &screen_pos);
     bool                      combo_path();
     void                      combo_draw();
     void                      file_browser_save_texture();
     void                      directory_browser_display();
     void                      menu_bar();
     void                      selected_tiles_panel();
     void                      tile_conflicts_panel();
     void                      hovered_tiles_panel();
     void                      combo_pupu();
     void                      combo_palette();
     void                      combo_bpp();
     void                      checkbox_mim_palette_texture();
     void                      combo_field();
     void                      combo_coo();
     void                      combo_draw_bit();
     std::string               save_texture_path() const;
     void                      update_field();
     bool                      mim_test() const;
     bool                      map_test() const;
     void                      checkbox_map_swizzle();
     void                      checkbox_map_disable_blending();
     void                      menuitem_locate_ff8();
     void                      menuitem_save_swizzle_textures();
     void                      menuitem_save_deswizzle_textures();
     void                      menuitem_load_swizzle_textures();
     void                      menuitem_load_swizzle_textures2();
     void                      menuitem_load_deswizzle_textures();
     void                      menuitem_load_deswizzle_textures2();
     void                      menuitem_save_texture(bool enabled = true);
     void                      menuitem_save_mim_file(bool enabled = true);
     void                      menuitem_save_map_file(bool enabled = true);
     void                      menuitem_save_map_file_modified(bool enabled = true);
     void                      menuitem_load_map_file(bool enabled = true);
     // void                      scale_window(float width = {}, float height = {});
     std::uint8_t              palette() const;
     open_viii::graphics::BPPT bpp() const;
     void                      combo_blend_modes();
     void                      combo_layers();
     void                      combo_texture_pages();
     void                      combo_animation_ids();
     void                      combo_animation_frames();
     void                      combo_filtered_palettes();
     void                      combo_filtered_bpps();
     void                      combo_blend_other();
     void                      combo_z();
     std::shared_ptr<open_viii::archive::FIFLFS<false>> init_field();
     void                                               text_mouse_position() const;
     void                                               on_click_not_imgui();
     void                                               combo_upscale_path();
     bool                    combo_upscale_path(ff_8::filter_old<std::filesystem::path, ff_8::FilterTag::Upscale> &filter) const;
     void                    combo_deswizzle_path();
     bool                    combo_deswizzle_path(ff_8::filter_old<std::filesystem::path, ff_8::FilterTag::Deswizzle> &filter) const;
     void                    combo_upscale_map_path();
     bool                    combo_upscale_map_path(ff_8::filter_old<std::filesystem::path, ff_8::FilterTag::UpscaleMap> &filter) const;
     void                    combo_deswizzle_map_path();
     bool                    combo_deswizzle_map_path(ff_8::filter_old<std::filesystem::path, ff_8::FilterTag::DeswizzleMap> &filter) const;
     const open_viii::LangT &get_coo() const;
     file_dialog_mode        m_file_dialog_mode       = {};
     map_directory_mode      m_modified_directory_map = {};
     // std::filesystem::path    m_loaded_swizzle_texture_path{};
     std::filesystem::path   m_loaded_deswizzle_texture_path{};
     std::vector<std::size_t> m_clicked_tile_indices{};
     //     void                                               popup_batch_reswizzle();
     //     void                                               popup_batch_deswizzle();
     // static void              popup_batch_common_filter_start(
     //                ff_8::filter_old<std::filesystem::path> &filter,
     //                std::string_view                         prefix,
     //                std::string_view                         base_name);
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
     void open_locate_ff8_filebrowser();
     variant_tile_t                           &combo_selected_tile(bool &changed);
     [[nodiscard]] bool                        browse_for_image_display_preview();
     void                                      generate_upscale_paths();
     void                                      generate_deswizzle_paths();
     void                                      generate_upscale_map_paths();
     void                                      generate_deswizzle_map_paths();
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
     void                                      filter_empty_import_tiles();
     void                                      collapsing_header_generated_tiles() const;
     void adjust_source_xy_texture_page_for_import_map(uint8_t next_source_y, const uint8_t next_texture_page);
     void find_selected_tile_for_import(variant_tile_t &current_tile) const;
     void event_type_key_released(const sf::Event::KeyEvent &key);
     void event_type_key_pressed(const sf::Event::KeyEvent &key);
     void event_type_mouse_button_pressed(const sf::Mouse::Button &button);
     void event_type_mouse_button_released(const sf::Mouse::Button &button);
     void file_menu();
     void edit_menu();
     void windows_menu();
     void refresh_map_swizzle();
     void refresh_map_disable_blending();
     void refresh_draw_mode();
     void refresh_mim_palette_texture();
     bool change_background_color(const fme::color &);
     void save_background_color();
     void refresh_path();
     void browse_buttons();
     void menu_upscale_paths();
     void menu_deswizzle_paths();
     void menu_upscale_map_paths();
     void menu_deswizzle_map_paths();
     void refresh_field();
     void refresh_bpp(open_viii::graphics::BPPT in_bpp);
     void refresh_palette(std::uint8_t palette);
     void refresh_render_texture(bool reload_textures = false);
     void refresh_coo();
     [[nodiscard]] static std::optional<std::string> handle_path_deletion(std::vector<std::string> &paths_vector, std::ptrdiff_t offset);
     [[nodiscard]] static std::string
       find_replacement_path_value(const std::vector<std::string> &paths, const std::vector<bool> &paths_enabled);
     [[nodiscard]] static std::ptrdiff_t add_delete_button(const std::string &path, const std::vector<std::string> &paths);

     [[nodiscard]] static std::ptrdiff_t add_delete_button(const std::ptrdiff_t index);


     static inline constinit bool        toggle_imgui_demo_window = { false };

     struct map_pupu_id
     {
          std::shared_ptr<map_sprite> m_map_sprite = {};
          const auto                 &values() const
          {
               return m_map_sprite->working_unique_pupu();
          }
          auto strings() const
          {
               return m_map_sprite->working_unique_pupu() | std::views::transform(AsString{});
          }
          auto tooltips() const
          {
               return m_map_sprite->working_unique_pupu()
                      | std::views::transform([](const ff_8::PupuID &pupu_id) -> decltype(auto) { return pupu_id.create_summary(); });
          }
          auto zip() const
          {
               return std::ranges::views::zip(values(), strings(), tooltips());
          }
     };
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_GUI_HPP
