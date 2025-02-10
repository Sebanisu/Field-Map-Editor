//
// Created by pcvii on 12/21/2022.
//
#ifndef FIELD_MAP_EDITOR_SELECTIONS_HPP
#define FIELD_MAP_EDITOR_SELECTIONS_HPP
#include "colors.hpp"
#include "Configuration.hpp"
#include "draw_mode.hpp"
#include "tile_sizes.hpp"
#include <filesystem>
#include <open_viii/graphics/background/BlendModeT.hpp>
#include <open_viii/graphics/BPPT.hpp>
#include <open_viii/strings/LangT.hpp>
#include <spdlog/spdlog.h>
namespace fme
{
/**
 * @brief Manages various settings and selections for the application.
 *
 * This struct contains configuration options and runtime selections, such as window dimensions,
 * drawing modes, selected tiles, and rendering options.
 */
struct Selections
{
     /**
      * @brief The path to the current working directory.
      *
      * Initializes to the current working directory on construction. If an error occurs while
      * determining the path, it logs a warning using spdlog.
      */
     std::string path = []() {
          std::error_code error_code = {};
          std::string     str        = std::filesystem::current_path(error_code).string();
          if (error_code)
          {
               spdlog::warn("{}:{} - {}: {} path: \"{}\"", __FILE__, __LINE__, error_code.value(), error_code.message(), str);
               error_code.clear();
          }
          return str;
     }();

     /**
      * @brief The default window width.
      */
     static constexpr std::uint32_t window_width_default           = 800;

     /**
      * @brief The default window height.
      */
     static constexpr std::uint32_t window_height_default          = 600;

     open_viii::graphics::BPPT      bpp                            = {};///< Bits per pixel for rendering.
     uint8_t                        palette                        = {};///< Selected palette index.
     int                            field                          = {};///< Selected field ID.
     open_viii::LangT               coo                            = {};///< Selected language.
     draw_mode                      draw                           = { draw_mode::draw_map };///< Current drawing mode.
     int                            selected_tile                  = { -1 };///< Index of the currently selected tile.
     std::uint32_t                  window_width                   = { window_width_default };///< Current window width.
     std::uint32_t                  window_height                  = { window_height_default };///< Current window height.
     tile_sizes                     tile_size_value                = { tile_sizes::default_size };///< Current tile size setting.

     bool                           draw_palette                   = { false };///< Whether to draw the palette.
     bool                           draw_grid                      = { false };///< Whether to draw the grid.
     bool                           draw_texture_page_grid         = { false };///< Whether to draw the texture page grid.
     bool                           draw_tile_conflict_rects       = { false };///< Whether to draw the tile conflicts rectangles.
     bool                           draw_swizzle                   = { false };///< Whether to enable swizzle rendering.
     bool                           draw_disable_blending          = { false };///< Whether to disable blending during rendering.
     bool                           display_batch_window           = { false };///< Whether to display the test batch window.
     bool                           display_import_image           = { false };///< Whether to display the imported image.
     bool                           import_image_grid              = { false };///< Whether to overlay a grid on the imported image.
     bool                           render_imported_image          = { false };///< Whether to render the imported image.
     bool                           batch_embed_map_warning_window = { false };///< Whether to display a warning window when embedding maps.
     bool                           display_history_window         = { false };///< Whether to display history window.
     bool                           display_control_panel_window   = { true };///< Whether to display control panel window.
     bool                           display_draw_window            = { true };///< Whether to display draw window.
     bool                           display_custom_paths_window    = { false };///< Whether to display custom paths window.
     bool                           display_field_file_window      = { false };///< Whether to display field file window.
     std::string                    ffnx_mod_path                  = {};
     std::string                    ffnx_override_path             = {};
     std::string                    ffnx_direct_mode_path          = {};
     color background_color = { fme::colors::White };///< Remember to user's selected Background Color for draw Window.


     /**
      * @brief Constructs a Selections object with default values. Loading from past configuration if possible.
      */
     Selections();
     // need to rerun when we change the path because ffnx might not be there.
     void refresh_ffnx_paths();
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_SELECTIONS_HPP
