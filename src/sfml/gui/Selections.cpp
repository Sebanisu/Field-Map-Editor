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

void fme::Selections::load_configuration()
{
     Configuration const config{};
     starter_field            = config["starter_field"].value_or(starter_field);
     // field
     path                     = config["selections_path"].value_or(path);
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
     current_pattern                  = config["selections_current_pattern"].value_or(current_pattern);

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

     using funct_t = std::move_only_function<void(Configuration &, const Selections &) const>;
     using map_t                 = std::map<ConfigKey, funct_t>;

     static const map_t updaters = []() {
          map_t m{};
#define MAP_MACRO(KEY, STR_NAME, FIELD_NAME) \
     m.emplace(KEY, [](Configuration &c, const Selections &s) { c->insert_or_assign(STR_NAME, s.FIELD_NAME); })

#define MAP_MACRO_UNDERLYING(KEY, STR_NAME, FIELD_NAME) \
     m.emplace(KEY, [](Configuration &c, const Selections &s) { c->insert_or_assign(STR_NAME, std::to_underlying(s.FIELD_NAME)); })

          MAP_MACRO(ConfigKey::StarterField, "starter_field", starter_field);
          MAP_MACRO(ConfigKey::SelectionsPath, "selections_path", path);
          MAP_MACRO(ConfigKey::WindowWidth, "selections_window_width", window_width);
          MAP_MACRO(ConfigKey::WindowHeight, "selections_window_height", window_height);
          MAP_MACRO(ConfigKey::Palette, "selections_palette", palette & 0xFU);
          MAP_MACRO(ConfigKey::Bpp, "selections_bpp", bpp.raw() & 3U);
          MAP_MACRO(ConfigKey::DrawGrid, "selections_draw_grid", draw_grid);
          MAP_MACRO(ConfigKey::DrawGrid, "selections_draw_grid", draw_grid);


          m.emplace(ConfigKey::BackgroundColor, [](Configuration &c, const Selections &s) {
               c->insert_or_assign("selections_background_color", std::bit_cast<std::uint32_t>(s.background_color));
          });

          MAP_MACRO_UNDERLYING(ConfigKey::Draw, "selections_draw", draw);
          MAP_MACRO_UNDERLYING(ConfigKey::Coo, "selections_coo", coo);
          MAP_MACRO_UNDERLYING(ConfigKey::TileSizeValue, "selections_tile_size_value", tile_size_value);
          MAP_MACRO_UNDERLYING(ConfigKey::BatchInputType, "batch_input_type", batch_input_type);
          MAP_MACRO_UNDERLYING(ConfigKey::BatchInputRootPathType, "batch_input_root_path_type", batch_input_root_path_type);
          MAP_MACRO_UNDERLYING(ConfigKey::BatchOutputType, "batch_output_type", batch_output_type);
          MAP_MACRO_UNDERLYING(ConfigKey::BatchOutputRootPathType, "batch_output_root_path_type", batch_output_root_path_type);
          MAP_MACRO(ConfigKey::SelectedTile, "selections_selected_tile", selected_tile);
          MAP_MACRO(ConfigKey::DrawDisableBlending, "selections_draw_disable_blending", draw_disable_blending);
          MAP_MACRO(ConfigKey::DrawGrid, "selections_draw_grid", draw_grid);
          MAP_MACRO(ConfigKey::DrawPalette, "selections_draw_palette", draw_palette);
          MAP_MACRO(ConfigKey::DrawSwizzle, "selections_draw_swizzle", draw_swizzle);
          MAP_MACRO(ConfigKey::RenderImportedImage, "selections_render_imported_image", render_imported_image);
          MAP_MACRO(ConfigKey::DrawTexturePageGrid, "selections_draw_texture_page_grid", draw_texture_page_grid);
          MAP_MACRO(ConfigKey::DrawTileConflictRects, "selections_draw_tile_conflict_rects", draw_tile_conflict_rects);
          MAP_MACRO(ConfigKey::DisplayBatchWindow, "selections_display_batch_window", display_batch_window);
          MAP_MACRO(ConfigKey::DisplayImportImage, "selections_display_import_image", display_import_image);
          MAP_MACRO(ConfigKey::ImportImageGrid, "selections_import_image_grid", import_image_grid);
          MAP_MACRO(ConfigKey::DisplayHistoryWindow, "selections_display_history_window", display_history_window);
          MAP_MACRO(ConfigKey::DisplayControlPanelWindow, "selections_display_control_panel_window", display_control_panel_window);
          MAP_MACRO(ConfigKey::DisplayDrawWindow, "selections_display_draw_window", display_draw_window);
          MAP_MACRO(ConfigKey::DisplayCustomPathsWindow, "selections_display_custom_paths_window", display_custom_paths_window);
          MAP_MACRO(ConfigKey::DisplayFieldFileWindow, "selections_display_field_file_window", display_field_file_window);
          MAP_MACRO(ConfigKey::OutputSwizzlePattern, "selections_output_swizzle_pattern", output_swizzle_pattern);
          MAP_MACRO(ConfigKey::OutputDeswizzlePattern, "selections_output_deswizzle_pattern", output_deswizzle_pattern);
          MAP_MACRO(ConfigKey::OutputMapPatternForSwizzle, "selections_output_map_pattern_for_swizzle", output_map_pattern_for_swizzle);
          MAP_MACRO(
            ConfigKey::OutputMapPatternForDeswizzle, "selections_output_map_pattern_for_deswizzle", output_map_pattern_for_deswizzle);
          MAP_MACRO(ConfigKey::CurrentPattern, "selections_current_pattern", current_pattern);
          MAP_MACRO(ConfigKey::BatchInputLoadMap, "batch_input_load_map", batch_input_load_map);
          MAP_MACRO(ConfigKey::BatchSaveMap, "batch_save_map", batch_save_map);

          m.emplace(ConfigKey::BackgroundColor, [](Configuration &c, const Selections &s) {
               c->insert_or_assign("selections_background_color", std::bit_cast<std::uint32_t>(s.background_color));
          });
          m.emplace(ConfigKey::BatchInputPath, [](Configuration &c, const Selections &s) {
               c->insert_or_assign("batch_input_path", std::string(s.batch_input_path.data()));
          });
          m.emplace(ConfigKey::BatchOutputPath, [](Configuration &c, const Selections &s) {
               c->insert_or_assign("batch_output_path", std::string(s.batch_output_path.data()));
          });
          m.emplace(ConfigKey::PathsWithPaletteAndTexturePage, [](Configuration &c, const Selections &s) {
               c.update_array("paths_with_palette_and_texture_page", s.paths_with_palette_and_texture_page);
          });
          m.emplace(ConfigKey::PathsWithTtexturePage, [](Configuration &c, const Selections &s) {
               c.update_array("paths_with_texture_page", s.paths_with_texture_page);
          });
          m.emplace(ConfigKey::PathsWithPupuID, [](Configuration &c, const Selections &s) {
               c.update_array("paths_with_pupu_id", s.paths_with_pupu_id);
          });
          m.emplace(ConfigKey::PathsNoPaletteAndTexturePage, [](Configuration &c, const Selections &s) {
               c.update_array("paths_no_palette_and_texture_page", s.paths_no_palette_and_texture_page);
          });
          m.emplace(ConfigKey::PathsCommonUpscale, [](Configuration &c, const Selections &s) {
               c.update_array("paths_common_upscale", s.paths_common_upscale);
          });
          m.emplace(ConfigKey::PathsCommonUpscaleForMaps, [](Configuration &c, const Selections &s) {
               c.update_array("paths_common_upscale_for_maps", s.paths_common_upscale_for_maps);
          });

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
          // updaters.at(key)(config, *this);
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
