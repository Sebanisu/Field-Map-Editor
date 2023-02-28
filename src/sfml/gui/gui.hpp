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
#include "Selections.hpp"
#include "upscales.hpp"
#include <cstdint>
#include <fmt/chrono.h>
#include <GL/glew.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>

#define USE_THREADS
namespace fme
{
template<typename T>
concept is_enum = std::is_enum_v<T>;
template<typename T>
concept is_enum_or_integral = is_enum<T> || std::integral<T>;
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
     sf::Color                          clear_color          = sf::Color::Black;
     std::mutex                         append_results_mutex = {};
     std::vector<std::filesystem::path> append_results       = {};
     std::shared_ptr<sf::Shader>        m_drag_sprite_shader = {};
     Selections                         m_selections         = {};
     scrolling                          m_scrolling          = {};
     batch_deswizzle                    m_batch_deswizzle    = {};
     batch_reswizzle                    m_batch_reswizzle    = {};
     batch_embed                        m_batch_embed =
       batch_embed{ "Operation 1: Find map, Replace map for each field, Save temp file.",
                    [this](const int in_pos, const std::filesystem::path &in_selected_path) {
                         if (in_pos <= 0)
                         {
                              append_results.clear();
                         }
                         launch_async(
                           [this](const int pos, const std::filesystem::path &selected_path) -> void {
                                auto field = m_archives_group.field(pos);
                                if (!field)
                                {
                                     return;
                                }
                                auto                   paths = find_maps_in_directory(selected_path);
                                const auto             tmp   = replace_entries(*field, paths);

                                const std::scoped_lock guard{ append_results_mutex };
                                append_results.insert(std::ranges::end(append_results), std::ranges::begin(tmp), std::ranges::end(tmp));
                           },
                           in_pos,
                           in_selected_path);
                    },
                    []() { return true; } };
     batch_embed m_batch_embed2 =
       batch_embed{ "Operation 2: Update fields archive, Remove Temporary Files",
                    [this](int &, const std::filesystem::path &) {
//                         if (check_futures())
//                         {
//                              --pos;
//                              return;
//                         }
                         [this]() {
                              const auto &fields = m_archives_group.archives().get<open_viii::archive::ArchiveTypeT::field>();
                              {
                                   // create a new fields archive with files replaced.
                                   const auto tmp = replace_entries(fields, append_results);
                                   std::ranges::for_each(append_results, [](const auto &path) { std::filesystem::remove(path); });
                                   // const std::scoped_lock guard{ append_results_mutex };
                                   append_results = tmp;
                              }
                         }();
                    },
                    []() {
                         format_imgui_wrapped_text("Waiting for smaller field archives to finish writing...");
                         ImGui::Separator();
                         return true;
                    } };
     batch_embed m_batch_embed3 = batch_embed{ "Operation 3: Embed fields in main.zzz (disabled)",
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
                                                    //                       file_data)
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
     batch_embed m_batch_embed4 = batch_embed{
          "Operation 4: Save",
          [this](int &, const std::filesystem::path &in_selected_path) {
               format_imgui_wrapped_text("{}", "Saving Files...");
//               if (check_futures())
//               {
//                    --pos;
//                    return;
//               }
               // launch_async(
               [this](const std::filesystem::path &selected_path) {
                    const auto move = [&](std::filesystem::path path) {
                         const auto found = std::ranges::find_if(
                           append_results, [&](const std::filesystem::path &tmp_path) { return tmp_path.filename() == path.filename(); });
                         if (found == append_results.end())
                         {
                              return;
                         }
                         std::error_code error_code{};
                         spdlog::info("{}:{} - Creating directory: \"{}\"", __FILE__, __LINE__, path.parent_path().string());
                         std::filesystem::create_directories(path.parent_path(), error_code);
                         if (error_code)
                         {
                              spdlog::warn(
                                "{}:{} - {}: {} - path: {}",
                                __FILE__,
                                __LINE__,
                                error_code.value(),
                                error_code.message(),
                                path.parent_path().string());
                              error_code.clear();
                         }
                         if (const auto tmp = safedir(path).case_insensitive_exists(); !tmp.empty())
                         {
                              path = tmp;
                              // IF files exist rename to same path .bak
                              spdlog::info(
                                R"({}:{} - Moving path: "{}" to "{}")", __FILE__, __LINE__, path.string(), path.string() + ".bak");
                              std::filesystem::copy(
                                path, path.string() + ".bak", std::filesystem::copy_options::overwrite_existing, error_code);
                              if (error_code)
                              {
                                   spdlog::warn(
                                     R"({}:{} - {}: {} path: "{}" to "{}")",
                                     __FILE__,
                                     __LINE__,
                                     error_code.value(),
                                     error_code.message(),
                                     path.string(),
                                     path.string() + ".bak");
                                   error_code.clear();
                              }
                         }
                         spdlog::info(R"({}:{} - Moving - path: "{}" to "{}")", __FILE__, __LINE__, found->string(), path);
                         std::filesystem::copy(*found, path, std::filesystem::copy_options::overwrite_existing, error_code);
                         if (error_code)
                         {
                              spdlog::warn(
                                "{}:{} - {}: {} - path: {}", __FILE__, __LINE__, error_code.value(), error_code.message(), found->string());
                              error_code.clear();
                         }
                         std::filesystem::remove(*found, error_code);
                         if (error_code)
                         {
                              spdlog::warn(
                                "{}:{} - {}: {} - path: {}", __FILE__, __LINE__, error_code.value(), error_code.message(), found->string());
                              error_code.clear();
                         }
                         append_results.erase(found);
                    };
                    // I need to detect the path where the game files are then
                    // save them there.
                    if (!m_archives_group.archives())
                    {
                         const auto copy = append_results;// move removes matches.
                         std::ranges::for_each(copy, [&](const std::filesystem::path &tmp_path) {
                              move(std::filesystem::path(m_selections.path) / tmp_path.filename());
                         });
                    }
                    else if (open_viii::archive::fiflfs_in_main_zzz(m_archives_group.archives()))
                    {
                         const open_viii::archive::ZZZ &zzz_main =
                           m_archives_group.archives().get<open_viii::archive::ArchiveTypeT::zzz_main>().value();
                         const auto &path_zzz_main = zzz_main.path();
                         const auto  copy          = append_results;// move removes matches.
                         std::ranges::for_each(copy, [&](const std::filesystem::path &tmp_path) {
                              if (path_zzz_main.filename() != tmp_path.filename())
                              {
                                   move(path_zzz_main.parent_path() / "DEMASTER_EXP" / "data" / tmp_path.filename());
                              }
                         });
                         move(path_zzz_main);// if main.zzz exists move it.
                    }
                    else
                    {
                         const open_viii::archive::FIFLFS<true> &fields =
                           m_archives_group.archives().get<open_viii::archive::ArchiveTypeT::field>();
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
//               if (check_futures())
//               {
//                    const auto current    = std::chrono::high_resolution_clock::now();
//                    const auto difference = current - m_batch_embed4.start_time();
//                    format_imgui_wrapped_text("{:%H:%M:%S}", difference);
//                    if (open_viii::archive::fiflfs_in_main_zzz(m_archives_group.archives()))
//                    {
//                         format_imgui_wrapped_text("{}", "Updating main.zzz...Finishing writing...");
//                    }
//                    else
//                    {
//                         format_imgui_wrapped_text(
//                           "{}",
//                           "Updating fields.fi, fields.fl, and fields.fs...Finishing "
//                           "writing...");
//                    }
//                    return false;
//               }
               format_imgui_wrapped_text("{}", "Choose where to save the files.");
               std::ranges::for_each(
                 append_results, [](const std::filesystem::path &path) { format_imgui_wrapped_text("\t\"{}\"", path); });
               format_imgui_wrapped_text("{}", "(If files exist they will renamed filename.bak)");

               combo_path();
               ImGui::SameLine();
               if (ImGui::Button("Browse"))
               {
                    file_browser_locate_ff8();
               }
               format_imgui_wrapped_text("This is where the files will be installed...");
               if (!m_archives_group.archives())
               {
                    std::ranges::for_each(append_results, [this](const std::filesystem::path &path) {
                         format_imgui_wrapped_text("\t\"{}\"", (std::filesystem::path(m_selections.path) / path.filename()).string());
                    });
               }
               else if (!open_viii::archive::fiflfs_in_main_zzz(m_archives_group.archives()))
               {
                    const open_viii::archive::FIFLFS<true> &fields =
                      m_archives_group.archives().get<open_viii::archive::ArchiveTypeT::field>();
                    std::ranges::for_each(
                      std::array{ fields.fi().path(), fields.fl().path(), fields.fs().path() },
                      [](const std::filesystem::path &path) { format_imgui_wrapped_text("\t\"{}\"", path.string()); });
               }
               else
               {
                    const open_viii::archive::ZZZ &zzz_main =
                      m_archives_group.archives().get<open_viii::archive::ArchiveTypeT::zzz_main>().value();
                    std::ranges::for_each(append_results, [&zzz_main](const std::filesystem::path &path) {
                         format_imgui_wrapped_text(
                           "\t\"{}\"", (zzz_main.path().parent_path() / "DEMASTER_EXP" / "data" / path.filename()).string());
                    });
               }

               return ImGui::Button("Okay");
          }
     };

     static constexpr std::int8_t                       tile_size_px           = { 16 };
     static constexpr std::uint8_t                      tile_size_px_unsigned  = { 16U };
     mouse_positions                                    m_mouse_positions      = {};
     float                                              m_scale_width          = {};
     sf::RenderWindow                                   m_window               = {};
     sf::Clock                                          m_delta_clock          = {};
     toml::array                                        m_paths                = {};
     toml::array                                        m_custom_upscale_paths = {};
     archives_group                                     m_archives_group       = {};
     std::vector<std::string>                           m_upscale_paths        = {};
     std::shared_ptr<open_viii::archive::FIFLFS<false>> m_field                = {};
     std::array<float, 2>                               xy                     = {};
     mim_sprite                                         m_mim_sprite           = {};
     map_sprite                                         m_map_sprite           = {};
     open_viii::graphics::background::Map               import_image_map       = {};
     std::string                                        m_import_image_path    = {};

     float                                              saved_window_width     = {};
     float                                              saved_window_height    = {};

     bool                                               m_changed              = { false };
     //  ImGuiStyle                  m_original_style  = {};
     sf::Event                                          m_event                = {};
     static constexpr std::array                        m_draw_selections      = { open_viii::graphics::background::Mim::EXT,
                                                                                   open_viii::graphics::background::Map::EXT };
     sf::Vector2f                                       m_cam_pos              = {};
     // create a file browser instances
     ImGui::FileBrowser                                 m_save_file_browser{ static_cast<ImGuiFileBrowserFlags>(
       static_cast<std::uint32_t>(ImGuiFileBrowserFlags_EnterNewFilename)
       | static_cast<std::uint32_t>(ImGuiFileBrowserFlags_CreateNewDir)) };
     ImGui::FileBrowser                                 m_load_file_browser{};
     ImGui::FileBrowser                                 m_directory_browser{ static_cast<ImGuiFileBrowserFlags>(
       static_cast<std::uint32_t>(ImGuiFileBrowserFlags_SelectDirectory)
       | static_cast<std::uint32_t>(ImGuiFileBrowserFlags_CreateNewDir)) };
     sf::Texture                                        loaded_image_texture        = {};
     sf::RenderTexture                                  loaded_image_render_texture = {};
     sf::Image                                          loaded_image_cpu            = {};
     static toml::array                                 get_paths();
     // imgui doesn't support std::string or std::string_view or
     // std::filesystem::path, only const char *
     archives_group                                     get_archives_group() const;
     sf::RenderWindow                                   get_render_window() const;
     void                                               update_path();
     mim_sprite                                         get_mim_sprite() const;
     map_sprite                                         get_map_sprite() const;
     void                                               init_and_get_style();
     void                                               loop_events();
     void                                               loop();
     bool                                               combo_path();
     void                                               combo_draw();
     void                                               file_browser_save_texture();
     void                                               file_browser_locate_ff8();
     void                                               menu_bar();
     void                                               combo_pupu();
     void                                               combo_palette();
     void                                               combo_bpp();
     void                                               checkbox_mim_palette_texture();
     void                                               combo_field();
     void                                               combo_coo();
     void                                               combo_draw_bit();
     std::string                                        save_texture_path() const;
     void                                               update_field();
     bool                                               mim_test() const;
     bool                                               map_test() const;
     void                                               checkbox_map_swizzle();
     void                                               checkbox_map_disable_blending();
     void                                               menuitem_locate_ff8();
     void                                               menuitem_save_swizzle_textures();
     void                                               menuitem_save_deswizzle_textures();
     void                                               menuitem_load_swizzle_textures();
     void                                               menuitem_load_deswizzle_textures();
     void                                               menuitem_save_texture(const std::string &path, bool enabled = true);
     void                                               menuitem_save_mim_file(const std::string &path, bool enabled = true);
     void                                               menuitem_save_map_file(const std::string &path, bool enabled = true);
     void                                               menuitem_save_map_file_modified(const std::string &path, bool enabled = true);
     void                                               menuitem_load_map_file(const std::string &path, bool enabled = true);
     void                                               scale_window(float width = {}, float height = {});
     int                                                get_selected_field();
     std::uint8_t                                       palette() const;
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
     map_dialog_mode                                    m_modified_map           = {};
     map_directory_mode                                 m_modified_directory_map = {};
     std::filesystem::path                              m_loaded_swizzle_texture_path{};
     std::filesystem::path                              m_loaded_deswizzle_texture_path{};
     void                                               combo_compact_type(ff_8::filter_old<compact_type> &) const;
     void                                               popup_batch_reswizzle();
     void                                               popup_batch_deswizzle();

     template<is_enum_or_integral number_type, is_enum_or_integral... rest_number_type>
     static constexpr number_type bitwise_or(number_type first, rest_number_type... rest)
     {
          return static_cast<number_type>(static_cast<std::uint32_t>(first) | (static_cast<std::uint32_t>(rest) | ...));
     }
     template<is_enum_or_integral number_type, is_enum_or_integral... rest_number_type>
     static constexpr number_type bitwise_and(number_type start, rest_number_type... rest)
     {
          return static_cast<number_type>(static_cast<std::uint32_t>(start) & (static_cast<std::uint32_t>(rest) & ...));
     }
     template<is_enum_or_integral number_type>
     static constexpr number_type bitwise_not(number_type value)
     {
          return static_cast<number_type>(~static_cast<std::uint32_t>(value));
     }
     static constexpr ImU32 imgui_color32(open_viii::struct_of_color32_byte auto color)
     {
          return imgui_color32(color.r, color.g, color.b, color.a);
     }
     static constexpr ImU32 imgui_color32(open_viii::Color auto color)
     {
          return imgui_color32(color.r(), color.b(), color.b(), color.a());
     }
     static constexpr ImU32 imgui_color32(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha)
     {
          constexpr auto R_SHIFT = 0U;
          constexpr auto G_SHIFT = 8U;
          constexpr auto B_SHIFT = 16U;
          constexpr auto A_SHIFT = 24U;
          return bitwise_or(ImU32{}, alpha << A_SHIFT, blue << B_SHIFT, green << G_SHIFT, red << R_SHIFT);
     }
     static std::string starter_field();
     static void        popup_batch_common_filter_start(
              ff_8::filter_old<std::filesystem::path> &filter,
              std::string_view                         prefix,
              std::string_view                         base_name);
     void popup_batch_embed();
     template<bool Nested = false>
     std::vector<std::filesystem::path>
       replace_entries(const open_viii::archive::FIFLFS<Nested> &field, const std::vector<std::filesystem::path> &paths) const;

     template<typename T, typename... argsT>
     void launch_async(T &&task, argsT &&...args)
     {
          std::invoke(std::forward<T>(task), std::forward<argsT>(args)...);
     }
     void                      batch_ops_ask_menu() const;
     bool                      combo_upscale_path(std::filesystem::path &path, const std::string &field_name, open_viii::LangT coo) const;
     void                      open_locate_ff8_filebrowser();
     void                      open_swizzle_filebrowser();
     void                      open_deswizzle_filebrowser();
     [[nodiscard]] static int &get_imgui_id()
     {
          static int imgui_id = {};
          return imgui_id;
     }
     [[nodiscard]] static auto PushPop()
     {
          ImGui::PushID(++get_imgui_id());
          return scope_guard{ &ImGui::PopID };
     }
     void               import_image_window();
     variant_tile_t    &combo_selected_tile(bool &changed);
     void               collapsing_tile_info(const variant_tile_t &current_tile) const;
     [[nodiscard]] bool browse_for_image_display_preview();
     template<open_viii::graphics::background::is_tile tileT>
     [[nodiscard]] bool create_tile_button(const tileT &tile, sf::Vector2f image_size = {}) const
     {
          const auto *texture = m_map_sprite.get_texture(tile);
          if (texture == nullptr)
          {
               return false;
          }
          const auto tile_texture_size = m_map_sprite.get_tile_texture_size(texture);
          const auto src_x             = [&tile, this]() -> std::uint32_t {
               if (m_map_sprite.filter().deswizzle.enabled())
               {
                    return static_cast<std::uint32_t>(tile.x());
               }
               auto       source_texture_page_width = tileT::texture_page_width(tile.depth());
               const auto texture_page_x_offset     = [this, &tile, &source_texture_page_width]() -> std::uint32_t {
                    if (m_map_sprite.filter().upscale.enabled())
                    {
                         return 0;
                    }
                    return tile.texture_id() * source_texture_page_width;
               }();
               return tile.source_x() + texture_page_x_offset;
          }();
          const auto src_y = [&tile, this]() -> std::uint32_t {
               if (m_map_sprite.filter().deswizzle.enabled())
               {
                    return static_cast<std::uint32_t>(tile.y());
               }
               return tile.source_y();
          }();
          static constexpr float tile_size = 16.F;
          sf::Sprite             sprite(
            *texture,
            sf::IntRect(
              static_cast<int>((static_cast<float>(src_x) / tile_size) * static_cast<float>(tile_texture_size.x)),
              static_cast<int>((static_cast<float>(src_y) / tile_size) * static_cast<float>(tile_texture_size.y)),
              static_cast<int>(tile_texture_size.x),
              static_cast<int>(tile_texture_size.y)));
          if (image_size == sf::Vector2f{})
          {
               image_size = sf::Vector2f(ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight());
          }
          return ImGui::ImageButton(sprite, image_size, 0);
     }
     [[nodiscard]] bool                        combo_tile_size();
     void                                      generate_upscale_paths(const std::string &field_name, open_viii::LangT coo);
     void                                      menuitem_locate_custom_upscale();
     void                                      open_locate_custom_upscale();
     static toml::array                        get_custom_upscale_paths_vector();
     void                                      checkbox_render_imported_image();
     void                                      update_imported_render_texture();
     void                                      save_swizzle_textures();
     void                                      reset_imported_image();
     void                                      update_scaled_up_render_texture();
     void                                      browse_for_embed_map_dir();
     void                                      begin_batch_embed_map_warning_window();
     void                                      sort_paths();
     void                                      control_panel_window();
     std::uint32_t                             image_height() const;
     static std::vector<std::filesystem::path> find_maps_in_directory(const std::filesystem::path &src, size_t reserve = {});

     void                                      slider_xy_sprite(const auto &sprite)
     {
          format_imgui_text("X: {:>9.3f} px  Width:  {:>4} px", -std::abs(m_cam_pos.x), sprite.width());
          format_imgui_text("Y: {:>9.3f} px  Height: {:>4} px", -std::abs(m_cam_pos.y), sprite.height());
          if (ImGui::SliderFloat2("Adjust", xy.data(), -1.0F, 0.0F) || m_changed)
          {
               move_camera(sprite);
          }
     }
     template<typename sprite_t>
     void move_camera(const sprite_t &sprite)
     {
          m_cam_pos = { -xy[0] * (static_cast<float>(sprite.width()) - m_scale_width), -xy[1] * static_cast<float>(sprite.height()) };
          m_changed = true;
          scale_window();
     }
     float                 scaled_menubar_gap() const;
     static void           render_dockspace();
     void                  background_color_picker();
     void                  collapsing_header_filters();
     void                  compact_flatten_buttons();
     static void           frame_rate();
     void                  control_panel_window_map();
     void                  control_panel_window_mim();
     std::filesystem::path path_with_prefix_and_base_name(std::filesystem::path selected_path) const;
     std::string           appends_prefix_base_name(std::string_view title) const;
     template<typename batch_opT, typename filterT, typename askT, typename processT>
     void popup_batch_common(batch_opT &&batch_op, filterT &&filter, askT &&ask, processT &&process) const
     {
          if (batch_op(
                m_archives_group.mapdata(),
                [&](const int &pos, std::filesystem::path selected_path, ff_8::filters filters, auto &&...rest) {
                     const auto field = m_archives_group.field(pos);
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

                     auto map = m_map_sprite.with_field(field,open_viii::LangT::generic).with_filters(filters);
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
                     if (std::filesystem::create_directories(selected_path))
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
     void generate_map_for_imported_image(const variant_tile_t &current_tile, bool changed);
     void collapsing_header_generated_tiles() const;
     void adjust_source_xy_texture_page_for_import_map(uint8_t next_source_y, const uint8_t next_texture_page);
     template<typename tiles_t>
     std::pair<std::uint8_t, std::uint8_t> get_next_unused_y_and_texture_page(const tiles_t &tiles) const
     {
          const auto max_texture_id_tile = (std::ranges::max)(tiles, {}, [](const auto &tile) { return tile.texture_id(); });
          const auto max_source_y_tile   = (std::ranges::max)(
            tiles | std::ranges::views::filter([&max_texture_id_tile](const auto &tile) {
                 return tile.texture_id() == max_texture_id_tile.texture_id();
            }),
            {},
            [](const auto &tile) { return tile.source_y(); });
          int const tile_y = max_source_y_tile.source_y() / tile_size_px;
          format_imgui_text("Last Used Texture Page {}, and Source Y / 16 = {}", max_texture_id_tile.texture_id(), tile_y);
          const auto         next_source_y = static_cast<uint8_t>((tile_y + 1) % tile_size_px);
          const std::uint8_t next_texture_page =
            tile_y + 1 == tile_size_px ? max_texture_id_tile.texture_id() + 1 : max_texture_id_tile.texture_id();
          return { next_source_y, next_texture_page };
     }
     void find_selected_tile_for_import(variant_tile_t &current_tile, std::string &current_item_str) const;
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
