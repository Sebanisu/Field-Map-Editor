//
// Created by pcvii on 12/21/2022.
//
#ifndef FIELD_MAP_EDITOR_SELECTIONS_HPP
#define FIELD_MAP_EDITOR_SELECTIONS_HPP
#include "Configuration.hpp"
#include "draw_mode.hpp"
#include "open_viii/strings/LangT.hpp"
#include "tile_sizes.hpp"
#include <filesystem>
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

     int                            bpp                            = {};///< Bits per pixel for rendering.
     int                            palette                        = {};///< Selected palette index.
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
     bool                           draw_swizzle                   = { false };///< Whether to enable swizzle rendering.
     bool                           draw_disable_blending          = { false };///< Whether to disable blending during rendering.
     bool                           test_batch_window              = { false };///< Whether to display the test batch window.
     bool                           display_import_image           = { false };///< Whether to display the imported image.
     bool                           import_image_grid              = { false };///< Whether to overlay a grid on the imported image.
     bool                           render_imported_image          = { false };///< Whether to render the imported image.
     bool                           batch_embed_map_warning_window = { false };///< Whether to display a warning window when embedding maps.
     bool                           display_history_window         = { false };///< Whether to display history window.

     /**
      * @brief Constructs a Selections object with default values.
      */
     Selections();
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_SELECTIONS_HPP
