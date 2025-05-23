//
// Created by pcvii on 12/21/2022.
//
#ifndef FIELD_MAP_EDITOR_SELECTIONS_HPP
#define FIELD_MAP_EDITOR_SELECTIONS_HPP
#include "colors.hpp"
#include "compact_type.hpp"
#include "Configuration.hpp"
#include "draw_mode.hpp"
#include "filter.hpp"
#include "tile_sizes.hpp"
#include <filesystem>
#include <open_viii/graphics/background/BlendModeT.hpp>
#include <open_viii/graphics/BPPT.hpp>
#include <open_viii/paths/Paths.hpp>
#include <open_viii/strings/LangT.hpp>
#include <spdlog/spdlog.h>
namespace fme
{
enum class ConfigKey
{
     StarterField,
     SelectionsPath,
     WindowWidth,
     WindowHeight,
     Palette,
     Bpp,
     Draw,
     Coo,
     SelectedTile,
     DrawDisableBlending,
     DrawGrid,
     DrawPalette,
     DrawSwizzle,
     RenderImportedImage,// if needed
     DrawTexturePageGrid,
     DrawTileConflictRects,
     DisplayBatchWindow,
     DisplayImportImage,
     ImportImageGrid,
     ImportLoadImageDirectory,
     TileSizeValue,
     DisplayHistoryWindow,
     DisplayControlPanelWindow,
     DisplayDrawWindow,
     DisplayCustomPathsWindow,
     DisplayFieldFileWindow,
     OutputSwizzlePattern,
     OutputDeswizzlePattern,
     OutputMapPatternForSwizzle,
     OutputMapPatternForDeswizzle,
     CurrentPattern,
     BatchInputType,
     BatchInputRootPathType,
     BatchOutputType,
     BatchOutputRootPathType,
     BackgroundColor,
     BatchInputPath,
     BatchOutputPath,
     BatchInputLoadMap,
     BatchOutputSaveMap,
     PathPatternsWithPaletteAndTexturePage,
     PathPatternsWithTexturePage,
     PathPatternsWithPupuID,
     PathPatternsNoPaletteAndTexturePage,
     PathPatternsCommonUpscale,
     PathPatternsCommonUpscaleForMaps,
     PathsVector,
     PathsVectorUpscale,
     PathsVectorDeswizzle,
     BatchCompact,
     BatchCompactType,
     BatchCompactEnabled,
     BatchFlatten,
     BatchFlattenType,
     BatchFlattenEnabled,
     SwizzlePath,
     DeswizzlePath,
     OutputImagePath,
     OutputMimPath,
     OutputMapPath,
     UpscalePathsIndex,
     // Add more as needed
     All,
};
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
     std::string                    path;

     /**
      * @brief The default window width.
      */
     static constexpr std::uint32_t window_width_default  = 800;

     /**
      * @brief The default window height.
      */
     static constexpr std::uint32_t window_height_default = 600;

     open_viii::graphics::BPPT      bpp;///< Bits per pixel for rendering.
     uint8_t                        palette;///< Selected palette index.
     // int                            field         = {};///< Selected field index. Set by gui after reading starter_field
     std::string                    starter_field;///< field name.
     open_viii::LangT               coo;///< Selected language.
     draw_mode                      draw;///< Current drawing mode.
     int                            selected_tile;///< Index of the currently selected tile.
     int                            upscale_paths_index;
     std::uint32_t                  window_width;///< Current window width.
     std::uint32_t                  window_height;///< Current window height.
     tile_sizes                     tile_size_value;///< Current tile size setting.

     bool                           draw_palette;///< Whether to draw the palette.
     bool                           draw_grid;///< Whether to draw the grid.
     bool                           draw_texture_page_grid;///< Whether to draw the texture page grid.
     bool                           draw_tile_conflict_rects;///< Whether to draw the tile conflicts rectangles.
     bool                           draw_swizzle;///< Whether to enable swizzle rendering.
     bool                           draw_disable_blending;///< Whether to disable blending during rendering.
     bool                           display_batch_window;///< Whether to display the test batch window.
     bool                           display_import_image;///< Whether to display the imported image.
     bool                           import_image_grid;///< Whether to overlay a grid on the imported image.
     bool                           render_imported_image;///< Whether to render the imported image.
     bool                           display_history_window;///< Whether to display history window.
     bool                           display_control_panel_window;///< Whether to display control panel window.
     bool                           display_draw_window;///< Whether to display draw window.
     bool                           display_custom_paths_window;///< Whether to display custom paths window.
     bool                           display_field_file_window;///< Whether to display field file window.
     std::string                    ffnx_mod_path;
     std::string                    ffnx_override_path;
     std::string                    ffnx_direct_mode_path;
     std::string                    output_swizzle_pattern;
     std::string                    output_deswizzle_pattern;
     std::string                    output_map_pattern_for_swizzle;
     std::string                    output_map_pattern_for_deswizzle;
     std::string                    swizzle_path;
     std::string                    deswizzle_path;
     std::string                    output_map_path;
     std::string                    output_mim_path;
     std::string                    output_image_path;
     std::string                    import_load_image_directory;
     PatternSelector                current_pattern;
     color                          background_color;///< Remember to user's selected Background Color for draw Window.

     input_types                    batch_input_type;
     root_path_types                batch_input_root_path_type;
     output_types                   batch_output_type;
     root_path_types                batch_output_root_path_type;

     std::string                    batch_input_path;
     std::string                    batch_output_path;
     bool                           batch_input_load_map;
     bool                           batch_output_save_map;

     ff_8::filter_old<compact_type, ff_8::FilterTag::Compact> batch_compact_type = { ff_8::FilterSettings::Default };
     ff_8::filter_old<flatten_type, ff_8::FilterTag::Flatten> batch_flatten_type = { ff_8::FilterSettings::Default };

     std::vector<std::string>                                 paths_with_palette_and_texture_page;

     std::vector<std::string>                                 paths_with_texture_page;

     std::vector<std::string>                                 paths_with_pupu_id;

     std::vector<std::string>                                 paths_no_palette_and_texture_page;

     std::vector<std::string>                                 paths_common_upscale;


     std::vector<std::string>                                 paths_vector;
     std::vector<std::string>                                 paths_vector_upscale   = {};
     std::vector<std::string>                                 paths_vector_deswizzle = {};

     std::vector<std::string>                                 paths_common_upscale_for_maps;

     /**
      * @brief Constructs a Selections object with default values.
      *
      * If @p load_config is true, it will automatically call load_configuration().
      *
      * @param load_config Whether to load the configuration during construction. Defaults to true.
      */
     Selections(bool load_config = true);

     /**
      * @brief Loads configuration from a saved state if available; otherwise, uses default values.
      *
      * This function should be called during initialization if prior settings are expected.
      */
     void                    load_configuration();

     /**
      * @brief Refreshes FFNx-related paths based on the current FF8 path.
      *
      * This function must be rerun if the FF8 path changes, as the presence and location
      * of FFNx components are path-dependent. It reads configuration from "FFNx.toml".
      */
     void                    refresh_ffnx_paths();

     void                    update_configuration() const;


     void                    update_configuration_key(ConfigKey key) const;

     static std::string_view key_to_string(ConfigKey key);

     static inline bool      has_balanced_braces([[maybe_unused]] const std::string_view s)
     {
          //      int balance = 0;
          //      for (const char c : s)
          //      {
          //           if (c == '{')
          //           {
          //                ++balance;
          //           }
          //           else if (c == '}')
          //           {
          //                --balance;
          //                if (balance < 0)
          //                {
          //                     spdlog::error("Unmatched closing brace in input: \"{}\" (note: literal braces shown as {{ and }})", s);
          //                     return false;
          //                }
          //           }
          //      }

          //      if (balance != 0)
          //      {
          //           spdlog::error("Mismatched brace count in input: \"{}\" ({} unmatched opening brace{{}})", s, balance);
          //           return false;
          //      }

          return true;
     }

     template<std::ranges::range R>
          requires std::convertible_to<std::ranges::range_value_t<R>, std::string_view>
     static inline bool has_balanced_braces([[maybe_unused]] const R &r)
     {
          // for (const auto &s : r)
          // {
          //      if (bool ok = has_balanced_braces(s); !ok)
          //      {
          //           return false;// found bad brace.
          //      }
          // }
          return true;
     }
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_SELECTIONS_HPP
