//
// Created by pcvii on 9/7/2021.
//

#ifndef MYPROJECT_GUI_HPP
#define MYPROJECT_GUI_HPP
#include "archives_group.hpp"
#include "events.hpp"
#include "filebrowser.hpp"
#include "grid.hpp"
#include <cstdint>
#include <SFML/Graphics/RenderWindow.hpp>
struct gui
{
public:
  gui(std::uint32_t width, std::uint32_t height, const char *title);
  explicit gui(const char *title);
  gui();
  void
    start() const;

private:
  static constexpr std::uint32_t    default_window_width  = 800U;
  static constexpr std::uint32_t    default_window_height = 600U;

  mutable int                       m_id                  = {};
  std::uint32_t                     m_window_width  = { default_window_width };
  mutable float                     m_scale_width   = {};
  std::uint32_t                     m_window_height = { default_window_height };
  const char                       *m_title         = {};
  mutable sf::RenderWindow          m_window        = {};
  mutable sf::Clock                 m_delta_clock   = {};
  mutable std::vector<std::string>  m_paths         = {};
  mutable std::vector<const char *> m_paths_c_str   = {};
  mutable archives_group            m_archives_group                 = {};
  mutable int                       m_selected_field                 = {};
  mutable std::shared_ptr<open_viii::archive::FIFLFS<false>> m_field = {};
  mutable std::array<float, 2>                               xy      = {};
  mutable int                 m_selected_bpp                         = {};
  mutable int                 m_selected_palette                     = {};
  mutable int                 m_selected_coo                         = {};
  mutable int                 m_selected_path                        = {};
  mutable bool                m_draw_palette           = { false };
  mutable mim_sprite          m_mim_sprite             = {};
  mutable bool                m_map_swizzle            = { false };
  mutable map_sprite          m_map_sprite             = {};
  mutable int                 m_selected_draw          = { 1 };
  mutable bool                m_changed                = { false };
  ImGuiStyle                  m_original_style         = {};
  mutable sf::Event           m_event                  = {};
  mutable bool                m_first                  = { true };
  static constexpr std::array m_draw_selections        = { "MIM", "MAP" };
  mutable bool                m_draw_grid              = { false };
  mutable bool                m_draw_texture_page_grid = { false };
  mutable sf::Vector2f        m_cam_pos                = {};
  // create a file browser instances
  mutable ImGui::FileBrowser  m_save_file_browser{
    static_cast<ImGuiFileBrowserFlags>(
      static_cast<std::uint32_t>(ImGuiFileBrowserFlags_EnterNewFilename)
      | static_cast<std::uint32_t>(ImGuiFileBrowserFlags_CreateNewDir))
  };
  mutable ImGui::FileBrowser m_directory_browser{
    ImGuiFileBrowserFlags_SelectDirectory
  };
  static std::vector<std::string>
    get_paths();
  // imgui doesn't support std::string or std::string_view or
  // std::filesystem::path, only const char *
  std::vector<const char *>
    get_paths_c_str() const;
  archives_group
    get_archives_group() const;
  sf::RenderWindow
    get_render_window() const;
  void
    update_path() const;
  mim_sprite
    get_mim_sprite() const;
  map_sprite
    get_map_sprite() const;
  ImGuiStyle
    init_and_get_style() const;
  void
    loop_events() const;
  void
    loop() const;
  void
    combo_path() const;
  void
    combo_draw() const;
  void
    menuitem_locate_ff8() const;
  void
    file_browser_save_texture() const;
  void
    file_browser_locate_ff8() const;
  void
    menu_bar() const;
  void
    slider_xy_sprite(auto &sprite) const;
  void
    combo_palette() const;
  void
    combo_bpp() const;
  void
    checkbox_mim_palette_texture() const;
  void
    combo_field() const;
  void
    combo_coo() const;
  std::string
    save_texture_path() const;
  void
    update_field() const;
  bool
    mim_test() const;
  bool
    map_test() const;
  void
    checkbox_map_swizzle() const;
  void
    menuitem_save_texture(const std::string &path, bool b) const;
  void
    menuitem_save_mim_file(const std::string &path, bool disable) const;
  void
    menuitem_save_map_file(const std::string &path, bool disable) const;
  void
    scale_window(float width = {}, float height = {}) const;
  int
    get_selected_field();
  std::uint8_t
    palette() const;
  open_viii::graphics::BPPT
    bpp() const;
  void
    combo_blend_modes() const;
  void
    combo_layers() const;
  void
    combo_texture_pages() const;
  void
    combo_animation_ids() const;
  void
    combo_animation_frames() const;
  void
    combo_filtered_palettes() const;
  void
    combo_filtered_bpps() const;
  void
    combo_blend_other() const;
  void
    combo_z() const;
};
#endif// MYPROJECT_GUI_HPP
