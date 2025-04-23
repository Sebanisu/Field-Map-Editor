//
// Created by pcvii on 12/21/2022.
//
#include "Selections.hpp"
#include <spdlog/spdlog.h>
using namespace open_viii;
using namespace open_viii::graphics;
using namespace open_viii::graphics::background;
fme::Selections::Selections()
{
     Configuration const config{};
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
     output_desizzle_pattern          = config["selections_output_deswizzle_pattern"].value_or(output_desizzle_pattern);
     output_map_pattern_for_swizzle   = config["selections_output_map_pattern_for_swizzle"].value_or(output_map_pattern_for_swizzle);
     output_map_pattern_for_deswizzle = config["selections_output_map_pattern_for_deswizzle"].value_or(output_map_pattern_for_deswizzle);
     current_pattern                  = config["selections_current_pattern"].value_or(current_pattern);

     batch_input_type =
       static_cast<input_types>(config["batch_input_type"].value_or(static_cast<std::underlying_type_t<input_types>>(batch_input_type)));
     batch_input_root_path_type = static_cast<root_path_types>(
       config["batch_input_root_path_type"].value_or(static_cast<std::underlying_type_t<root_path_types>>(batch_input_root_path_type)));
      batch_output_type =
       static_cast<output_types>(config["batch_output_type"].value_or(static_cast<std::underlying_type_t<output_types>>(batch_output_type)));
     batch_output_root_path_type = static_cast<root_path_types>(
       config["batch_output_root_path_type"].value_or(static_cast<std::underlying_type_t<root_path_types>>(batch_output_root_path_type)));
     background_color =
       std::bit_cast<fme::color>(config["selections_background_color"].value_or(std::bit_cast<std::uint32_t>(background_color)));


     batch_input_path  = config["batch_input_path"].value_or(std::string(batch_input_path.data()));
     batch_output_path = config["batch_output_path"].value_or(std::string(batch_output_path.data()));
     batch_input_load_map  = config["batch_input_load_map"].value_or(batch_input_load_map);
     batch_save_map        = config["batch_save_map"].value_or(batch_save_map);


     batch_compact_type =
       decltype(batch_compact_type){ static_cast<compact_type>(config[ff_8::ConfigKeys<ff_8::FilterTag::Compact>::key_name].value_or(
                                       std::to_underlying(batch_compact_type.value()))),
                                     config[ff_8::ConfigKeys<ff_8::FilterTag::Compact>::enabled_key_name].value_or(false) };

     batch_flatten_type =
       decltype(batch_flatten_type){ static_cast<flatten_type>(config[ff_8::ConfigKeys<ff_8::FilterTag::Flatten>::key_name].value_or(
                                       std::to_underlying(batch_flatten_type.value()))),
                                     config[ff_8::ConfigKeys<ff_8::FilterTag::Flatten>::enabled_key_name].value_or(false) };

     refresh_ffnx_paths();
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
     static constexpr std::string_view default_mod_path         = "mods/Textures";
     static constexpr std::string_view default_override         = "override";
     static constexpr std::string_view default_direct_mode_path = "direct";
     if (exists)
     {
          const auto ffnx_config = Configuration(ffnx_settings_toml);
          ffnx_mod_path          = ffnx_config["mod_path"].value_or(std::string{ default_mod_path.begin(), default_mod_path.end() });
          ffnx_override_path     = ffnx_config["override_path"].value_or(std::string{ default_override.begin(), default_override.end() });
          ffnx_direct_mode_path =
            ffnx_config["direct_mode_path"].value_or(std::string{ default_direct_mode_path.begin(), default_direct_mode_path.end() });
     }
     else
     {
          ffnx_mod_path         = std::string{ default_mod_path.begin(), default_mod_path.end() };
          ffnx_override_path    = std::string{ default_override.begin(), default_override.end() };
          ffnx_direct_mode_path = std::string{ default_direct_mode_path.begin(), default_direct_mode_path.end() };
     }
}
