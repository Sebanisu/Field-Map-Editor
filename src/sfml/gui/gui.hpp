//
// Created by pcvii on 9/7/2021.
//

#ifndef FIELD_MAP_EDITOR_GUI_HPP
#define FIELD_MAP_EDITOR_GUI_HPP
#include "archives_group.hpp"
#include "batch_deswizzle.hpp"
#include "batch_embed.hpp"
#include "batch_reswizzle.hpp"
#include "Configuration.hpp"
#include "events.hpp"
#include "filebrowser.hpp"
#include "formatters.hpp"
#include "generic_combo.hpp"
#include "grid.hpp"
#include "map_dialog_mode.hpp"
#include "map_directory_mode.hpp"
#include "mouse_positions.hpp"
#include "safedir.hpp"
#include "scope_guard.hpp"
#include "scrolling.hpp"
#include "selections.hpp"
#include "upscales.hpp"
#include <cstdint>
#include <fmt/chrono.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics/RenderWindow.hpp>

#define USE_THREADS
namespace fme
{
struct gui
{
public:
  gui();
  void start() const;

private:
  inline static std::mutex                         append_results_mutex = {};
  inline static std::vector<std::filesystem::path> append_results       = {};
  mutable selections                               m_selections         = {};
  mutable scrolling                                m_scrolling          = {};
  mutable batch_deswizzle                          m_batch_deswizzle    = {};
  mutable batch_reswizzle                          m_batch_reswizzle    = {};
  mutable batch_embed                              m_batch_embed = batch_embed{
    "Operation 1: Find map, Replace map for each field, Save temp file.",
    [this](const int in_pos, const std::filesystem::path &in_selected_path) {
      if (in_pos <= 0)
      {
        append_results.clear();
      }
      launch_async(
        [this](const int pos, const std::filesystem::path selected_path) {
          auto field = m_archives_group.field(pos);
          if (!field)
          {
            return;
          }
          auto                   paths = find_maps_in_directory(selected_path);
          const auto             tmp   = replace_entries(*field, paths);

          const std::scoped_lock guard{ append_results_mutex };
          append_results.insert(
            std::ranges::end(append_results),
            std::ranges::begin(tmp),
            std::ranges::end(tmp));
        },
        in_pos,
        in_selected_path);
    },
    []() { return true; }
  };
  mutable batch_embed m_batch_embed2 = batch_embed{
    "Operation 2: Update fields archive, Remove Temporary Files",
    [this](int &pos, const std::filesystem::path &) {
      if (check_futures())
      {
        --pos;
        return;
      }
      [this]() {
        const auto &fields = m_archives_group.archives()
                               .get<open_viii::archive::ArchiveTypeT::field>();
        {
          // create a new fields archive with files replaced.
          const auto tmp = replace_entries(fields, append_results);
          std::ranges::for_each(append_results, [](const auto &path) {
            std::filesystem::remove(path);
          });
          // const std::scoped_lock guard{ append_results_mutex };
          append_results = tmp;
        }
      }();
    },
    []() {
      format_imgui_wrapped_text(
        "Waiting for smaller field archives to finish writing...");
      ImGui::Separator();
      return true;
    }
  };
  mutable batch_embed m_batch_embed3 =
    batch_embed{ "Operation 3: Embed fields in main.zzz (disabled)",
                 []([[maybe_unused]] int &pos, const std::filesystem::path &) {
                   // todo rewrite this code to work with new zzz.
                   //               // format_imgui_wrapped_text("{}", "Updating
                   //               main.zzz..."); format_imgui_wrapped_text(
                   //                 "{}", "Updating fields.fi, fields.fl, and
                   //                 fields.fs...");
                   //               if (check_futures())
                   //               {
                   //                 --pos;
                   //                 return;
                   //               }
                   //               wait_for_futures();
                   //               launch_async(
                   //                 [this]()
                   //                 {
                   //                   const open_viii::archive::ZZZ &zzzmain =
                   //                     m_archives_group.archives()
                   //                       .get<open_viii::archive::ArchiveTypeT::zzz_main>()
                   //                       .value();
                   //                   auto src = zzzmain.data();
                   //
                   //                   std::ranges::sort(
                   //                     src,
                   //                     [](
                   //                       const open_viii::archive::FileData
                   //                       &l, const
                   //                       open_viii::archive::FileData &r)
                   //                     { return std::cmp_less(l.offset(),
                   //                     r.offset()); });
                   //                   auto        dst  = src;// copy
                   //                   const auto &path = zzzmain.path();
                   //
                   //                   spdlog::info("Attempting to work:
                   //                   \"{}\"", path.string()); if
                   //                   (!any_matches(results, dst))
                   //                   {
                   //                     return;
                   //                   }
                   //                   // danger here there are files with the
                   //                   same filename. Should
                   //                   // be fine for fields because there is
                   //                   only one copy. auto transform_view =
                   //                     dst
                   //                     | std::views::transform(
                   //                       [](const
                   //                       open_viii::archive::FileData
                   //                       &file_data) {
                   //                         return std::ranges::size(
                   //                                  file_data.get_path_string_view())
                   //                                + 16U;
                   //                       });
                   //                   const auto header_size = std::reduce(
                   //                     transform_view.begin(),
                   //                     transform_view.end(),
                   //                     sizeof(std::uint32_t));
                   //                   std::ranges::transform(
                   //                     dst,
                   //                     std::ranges::begin(dst),
                   //                     [&, i = uint64_t{ header_size }](
                   //                       open_viii::archive::FileData
                   //                       file_data) mutable
                   //                     {
                   //                       if (auto match =
                   //                       open_viii::archive::find_match(
                   //                             results,
                   //                             file_data.get_path());
                   //                           match !=
                   //                           std::ranges::end(results))
                   //                       {
                   //                         file_data =
                   //                         file_data.with_uncompressed_size(
                   //                           static_cast<std::uint32_t>(
                   //                             std::filesystem::file_size(*match)));
                   //                       }
                   //                       file_data =
                   //                       file_data.with_offset(i); i +=
                   //                       file_data.uncompressed_size();
                   //                       return file_data;
                   //                     });
                   //
                   //                   const auto   &temp =
                   //                   std::filesystem::temp_directory_path();
                   //                   auto out_path = temp / path.filename();
                   //                   std::ofstream fs_mainzzz(
                   //                     out_path,
                   //                     std::ios::out | std::ios::binary |
                   //                     std::ios::trunc);
                   //                   spdlog::info("Creating: \"{}\"",
                   //                   out_path.string()); const auto count =
                   //                     std::bit_cast<std::array<char,
                   //                     sizeof(std::uint32_t)>>(
                   //                       static_cast<std::uint32_t>(std::ranges::size(dst)));
                   //                   fs_mainzzz.write(count.data(),
                   //                   count.size()); std::ranges::for_each(
                   //                     dst,
                   //                     [&](const open_viii::archive::FileData
                   //                     &file_data)
                   //                     {
                   //                       std::string filename =
                   //                       file_data.get_path_string();
                   //                       tl::string::undo_replace_slashes(filename);
                   //                       const auto filename_length =
                   //                         std::bit_cast<std::array<char,
                   //                         sizeof(std::uint32_t)>>(
                   //                           static_cast<std::uint32_t>(
                   //                             std::ranges::size(filename)));
                   //                       fs_mainzzz.write(
                   //                         filename_length.data(),
                   //                         filename_length.size());
                   //                       fs_mainzzz.write(
                   //                         filename.data(),
                   //                         static_cast<std::streamsize>(filename.size()));
                   //                       const auto offset =
                   //                         std::bit_cast<std::array<char,
                   //                         sizeof(std::uint64_t)>>(
                   //                           file_data.offset());
                   //                       fs_mainzzz.write(offset.data(),
                   //                       offset.size()); const auto
                   //                       uncompressed_size =
                   //                         std::bit_cast<std::array<char,
                   //                         sizeof(std::uint32_t)>>(
                   //                           file_data.uncompressed_size());
                   //                       fs_mainzzz.write(
                   //                         uncompressed_size.data(),
                   //                         uncompressed_size.size());
                   //                     });
                   //
                   //                   std::ifstream in_fs_mainzzz(
                   //                     path, std::ios::in |
                   //                     std::ios::binary);
                   //                   tl::read::input  input(&in_fs_mainzzz);
                   //
                   //                   std::scoped_lock guard{
                   //                   append_results_mutex
                   //                   }; std::ranges::for_each(
                   //                     src,
                   //                     [&](const open_viii::archive::FileData
                   //                     &file_data)
                   //                     {
                   //                       if (auto match =
                   //                       open_viii::archive::find_match(
                   //                             results,
                   //                             file_data.get_path());
                   //                           match !=
                   //                           std::ranges::end(results))
                   //                       {
                   //                         const auto buffer =
                   //                           open_viii::tools::read_entire_file(*match);
                   //                         fs_mainzzz.write(
                   //                           buffer.data(),
                   //                           static_cast<std::streamsize>(buffer.size()));
                   //                         return;
                   //                       }
                   //                       const auto buffer =
                   //                         open_viii::archive::FS::get_entry(input,
                   //                         file_data);
                   //                       fs_mainzzz.write(
                   //                         buffer.data(),
                   //                         static_cast<std::streamsize>(buffer.size()));
                   //                     });
                   //                   // make sure this code is ran.
                   //                   std::ranges::for_each(
                   //                     results,
                   //                     [](const auto &rem_path)
                   //                     { std::filesystem::remove(rem_path);
                   //                     });
                   //                   results.clear();
                   //                   results.push_back(out_path);
                   //                 });
                 },
                 []() {
                   //               format_imgui_wrapped_text(
                   //                 "{}", "{}", "Updating fields.fi,
                   //                 fields.fl, and fields.fs...");
                   //               return !check_futures();
                   return true;
                 } };
  mutable batch_embed m_batch_embed4 = batch_embed{
    "Operation 4: Save",
    [this](int &pos, const std::filesystem::path &in_selected_path) {
      format_imgui_wrapped_text("{}", "Saving Files...");
      if (check_futures())
      {
        --pos;
        return;
      }
      // launch_async(
      [this](const std::filesystem::path selected_path) {
        const auto move = [&](std::filesystem::path path) {
          const auto it = std::ranges::find_if(
            append_results, [&](const std::filesystem::path &tmp_path) {
              return tmp_path.filename() == path.filename();
            });
          if (it == append_results.end())
          {
            return;
          }
          std::error_code ec{};
          spdlog::info(
            "{}:{} - Creating directory: \"{}\"",
            __FILE__,
            __LINE__,
            path.parent_path().string());
          std::filesystem::create_directories(path.parent_path(), ec);
          if (ec)
          {
            spdlog::warn(
              "{}:{} - {}: {} - path: {}",
              __FILE__,
              __LINE__,
              ec.value(),
              ec.message(),
              path.parent_path().string());
            ec.clear();
          }
          if (const auto tmp = safedir(path).case_insensitive_exists();
              !tmp.empty())
          {
            path = tmp;
            // IF files exist rename to same path .bak
            spdlog::info(
              "{}:{} - Moving path: \"{}\" to \"{}\"",
              __FILE__,
              __LINE__,
              path.string(),
              path.string() + ".bak");
            std::filesystem::copy(
              path,
              path.string() + ".bak",
              std::filesystem::copy_options::overwrite_existing,
              ec);
            if (ec)
            {
              spdlog::warn(
                "{}:{} - {}: {} path: \"{}\" to \"{}\"",
                __FILE__,
                __LINE__,
                ec.value(),
                ec.message(),
                path.string(),
                path.string() + ".bak");
              ec.clear();
            }
          }
          spdlog::info(
            "{}:{} - Moving - path: \"{}\" to \"{}\"",
            __FILE__,
            __LINE__,
            it->string(),
            path);
          std::filesystem::copy(
            *it, path, std::filesystem::copy_options::overwrite_existing, ec);
          if (ec)
          {
            spdlog::warn(
              "{}:{} - {}: {} - path: {}",
              __FILE__,
              __LINE__,
              ec.value(),
              ec.message(),
              it->string());
            ec.clear();
          }
          std::filesystem::remove(*it, ec);
          if (ec)
          {
            spdlog::warn(
              "{}:{} - {}: {} - path: {}",
              __FILE__,
              __LINE__,
              ec.value(),
              ec.message(),
              it->string());
            ec.clear();
          }
          append_results.erase(it);
        };
        // I need to detect the path where the game files are then
        // save them there.
        if (!m_archives_group.archives())
        {
          const auto copy = append_results;// move removes matches.
          std::ranges::for_each(
            copy, [&](const std::filesystem::path &tmp_path) {
              move(
                std::filesystem::path(m_selections.path) / tmp_path.filename());
            });
        }
        else if (open_viii::archive::fiflfs_in_main_zzz(
                   m_archives_group.archives()))
        {
          const open_viii::archive::ZZZ &zzz_main =
            m_archives_group.archives()
              .get<open_viii::archive::ArchiveTypeT::zzz_main>()
              .value();
          const auto &path_zzz_main = zzz_main.path();
          const auto  copy          = append_results;// move removes matches.
          std::ranges::for_each(
            copy, [&](const std::filesystem::path &tmp_path) {
              if (path_zzz_main.filename() != tmp_path.filename())
              {
                move(
                  path_zzz_main.parent_path() / "DEMASTER_EXP" / "data"
                  / tmp_path.filename());
              }
            });
          move(path_zzz_main);// if main.zzz exists move it.
        }
        else
        {
          const open_viii::archive::FIFLFS<true> &fields =
            m_archives_group.archives()
              .get<open_viii::archive::ArchiveTypeT::field>();
          const auto &path_fi = fields.fi().path_or_nested_path();
          move(path_fi);
          const auto &path_fl = fields.fl().path_or_nested_path();
          move(path_fl);
          const auto &path_fs = fields.fs().path_or_nested_path();
          move(path_fs);
        }
        while (!append_results.empty())
        {
          // If I can't find the files just save to
          // the selected directory.
          move(selected_path.parent_path() / append_results.back().filename());
        }
      }(in_selected_path);
    },
    [this]() {
      if (check_futures())
      {
        const auto current    = std::chrono::high_resolution_clock::now();
        const auto difference = current - m_batch_embed4.start_time();
        format_imgui_wrapped_text("{:%H:%M:%S}", difference);
        if (open_viii::archive::fiflfs_in_main_zzz(m_archives_group.archives()))
        {
          format_imgui_wrapped_text(
            "{}", "Updating main.zzz...Finishing writing...");
        }
        else
        {
          format_imgui_wrapped_text(
            "{}",
            "Updating fields.fi, fields.fl, and fields.fs...Finishing "
            "writing...");
        }
        return false;
      }
      format_imgui_wrapped_text("{}", "Choose where to save the files.");
      std::ranges::for_each(
        append_results, [](const std::filesystem::path &path) {
          format_imgui_wrapped_text("\t\"{}\"", path);
        });
      format_imgui_wrapped_text(
        "{}", "(If files exist they will renamed filename.bak)");

      combo_path();
      ImGui::SameLine();
      if (ImGui::Button("Browse"))
      {
        file_browser_locate_ff8();
      }
      format_imgui_wrapped_text("This is where the files will be installed...");
      if (!m_archives_group.archives())
      {
        std::ranges::for_each(
          append_results, [this](const std::filesystem::path &path) {
            format_imgui_wrapped_text(
              "\t\"{}\"",
              (std::filesystem::path(m_selections.path) / path.filename())
                .string());
          });
      }
      else if (!open_viii::archive::fiflfs_in_main_zzz(
                 m_archives_group.archives()))
      {
        const open_viii::archive::FIFLFS<true> &fields =
          m_archives_group.archives()
            .get<open_viii::archive::ArchiveTypeT::field>();
        std::ranges::for_each(
          std::array{
            fields.fi().path(), fields.fl().path(), fields.fs().path() },
          [](const std::filesystem::path &path) {
            format_imgui_wrapped_text("\t\"{}\"", path.string());
          });
      }
      else
      {
        const open_viii::archive::ZZZ &zzz_main =
          m_archives_group.archives()
            .get<open_viii::archive::ArchiveTypeT::zzz_main>()
            .value();
        std::ranges::for_each(
          append_results, [&zzz_main](const std::filesystem::path &path) {
            format_imgui_wrapped_text(
              "\t\"{}\"",
              (zzz_main.path().parent_path() / "DEMASTER_EXP" / "data"
               / path.filename())
                .string());
          });
      }

      return ImGui::Button("Okay");
    }
  };
  mutable int                      m_id                                   = {};
  mutable mouse_positions          m_mouse_positions                      = {};
  mutable float                    m_scale_width                          = {};
  mutable sf::RenderWindow         m_window                               = {};
  mutable sf::Clock                m_delta_clock                          = {};
  mutable toml::array              m_paths                                = {};
  mutable toml::array              m_custom_upscale_paths                 = {};
  mutable archives_group           m_archives_group                       = {};
  mutable std::vector<std::string> m_upscale_paths                        = {};
  mutable std::shared_ptr<open_viii::archive::FIFLFS<false>> m_field      = {};
  mutable std::array<float, 2>                               xy           = {};
  mutable mim_sprite                                         m_mim_sprite = {};
  mutable map_sprite                                         m_map_sprite = {};
  mutable open_viii::graphics::background::Map import_image_map           = {};
  mutable std::string                          m_import_image_path        = {};

  mutable bool                                 m_changed         = { false };
  //  ImGuiStyle                  m_original_style  = {};
  mutable sf::Event                            m_event           = {};
  mutable bool                                 m_first           = { true };
  static constexpr std::array                  m_draw_selections = {
    open_viii::graphics::background::Mim::EXT,
    open_viii::graphics::background::Map::EXT
  };
  mutable sf::Vector2f       m_cam_pos = {};
  // create a file browser instances
  mutable ImGui::FileBrowser m_save_file_browser{
    static_cast<ImGuiFileBrowserFlags>(
      static_cast<std::uint32_t>(ImGuiFileBrowserFlags_EnterNewFilename)
      | static_cast<std::uint32_t>(ImGuiFileBrowserFlags_CreateNewDir))
  };
  mutable ImGui::FileBrowser m_load_file_browser{};
  mutable ImGui::FileBrowser m_directory_browser{
    static_cast<ImGuiFileBrowserFlags>(
      static_cast<std::uint32_t>(ImGuiFileBrowserFlags_SelectDirectory)
      | static_cast<std::uint32_t>(ImGuiFileBrowserFlags_CreateNewDir))
  };
  mutable sf::Texture       loaded_image_texture        = {};
  mutable sf::RenderTexture loaded_image_render_texture = {};
  mutable sf::Image         loaded_image_cpu            = {};
  static toml::array        get_paths();
  // imgui doesn't support std::string or std::string_view or
  // std::filesystem::path, only const char *
  archives_group            get_archives_group() const;
  sf::RenderWindow          get_render_window() const;
  void                      update_path() const;
  mim_sprite                get_mim_sprite() const;
  map_sprite                get_map_sprite() const;
  void                      init_and_get_style() const;
  void                      loop_events() const;
  void                      loop() const;
  bool                      combo_path() const;
  void                      combo_draw() const;
  void                      file_browser_save_texture() const;
  void                      file_browser_locate_ff8() const;
  void                      menu_bar() const;
  void                      slider_xy_sprite(auto &sprite) const;
  void                      combo_pupu() const;
  void                      combo_palette() const;
  void                      combo_bpp() const;
  void                      checkbox_mim_palette_texture() const;
  void                      combo_field() const;
  void                      combo_coo() const;
  void                      combo_draw_bit() const;
  std::string               save_texture_path() const;
  void                      update_field() const;
  bool                      mim_test() const;
  bool                      map_test() const;
  void                      checkbox_map_swizzle() const;
  void                      checkbox_map_disable_blending() const;
  void                      menuitem_locate_ff8() const;
  void                      menuitem_save_swizzle_textures() const;
  void                      menuitem_save_deswizzle_textures() const;
  void                      menuitem_load_swizzle_textures() const;
  void                      menuitem_load_deswizzle_textures() const;
  void
    menuitem_save_texture(const std::string &path, bool enabled = true) const;
  void
    menuitem_save_mim_file(const std::string &path, bool enabled = true) const;
  void
    menuitem_save_map_file(const std::string &path, bool enabled = true) const;
  void menuitem_save_map_file_modified(
    const std::string &path,
    bool               enabled = true) const;
  void
    menuitem_load_map_file(const std::string &path, bool enabled = true) const;
  void         scale_window(float width = {}, float height = {}) const;
  int          get_selected_field();
  std::uint8_t palette() const;
  open_viii::graphics::BPPT bpp() const;
  void                      combo_blend_modes() const;
  void                      combo_layers() const;
  void                      combo_texture_pages() const;
  void                      combo_animation_ids() const;
  void                      combo_animation_frames() const;
  void                      combo_filtered_palettes() const;
  void                      combo_filtered_bpps() const;
  void                      combo_blend_other() const;
  void                      combo_z() const;
  bool                      handle_mouse_cursor() const;
  std::shared_ptr<open_viii::archive::FIFLFS<false>> init_field();
  void text_mouse_position() const;
  void on_click_not_imgui() const;
  void combo_upscale_path() const;
  bool combo_upscale_path(::filter<std::filesystem::path> &filter) const;
  void combo_deswizzle_path() const;
  const open_viii::LangT       &get_coo() const;
  mutable map_dialog_mode       m_modified_map           = {};
  mutable map_directory_mode    m_modified_directory_map = {};
  mutable std::filesystem::path m_loaded_swizzle_texture_path{};
  mutable std::filesystem::path m_loaded_deswizzle_texture_path{};
  void        combo_compact_type(filter<compact_type> &) const;
  void        popup_batch_reswizzle() const;
  void        popup_batch_deswizzle() const;
  std::string starter_field() const;
  void        popup_batch_common_filter_start(
           ::filter<std::filesystem::path> &filter,
           std::string_view                 prefix,
           std::string_view                 base_name) const;
  template<
    typename batch_opT,
    typename filterT,
    typename askT,
    typename processT>
  void popup_batch_common(
    batch_opT &&batch_op,
    filterT   &&filter,
    askT      &&ask,
    processT  &&process) const;
  void popup_batch_embed() const;
  template<bool Nested = false>
  std::vector<std::filesystem::path> replace_entries(
    const open_viii::archive::FIFLFS<Nested> &field,
    const std::vector<std::filesystem::path> &paths) const;

  mutable std::vector<std::future<void>> m_futures = {};
  template<typename T, typename... argsT>
  void launch_async(T &&task, argsT &&...args) const
  {
#ifdef USE_THREADS
    m_futures.emplace_back(std::async(
      std::launch::async, std::forward<T>(task), std::forward<argsT>(args)...));
#undef USE_THREADS
#else
    task(std::forward<argsT>(args)...);
#endif
  }
  bool check_futures() const;
  void batch_ops_ask_menu() const;
  bool combo_upscale_path(
    std::filesystem::path &path,
    const std::string     &field_name,
    open_viii::LangT       coo) const;
  void               open_locate_ff8_filebrowser() const;
  void               open_swizzle_filebrowser() const;
  void               open_deswizzle_filebrowser() const;
  [[nodiscard]] auto PushPop() const
  {
    ImGui::PushID(++m_id);
    return scope_guard{ &ImGui::PopID };
  }
  void import_image_window() const;
  std::variant<
    std::monostate,
    open_viii::graphics::background::Tile1,
    open_viii::graphics::background::Tile2,
    open_viii::graphics::background::Tile3>    &
    combo_selected_tile(bool &changed) const;
  void collapsing_tile_info(
    const std::variant<
      std::monostate,
      open_viii::graphics::background::Tile1,
      open_viii::graphics::background::Tile2,
      open_viii::graphics::background::Tile3> &current_tile) const;
  [[nodiscard]] bool browse_for_image_display_preview() const;
  template<open_viii::graphics::background::is_tile tileT>
  [[nodiscard]] bool
    create_tile_button(const tileT &tile, sf::Vector2f image_size = {}) const
  {
    const auto *texture = m_map_sprite.get_texture(tile);
    if (texture == nullptr)
    {
      return false;
    }
    const auto draw_size = m_map_sprite.get_tile_draw_size();
    auto       src_tpw   = tileT::texture_page_width(tile.depth());
    const auto x         = [this, &tile, &src_tpw]() -> std::uint32_t {
      if (m_map_sprite.filter().upscale.enabled())
      {
        return 0;
      }
      return tile.texture_id() * src_tpw;
    }();
    const auto src_x = [&tile, &x, this]() -> std::uint32_t {
      if (m_map_sprite.filter().deswizzle.enabled())
        return static_cast<std::uint32_t>(tile.x());
      return tile.source_x() + x;
    }();
    const auto src_y = [&tile, this]() -> std::uint32_t {
      if (m_map_sprite.filter().deswizzle.enabled())
        return static_cast<std::uint32_t>(tile.y());
      return tile.source_y();
    }();
    sf::Sprite sprite(
      *texture,
      sf::IntRect(
        static_cast<int>(
          (static_cast<float>(src_x) / 16.F) * static_cast<float>(draw_size.x)),
        static_cast<int>(
          (static_cast<float>(src_y) / 16.F) * static_cast<float>(draw_size.y)),
        static_cast<int>(draw_size.x),
        static_cast<int>(draw_size.y)));
    if (image_size == sf::Vector2f{})
    {
      image_size =
        sf::Vector2f(ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight());
    }
    return ImGui::ImageButton(sprite, image_size, 0);
  }
  [[nodiscard]] bool combo_tile_size() const;
  void               generate_upscale_paths(
                  const std::string &field_name,
                  open_viii::LangT   coo) const;
  void        menuitem_locate_custom_upscale() const;
  void        open_locate_custom_upscale() const;
  toml::array get_custom_upscale_paths_vector();
  void        checkbox_render_imported_image() const;
  void        update_imported_render_texture() const;
  void        save_swizzle_textures() const;
  void        reset_imported_image() const;
  void        update_scaled_up_render_texture() const;
  void        browse_for_embed_map_dir() const;
  void        begin_batch_embed_map_warning_window() const;
  void        sort_paths() const;
  static std::vector<std::filesystem::path>
    find_maps_in_directory(std::filesystem::path src, size_t reserve = {});
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_GUI_HPP
