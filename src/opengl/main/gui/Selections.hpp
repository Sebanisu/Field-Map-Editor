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
     // TODO fix import image
     // DisplayImportImageWindow,
     DisplayImportImage,
     ForceReloadingOfTextures,
     ForceRenderingOfMap,
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
     CurrentPatternIndex,
     BatchInputType,
     BatchInputRootPathType,
     BatchOutputType,
     BatchOutputRootPathType,
     BatchMapListEnabled,
     BackgroundColor,
     BatchInputPath,
     BatchOutputPath,
     BatchInputLoadMap,
     BatchOutputSaveMap,
     PathPatternsWithPaletteAndTexturePage,
     PathPatternsWithPalette,
     PathPatternsWithTexturePage,
     PathPatternsWithPupuID,
     PatternsBase,
     PatternsCommonPrefixes,
     PatternsCommonPrefixesForMaps,
     FF8DirectoryPaths,
     ExternalTexturesDirectoryPaths,
     ExternalMapsDirectoryPaths,

     SwizzlePath,
     DeswizzlePath,
     OutputImagePath,
     OutputMimPath,
     OutputMapPath,
     SwizzlePathsIndex,

     CacheTexturePaths,
     CacheSwizzlePathsEnabled,
     CacheSwizzleAsOneImagePathsEnabled,
     CacheDeswizzlePathsEnabled,
     CacheMapPaths,
     CacheMapPathsEnabled,

     // Add more as needed
     All,

     // not required by update or load.
     BatchCompactType,
     BatchCompactEnabled,
     BatchFlattenType,
     BatchFlattenEnabled,
};


[[nodiscard]] consteval std::string_view key_to_string(ConfigKey key)
{
     using namespace std::string_view_literals;
     switch (key)
     {
          case ConfigKey::BackgroundColor:
               return "selections_background_color"sv;
          case ConfigKey::BatchCompactEnabled:
               return ff_8::ConfigKeys<ff_8::FilterTag::Compact>::enabled_key_name;
          case ConfigKey::BatchCompactType:
               return ff_8::ConfigKeys<ff_8::FilterTag::Compact>::key_name;
          case ConfigKey::BatchFlattenEnabled:
               return ff_8::ConfigKeys<ff_8::FilterTag::Flatten>::enabled_key_name;
          case ConfigKey::BatchFlattenType:
               return ff_8::ConfigKeys<ff_8::FilterTag::Flatten>::key_name;
          case ConfigKey::BatchInputLoadMap:
               return "batch_input_load_map"sv;
          case ConfigKey::BatchInputPath:
               return "batch_input_path"sv;
          case ConfigKey::BatchInputRootPathType:
               return "batch_input_root_path_type"sv;
          case ConfigKey::BatchInputType:
               return "batch_input_type"sv;
          case ConfigKey::BatchOutputPath:
               return "batch_output_path"sv;
          case ConfigKey::BatchOutputRootPathType:
               return "batch_output_root_path_type"sv;
          case ConfigKey::BatchMapListEnabled:
               return "batch_map_list_enabled"sv;
          case ConfigKey::BatchOutputType:
               return "batch_output_type"sv;
          case ConfigKey::BatchOutputSaveMap:
               return "batch_output_save_map"sv;
          case ConfigKey::Bpp:
               return "selections_bpp"sv;
          case ConfigKey::Coo:
               return "selections_coo"sv;
          case ConfigKey::CurrentPattern:
               return "selections_current_pattern"sv;
          case ConfigKey::CurrentPatternIndex:
               return "selections_current_pattern_index"sv;
          case ConfigKey::DeswizzlePath:
               return "selections_deswizzle_path"sv;
          case ConfigKey::DisplayBatchWindow:
               return "selections_display_batch_window"sv;
          case ConfigKey::DisplayControlPanelWindow:
               return "selections_display_control_panel_window"sv;
          case ConfigKey::DisplayCustomPathsWindow:
               return "selections_display_custom_paths_window"sv;
          case ConfigKey::DisplayDrawWindow:
               return "selections_display_draw_window"sv;
          case ConfigKey::DisplayFieldFileWindow:
               return "selections_display_field_file_window"sv;
          case ConfigKey::DisplayHistoryWindow:
               return "selections_display_history_window"sv;
          case ConfigKey::DisplayImportImage:
               return "selections_display_import_image"sv;
               // todo fix import window
          // case ConfigKey::DisplayImportImageWindow:
          //      return "selections_display_import_image_window"sv;
          case ConfigKey::ForceReloadingOfTextures:
               return "selections_force_reloading_of_textures"sv;
          case ConfigKey::ForceRenderingOfMap:
               return "selections_force_rendering_of_map"sv;
          case ConfigKey::Draw:
               return "selections_draw"sv;
          case ConfigKey::DrawDisableBlending:
               return "selections_draw_disable_blending"sv;
          case ConfigKey::DrawGrid:
               return "selections_draw_grid"sv;
          case ConfigKey::DrawPalette:
               return "selections_draw_palette"sv;
          case ConfigKey::DrawSwizzle:
               return "selections_draw_swizzle"sv;
          case ConfigKey::DrawTexturePageGrid:
               return "selections_draw_texture_page_grid"sv;
          case ConfigKey::DrawTileConflictRects:
               return "selections_draw_tile_conflict_rects"sv;
          case ConfigKey::ImportImageGrid:
               return "selections_import_image_grid"sv;
          case ConfigKey::ImportLoadImageDirectory:
               return "import_load_image_directory"sv;
          case ConfigKey::OutputDeswizzlePattern:
               return "selections_output_deswizzle_pattern"sv;
          case ConfigKey::OutputImagePath:
               return "output_image_path"sv;
          case ConfigKey::OutputMapPatternForDeswizzle:
               return "selections_output_map_pattern_for_deswizzle"sv;
          case ConfigKey::OutputMapPatternForSwizzle:
               return "selections_output_map_pattern_for_swizzle"sv;
          case ConfigKey::OutputMapPath:
               return "output_map_path"sv;
          case ConfigKey::OutputMimPath:
               return "output_mim_path"sv;
          case ConfigKey::OutputSwizzlePattern:
               return "selections_output_swizzle_pattern"sv;
          case ConfigKey::Palette:
               return "selections_palette"sv;
          case ConfigKey::PatternsCommonPrefixes:
               return "patterns_common_prefixes"sv;
          case ConfigKey::PatternsCommonPrefixesForMaps:
               return "patterns_common_prefixes_for_maps"sv;
          case ConfigKey::PatternsBase:
               return "patterns_base"sv;
          case ConfigKey::PathPatternsWithPaletteAndTexturePage:
               return "patterns_with_palette_and_texture_page"sv;
          case ConfigKey::PathPatternsWithPalette:
               return "patterns_with_palette"sv;
          case ConfigKey::PathPatternsWithTexturePage:
               return "patterns_with_texture_page"sv;
          case ConfigKey::PathPatternsWithPupuID:
               return "patterns_with_pupu_id"sv;
          case ConfigKey::FF8DirectoryPaths:
               return "ff8_directory_paths"sv;
          case ConfigKey::ExternalTexturesDirectoryPaths:
               return "external_textures_directory_paths"sv;
          case ConfigKey::ExternalMapsDirectoryPaths:
               return "external_maps_directory_paths"sv;
          case ConfigKey::RenderImportedImage:
               return "selections_render_imported_image"sv;
          case ConfigKey::SelectedTile:
               return "selections_selected_tile"sv;
          case ConfigKey::SelectionsPath:
               return "selections_path"sv;
          case ConfigKey::StarterField:
               return "starter_field"sv;
          case ConfigKey::SwizzlePath:
               return "selections_swizzle_path"sv;
          case ConfigKey::TileSizeValue:
               return "selections_tile_size_value"sv;
          case ConfigKey::SwizzlePathsIndex:
               return "swizzle_paths_index"sv;
          case ConfigKey::WindowHeight:
               return "selections_window_height"sv;
          case ConfigKey::WindowWidth:
               return "selections_window_width"sv;
          case ConfigKey::CacheTexturePaths:
               return "selections_cache_texture_paths"sv;
          case ConfigKey::CacheSwizzlePathsEnabled:
               return "selections_cache_swizzle_paths_enabled"sv;
          case ConfigKey::CacheSwizzleAsOneImagePathsEnabled:
               return "selections_cache_swizzle_as_one_image_paths_enabled"sv;
          case ConfigKey::CacheDeswizzlePathsEnabled:
               return "selections_cache_deswizzle_paths_enabled"sv;
          case ConfigKey::CacheMapPaths:
               return "selections_cache_map_paths"sv;
          case ConfigKey::CacheMapPathsEnabled:
               return "selections_cache_map_paths_enabled"sv;


          default: {
               spdlog::error("{}:{} Unknown configuration key (ConfigKey): {}", __FILE__, __LINE__, std::to_underlying(key));
               return {};
          }
     };
}
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
     std::string                   path;

     /**
      * @brief The default window width.
      */
     static constexpr std::int32_t window_width_default  = 1280;

     /**
      * @brief The default window height.
      */
     static constexpr std::int32_t window_height_default = 720;

     open_viii::graphics::BPPT     bpp;///< Bits per pixel for rendering.
     uint8_t                       palette;///< Selected palette index.
     // int                            field         = {};///< Selected field index. Set by gui after reading starter_field
     std::string                   starter_field;///< field name.
     open_viii::LangT              coo;///< Selected language.
     draw_mode                     draw;///< Current drawing mode.
     int                           selected_tile;///< Index of the currently selected tile.
     int                           swizzle_paths_index;
     std::int32_t                  window_width;///< Current window width.
     std::int32_t                  window_height;///< Current window height.
     tile_sizes                    tile_size_value;///< Current tile size setting.

     bool                          draw_palette;///< Whether to draw the palette.
     bool                          draw_grid;///< Whether to draw the grid.
     bool                          draw_texture_page_grid;///< Whether to draw the texture page grid.
     bool                          draw_tile_conflict_rects;///< Whether to draw the tile conflicts rectangles.
     bool                          draw_swizzle;///< Whether to enable swizzle rendering.
     bool                          draw_disable_blending;///< Whether to disable blending during rendering.
     bool                          display_batch_window;///< Whether to display the test batch window.
     // todo fix imports
     //  bool                     display_import_image_window = false;
     bool                          display_import_image;///< Whether to display the imported image.
     bool                          force_reloading_of_textures;///< Force reloading textures and rendering of map on every frame.
     bool                          force_rendering_of_map;///< Force rendering of map on every frame.
     bool                          import_image_grid;///< Whether to overlay a grid on the imported image.
     bool                          render_imported_image;///< Whether to render the imported image.
     bool                          display_history_window;///< Whether to display history window.
     bool                          display_control_panel_window;///< Whether to display control panel window.
     bool                          display_draw_window;///< Whether to display draw window.
     bool                          display_custom_paths_window;///< Whether to display custom paths window.
     bool                          display_field_file_window;///< Whether to display field file window.
     std::string                   ffnx_mod_path;
     std::string                   ffnx_override_path;
     std::string                   ffnx_direct_mode_path;
     std::string                   output_swizzle_pattern;
     std::string                   output_deswizzle_pattern;
     std::string                   output_map_pattern_for_swizzle;
     std::string                   output_map_pattern_for_deswizzle;
     std::string                   swizzle_path;
     std::string                   deswizzle_path;
     std::string                   output_map_path;
     std::string                   output_mim_path;
     std::string                   output_image_path;
     std::string                   import_load_image_directory;
     PatternSelector               current_pattern;
     int                           current_pattern_index;
     color                         background_color;///< Remember to user's selected Background Color for draw Window.

     input_types                   batch_input_type;
     root_path_types               batch_input_root_path_type;
     output_types                  batch_output_type;
     root_path_types               batch_output_root_path_type;
     std::vector<bool>             batch_map_list_enabled;

     std::string                   batch_input_path;
     std::string                   batch_output_path;
     bool                          batch_input_load_map;
     bool                          batch_output_save_map;

     ff_8::filter_old<compact_type, ff_8::FilterTag::Compact> batch_compact_type;
     ff_8::filter_old<flatten_type, ff_8::FilterTag::Flatten> batch_flatten_type;
     /// @brief File path patterns that require both a palette and a texture page to be substituted.
     std::vector<std::string>                                 patterns_with_palette_and_texture_page;

     /// @brief File path patterns that require only a palette to be substituted.
     std::vector<std::string>                                 patterns_with_palette;

     /// @brief File path patterns that require only a texture page to be substituted.
     std::vector<std::string>                                 patterns_with_texture_page;

     /// @brief File path patterns that require a PUPU ID to be substituted.
     std::vector<std::string>                                 patterns_with_pupu_id;

     /// @brief File path patterns that use no additional substitution values (e.g., no palette or texture page).
     std::vector<std::string>                                 patterns_base;

     /// @brief Common directory prefixes prepended to pattern paths when generating full file paths.
     std::vector<std::string>                                 patterns_common_prefixes;

     /// @brief Additional directory prefixes used specifically when searching for map files. Combined with patterns_common_prefixes.
     std::vector<std::string>                                 patterns_common_prefixes_for_maps;

     /// @brief List of base directories where FF8 game data is installed or extracted.
     /// This is user-configurable, but a default set of paths is provided initially.
     /// These directories are also included in pattern-based searches, since modded files are often placed here.
     std::vector<std::string>                                 ff8_directory_paths;

     /// @brief User-configured root directories for searching external texture files using path patterns.
     /// These directories serve as the starting point for pattern-based texture file lookups.
     std::vector<std::string>                                 external_textures_directory_paths;

     /// @brief User-configured root directories for searching external map files using path patterns.
     /// These directories serve as the starting point for pattern-based map file lookups.
     std::vector<std::string>                                 external_maps_directory_paths;


     /**
      * @brief Paths to cache directories for texture-related operations.
      *
      * Used for swizzle, deswizzle, and swizzle-as-one-image processing.
      * Each path corresponds by index to entries in the enabled vectors below.
      */
     std::vector<std::string>                                 cache_texture_paths;

     /**
      * @brief Indicates if swizzle files were detected in each texture cache path.
      *
      * Matches size and index with `cache_texture_paths`.
      */
     std::vector<bool>                                        cache_swizzle_paths_enabled;

     /**
      * @brief Indicates if swizzle-as-one-image files were detected in each texture cache path.
      *
      * Matches size and index with `cache_texture_paths`.
      */
     std::vector<bool>                                        cache_swizzle_as_one_image_paths_enabled;

     /**
      * @brief Indicates if deswizzle files were detected in each texture cache path.
      *
      * Matches size and index with `cache_texture_paths`.
      */
     std::vector<bool>                                        cache_deswizzle_paths_enabled;

     /**
      * @brief Indicates if any map files were detected in each map cache path.
      *
      * Matches size and index with `cache_map_paths`.
      */
     std::vector<bool>                                        cache_map_paths_enabled;


     /**
      * @brief Paths to cache directories for map-related operations.
      *
      * Used for both swizzle and deswizzle map processing.
      * Each path corresponds by index to entries in the map enabled vector.
      */
     std::vector<std::string>                                 cache_map_paths;


     /**
      * @brief Constructs a Selections object with default values.
      */
     Selections(const Configuration config = {});


     /**
      * @brief Refreshes FFNx-related paths based on the current FF8 path.
      *
      * This function must be rerun if the FF8 path changes, as the presence and location
      * of FFNx components are path-dependent. It reads configuration from "FFNx.toml".
      */
     void refresh_ffnx_paths();

     void update_configuration() const;


     template<ConfigKey... Keys>
     void update_configuration_key() const
     {
          Configuration config{};
          (update<Keys>(config), ...);
          config.save();
     }

     static bool has_balanced_braces([[maybe_unused]] const std::string_view s);

     template<std::ranges::range R>
          requires std::convertible_to<std::ranges::range_value_t<R>, std::string_view>
     static inline bool has_balanced_braces([[maybe_unused]] const R &r)
     {
          for (const auto &s : r)
          {
               if (bool ok = has_balanced_braces(s); !ok)
               {
                    return false;// found bad brace.
               }
          }
          return true;
     }

   private:
     template<ConfigKey K>
     void load(const Configuration &) = delete;// delete fine for load because all defined and used in Selections

     template<ConfigKey K>
     void update(Configuration &)
       const;// can't delete or else other cpp can't find the functions in Selections.cpp will get linker errors if missing.
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_SELECTIONS_HPP
