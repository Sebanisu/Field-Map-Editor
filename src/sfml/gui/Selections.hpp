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
     BatchSaveMap,
     PathsWithPaletteAndTexturePage,
     PathsWithTtexturePage,
     PathsWithPupuID,
     PathsNoPaletteAndTexturePage,
     PathsCommonUpscale,
     PathsCommonUpscaleForMaps,
     BatchCompact,
     BatchFlatten,
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
     int                            field                          = {};///< Selected field index. Set by gui after reading starter_field
     std::string                    starter_field                  = { "ecenter3" };///< field name.
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
     std::string                    ffnx_mod_path                  = { "mods/Textures" };
     std::string                    ffnx_override_path             = { "override" };
     std::string                    ffnx_direct_mode_path          = { "direct" };
     std::string                    output_swizzle_pattern         = { "{selected_path}\\{demaster}" };
     std::string                    output_deswizzle_pattern       = { "{selected_path}\\deswizzle\\{demaster}" };
     std::string                    output_map_pattern_for_swizzle = { "{selected_path}\\{demaster}" };
     std::string                    output_map_pattern_for_deswizzle = { "{selected_path}\\deswizzle\\{demaster}" };
     PatternSelector                current_pattern                  = {};
     color           background_color            = { fme::colors::White };///< Remember to user's selected Background Color for draw Window.

     input_types     batch_input_type            = {};
     root_path_types batch_input_root_path_type  = {};
     output_types    batch_output_type           = {};
     root_path_types batch_output_root_path_type = {};

     std::string     batch_input_path            = {};
     std::string     batch_output_path           = {};
     bool            batch_input_load_map        = { false };
     bool            batch_save_map              = { true };

     ff_8::filter_old<compact_type, ff_8::FilterTag::Compact> batch_compact_type                  = { ff_8::FilterSettings::Default };
     ff_8::filter_old<flatten_type, ff_8::FilterTag::Flatten> batch_flatten_type                  = { ff_8::FilterSettings::Default };

     std::vector<std::string>                                 paths_with_palette_and_texture_page = []() {
          const auto ret = std::vector<std::string>{
               "{selected_path}/{field_name}{_{2_letter_lang}}_0{texture_page}_0{palette}{ext}",
               "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}_0{palette}{ext}",
               "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}_0{palette}{ext}",

               "{selected_path}/{field_name}_0{texture_page}_0{palette}{ext}",
               "{selected_path}/{field_name}/{field_name}_0{texture_page}_0{palette}{ext}",
               "{selected_path}/{field_prefix}/{field_name}/{field_name}_0{texture_page}_0{palette}{ext}",

               "{selected_path}/{field_name}{_{2_letter_lang}}_{texture_page}_{palette}{ext}",
               "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}_{palette}{ext}",
               "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}_{palette}{ext}",

               "{selected_path}/{field_name}_{texture_page}_{palette}{ext}",
               "{selected_path}/{field_name}/{field_name}_{texture_page}_{palette}{ext}",
               "{selected_path}/{field_prefix}/{field_name}/{field_name}_{texture_page}_{palette}{ext}"
          };
          assert(fme::key_value_data::has_balanced_braces(ret));
          return ret;
     }();

     std::vector<std::string> paths_with_texture_page = []() {
          const auto ret =
            std::vector<std::string>{ "{selected_path}/{field_name}{_{2_letter_lang}}_0{texture_page}{ext}",
                                      "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}{ext}",
                                      "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}{ext}",

                                      "{selected_path}/{field_name}_0{texture_page}{ext}",
                                      "{selected_path}/{field_name}/{field_name}_0{texture_page}{ext}",
                                      "{selected_path}/{field_prefix}/{field_name}/{field_name}_0{texture_page}{ext}",

                                      "{selected_path}/{field_name}{_{2_letter_lang}}_{texture_page}{ext}",
                                      "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}{ext}",
                                      "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}{ext}",

                                      "{selected_path}/{field_name}_{texture_page}{ext}",
                                      "{selected_path}/{field_name}/{field_name}_{texture_page}{ext}",
                                      "{selected_path}/{field_prefix}/{field_name}/{field_name}_{texture_page}{ext}" };
          assert(fme::key_value_data::has_balanced_braces(ret));
          return ret;
     }();

     std::vector<std::string> paths_with_pupu_id = []() {
          const auto ret = std::vector<std::string>{ "{selected_path}/{field_name}_{pupu_id}{ext}",
                                                     "{selected_path}/{field_name}/{field_name}_{pupu_id}{ext}",
                                                     "{selected_path}/{field_prefix}/{field_name}/{field_name}_{pupu_id}{ext}" };
          assert(fme::key_value_data::has_balanced_braces(ret));
          return ret;
     }();

     std::vector<std::string> paths_no_palette_and_texture_page = []() {
          const auto ret = std::vector<std::string>{ "{selected_path}/{field_name}{_{2_letter_lang}}{ext}",
                                                     "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}{ext}",
                                                     "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}{ext}",

                                                     "{selected_path}/{field_name}{ext}",
                                                     "{selected_path}/{field_name}/{field_name}{ext}",
                                                     "{selected_path}/{field_prefix}/{field_name}/{field_name}{ext}" };
          assert(fme::key_value_data::has_balanced_braces(ret));
          return ret;
     }();

     std::vector<std::string> paths_common_upscale = []() {
          const auto ret = std::vector<std::string>{ "{selected_path}/{ffnx_mod_path}/field/mapdata/",
                                                     "{selected_path}/mods/Textures",
                                                     "{selected_path}/{demaster_mod_path}/textures/field_bg",
                                                     "{selected_path}/field_bg",
                                                     "{selected_path}/textures/fields",
                                                     "{selected_path}/textures",
                                                     "{selected_path}/ff8/Data/{3_letter_lang}/field/mapdata",
                                                     "{selected_path}/ff8/Data/{3_letter_lang}/FIELD/mapdata",
                                                     "{selected_path}/ff8/Data/{eng}/field/mapdata",
                                                     "{selected_path}/ff8/Data/{eng}/FIELD/mapdata",
                                                     "{selected_path}/ff8/Data/{fre}/field/mapdata",
                                                     "{selected_path}/ff8/Data/{fre}/FIELD/mapdata",
                                                     "{selected_path}/ff8/Data/{ger}/field/mapdata",
                                                     "{selected_path}/ff8/Data/{ger}/FIELD/mapdata",
                                                     "{selected_path}/ff8/Data/{ita}/field/mapdata",
                                                     "{selected_path}/ff8/Data/{ita}/FIELD/mapdata",
                                                     "{selected_path}/ff8/Data/{spa}/field/mapdata",
                                                     "{selected_path}/ff8/Data/{spa}/FIELD/mapdata",
                                                     "{selected_path}/ff8/Data/{jp}/field/mapdata",
                                                     "{selected_path}/ff8/Data/{jp}/FIELD/mapdata",
                                                     "{selected_path}/ff8/Data/{x}/field/mapdata",
                                                     "{selected_path}/ff8/Data/{x}/FIELD/mapdata" };
          assert(fme::key_value_data::has_balanced_braces(ret));
          return ret;
     }();

     std::vector<std::string> paths_common_upscale_for_maps = []() {
          const auto ret =
            std::vector<std::string>{ // todo ffnx uses a sepperate directory for map files which means we might not see it with our
                                      // current method of selecting one path ffnx_direct_mode_path might not want to be in the regular
                                      // paths list might need to be somewhere else. maybe a get paths map.
                                      "{selected_path}/{ffnx_direct_mode_path}/field/mapdata/"
            };
          assert(fme::key_value_data::has_balanced_braces(ret));
          return ret;
     }();

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
     void load_configuration();

     /**
      * @brief Refreshes FFNx-related paths based on the current FF8 path.
      *
      * This function must be rerun if the FF8 path changes, as the presence and location
      * of FFNx components are path-dependent. It reads configuration from "FFNx.toml".
      */
     void refresh_ffnx_paths();

     void update_configuration() const;


     void update_configuration_key(ConfigKey key) const;
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_SELECTIONS_HPP
