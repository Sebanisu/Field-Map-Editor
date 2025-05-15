//
// Created by pcvii on 12/21/2022.
//
#include "Selections.hpp"
#include <spdlog/spdlog.h>
using namespace open_viii;
using namespace open_viii::graphics;
using namespace open_viii::graphics::background;
fme::Selections::Selections(bool load_config)
{
     if (load_config)
     {
          load_configuration();
     }
}

std::string_view fme::Selections::key_to_string(ConfigKey key)
{
     using map_t                 = std::map<ConfigKey, std::string_view>;
     static const map_t updaters = []() {
          map_t      m{};
          const auto add_to_map = [&m](this const auto &self, const ConfigKey key, const std::string_view str, const auto... rest) {
               m.emplace(key, str);
               if constexpr (sizeof...(rest) > 0)
               {
                    self(rest...);
               }
          };
          add_to_map(
            ConfigKey::Draw,
            "selections_draw",
            ConfigKey::Coo,
            "selections_coo",
            ConfigKey::TileSizeValue,
            "selections_tile_size_value",
            ConfigKey::BatchInputType,
            "batch_input_type",
            ConfigKey::BatchInputRootPathType,
            "batch_input_root_path_type",
            ConfigKey::BatchOutputType,
            "batch_output_type",
            ConfigKey::BatchOutputRootPathType,
            "batch_output_root_path_type",
            ConfigKey::CurrentPattern,
            "selections_current_pattern",
            ConfigKey::StarterField,
            "starter_field",
            ConfigKey::SelectionsPath,
            "selections_path",
            ConfigKey::SwizzlePath,
            "selections_swizzle_path",
            ConfigKey::DeswizzlePath,
            "selections_deswizzle_path",
            ConfigKey::WindowWidth,
            "selections_window_width",
            ConfigKey::WindowHeight,
            "selections_window_height",
            ConfigKey::Palette,
            "selections_palette",
            ConfigKey::Bpp,
            "selections_bpp",
            ConfigKey::DrawGrid,
            "selections_draw_grid",
            ConfigKey::DrawGrid,
            "selections_draw_grid",
            ConfigKey::SelectedTile,
            "selections_selected_tile",
            ConfigKey::DrawDisableBlending,
            "selections_draw_disable_blending",
            ConfigKey::DrawGrid,
            "selections_draw_grid",
            ConfigKey::DrawPalette,
            "selections_draw_palette",
            ConfigKey::DrawSwizzle,
            "selections_draw_swizzle",
            ConfigKey::RenderImportedImage,
            "selections_render_imported_image",
            ConfigKey::DrawTexturePageGrid,
            "selections_draw_texture_page_grid",
            ConfigKey::DrawTileConflictRects,
            "selections_draw_tile_conflict_rects",
            ConfigKey::DisplayBatchWindow,
            "selections_display_batch_window",
            ConfigKey::DisplayImportImage,
            "selections_display_import_image",
            ConfigKey::ImportImageGrid,
            "selections_import_image_grid",
            ConfigKey::DisplayHistoryWindow,
            "selections_display_history_window",
            ConfigKey::DisplayControlPanelWindow,
            "selections_display_control_panel_window",
            ConfigKey::DisplayDrawWindow,
            "selections_display_draw_window",
            ConfigKey::DisplayCustomPathsWindow,
            "selections_display_custom_paths_window",
            ConfigKey::DisplayFieldFileWindow,
            "selections_display_field_file_window",
            ConfigKey::OutputSwizzlePattern,
            "selections_output_swizzle_pattern",
            ConfigKey::OutputDeswizzlePattern,
            "selections_output_deswizzle_pattern",
            ConfigKey::OutputMapPatternForSwizzle,
            "selections_output_map_pattern_for_swizzle",
            ConfigKey::OutputMapPatternForDeswizzle,
            "selections_output_map_pattern_for_deswizzle",
            ConfigKey::BatchInputLoadMap,
            "batch_input_load_map",
            ConfigKey::BatchSaveMap,
            "batch_save_map",
            ConfigKey::BackgroundColor,
            "selections_background_color",
            ConfigKey::BatchInputPath,
            "batch_input_path",
            ConfigKey::BatchOutputPath,
            "batch_output_path",
            ConfigKey::PathsWithPaletteAndTexturePage,
            "paths_with_palette_and_texture_page",
            ConfigKey::PathsWithTexturePage,
            "paths_with_texture_page",
            ConfigKey::PathsWithPupuID,
            "paths_with_pupu_id",
            ConfigKey::PathsNoPaletteAndTexturePage,
            "paths_no_palette_and_texture_page",
            ConfigKey::PathsCommonUpscale,
            "paths_common_upscale",
            ConfigKey::PathsCommonUpscaleForMaps,
            "paths_common_upscale_for_maps",
            ConfigKey::BatchCompactType,
            "batch_compact_type",
            ConfigKey::BatchCompactEnabled,
            "batch_compact_enabled",
            ConfigKey::BatchFlattenType,
            "batch_flatten_type",
            ConfigKey::BatchFlattenEnabled,
            "batch_flatten_enabled");
// #define MAP_MACRO(KEY, STR_NAME) m.emplace(KEY, STR_NAME)
//           MAP_MACRO(ConfigKey::Draw, "selections_draw");
//           MAP_MACRO(ConfigKey::Coo, "selections_coo");
//           MAP_MACRO(ConfigKey::TileSizeValue, "selections_tile_size_value");
//           MAP_MACRO(ConfigKey::BatchInputType, "batch_input_type");
//           MAP_MACRO(ConfigKey::BatchInputRootPathType, "batch_input_root_path_type");
//           MAP_MACRO(ConfigKey::BatchOutputType, "batch_output_type");
//           MAP_MACRO(ConfigKey::BatchOutputRootPathType, "batch_output_root_path_type");
//           MAP_MACRO(ConfigKey::CurrentPattern, "selections_current_pattern");
//           MAP_MACRO(ConfigKey::StarterField, "starter_field");
//           MAP_MACRO(ConfigKey::SelectionsPath, "selections_path");
//           MAP_MACRO(ConfigKey::SwizzlePath, "selections_swizzle_path");
//           MAP_MACRO(ConfigKey::DeswizzlePath, "selections_deswizzle_path");
//           MAP_MACRO(ConfigKey::WindowWidth, "selections_window_width");
//           MAP_MACRO(ConfigKey::WindowHeight, "selections_window_height");
//           MAP_MACRO(ConfigKey::Palette, "selections_palette");
//           MAP_MACRO(ConfigKey::Bpp, "selections_bpp");
//           MAP_MACRO(ConfigKey::DrawGrid, "selections_draw_grid");
//           MAP_MACRO(ConfigKey::DrawGrid, "selections_draw_grid");
//           MAP_MACRO(ConfigKey::SelectedTile, "selections_selected_tile");
//           MAP_MACRO(ConfigKey::DrawDisableBlending, "selections_draw_disable_blending");
//           MAP_MACRO(ConfigKey::DrawGrid, "selections_draw_grid");
//           MAP_MACRO(ConfigKey::DrawPalette, "selections_draw_palette");
//           MAP_MACRO(ConfigKey::DrawSwizzle, "selections_draw_swizzle");
//           MAP_MACRO(ConfigKey::RenderImportedImage, "selections_render_imported_image");
//           MAP_MACRO(ConfigKey::DrawTexturePageGrid, "selections_draw_texture_page_grid");
//           MAP_MACRO(ConfigKey::DrawTileConflictRects, "selections_draw_tile_conflict_rects");
//           MAP_MACRO(ConfigKey::DisplayBatchWindow, "selections_display_batch_window");
//           MAP_MACRO(ConfigKey::DisplayImportImage, "selections_display_import_image");
//           MAP_MACRO(ConfigKey::ImportImageGrid, "selections_import_image_grid");
//           MAP_MACRO(ConfigKey::DisplayHistoryWindow, "selections_display_history_window");
//           MAP_MACRO(ConfigKey::DisplayControlPanelWindow, "selections_display_control_panel_window");
//           MAP_MACRO(ConfigKey::DisplayDrawWindow, "selections_display_draw_window");
//           MAP_MACRO(ConfigKey::DisplayCustomPathsWindow, "selections_display_custom_paths_window");
//           MAP_MACRO(ConfigKey::DisplayFieldFileWindow, "selections_display_field_file_window");
//           MAP_MACRO(ConfigKey::OutputSwizzlePattern, "selections_output_swizzle_pattern");
//           MAP_MACRO(ConfigKey::OutputDeswizzlePattern, "selections_output_deswizzle_pattern");
//           MAP_MACRO(ConfigKey::OutputMapPatternForSwizzle, "selections_output_map_pattern_for_swizzle");
//           MAP_MACRO(ConfigKey::OutputMapPatternForDeswizzle, "selections_output_map_pattern_for_deswizzle");
//           MAP_MACRO(ConfigKey::BatchInputLoadMap, "batch_input_load_map");
//           MAP_MACRO(ConfigKey::BatchSaveMap, "batch_save_map");
//           MAP_MACRO(ConfigKey::BackgroundColor, "selections_background_color");
//           MAP_MACRO(ConfigKey::BatchInputPath, "batch_input_path");
//           MAP_MACRO(ConfigKey::BatchOutputPath, "batch_output_path");
//           MAP_MACRO(ConfigKey::PathsWithPaletteAndTexturePage, "paths_with_palette_and_texture_page");
//           MAP_MACRO(ConfigKey::PathsWithTexturePage, "paths_with_texture_page");
//           MAP_MACRO(ConfigKey::PathsWithPupuID, "paths_with_pupu_id");
//           MAP_MACRO(ConfigKey::PathsNoPaletteAndTexturePage, "paths_no_palette_and_texture_page");
//           MAP_MACRO(ConfigKey::PathsCommonUpscale, "paths_common_upscale");
//           MAP_MACRO(ConfigKey::PathsCommonUpscaleForMaps, "paths_common_upscale_for_maps");
//           MAP_MACRO(ConfigKey::BatchCompactType, "batch_compact_type");
//           MAP_MACRO(ConfigKey::BatchCompactEnabled, "batch_compact_enabled");
//           MAP_MACRO(ConfigKey::BatchFlattenType, "batch_flatten_type");
//           MAP_MACRO(ConfigKey::BatchFlattenEnabled, "batch_flatten_enabled");

// #undef MAP_MACRO
          // Add more mappings here...
          return m;
     }();

     if (const auto it = updaters.find(key); it != updaters.end()) [[likely]]
     {
          // updaters.at(key)(config, *this);
          return it->second;
     }
     else [[unlikely]]
     {
          spdlog::error("Unknown configuration key (enum): {}", std::to_underlying(key));
     }
     return {};
}

void fme::Selections::load_configuration()
{
     Configuration const config{};
     starter_field            = config[key_to_string(ConfigKey::StarterField)].value_or(starter_field);
     // field
     path                     = config[key_to_string(ConfigKey::SelectionsPath)].value_or(path);
     swizzle_path             = config[key_to_string(ConfigKey::SwizzlePath)].value_or(swizzle_path);
     deswizzle_path           = config[key_to_string(ConfigKey::DeswizzlePath)].value_or(deswizzle_path);
     window_width             = config["selections_window_width"].value_or(window_width);
     window_height            = config["selections_window_width"].value_or(window_height);
     palette                  = config["selections_palette"].value_or(palette) & 0xFU;
     bpp                      = BPPT{ config["selections_bpp"].value_or(bpp.raw()) & 3U };
     draw                     = static_cast<draw_mode>(config["selections_draw"].value_or(std::to_underlying(draw)));
     coo                      = static_cast<LangT>(config["selections_coo"].value_or(std::to_underlying(coo)));
     selected_tile            = config["selections_selected_tile"].value_or(selected_tile);
     draw_disable_blending    = config["selections_draw_disable_blending"].value_or(draw_disable_blending);
     draw_grid                = config["selections_draw_grid"].value_or(draw_grid);
     draw_palette             = config["selections_draw_palette"].value_or(draw_palette);
     draw_swizzle             = config["selections_draw_swizzle"].value_or(draw_swizzle);
     //  render_imported_image =
     //  config["selections_render_imported_image"].value_or(
     //    render_imported_image);
     draw_texture_page_grid   = config["selections_draw_texture_page_grid"].value_or(draw_texture_page_grid);
     draw_tile_conflict_rects = config["selections_draw_tile_conflict_rects"].value_or(draw_tile_conflict_rects);
     display_batch_window     = config["selections_display_batch_window"].value_or(display_batch_window);
     display_import_image     = config["selections_display_import_image"].value_or(display_import_image);
     import_image_grid        = config["selections_import_image_grid"].value_or(import_image_grid);
     tile_size_value          = static_cast<tile_sizes>(config["selections_tile_size_value"].value_or(std::to_underlying(tile_size_value)));
     display_history_window   = config["selections_display_history_window"].value_or(display_history_window);
     display_control_panel_window     = config["selections_display_control_panel_window"].value_or(display_control_panel_window);
     display_draw_window              = config["selections_display_draw_window"].value_or(display_draw_window);
     display_custom_paths_window      = config["selections_display_custom_paths_window"].value_or(display_custom_paths_window);
     display_field_file_window        = config["selections_display_field_file_window"].value_or(display_field_file_window);

     output_swizzle_pattern           = config["selections_output_swizzle_pattern"].value_or(output_swizzle_pattern);
     output_deswizzle_pattern         = config["selections_output_deswizzle_pattern"].value_or(output_deswizzle_pattern);
     output_map_pattern_for_swizzle   = config["selections_output_map_pattern_for_swizzle"].value_or(output_map_pattern_for_swizzle);
     output_map_pattern_for_deswizzle = config["selections_output_map_pattern_for_deswizzle"].value_or(output_map_pattern_for_deswizzle);
     current_pattern =
       static_cast<decltype(current_pattern)>(config["selections_current_pattern"].value_or(std::to_underlying(current_pattern)));

     batch_input_type =
       static_cast<input_types>(config["batch_input_type"].value_or(static_cast<std::underlying_type_t<input_types>>(batch_input_type)));
     batch_input_root_path_type = static_cast<root_path_types>(
       config["batch_input_root_path_type"].value_or(static_cast<std::underlying_type_t<root_path_types>>(batch_input_root_path_type)));
     batch_output_type = static_cast<output_types>(
       config["batch_output_type"].value_or(static_cast<std::underlying_type_t<output_types>>(batch_output_type)));
     batch_output_root_path_type = static_cast<root_path_types>(
       config["batch_output_root_path_type"].value_or(static_cast<std::underlying_type_t<root_path_types>>(batch_output_root_path_type)));
     background_color =
       std::bit_cast<fme::color>(config["selections_background_color"].value_or(std::bit_cast<std::uint32_t>(background_color)));


     batch_input_path     = config["batch_input_path"].value_or(std::string(batch_input_path.data()));
     batch_output_path    = config["batch_output_path"].value_or(std::string(batch_output_path.data()));
     batch_input_load_map = config["batch_input_load_map"].value_or(batch_input_load_map);
     batch_save_map       = config["batch_save_map"].value_or(batch_save_map);

     refresh_ffnx_paths();

     // todo load_array returns true than check with has_balanced_braces
     config.load_array("paths_with_palette_and_texture_page", paths_with_palette_and_texture_page);
     config.load_array("paths_with_texture_page", paths_with_texture_page);
     config.load_array("paths_with_pupu_id", paths_with_pupu_id);
     config.load_array("paths_no_palette_and_texture_page", paths_no_palette_and_texture_page);
     config.load_array("paths_common_upscale", paths_common_upscale);
     config.load_array("paths_common_upscale_for_maps", paths_common_upscale_for_maps);

     assert(fme::key_value_data::has_balanced_braces(paths_with_palette_and_texture_page));
     assert(fme::key_value_data::has_balanced_braces(paths_with_texture_page));
     assert(fme::key_value_data::has_balanced_braces(paths_with_pupu_id));
     assert(fme::key_value_data::has_balanced_braces(paths_no_palette_and_texture_page));
     assert(fme::key_value_data::has_balanced_braces(paths_common_upscale));
     assert(fme::key_value_data::has_balanced_braces(paths_common_upscale_for_maps));
}


void fme::Selections::update_configuration() const
{
     update_configuration_key(ConfigKey::All);
}

void fme::Selections::update_configuration_key(ConfigKey key) const
{
     Configuration config{};

     using funct_t               = std::move_only_function<void(Configuration &, const Selections &) const>;
     using map_t                 = std::map<ConfigKey, funct_t>;

     static const map_t updaters = []() {
          map_t m{};
#define MAP_MACRO(KEY, FIELD_NAME) \
     m.emplace(KEY, [](Configuration &c, const Selections &s) { c->insert_or_assign(key_to_string(KEY), s.FIELD_NAME); })

#define MAP_MACRO_UNDERLYING(KEY, FIELD_NAME) \
     m.emplace(                               \
       KEY, [](Configuration &c, const Selections &s) { c->insert_or_assign(key_to_string(KEY), std::to_underlying(s.FIELD_NAME)); })

#define MAP_INSERT_OR_ASSIGN(config_key, value_expr) \
     m.emplace(config_key, [](Configuration &c, const Selections &s) { c->insert_or_assign(key_to_string(config_key), value_expr); })

#define MAP_UPDATE_ARRAY(config_key, member) \
     m.emplace(config_key, [](Configuration &c, const Selections &s) { c.update_array(key_to_string(config_key), s.member); })


          MAP_MACRO_UNDERLYING(ConfigKey::Draw, draw);
          MAP_MACRO_UNDERLYING(ConfigKey::Coo, coo);
          MAP_MACRO_UNDERLYING(ConfigKey::TileSizeValue, tile_size_value);
          MAP_MACRO_UNDERLYING(ConfigKey::BatchInputType, batch_input_type);
          MAP_MACRO_UNDERLYING(ConfigKey::BatchInputRootPathType, batch_input_root_path_type);
          MAP_MACRO_UNDERLYING(ConfigKey::BatchOutputType, batch_output_type);
          MAP_MACRO_UNDERLYING(ConfigKey::BatchOutputRootPathType, batch_output_root_path_type);
          MAP_MACRO_UNDERLYING(ConfigKey::CurrentPattern, current_pattern);
          MAP_MACRO(ConfigKey::StarterField, starter_field);
          MAP_MACRO(ConfigKey::SelectionsPath, path);
          MAP_MACRO(ConfigKey::SelectionsPath, swizzle_path);
          MAP_MACRO(ConfigKey::SelectionsPath, deswizzle_path);
          MAP_MACRO(ConfigKey::WindowWidth, window_width);
          MAP_MACRO(ConfigKey::WindowHeight, window_height);
          MAP_MACRO(ConfigKey::Palette, palette & 0xFU);
          MAP_MACRO(ConfigKey::Bpp, bpp.raw() & 3U);
          MAP_MACRO(ConfigKey::DrawGrid, draw_grid);
          MAP_MACRO(ConfigKey::DrawGrid, draw_grid);
          MAP_MACRO(ConfigKey::SelectedTile, selected_tile);
          MAP_MACRO(ConfigKey::DrawDisableBlending, draw_disable_blending);
          MAP_MACRO(ConfigKey::DrawGrid, draw_grid);
          MAP_MACRO(ConfigKey::DrawPalette, draw_palette);
          MAP_MACRO(ConfigKey::DrawSwizzle, draw_swizzle);
          MAP_MACRO(ConfigKey::RenderImportedImage, render_imported_image);
          MAP_MACRO(ConfigKey::DrawTexturePageGrid, draw_texture_page_grid);
          MAP_MACRO(ConfigKey::DrawTileConflictRects, draw_tile_conflict_rects);
          MAP_MACRO(ConfigKey::DisplayBatchWindow, display_batch_window);
          MAP_MACRO(ConfigKey::DisplayImportImage, display_import_image);
          MAP_MACRO(ConfigKey::ImportImageGrid, import_image_grid);
          MAP_MACRO(ConfigKey::DisplayHistoryWindow, display_history_window);
          MAP_MACRO(ConfigKey::DisplayControlPanelWindow, display_control_panel_window);
          MAP_MACRO(ConfigKey::DisplayDrawWindow, display_draw_window);
          MAP_MACRO(ConfigKey::DisplayCustomPathsWindow, display_custom_paths_window);
          MAP_MACRO(ConfigKey::DisplayFieldFileWindow, display_field_file_window);
          MAP_MACRO(ConfigKey::OutputSwizzlePattern, output_swizzle_pattern);
          MAP_MACRO(ConfigKey::OutputDeswizzlePattern, output_deswizzle_pattern);
          MAP_MACRO(ConfigKey::OutputMapPatternForSwizzle, output_map_pattern_for_swizzle);
          MAP_MACRO(ConfigKey::OutputMapPatternForDeswizzle, output_map_pattern_for_deswizzle);
          MAP_MACRO(ConfigKey::BatchInputLoadMap, batch_input_load_map);
          MAP_MACRO(ConfigKey::BatchSaveMap, batch_save_map);

          MAP_MACRO_UNDERLYING(ConfigKey::BatchCompactType, batch_compact_type.value());
          MAP_MACRO(ConfigKey::BatchCompactEnabled, batch_compact_type.enabled());
          MAP_MACRO_UNDERLYING(ConfigKey::BatchFlattenType, batch_flatten_type.value());
          MAP_MACRO(ConfigKey::BatchFlattenEnabled, batch_flatten_type.enabled());

          MAP_INSERT_OR_ASSIGN(ConfigKey::BackgroundColor, std::bit_cast<std::uint32_t>(s.background_color));
          MAP_INSERT_OR_ASSIGN(ConfigKey::BatchInputPath, std::string(s.batch_input_path.data()));
          MAP_INSERT_OR_ASSIGN(ConfigKey::BatchOutputPath, std::string(s.batch_output_path.data()));

          MAP_UPDATE_ARRAY(ConfigKey::PathsWithPaletteAndTexturePage, paths_with_palette_and_texture_page);
          MAP_UPDATE_ARRAY(ConfigKey::PathsWithTexturePage, paths_with_texture_page);
          MAP_UPDATE_ARRAY(ConfigKey::PathsWithPupuID, paths_with_pupu_id);
          MAP_UPDATE_ARRAY(ConfigKey::PathsNoPaletteAndTexturePage, paths_no_palette_and_texture_page);
          MAP_UPDATE_ARRAY(ConfigKey::PathsCommonUpscale, paths_common_upscale);
          MAP_UPDATE_ARRAY(ConfigKey::PathsCommonUpscaleForMaps, paths_common_upscale_for_maps);

          m.emplace(ConfigKey::BatchCompact, [](Configuration &c, const Selections &s) {
               c->insert_or_assign(key_to_string(ConfigKey::BatchCompactType), std::to_underlying(s.batch_compact_type.value()));
               c->insert_or_assign(key_to_string(ConfigKey::BatchCompactEnabled), s.batch_compact_type.enabled());
          });
          m.emplace(ConfigKey::BatchFlatten, [](Configuration &c, const Selections &s) {
               c->insert_or_assign(key_to_string(ConfigKey::BatchFlattenType), std::to_underlying(s.batch_flatten_type.value()));
               c->insert_or_assign(key_to_string(ConfigKey::BatchFlattenEnabled), s.batch_flatten_type.enabled());
          });
#undef MAP_MACRO
#undef MAP_MACRO_UNDERLYING
#undef MAP_INSERT_OR_ASSIGN
#undef MAP_UPDATE_ARRAY

          // Add more mappings here...
          return m;
     }();
     if (key == ConfigKey::All) [[unlikely]]
     {
          for (const auto &[_, funct] : updaters)
          {
               std::invoke(funct, config, *this);
          }
          config.save();// Save the single key update
     }
     else if (const auto it = updaters.find(key); it != updaters.end()) [[likely]]
     {
          std::invoke(it->second, config, *this);
          config.save();// Save the single key update
     }
     else [[unlikely]]
     {
          spdlog::error("Unknown configuration key (enum): {}", std::to_underlying(key));
     }
}


void fme::Selections::refresh_ffnx_paths()
{
     const auto      ffnx_settings_toml = std::filesystem::path(path) / "FFNx.toml";
     std::error_code error_code         = {};
     bool            exists             = std::filesystem::exists(ffnx_settings_toml, error_code);
     if (error_code)
     {
          spdlog::warn("{}:{} - {}: {} path: \"{}\"", __FILE__, __LINE__, error_code.value(), error_code.message(), ffnx_settings_toml);
          error_code.clear();
     }

     if (exists)
     {
          const auto ffnx_config = Configuration(ffnx_settings_toml);
          ffnx_mod_path          = ffnx_config["mod_path"].value_or(ffnx_mod_path);
          ffnx_override_path     = ffnx_config["override_path"].value_or(ffnx_override_path);
          ffnx_direct_mode_path  = ffnx_config["direct_mode_path"].value_or(ffnx_direct_mode_path);
     }
}
