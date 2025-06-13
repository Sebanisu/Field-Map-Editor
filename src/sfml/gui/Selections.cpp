//
// Created by pcvii on 12/21/2022.
//
#include "Selections.hpp"
#include "formatters.hpp"
#include <spdlog/spdlog.h>
using namespace open_viii;
using namespace open_viii::graphics;
using namespace open_viii::graphics::background;
using namespace std::string_literals;
using namespace std::string_view_literals;


fme::Selections::Selections(const Configuration config)
  : batch_compact_type(
      static_cast<fme::compact_type>(config[key_to_string(ConfigKey::BatchCompactType)].value_or(std::to_underlying(fme::compact_type{}))),
      ff_8::WithFlag(
        ff_8::FilterSettings::Default,
        ff_8::FilterSettings::Toggle_Enabled,
        config[key_to_string(ConfigKey::BatchCompactEnabled)].value_or(false)))
  , batch_flatten_type(
      static_cast<fme::flatten_type>(config[key_to_string(ConfigKey::BatchFlattenType)].value_or(std::to_underlying(fme::flatten_type{}))),
      ff_8::WithFlag(
        ff_8::FilterSettings::Default,
        ff_8::FilterSettings::Toggle_Enabled,
        config[key_to_string(ConfigKey::BatchFlattenEnabled)].value_or(false)))
{
     const auto start_time = std::chrono::system_clock::now();
     if (auto opt_path = config[key_to_string(ConfigKey::SelectionsPath)].value<std::string>(); opt_path.has_value())
     {
          path = std::move(opt_path.value());
     }
     else
     {
          std::error_code error_code = {};
          std::string     str        = std::filesystem::current_path(error_code).string();
          if (error_code)
          {
               spdlog::warn("{}:{} - {}: {} path: \"{}\"", __FILE__, __LINE__, error_code.value(), error_code.message(), str);
               error_code.clear();
          }
          path = str;
     }

     background_color = std::bit_cast<fme::color>(
       config[key_to_string(ConfigKey::BackgroundColor)].value_or(std::bit_cast<std::uint32_t>(fme::colors::White)));

     batch_input_path           = config[key_to_string(ConfigKey::BatchInputPath)].value_or(std::string{});
     batch_input_root_path_type = static_cast<root_path_types>(config[key_to_string(ConfigKey::BatchInputRootPathType)].value_or(
       static_cast<std::underlying_type_t<root_path_types>>(root_path_types{})));
     batch_input_type           = static_cast<input_types>(
       config[key_to_string(ConfigKey::BatchInputType)].value_or(static_cast<std::underlying_type_t<input_types>>(input_types{})));
     batch_input_load_map        = config[key_to_string(ConfigKey::BatchInputLoadMap)].value_or(false);

     batch_output_path           = config[key_to_string(ConfigKey::BatchOutputPath)].value_or(std::string{});
     batch_output_root_path_type = static_cast<root_path_types>(
       config[key_to_string(ConfigKey::BatchOutputRootPathType)].value_or(std::to_underlying(root_path_types{})));
     batch_output_type =
       static_cast<output_types>(config[key_to_string(ConfigKey::BatchOutputType)].value_or(std::to_underlying(output_types{})));
     batch_output_save_map = config[key_to_string(ConfigKey::BatchOutputSaveMap)].value_or(true);

     bpp                   = BPPT{ config[key_to_string(ConfigKey::Bpp)].value_or(BPPT{}.raw()) & 3U };

     coo                   = static_cast<LangT>(config[key_to_string(ConfigKey::Coo)].value_or(std::to_underlying(LangT{})));

     current_pattern =
       static_cast<PatternSelector>(config[key_to_string(ConfigKey::CurrentPattern)].value_or(std::to_underlying(PatternSelector{})));
     current_pattern_index        = config[key_to_string(ConfigKey::CurrentPatternIndex)].value_or(int{ -1 });

     deswizzle_path               = config[key_to_string(ConfigKey::DeswizzlePath)].value_or(path);

     display_batch_window         = config[key_to_string(ConfigKey::DisplayBatchWindow)].value_or(false);
     display_control_panel_window = config[key_to_string(ConfigKey::DisplayControlPanelWindow)].value_or(true);
     display_custom_paths_window  = config[key_to_string(ConfigKey::DisplayCustomPathsWindow)].value_or(false);
     display_draw_window          = config[key_to_string(ConfigKey::DisplayDrawWindow)].value_or(true);
     display_field_file_window    = config[key_to_string(ConfigKey::DisplayFieldFileWindow)].value_or(false);
     display_history_window       = config[key_to_string(ConfigKey::DisplayHistoryWindow)].value_or(false);
     display_import_image         = config[key_to_string(ConfigKey::DisplayImportImage)].value_or(false);

     draw = static_cast<draw_mode>(config[key_to_string(ConfigKey::Draw)].value_or(std::to_underlying(draw_mode::draw_map)));
     draw_disable_blending       = config[key_to_string(ConfigKey::DrawDisableBlending)].value_or(false);
     draw_grid                   = config[key_to_string(ConfigKey::DrawGrid)].value_or(false);
     draw_palette                = config[key_to_string(ConfigKey::DrawPalette)].value_or(false);
     draw_swizzle                = config[key_to_string(ConfigKey::DrawSwizzle)].value_or(false);
     draw_texture_page_grid      = config[key_to_string(ConfigKey::DrawTexturePageGrid)].value_or(false);
     draw_tile_conflict_rects    = config[key_to_string(ConfigKey::DrawTileConflictRects)].value_or(false);

     import_image_grid           = config[key_to_string(ConfigKey::ImportImageGrid)].value_or(false);
     import_load_image_directory = config[key_to_string(ConfigKey::ImportLoadImageDirectory)].value_or(path);

     output_deswizzle_pattern =
       config[key_to_string(ConfigKey::OutputDeswizzlePattern)].value_or("{selected_path}\\deswizzle\\{demaster}"s);
     output_map_pattern_for_deswizzle =
       config[key_to_string(ConfigKey::OutputMapPatternForDeswizzle)].value_or("{selected_path}\\deswizzle\\{demaster}"s);
     output_map_pattern_for_swizzle = config[key_to_string(ConfigKey::OutputMapPatternForSwizzle)].value_or("{selected_path}\\{demaster}"s);
     output_swizzle_pattern         = config[key_to_string(ConfigKey::OutputSwizzlePattern)].value_or("{selected_path}\\{demaster}"s);

     palette                        = config[key_to_string(ConfigKey::Palette)].value_or(std::uint8_t{}) & 0xFU;

     output_map_path                = config[key_to_string(ConfigKey::OutputMapPath)].value_or(path);
     output_mim_path                = config[key_to_string(ConfigKey::OutputMimPath)].value_or(path);
     output_image_path              = config[key_to_string(ConfigKey::OutputImagePath)].value_or(path);

     render_imported_image          = config[key_to_string(ConfigKey::RenderImportedImage)].value_or(false);

     selected_tile                  = config[key_to_string(ConfigKey::SelectedTile)].value_or(-1);

     starter_field                  = config[key_to_string(ConfigKey::StarterField)].value_or("ecenter3"s);

     swizzle_path                   = config[key_to_string(ConfigKey::SwizzlePath)].value_or(path);

     tile_size_value =
       static_cast<tile_sizes>(config[key_to_string(ConfigKey::TileSizeValue)].value_or(std::to_underlying(tile_sizes::default_size)));

     upscale_paths_index = config[key_to_string(ConfigKey::UpscalePathsIndex)].value_or(int{});

     window_height       = config[key_to_string(ConfigKey::WindowHeight)].value_or(window_height_default);
     window_width        = config[key_to_string(ConfigKey::WindowWidth)].value_or(window_width_default);


     // Arrays
     if (!config.load_array(key_to_string(ConfigKey::PathPatternsCommonUpscale), paths_common_upscale))
     {
          paths_common_upscale = { "{selected_path}",

                                   "{selected_path}/{ffnx_mod_path}/field/mapdata/",
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
     }
     assert(has_balanced_braces(paths_common_upscale));

     if (!config.load_array(key_to_string(ConfigKey::PathPatternsCommonUpscaleForMaps), paths_common_upscale_for_maps))
     {
          // todo ffnx uses a sepperate directory for map files which means we might not see it with our
          // current method of selecting one path ffnx_direct_mode_path might not want to be in the regular
          // paths list might need to be somewhere else. maybe a get paths map.
          paths_common_upscale_for_maps = { "{selected_path}/{ffnx_direct_mode_path}/field/mapdata/" };
     }
     assert(has_balanced_braces(paths_common_upscale_for_maps));

     if (!config.load_array(key_to_string(ConfigKey::PathPatternsNoPaletteAndTexturePage), paths_no_palette_and_texture_page))
     {
          paths_no_palette_and_texture_page = { "{selected_path}/{field_name}{_{2_letter_lang}}{ext}",
                                                "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}{ext}",
                                                "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}{ext}",

                                                "{selected_path}/{field_name}{ext}",
                                                "{selected_path}/{field_name}/{field_name}{ext}",
                                                "{selected_path}/{field_prefix}/{field_name}/{field_name}{ext}",

                                                "{selected_path}/{demaster}",

                                                "{selected_path}/{ffnx_multi_texture}",
                                                "{selected_path}/{ffnx_single_texture}",
                                                "{selected_path}/{ffnx_map}" };
     }
     assert(has_balanced_braces(paths_no_palette_and_texture_page));

     if (!config.load_array(key_to_string(ConfigKey::PathPatternsWithPaletteAndTexturePage), paths_with_palette_and_texture_page))
     {
          paths_with_palette_and_texture_page = {
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
     }
     assert(has_balanced_braces(paths_with_palette_and_texture_page));

     if (!config.load_array(key_to_string(ConfigKey::PathPatternsWithPupuID), paths_with_pupu_id))
     {
          paths_with_pupu_id = { "{selected_path}/{field_name}_{pupu_id}{ext}",
                                 "{selected_path}/{field_name}/{field_name}_{pupu_id}{ext}",
                                 "{selected_path}/{field_prefix}/{field_name}/{field_name}_{pupu_id}{ext}" };
     }
     assert(has_balanced_braces(paths_with_pupu_id));

     if (!config.load_array(key_to_string(ConfigKey::PathPatternsWithTexturePage), paths_with_texture_page))
     {
          paths_with_texture_page = { "{selected_path}/{field_name}{_{2_letter_lang}}_0{texture_page}{ext}",
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
     }
     assert(has_balanced_braces(paths_with_texture_page));

     if (!config.load_array(key_to_string(ConfigKey::PathsVector), paths_vector))
     {
          const auto &default_paths = open_viii::Paths::get();
          paths_vector              = { default_paths.begin(), default_paths.end() };
     }
     assert(has_balanced_braces(paths_vector));

     if (!config.load_array(key_to_string(ConfigKey::PathsVectorUpscale), paths_vector_upscale))
     {
          paths_vector_deswizzle = { R"(D:\Angelwing-Ultima_Remastered_v1-0-a)" };
     }
     assert(has_balanced_braces(paths_vector_upscale));

     if (!config.load_array(key_to_string(ConfigKey::PathsVectorUpscaleMap), paths_vector_upscale_map))
     {
     }
     assert(has_balanced_braces(paths_vector_upscale_map));

     if (!config.load_array(key_to_string(ConfigKey::PathsVectorDeswizzle), paths_vector_deswizzle))
     {
          paths_vector_deswizzle = { R"(D:\dev\Field-Map-Editor\bin\RelWithDebInfo\deswizzle)" };
     }
     assert(has_balanced_braces(paths_vector_deswizzle));
     if (!config.load_array(key_to_string(ConfigKey::PathsVectorDeswizzleMap), paths_vector_deswizzle_map))
     {
     }
     assert(has_balanced_braces(paths_vector_deswizzle_map));

     refresh_ffnx_paths();

     const auto end_time    = std::chrono::system_clock::now();
     const auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
     spdlog::info("Configuration Load Time: {:.3f} ms", static_cast<float>(duration_us) / 1000.0F);
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
          // MAP_MACRO(ConfigKey::BatchCompactEnabled, batch_compact_type.enabled());
          // MAP_MACRO_UNDERLYING(ConfigKey::BatchCompactType, batch_compact_type.value());
          // MAP_MACRO_UNDERLYING(ConfigKey::BatchFlattenType, batch_flatten_type.value());
          // MAP_MACRO(ConfigKey::BatchFlattenEnabled, batch_flatten_type.enabled());
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
          MAP_MACRO(ConfigKey::CurrentPatternIndex, current_pattern_index);
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
          MAP_UPDATE_ARRAY(ConfigKey::PathsVectorUpscaleMap, paths_vector_upscale_map);
          MAP_UPDATE_ARRAY(ConfigKey::PathsVectorDeswizzleMap, paths_vector_deswizzle_map);
          MAP_MACRO(ConfigKey::RenderImportedImage, render_imported_image);
          MAP_MACRO(ConfigKey::SelectedTile, selected_tile);
          MAP_MACRO(ConfigKey::SelectionsPath, path);
          MAP_MACRO(ConfigKey::StarterField, starter_field);
          MAP_MACRO(ConfigKey::SwizzlePath, swizzle_path);
          MAP_MACRO_UNDERLYING(ConfigKey::TileSizeValue, tile_size_value);
          MAP_MACRO(ConfigKey::UpscalePathsIndex, upscale_paths_index);
          MAP_MACRO(ConfigKey::WindowHeight, window_height);
          MAP_MACRO(ConfigKey::WindowWidth, window_width);

          // m.emplace(ConfigKey::BatchCompact, [](Configuration &c, const Selections &s) {
          //      c->insert_or_assign(key_to_string(ConfigKey::BatchCompactType), std::to_underlying(s.batch_compact_type.value()));
          //      c->insert_or_assign(key_to_string(ConfigKey::BatchCompactEnabled), s.batch_compact_type.enabled());
          // });
          // m.emplace(ConfigKey::BatchFlatten, [](Configuration &c, const Selections &s) {
          //      c->insert_or_assign(key_to_string(ConfigKey::BatchFlattenType), std::to_underlying(s.batch_flatten_type.value()));
          //      c->insert_or_assign(key_to_string(ConfigKey::BatchFlattenEnabled), s.batch_flatten_type.enabled());
          // });
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
          ffnx_mod_path          = ffnx_config["mod_path"].value_or("mods/Textures"s);
          ffnx_override_path     = ffnx_config["override_path"].value_or("override"s);
          ffnx_direct_mode_path  = ffnx_config["direct_mode_path"].value_or("direct"s);
     }
}
