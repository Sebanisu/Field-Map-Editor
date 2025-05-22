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
          map_t m{};
          m.emplace(ConfigKey::BackgroundColor, "selections_background_color");
          m.emplace(ConfigKey::BatchCompactEnabled, "batch_compact_enabled");
          m.emplace(ConfigKey::BatchCompactType, "batch_compact_type");
          m.emplace(ConfigKey::BatchFlattenEnabled, "batch_flatten_enabled");
          m.emplace(ConfigKey::BatchFlattenType, "batch_flatten_type");
          m.emplace(ConfigKey::BatchInputLoadMap, "batch_input_load_map");
          m.emplace(ConfigKey::BatchInputPath, "batch_input_path");
          m.emplace(ConfigKey::BatchInputRootPathType, "batch_input_root_path_type");
          m.emplace(ConfigKey::BatchInputType, "batch_input_type");
          m.emplace(ConfigKey::BatchOutputPath, "batch_output_path");
          m.emplace(ConfigKey::BatchOutputRootPathType, "batch_output_root_path_type");
          m.emplace(ConfigKey::BatchOutputType, "batch_output_type");
          m.emplace(ConfigKey::BatchOutputSaveMap, "batch_output_save_map");
          m.emplace(ConfigKey::Bpp, "selections_bpp");
          m.emplace(ConfigKey::Coo, "selections_coo");
          m.emplace(ConfigKey::CurrentPattern, "selections_current_pattern");
          m.emplace(ConfigKey::DeswizzlePath, "selections_deswizzle_path");
          m.emplace(ConfigKey::DisplayBatchWindow, "selections_display_batch_window");
          m.emplace(ConfigKey::DisplayControlPanelWindow, "selections_display_control_panel_window");
          m.emplace(ConfigKey::DisplayCustomPathsWindow, "selections_display_custom_paths_window");
          m.emplace(ConfigKey::DisplayDrawWindow, "selections_display_draw_window");
          m.emplace(ConfigKey::DisplayFieldFileWindow, "selections_display_field_file_window");
          m.emplace(ConfigKey::DisplayHistoryWindow, "selections_display_history_window");
          m.emplace(ConfigKey::DisplayImportImage, "selections_display_import_image");
          m.emplace(ConfigKey::Draw, "selections_draw");
          m.emplace(ConfigKey::DrawDisableBlending, "selections_draw_disable_blending");
          m.emplace(ConfigKey::DrawGrid, "selections_draw_grid");
          m.emplace(ConfigKey::DrawPalette, "selections_draw_palette");
          m.emplace(ConfigKey::DrawSwizzle, "selections_draw_swizzle");
          m.emplace(ConfigKey::DrawTexturePageGrid, "selections_draw_texture_page_grid");
          m.emplace(ConfigKey::DrawTileConflictRects, "selections_draw_tile_conflict_rects");
          m.emplace(ConfigKey::ImportImageGrid, "selections_import_image_grid");
          m.emplace(ConfigKey::ImportLoadImageDirectory, "import_load_image_directory");
          m.emplace(ConfigKey::OutputDeswizzlePattern, "selections_output_deswizzle_pattern");
          m.emplace(ConfigKey::OutputImagePath, "output_image_path");
          m.emplace(ConfigKey::OutputMapPatternForDeswizzle, "selections_output_map_pattern_for_deswizzle");
          m.emplace(ConfigKey::OutputMapPatternForSwizzle, "selections_output_map_pattern_for_swizzle");
          m.emplace(ConfigKey::OutputMapPath, "output_map_path");
          m.emplace(ConfigKey::OutputMimPath, "output_mim_path");
          m.emplace(ConfigKey::OutputSwizzlePattern, "selections_output_swizzle_pattern");
          m.emplace(ConfigKey::Palette, "selections_palette");
          m.emplace(ConfigKey::PathPatternsCommonUpscale, "paths_common_upscale");
          m.emplace(ConfigKey::PathPatternsCommonUpscaleForMaps, "paths_common_upscale_for_maps");
          m.emplace(ConfigKey::PathPatternsNoPaletteAndTexturePage, "paths_no_palette_and_texture_page");
          m.emplace(ConfigKey::PathPatternsWithPaletteAndTexturePage, "paths_with_palette_and_texture_page");
          m.emplace(ConfigKey::PathPatternsWithPupuID, "paths_with_pupu_id");
          m.emplace(ConfigKey::PathPatternsWithTexturePage, "paths_with_texture_page");
          m.emplace(ConfigKey::PathsVector, "paths_vector");
          m.emplace(ConfigKey::PathsVectorUpscale, "custom_upscale_paths_vector");
          m.emplace(ConfigKey::PathsVectorDeswizzle, "paths_vector_deswizzle");
          m.emplace(ConfigKey::RenderImportedImage, "selections_render_imported_image");
          m.emplace(ConfigKey::SelectedTile, "selections_selected_tile");
          m.emplace(ConfigKey::SelectionsPath, "selections_path");
          m.emplace(ConfigKey::StarterField, "starter_field");
          m.emplace(ConfigKey::SwizzlePath, "selections_swizzle_path");
          m.emplace(ConfigKey::TileSizeValue, "selections_tile_size_value");
          m.emplace(ConfigKey::UpscalePathsIndex, "upscale_paths_index");
          m.emplace(ConfigKey::WindowHeight, "selections_window_height");
          m.emplace(ConfigKey::WindowWidth, "selections_window_width");
          return m;
     }();

     if (const auto it = updaters.find(key); it != updaters.end()) [[likely]]
     {
          return it->second;
     }
     else [[unlikely]]
     {
          spdlog::error("{}:{} Unknown configuration key (ConfigKey): {}", __FILE__, __LINE__, std::to_underlying(key));
          return {};
     }
}

void fme::Selections::load_configuration()
{
     Configuration const config{};
     background_color = std::bit_cast<fme::color>(
       config[key_to_string(ConfigKey::BackgroundColor)].value_or(std::bit_cast<std::uint32_t>(background_color)));

     batch_input_path           = config[key_to_string(ConfigKey::BatchInputPath)].value_or(batch_input_path);
     batch_input_root_path_type = static_cast<root_path_types>(config[key_to_string(ConfigKey::BatchInputRootPathType)].value_or(
       static_cast<std::underlying_type_t<root_path_types>>(batch_input_root_path_type)));
     batch_input_type           = static_cast<input_types>(
       config[key_to_string(ConfigKey::BatchInputType)].value_or(static_cast<std::underlying_type_t<input_types>>(batch_input_type)));
     batch_input_load_map        = config[key_to_string(ConfigKey::BatchInputLoadMap)].value_or(batch_input_load_map);

     batch_output_path           = config[key_to_string(ConfigKey::BatchOutputPath)].value_or(batch_output_path);
     batch_output_root_path_type = static_cast<root_path_types>(config[key_to_string(ConfigKey::BatchOutputRootPathType)].value_or(
       static_cast<std::underlying_type_t<root_path_types>>(batch_output_root_path_type)));
     batch_output_type           = static_cast<output_types>(
       config[key_to_string(ConfigKey::BatchOutputType)].value_or(static_cast<std::underlying_type_t<output_types>>(batch_output_type)));
     batch_output_save_map = config[key_to_string(ConfigKey::BatchOutputSaveMap)].value_or(batch_output_save_map);

     bpp                   = BPPT{ config[key_to_string(ConfigKey::Bpp)].value_or(bpp.raw()) & 3U };

     coo                   = static_cast<LangT>(config[key_to_string(ConfigKey::Coo)].value_or(std::to_underlying(coo)));

     current_pattern       = static_cast<decltype(current_pattern)>(
       config[key_to_string(ConfigKey::CurrentPattern)].value_or(std::to_underlying(current_pattern)));

     deswizzle_path               = config[key_to_string(ConfigKey::DeswizzlePath)].value_or(deswizzle_path);

     display_batch_window         = config[key_to_string(ConfigKey::DisplayBatchWindow)].value_or(display_batch_window);
     display_control_panel_window = config[key_to_string(ConfigKey::DisplayControlPanelWindow)].value_or(display_control_panel_window);
     display_custom_paths_window  = config[key_to_string(ConfigKey::DisplayCustomPathsWindow)].value_or(display_custom_paths_window);
     display_draw_window          = config[key_to_string(ConfigKey::DisplayDrawWindow)].value_or(display_draw_window);
     display_field_file_window    = config[key_to_string(ConfigKey::DisplayFieldFileWindow)].value_or(display_field_file_window);
     display_history_window       = config[key_to_string(ConfigKey::DisplayHistoryWindow)].value_or(display_history_window);
     display_import_image         = config[key_to_string(ConfigKey::DisplayImportImage)].value_or(display_import_image);

     draw                         = static_cast<draw_mode>(config[key_to_string(ConfigKey::Draw)].value_or(std::to_underlying(draw)));
     draw_disable_blending        = config[key_to_string(ConfigKey::DrawDisableBlending)].value_or(draw_disable_blending);
     draw_grid                    = config[key_to_string(ConfigKey::DrawGrid)].value_or(draw_grid);
     draw_palette                 = config[key_to_string(ConfigKey::DrawPalette)].value_or(draw_palette);
     draw_swizzle                 = config[key_to_string(ConfigKey::DrawSwizzle)].value_or(draw_swizzle);
     draw_texture_page_grid       = config[key_to_string(ConfigKey::DrawTexturePageGrid)].value_or(draw_texture_page_grid);
     draw_tile_conflict_rects     = config[key_to_string(ConfigKey::DrawTileConflictRects)].value_or(draw_tile_conflict_rects);

     import_image_grid            = config[key_to_string(ConfigKey::ImportImageGrid)].value_or(import_image_grid);
     import_load_image_directory  = config[key_to_string(ConfigKey::ImportLoadImageDirectory)].value_or(import_load_image_directory);

     output_deswizzle_pattern     = config[key_to_string(ConfigKey::OutputDeswizzlePattern)].value_or(output_deswizzle_pattern);
     output_map_pattern_for_deswizzle =
       config[key_to_string(ConfigKey::OutputMapPatternForDeswizzle)].value_or(output_map_pattern_for_deswizzle);
     output_map_pattern_for_swizzle = config[key_to_string(ConfigKey::OutputMapPatternForSwizzle)].value_or(output_map_pattern_for_swizzle);
     output_swizzle_pattern         = config[key_to_string(ConfigKey::OutputSwizzlePattern)].value_or(output_swizzle_pattern);

     palette                        = config[key_to_string(ConfigKey::Palette)].value_or(palette) & 0xFU;

     path                           = config[key_to_string(ConfigKey::SelectionsPath)].value_or(path);

     output_map_path                = config[key_to_string(ConfigKey::OutputMapPath)].value_or(output_map_path);
     output_mim_path                = config[key_to_string(ConfigKey::OutputMimPath)].value_or(output_mim_path);
     output_image_path              = config[key_to_string(ConfigKey::OutputImagePath)].value_or(output_image_path);

     render_imported_image          = config[key_to_string(ConfigKey::RenderImportedImage)].value_or(render_imported_image);

     selected_tile                  = config[key_to_string(ConfigKey::SelectedTile)].value_or(selected_tile);

     starter_field                  = config[key_to_string(ConfigKey::StarterField)].value_or(starter_field);

     swizzle_path                   = config[key_to_string(ConfigKey::SwizzlePath)].value_or(swizzle_path);

     tile_size_value =
       static_cast<tile_sizes>(config[key_to_string(ConfigKey::TileSizeValue)].value_or(std::to_underlying(tile_size_value)));

     upscale_paths_index = config[key_to_string(ConfigKey::UpscalePathsIndex)].value_or(upscale_paths_index);

     window_height       = config[key_to_string(ConfigKey::WindowHeight)].value_or(window_height);
     window_width        = config[key_to_string(ConfigKey::WindowWidth)].value_or(window_width);

     // Arrays
     if (config.load_array(key_to_string(ConfigKey::PathPatternsCommonUpscale), paths_common_upscale))
     {
          assert(has_balanced_braces(paths_common_upscale));
     }
     if (config.load_array(key_to_string(ConfigKey::PathPatternsCommonUpscaleForMaps), paths_common_upscale_for_maps))
     {
          assert(has_balanced_braces(paths_common_upscale_for_maps));
     }
     if (config.load_array(key_to_string(ConfigKey::PathPatternsNoPaletteAndTexturePage), paths_no_palette_and_texture_page))
     {
          assert(has_balanced_braces(paths_no_palette_and_texture_page));
     }
     if (config.load_array(key_to_string(ConfigKey::PathPatternsWithPaletteAndTexturePage), paths_with_palette_and_texture_page))
     {
          assert(has_balanced_braces(paths_with_palette_and_texture_page));
     }
     if (config.load_array(key_to_string(ConfigKey::PathPatternsWithPupuID), paths_with_pupu_id))
     {
          assert(has_balanced_braces(paths_with_pupu_id));
     }
     if (config.load_array(key_to_string(ConfigKey::PathPatternsWithTexturePage), paths_with_texture_page))
     {
          assert(has_balanced_braces(paths_with_texture_page));
     }
     if (config.load_array(key_to_string(ConfigKey::PathsVector), paths_vector))
     {
          // assert(has_balanced_braces(paths_vector));
     }
     if (config.load_array(key_to_string(ConfigKey::PathsVectorUpscale), paths_vector_upscale))
     {
          // assert(has_balanced_braces(paths_vector_upscale));
     }
     if (config.load_array(key_to_string(ConfigKey::PathsVectorDeswizzle), paths_vector_deswizzle))
     {
          // assert(has_balanced_braces(paths_vector_deswizzle));
     }
     if (paths_vector.empty())
     {
          paths_vector = []() -> std::vector<std::string> {
               const auto &default_paths = open_viii::Paths::get();
               return { default_paths.begin(), default_paths.end() };
          }();
     }


     refresh_ffnx_paths();
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

#define MAP_INSERT_OR_ASSIGN(KEY, FIELD_NAME) \
     m.emplace(KEY, [](Configuration &c, const Selections &s) { c->insert_or_assign(key_to_string(KEY), FIELD_NAME); })

#define MAP_UPDATE_ARRAY(KEY, FIELD_NAME) \
     m.emplace(KEY, [](Configuration &c, const Selections &s) { c.update_array(key_to_string(KEY), s.FIELD_NAME); })

          MAP_INSERT_OR_ASSIGN(ConfigKey::BackgroundColor, std::bit_cast<std::uint32_t>(s.background_color));
          MAP_MACRO(ConfigKey::BatchCompactEnabled, batch_compact_type.enabled());
          MAP_MACRO_UNDERLYING(ConfigKey::BatchCompactType, batch_compact_type.value());
          MAP_MACRO_UNDERLYING(ConfigKey::BatchFlattenType, batch_flatten_type.value());
          MAP_MACRO(ConfigKey::BatchFlattenEnabled, batch_flatten_type.enabled());
          MAP_INSERT_OR_ASSIGN(ConfigKey::BatchInputPath, std::string(s.batch_input_path.data()));
          MAP_MACRO_UNDERLYING(ConfigKey::BatchInputRootPathType, batch_input_root_path_type);
          MAP_MACRO(ConfigKey::BatchInputLoadMap, batch_input_load_map);
          MAP_MACRO_UNDERLYING(ConfigKey::BatchInputType, batch_input_type);
          MAP_INSERT_OR_ASSIGN(ConfigKey::BatchOutputPath, std::string(s.batch_output_path.data()));
          MAP_MACRO_UNDERLYING(ConfigKey::BatchOutputRootPathType, batch_output_root_path_type);
          MAP_MACRO(ConfigKey::BatchOutputSaveMap, batch_output_save_map);
          MAP_MACRO_UNDERLYING(ConfigKey::BatchOutputType, batch_output_type);
          MAP_MACRO(ConfigKey::Bpp, bpp.raw() & 3U);
          MAP_MACRO_UNDERLYING(ConfigKey::Coo, coo);
          MAP_MACRO_UNDERLYING(ConfigKey::CurrentPattern, current_pattern);
          MAP_MACRO(ConfigKey::DeswizzlePath, deswizzle_path);
          MAP_MACRO(ConfigKey::DisplayBatchWindow, display_batch_window);
          MAP_MACRO(ConfigKey::DisplayControlPanelWindow, display_control_panel_window);
          MAP_MACRO(ConfigKey::DisplayCustomPathsWindow, display_custom_paths_window);
          MAP_MACRO(ConfigKey::DisplayDrawWindow, display_draw_window);
          MAP_MACRO(ConfigKey::DisplayFieldFileWindow, display_field_file_window);
          MAP_MACRO(ConfigKey::DisplayHistoryWindow, display_history_window);
          MAP_MACRO(ConfigKey::DisplayImportImage, display_import_image);
          MAP_MACRO_UNDERLYING(ConfigKey::Draw, draw);
          MAP_MACRO(ConfigKey::DrawDisableBlending, draw_disable_blending);
          MAP_MACRO(ConfigKey::DrawGrid, draw_grid);
          MAP_MACRO(ConfigKey::DrawPalette, draw_palette);
          MAP_MACRO(ConfigKey::DrawSwizzle, draw_swizzle);
          MAP_MACRO(ConfigKey::DrawTexturePageGrid, draw_texture_page_grid);
          MAP_MACRO(ConfigKey::DrawTileConflictRects, draw_tile_conflict_rects);
          MAP_MACRO(ConfigKey::ImportImageGrid, import_image_grid);
          MAP_MACRO(ConfigKey::ImportLoadImageDirectory, import_load_image_directory);
          MAP_MACRO(ConfigKey::OutputDeswizzlePattern, output_deswizzle_pattern);
          MAP_MACRO(ConfigKey::OutputImagePath, output_image_path);
          MAP_MACRO(ConfigKey::OutputMapPatternForDeswizzle, output_map_pattern_for_deswizzle);
          MAP_MACRO(ConfigKey::OutputMapPatternForSwizzle, output_map_pattern_for_swizzle);
          MAP_MACRO(ConfigKey::OutputMapPath, output_map_path);
          MAP_MACRO(ConfigKey::OutputMimPath, output_mim_path);
          MAP_MACRO(ConfigKey::OutputSwizzlePattern, output_swizzle_pattern);
          MAP_MACRO(ConfigKey::Palette, palette & 0xFU);
          MAP_UPDATE_ARRAY(ConfigKey::PathPatternsCommonUpscale, paths_common_upscale);
          MAP_UPDATE_ARRAY(ConfigKey::PathPatternsCommonUpscaleForMaps, paths_common_upscale_for_maps);
          MAP_UPDATE_ARRAY(ConfigKey::PathPatternsNoPaletteAndTexturePage, paths_no_palette_and_texture_page);
          MAP_UPDATE_ARRAY(ConfigKey::PathPatternsWithPaletteAndTexturePage, paths_with_palette_and_texture_page);
          MAP_UPDATE_ARRAY(ConfigKey::PathPatternsWithPupuID, paths_with_pupu_id);
          MAP_UPDATE_ARRAY(ConfigKey::PathPatternsWithTexturePage, paths_with_texture_page);
          MAP_UPDATE_ARRAY(ConfigKey::PathsVector, paths_vector);
          MAP_UPDATE_ARRAY(ConfigKey::PathsVectorUpscale, paths_vector_upscale);
          MAP_UPDATE_ARRAY(ConfigKey::PathsVectorDeswizzle, paths_vector_deswizzle);
          MAP_MACRO(ConfigKey::RenderImportedImage, render_imported_image);
          MAP_MACRO(ConfigKey::SelectedTile, selected_tile);
          MAP_MACRO(ConfigKey::SelectionsPath, path);
          MAP_MACRO(ConfigKey::StarterField, starter_field);
          MAP_MACRO(ConfigKey::SwizzlePath, swizzle_path);
          MAP_MACRO_UNDERLYING(ConfigKey::TileSizeValue, tile_size_value);
          MAP_MACRO(ConfigKey::UpscalePathsIndex, upscale_paths_index);
          MAP_MACRO(ConfigKey::WindowHeight, window_height);
          MAP_MACRO(ConfigKey::WindowWidth, window_width);

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
          spdlog::error("{}:{} Unknown configuration key (ConfigKey): {}", __FILE__, __LINE__, std::to_underlying(key));
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
