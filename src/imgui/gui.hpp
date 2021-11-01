//
// Created by pcvii on 9/7/2021.
//

#ifndef MYPROJECT_GUI_HPP
#define MYPROJECT_GUI_HPP
#include "archives_group.hpp"
#include "events.hpp"
#include "filebrowser.hpp"
#include "grid.hpp"
#include "upscales.hpp"
#include <cstdint>
#include <fmt/chrono.h>
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
  enum struct map_dialog_mode
  {
    save_unmodified,
    save_modified,
    load
  };
  enum struct map_directory_mode
  {
    ff8_install_directory,
    save_swizzle_textures,
    save_deswizzle_textures,
    load_swizzle_textures,
    load_deswizzle_textures,
    batch_save_deswizzle_textures,
    batch_load_deswizzle_textures,
    batch_save_swizzle_textures,
  };
  struct batch_deswizzle
  {
    void
      enable(std::filesystem::path in_outgoing);
    void
      disable();
    template<typename lambdaT, typename askT>
    bool
      operator()(const std::vector<std::string> &fields,
        lambdaT                                &&lambda,
        askT                                   &&ask_lambda);

  private:
    bool                  enabled  = { false };
    std::size_t           pos      = {};
    std::filesystem::path outgoing = {};
    bool                  asked    = { false };
    ::filters             filters  = {};
    std::chrono::time_point<std::chrono::high_resolution_clock> start = {};
  };
  struct batch_reswizzle
  {
    void
      enable(std::filesystem::path in_incoming,
        std::filesystem::path      in_outgoing)
    {
      enabled = true;
      pos     = 0;
      filters.deswizzle.update(std::move(in_incoming)).enable();
      outgoing = std::move(in_outgoing);
      asked    = true;// disable asking.
      start    = std::chrono::high_resolution_clock::now();
    }
    void
      disable()
    {
      enabled = false;
    }
    template<typename lambdaT, typename askT>
    bool
      operator()(const std::vector<std::string> &fields,
        lambdaT                                &&lambda,
        [[maybe_unused]] askT                  &&ask_lambda)
    {
      if (!enabled)
      {
        return false;
      }
      const char *title = "Batch saving swizzle textures...";
      ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
        ImGuiCond_Always,
        ImVec2(0.5F, 0.5F));
      ImGui::OpenPopup(title);
      if (ImGui::BeginPopupModal(title,
            nullptr,
            ImGuiWindowFlags_AlwaysAutoResize
              | ImGuiWindowFlags_NoSavedSettings))
      {
        if (fields.size() <= pos)
        {
          auto current = std::chrono::high_resolution_clock::now();
          fmt::print(
            "{:%H:%M:%S} - Finished the batch swizzle...\n", current - start);
          disable();
          return pos > 0U;
        }
        if (!asked)
        {
          // asked = ask_lambda(filters.upscale);
        }
        else
        {
          auto current = std::chrono::high_resolution_clock::now();
          format_imgui_text("{:%H:%M:%S} - {:>3.2f}% - Processing {}...",
            current - start,
            static_cast<float>(pos) * 100.F
              / static_cast<float>(std::size(fields)),
            fields[pos]);
          ImGui::Separator();
          lambda(static_cast<int>(pos), outgoing, filters);
          ++pos;
        }
        ImGui::EndPopup();
      }
      return false;
    }

  private:
    bool                  enabled  = { false };
    std::size_t           pos      = {};
    std::filesystem::path outgoing = {};
    bool                  asked    = { false };
    ::filters             filters  = {};
    std::chrono::time_point<std::chrono::high_resolution_clock> start = {};
  };
  struct mouse_positions
  {
    sf::Vector2i pixel         = {};
    sf::Vector2i tile          = {};
    std::uint8_t texture_page  = {};
    bool         left          = { false };
    bool         mouse_enabled = { false };
    bool         mouse_moved   = { false };
    sf::Sprite   sprite        = {};
    std::uint8_t max_tile_x    = {};
    // sf::Sprite   cover         = {};
    void
      update()
    {
      old_left    = left;
      mouse_moved = false;
    }
    [[nodiscard]] bool
      left_changed() const
    {
      const auto condition = old_left != left;
      if (!mouse_enabled && condition)
      {
        std::cout << "Warning! mouse up off screen!" << std::endl;
      }
      return condition;
    }
    void
      update_sprite_pos(bool swizzle, float spacing = 256.F)
    {
      float x = {};
      if (swizzle && max_tile_x != 0U)
      {
        x = ((std::min)(static_cast<std::uint8_t>(tile.x), max_tile_x) * 16.F)
            + (texture_page * spacing);
      }
      else
      {
        x = (static_cast<float>(pixel.x / 16)) * 16.F;
      }
      float y = (static_cast<float>(pixel.y / 16)) * 16.F;
      sprite.setPosition(x, y);
    }

  private:
    bool old_left = { false };
  };
  struct scrolling
  {
    std::array<float, 2U> total_scroll_time = { 1000.F, 1000.F };
    bool                  left{};
    bool                  right{};
    bool                  up{};
    bool                  down{};
    void
      reset() noexcept
    {
      left = right = up = down = false;
    };
    bool
      scroll(std::array<float, 2U> &in_xy, const sf::Time &time)
    {
      bool changed = false;
      if (!(left || right || up || down))
      {
        return changed;
      }
      const auto time_ms = static_cast<float>(time.asMicroseconds()) / 1000.F;
      if (left && right)
      {
      }
      else
      {
        float total_time = (in_xy[0] + 1.F) * total_scroll_time[0];
        //        fmt::print("{:.2f} = ({:.2f} + 1.00) * {:.2f}\n",
        //          total_time,
        //          in_xy[0],
        //          total_scroll_time[0]);
        if (left)
        {
          in_xy[0] = std::lerp(-1.F,
            0.F,
            std::clamp(
              (total_time + time_ms) / total_scroll_time[0], 0.F, 1.F));
          changed  = true;
        }
        else if (right)
        {
          in_xy[0] = std::lerp(-1.F,
            0.F,
            std::clamp(
              (total_time - time_ms) / total_scroll_time[0], 0.F, 1.F));
          changed  = true;
        }
      }
      if (up && down)
      {
      }
      else
      {
        float total_time = (in_xy[1] + 1.F) * total_scroll_time[1];
        if (up)
        {
          in_xy[1] = std::lerp(-1.F,
            0.F,
            std::clamp(
              (total_time + time_ms) / total_scroll_time[1], 0.F, 1.F));
          changed  = true;
        }
        else if (down)
        {
          in_xy[1] = std::lerp(-1.F,
            0.F,
            std::clamp(
              (total_time - time_ms) / total_scroll_time[1], 0.F, 1.F));
          changed  = true;
        }
      }
      return changed;
    }
  };
  struct selections
  {
    int  bpp                    = {};
    int  palette                = {};
    int  field                  = {};
    int  coo                    = {};
    int  path                   = {};
    int  draw                   = { 1 };
    bool draw_palette           = { false };
    bool draw_grid              = { false };
    bool draw_texture_page_grid = { false };
    bool draw_swizzle           = { false };
  };
  static constexpr std::uint32_t    default_window_width  = 800U;
  static constexpr std::uint32_t    default_window_height = 600U;
  mutable scrolling                 m_scrolling           = {};
  mutable batch_deswizzle           m_batch_deswizzle     = {};
  mutable batch_reswizzle           m_batch_reswizzle     = {};
  mutable int                       m_id                  = {};
  mutable mouse_positions           m_mouse_positions     = {};
  mutable selections                m_selections          = {};
  std::uint32_t                     m_window_width  = { default_window_width };
  mutable float                     m_scale_width   = {};
  std::uint32_t                     m_window_height = { default_window_height };
  const char                       *m_title         = {};
  mutable sf::RenderWindow          m_window        = {};
  mutable sf::Clock                 m_delta_clock   = {};
  mutable std::vector<std::string>  m_paths         = {};
  mutable std::vector<const char *> m_paths_c_str   = {};
  mutable archives_group            m_archives_group                      = {};
  mutable std::shared_ptr<open_viii::archive::FIFLFS<false>> m_field      = {};
  mutable std::array<float, 2>                               xy           = {};
  mutable mim_sprite                                         m_mim_sprite = {};
  mutable map_sprite                                         m_map_sprite = {};
  mutable bool                m_changed         = { false };
  ImGuiStyle                  m_original_style  = {};
  mutable sf::Event           m_event           = {};
  mutable bool                m_first           = { true };
  static constexpr std::array m_draw_selections = { "MIM", "MAP" };
  mutable sf::Vector2f        m_cam_pos         = {};
  // create a file browser instances
  mutable ImGui::FileBrowser  m_save_file_browser{
    static_cast<ImGuiFileBrowserFlags>(
      static_cast<std::uint32_t>(ImGuiFileBrowserFlags_EnterNewFilename)
      | static_cast<std::uint32_t>(ImGuiFileBrowserFlags_CreateNewDir))
  };
  mutable ImGui::FileBrowser m_directory_browser{
    static_cast<ImGuiFileBrowserFlags>(
      static_cast<std::uint32_t>(ImGuiFileBrowserFlags_SelectDirectory)
      | static_cast<std::uint32_t>(ImGuiFileBrowserFlags_CreateNewDir))
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
    file_browser_save_texture() const;
  void
    file_browser_locate_ff8() const;
  void
    menu_bar() const;
  void
    slider_xy_sprite(auto &sprite) const;
  void
    combo_pupu() const;
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
    menuitem_locate_ff8() const;
  void
    menuitem_save_swizzle_textures() const;
  void
    menuitem_save_deswizzle_textures() const;
  void
    menuitem_load_swizzle_textures() const;
  void
    menuitem_load_deswizzle_textures() const;
  void
    menuitem_save_texture(const std::string &path, bool enabled = true) const;
  void
    menuitem_save_mim_file(const std::string &path, bool enabled = true) const;
  void
    menuitem_save_map_file(const std::string &path, bool enabled = true) const;
  void
    menuitem_save_map_file_modified(const std::string &path,
      bool                                             enabled = true) const;
  void
    menuitem_load_map_file(const std::string &path, bool enabled = true) const;
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
  bool
    handle_mouse_cursor() const;
  std::shared_ptr<open_viii::archive::FIFLFS<false>>
    init_field();
  void
    text_mouse_position() const;
  void
    on_click_not_imgui() const;
  void
    combo_upscale_path() const;
  bool
    combo_upscale_path(::filter<std::filesystem::path> &filter,
      const std::string                                &field_name,
      open_viii::LangT                                  coo = {}) const;
  void
    combo_deswizzle_path() const;
  const open_viii::LangT                             &
    get_coo() const;
  mutable map_dialog_mode       m_modified_map           = {};
  mutable map_directory_mode    m_modified_directory_map = {};
  mutable std::filesystem::path m_loaded_swizzle_texture_path{};
  mutable std::filesystem::path m_loaded_deswizzle_texture_path{};
};
#endif// MYPROJECT_GUI_HPP
