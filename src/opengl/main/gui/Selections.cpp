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

// // template<>
// // void fme::Selections::load<fme::ConfigKey::StarterField>(const Configuration &config)
// // {
// //      starter_field = config[key_to_string(ConfigKey::StarterField)].value_or("ecenter3"s);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::FF8Path>(const Configuration &config)
// // {
// //      if (auto opt_path = config[key_to_string(ConfigKey::FF8Path)].value<std::string>(); opt_path.has_value())
// //      {
// //           path = std::move(opt_path.value());
// //      }
// //      else
// //      {
// //           std::error_code error_code = {};
// //           std::string     str        = std::filesystem::current_path(error_code).string();
// //           if (error_code)
// //           {
// //                spdlog::warn("{}:{} - {}: {} path: \"{}\"", __FILE__, __LINE__, error_code.value(), error_code.message(), str);
// //                error_code.clear();
// //           }
// //           path = str;
// //      }
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::WindowWidth>(const Configuration &config)
// // {
// //      window_width = config[key_to_string(ConfigKey::WindowWidth)].value_or(window_width_default);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::WindowHeight>(const Configuration &config)
// // {
// //      window_height = config[key_to_string(ConfigKey::WindowHeight)].value_or(window_height_default);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::Palette>(const Configuration &config)
// // {
// //      palette = config[key_to_string(ConfigKey::Palette)].value_or(std::uint8_t{}) & 0xFU;
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::Bpp>(const Configuration &config)
// // {
// //      bpp = BPPT{ static_cast<decltype(BPPT{}.raw())>(config[key_to_string(ConfigKey::Bpp)].value_or(BPPT{}.raw()) & 3U) };
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::DrawMode>(const Configuration &config)
// // {
// //      draw = static_cast<draw_mode>(config[key_to_string(ConfigKey::DrawMode)].value_or(std::to_underlying(draw_mode::draw_map)));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::Coo>(const Configuration &config)
// // {
// //      coo = static_cast<LangT>(config[key_to_string(ConfigKey::Coo)].value_or(std::to_underlying(LangT{})));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::ImportSelectedTile>(const Configuration &config)
// // {
// //      selected_tile = config[key_to_string(ConfigKey::ImportSelectedTile)].value_or(-1);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::DrawDisableBlending>(const Configuration &config)
// // {
// //      draw_disable_blending = config[key_to_string(ConfigKey::DrawDisableBlending)].value_or(false);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::DrawGrid>(const Configuration &config)
// // {
// //      draw_grid = config[key_to_string(ConfigKey::DrawGrid)].value_or(false);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::DrawPalette>(const Configuration &config)
// // {

// //      draw_palette = config[key_to_string(ConfigKey::DrawPalette)].value_or(false);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::DrawSwizzle>(const Configuration &config)
// // {
// //      draw_swizzle = config[key_to_string(ConfigKey::DrawSwizzle)].value_or(false);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::RenderImportedImage>(const Configuration &config)
// // {
// //      render_imported_image = config[key_to_string(ConfigKey::RenderImportedImage)].value_or(false);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::DrawTexturePageGrid>(const Configuration &config)
// // {
// //      draw_texture_page_grid = config[key_to_string(ConfigKey::DrawTexturePageGrid)].value_or(false);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::DrawTileConflictRects>(const Configuration &config)
// // {
// //      draw_tile_conflict_rects = config[key_to_string(ConfigKey::DrawTileConflictRects)].value_or(false);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::DisplayBatchWindow>(const Configuration &config)
// // {
// //      display_batch_window = config[key_to_string(ConfigKey::DisplayBatchWindow)].value_or(false);
// // }

// // // todo fix later. I want to merge the current branch and will make a new branch to work on imports.
// // // template<>
// // // void fme::Selections::load<fme::ConfigKey::DisplayImportImageWindow>(const Configuration &config)
// // // {
// // //      display_import_image_window  = config[key_to_string(ConfigKey::DisplayImportImageWindow)].value_or(false);
// // // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::DisplayImportImage>(const Configuration &config)
// // {
// //      display_import_image = config[key_to_string(ConfigKey::DisplayImportImage)].value_or(false);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::ForceReloadingOfTextures>(const Configuration &config)
// // {
// //      force_reloading_of_textures = config[key_to_string(ConfigKey::ForceReloadingOfTextures)].value_or(false);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::ForceRenderingOfMap>(const Configuration &config)
// // {
// //      force_rendering_of_map = config[key_to_string(ConfigKey::ForceRenderingOfMap)].value_or(false);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::ImportImageGrid>(const Configuration &config)
// // {
// //      import_image_grid = config[key_to_string(ConfigKey::ImportImageGrid)].value_or(false);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::ImportLoadImageDirectory>(const Configuration &config)
// // {
// //      import_load_image_directory = config[key_to_string(ConfigKey::ImportLoadImageDirectory)].value_or(path);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::TileSizeValue>(const Configuration &config)
// // {
// //      tile_size_value =
// // static_cast<tile_sizes>(config[key_to_string(ConfigKey::TileSizeValue)].value_or(std::to_underlying(tile_sizes::default_size)));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::DisplayHistoryWindow>(const Configuration &config)
// // {
// //      display_history_window = config[key_to_string(ConfigKey::DisplayHistoryWindow)].value_or(false);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::DisplayControlPanelWindow>(const Configuration &config)
// // {
// //      display_control_panel_window = config[key_to_string(ConfigKey::DisplayControlPanelWindow)].value_or(true);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::DisplayDrawWindow>(const Configuration &config)
// // {
// //      display_draw_window = config[key_to_string(ConfigKey::DisplayDrawWindow)].value_or(true);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::DisplayCustomPathsWindow>(const Configuration &config)
// // {
// //      display_custom_paths_window = config[key_to_string(ConfigKey::DisplayCustomPathsWindow)].value_or(false);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::DisplayFieldFileWindow>(const Configuration &config)
// // {
// //      display_field_file_window = config[key_to_string(ConfigKey::DisplayFieldFileWindow)].value_or(false);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::OutputSwizzlePattern>(const Configuration &config)
// // {
// //      get<ConfigKey::OutputSwizzlePattern>() =
// //        config[key_to_string(ConfigKey::OutputSwizzlePattern)].value_or("{selected_path}\\{demaster}"s);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::OutputDeswizzlePattern>(const Configuration &config)
// // {
// //      get<ConfigKey::OutputDeswizzlePattern>() =
// //        config[key_to_string(ConfigKey::OutputDeswizzlePattern)].value_or("{selected_path}\\deswizzle\\{demaster}"s);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::OutputMapPatternForSwizzle>(const Configuration &config)
// // {
// //      get<ConfigKey::OutputMapPatternForSwizzle>() =
// //        config[key_to_string(ConfigKey::OutputMapPatternForSwizzle)].value_or("{selected_path}\\{demaster}"s);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::OutputMapPatternForDeswizzle>(const Configuration &config)
// // {
// //      get<ConfigKey::OutputMapPatternForDeswizzle>() =
// //        config[key_to_string(ConfigKey::OutputMapPatternForDeswizzle)].value_or("{selected_path}\\deswizzle\\{demaster}"s);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::CurrentPattern>(const Configuration &config)
// // {
// //      get<ConfigKey::CurrentPattern>() =
// //        static_cast<PatternSelector>(config[key_to_string(ConfigKey::CurrentPattern)].value_or(std::to_underlying(PatternSelector{})));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::CurrentPatternIndex>(const Configuration &config)
// // {
// //      current_pattern_index = config[key_to_string(ConfigKey::CurrentPatternIndex)].value_or(int{ -1 });
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::BatchInputType>(const Configuration &config)
// // {
// //      batch_input_type = static_cast<input_types>(
// //        config[key_to_string(ConfigKey::BatchInputType)].value_or(static_cast<std::underlying_type_t<input_types>>(input_types{})));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::BatchInputRootPathType>(const Configuration &config)
// // {
// //      batch_input_root_path_type = static_cast<root_path_types>(config[key_to_string(ConfigKey::BatchInputRootPathType)].value_or(
// //        static_cast<std::underlying_type_t<root_path_types>>(root_path_types{})));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::BatchOutputType>(const Configuration &config)
// // {
// //      batch_output_type =
// //        static_cast<output_types>(config[key_to_string(ConfigKey::BatchOutputType)].value_or(std::to_underlying(output_types{})));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::BatchOutputRootPathType>(const Configuration &config)
// // {
// //      batch_output_root_path_type = static_cast<root_path_types>(
// //        config[key_to_string(ConfigKey::BatchOutputRootPathType)].value_or(std::to_underlying(root_path_types{})));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::BatchMapListEnabled>(const Configuration &config)
// // {
// //      (void)config.load_array(key_to_string(ConfigKey::BatchMapListEnabled), get<ConfigKey::BatchMapListEnabled>());
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::BackgroundColor>(const Configuration &config)
// // {
// //      background_color = std::bit_cast<fme::color>(
// //        config[key_to_string(ConfigKey::BackgroundColor)].value_or(std::bit_cast<std::uint32_t>(fme::colors::White)));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::BatchInputPath>(const Configuration &config)
// // {
// //      batch_input_path = config[key_to_string(ConfigKey::BatchInputPath)].value_or(std::string{});
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::BatchOutputPath>(const Configuration &config)
// // {
// //      batch_output_path = config[key_to_string(ConfigKey::BatchOutputPath)].value_or(std::string{});
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::BatchInputLoadMap>(const Configuration &config)
// // {
// //      batch_input_load_map = config[key_to_string(ConfigKey::BatchInputLoadMap)].value_or(false);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::BatchOutputSaveMap>(const Configuration &config)
// // {
// //      batch_output_save_map = config[key_to_string(ConfigKey::BatchOutputSaveMap)].value_or(true);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::PathPatternsWithPaletteAndTexturePage>(const Configuration &config)
// // {

// //      if (!config.load_array(
// //            key_to_string(ConfigKey::PathPatternsWithPaletteAndTexturePage), get<ConfigKey::PathPatternsWithPaletteAndTexturePage>()))
// //      {
// //           get<ConfigKey::PathPatternsWithPaletteAndTexturePage>() = {
// //                "{selected_path}/{field_name}{_{2_letter_lang}}_0{texture_page}_0{palette}{ext}"s,
// //                "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}_0{palette}{ext}"s,
// //                "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}_0{palette}{ext}"s,

// //                "{selected_path}/{field_name}_0{texture_page}_0{palette}{ext}"s,
// //                "{selected_path}/{field_name}/{field_name}_0{texture_page}_0{palette}{ext}"s,
// //                "{selected_path}/{field_prefix}/{field_name}/{field_name}_0{texture_page}_0{palette}{ext}"s,

// //                "{selected_path}/{field_name}{_{2_letter_lang}}_{texture_page}_{palette}{ext}"s,
// //                "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}_{palette}{ext}"s,
// //                "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}_{palette}{ext}"s,

// //                "{selected_path}/{field_name}_{texture_page}_{palette}{ext}"s,
// //                "{selected_path}/{field_name}/{field_name}_{texture_page}_{palette}{ext}"s,
// //                "{selected_path}/{field_prefix}/{field_name}/{field_name}_{texture_page}_{palette}{ext}"s
// //           };
// //      }
// //      assert(has_balanced_braces(get<ConfigKey::PathPatternsWithPaletteAndTexturePage>()));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::PathPatternsWithPalette>(const Configuration &config)
// // {
// //      if (!config.load_array(key_to_string(ConfigKey::PathPatternsWithPalette), get<ConfigKey::PathPatternsWithPalette>()))
// //      {
// //           get<ConfigKey::PathPatternsWithPalette>() = {
// //                "{selected_path}/{field_name}{_{2_letter_lang}}_0{palette}{ext}"s,
// //                "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_0{palette}{ext}"s,
// //                "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_0{palette}{ext}"s,

// //                "{selected_path}/{field_name}_0{palette}{ext}"s,
// //                "{selected_path}/{field_name}/{field_name}_0{palette}{ext}"s,
// //                "{selected_path}/{field_prefix}/{field_name}/{field_name}_0{palette}{ext}"s,

// //                "{selected_path}/{field_name}{_{2_letter_lang}}_{palette}{ext}"s,
// //                "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_{palette}{ext}"s,
// //                "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_{palette}{ext}"s,

// //                "{selected_path}/{field_name}_{palette}{ext}"s,
// //                "{selected_path}/{field_name}/{field_name}_{palette}{ext}"s,
// //                "{selected_path}/{field_prefix}/{field_name}/{field_name}_{palette}{ext}"s
// //           };
// //      }
// //      assert(has_balanced_braces(get<ConfigKey::PathPatternsWithPalette>()));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::PathPatternsWithTexturePage>(const Configuration &config)
// // {

// //      if (!config.load_array(key_to_string(ConfigKey::PathPatternsWithTexturePage), get<ConfigKey::PathPatternsWithTexturePage>()))
// //      {
// //           get<ConfigKey::PathPatternsWithTexturePage>() = { "{selected_path}/{field_name}{_{2_letter_lang}}_0{texture_page}{ext}"s,
// //                                          "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}{ext}"s,
// // "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}{ext}"s,

// //                                          "{selected_path}/{field_name}_0{texture_page}{ext}"s,
// //                                          "{selected_path}/{field_name}/{field_name}_0{texture_page}{ext}"s,
// //                                          "{selected_path}/{field_prefix}/{field_name}/{field_name}_0{texture_page}{ext}"s,

// //                                          "{selected_path}/{field_name}{_{2_letter_lang}}_{texture_page}{ext}"s,
// //                                          "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}{ext}"s,
// // "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}{ext}"s,

// //                                          "{selected_path}/{field_name}_{texture_page}{ext}"s,
// //                                          "{selected_path}/{field_name}/{field_name}_{texture_page}{ext}"s,
// //                                          "{selected_path}/{field_prefix}/{field_name}/{field_name}_{texture_page}{ext}"s };
// //      }
// //      assert(has_balanced_braces(get<ConfigKey::PathPatternsWithTexturePage>()));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::PathPatternsWithPupuID>(const Configuration &config)
// // {
// //      if (!config.load_array(key_to_string(ConfigKey::PathPatternsWithPupuID), get<ConfigKey::PathPatternsWithPupuID>()))
// //      {
// //           get<ConfigKey::PathPatternsWithPupuID>() = { "{selected_path}/{field_name}_{pupu_id}{ext}"s,
// //                                     "{selected_path}/{field_name}/{field_name}_{pupu_id}{ext}"s,
// //                                     "{selected_path}/{field_prefix}/{field_name}/{field_name}_{pupu_id}{ext}"s };
// //      }
// //      assert(has_balanced_braces(get<ConfigKey::PathPatternsWithPupuID>()));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::PatternsBase>(const Configuration &config)
// // {

// //      if (!config.load_array(key_to_string(ConfigKey::PatternsBase), get<ConfigKey::PatternsBase>()))
// //      {
// //           get<ConfigKey::PatternsBase>() = { "{selected_path}/{field_name}{_{2_letter_lang}}{ext}"s,
// //                                              "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}{ext}"s,
// //                                              "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}{ext}"s,

// //                                              "{selected_path}/{field_name}{ext}"s,
// //                                              "{selected_path}/{field_name}/{field_name}{ext}"s,
// //                                              "{selected_path}/{field_prefix}/{field_name}/{field_name}{ext}"s,

// //                                              "{selected_path}/{demaster}"s,

// //                                              "{selected_path}/{ffnx_multi_texture}"s,
// //                                              "{selected_path}/{ffnx_single_texture}"s,
// //                                              "{selected_path}/{ffnx_map}"s };
// //      }
// //      assert(has_balanced_braces(get<ConfigKey::PatternsBase>()));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::PatternsCommonPrefixes>(const Configuration &config)
// // {
// //      if (!config.load_array(key_to_string(ConfigKey::PatternsCommonPrefixes), get<ConfigKey::PatternsCommonPrefixes>()))
// //      {
// //           get<ConfigKey::PatternsCommonPrefixes>() = { "{selected_path}"s,

// //                                                        "{selected_path}/{ffnx_mod_path}/field/mapdata/"s,
// //                                                        "{selected_path}/mods/Textures"s,

// //                                                        "{selected_path}/{demaster_mod_path}/textures/field_bg"s,
// //                                                        "{selected_path}/field_bg"s,

// //                                                        "{selected_path}/textures/fields"s,
// //                                                        "{selected_path}/textures"s,

// //                                                        "{selected_path}/ff8/Data/{3_letter_lang}/field/mapdata"s,
// //                                                        "{selected_path}/ff8/Data/{3_letter_lang}/FIELD/mapdata"s,

// //                                                        "{selected_path}/ff8/Data/{eng}/field/mapdata"s,
// //                                                        "{selected_path}/ff8/Data/{eng}/FIELD/mapdata"s,

// //                                                        "{selected_path}/ff8/Data/{fre}/field/mapdata"s,
// //                                                        "{selected_path}/ff8/Data/{fre}/FIELD/mapdata"s,

// //                                                        "{selected_path}/ff8/Data/{ger}/field/mapdata"s,
// //                                                        "{selected_path}/ff8/Data/{ger}/FIELD/mapdata"s,

// //                                                        "{selected_path}/ff8/Data/{ita}/field/mapdata"s,
// //                                                        "{selected_path}/ff8/Data/{ita}/FIELD/mapdata"s,

// //                                                        "{selected_path}/ff8/Data/{spa}/field/mapdata"s,
// //                                                        "{selected_path}/ff8/Data/{spa}/FIELD/mapdata"s,

// //                                                        "{selected_path}/ff8/Data/{jp}/field/mapdata"s,
// //                                                        "{selected_path}/ff8/Data/{jp}/FIELD/mapdata"s,

// //                                                        "{selected_path}/ff8/Data/{x}/field/mapdata"s,
// //                                                        "{selected_path}/ff8/Data/{x}/FIELD/mapdata"s };
// //      }
// //      assert(has_balanced_braces(get<ConfigKey::PatternsCommonPrefixes>()));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::PatternsCommonPrefixesForMaps>(const Configuration &config)
// // {
// //      if (!config.load_array(key_to_string(ConfigKey::PatternsCommonPrefixesForMaps), get<ConfigKey::PatternsCommonPrefixesForMaps>()))
// //      {
// //           // todo ffnx uses a sepperate directory for map files which means we might not see it with our
// //           // current method of selecting one path ffnx_direct_mode_path might not want to be in the regular
// //           // paths list might need to be somewhere else. maybe a get paths map.
// //           get<ConfigKey::PatternsCommonPrefixesForMaps>() = { "{selected_path}/{ffnx_direct_mode_path}/field/mapdata/"s };
// //      }
// //      assert(has_balanced_braces(get<ConfigKey::PatternsCommonPrefixesForMaps>()));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::FF8DirectoryPaths>(const Configuration &config)
// // {
// //      if (!config.load_array(key_to_string(ConfigKey::FF8DirectoryPaths), ff8_directory_paths))
// //      {
// //           const auto &default_paths = open_viii::Paths::get();
// //           ff8_directory_paths       = { default_paths.begin(), default_paths.end() };
// //      }
// //      assert(has_balanced_braces(ff8_directory_paths));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::ExternalTexturesDirectoryPaths>(const Configuration &config)
// // {
// //      if (!config.load_array(key_to_string(ConfigKey::ExternalTexturesDirectoryPaths), external_textures_directory_paths))
// //      {
// //           external_textures_directory_paths = { R"(D:\Angelwing-Ultima_Remastered_v1-0-a)"s,
// //                                                 R"(D:\dev\Field-Map-Editor\bin\RelWithDebInfo\deswizzle)"s };
// //      }
// //      assert(has_balanced_braces(external_textures_directory_paths));
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::ExternalMapsDirectoryPaths>(const Configuration &config)
// // {
// //      std::ignore = config.load_array(key_to_string(ConfigKey::ExternalMapsDirectoryPaths), external_maps_directory_paths);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::SwizzlePath>(const Configuration &config)
// // {
// //      swizzle_path = config[key_to_string(ConfigKey::SwizzlePath)].value_or(path);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::DeswizzlePath>(const Configuration &config)
// // {
// //      deswizzle_path = config[key_to_string(ConfigKey::DeswizzlePath)].value_or(path);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::OutputImagePath>(const Configuration &config)
// // {
// //      output_image_path = config[key_to_string(ConfigKey::OutputImagePath)].value_or(path);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::OutputMimPath>(const Configuration &config)
// // {
// //      output_mim_path = config[key_to_string(ConfigKey::OutputMimPath)].value_or(path);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::OutputMapPath>(const Configuration &config)
// // {
// //      output_map_path = config[key_to_string(ConfigKey::OutputMapPath)].value_or(path);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::CacheTexturePaths>(const Configuration &config)
// // {
// //      std::ignore = config.load_array(key_to_string(ConfigKey::CacheTexturePaths), cache_texture_paths);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::CacheSwizzlePathsEnabled>(const Configuration &config)
// // {
// //      std::ignore = config.load_array(key_to_string(ConfigKey::CacheSwizzlePathsEnabled), cache_swizzle_paths_enabled);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::CacheSwizzleAsOneImagePathsEnabled>(const Configuration &config)
// // {
// //      std::ignore =
// //        config.load_array(key_to_string(ConfigKey::CacheSwizzleAsOneImagePathsEnabled), cache_swizzle_as_one_image_paths_enabled);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::CacheDeswizzlePathsEnabled>(const Configuration &config)
// // {
// //      std::ignore = config.load_array(key_to_string(ConfigKey::CacheDeswizzlePathsEnabled), cache_deswizzle_paths_enabled);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::CacheMapPaths>(const Configuration &config)
// // {
// //      std::ignore = config.load_array(key_to_string(ConfigKey::CacheMapPaths), cache_map_paths);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::CacheMapPathsEnabled>(const Configuration &config)
// // {
// //      std::ignore = config.load_array(key_to_string(ConfigKey::CacheMapPathsEnabled), cache_map_paths_enabled);
// // }

// // template<>
// // void fme::Selections::load<fme::ConfigKey::All>(const Configuration &config)
// // {
// //      [&]<std::size_t... Is>(std::index_sequence<Is...>) {
// //           (
// //             [&]<ConfigKey Key>() {
// //                  if constexpr (!SelectionUseFFNXConfig<Key>::value)
// //                       load<Key>(config);
// //             }.template operator()<static_cast<ConfigKey>(Is)>(),
// //             ...);
// //      }(std::make_index_sequence<static_cast<std::size_t>(fme::ConfigKey::All)>{});
// // }


// template<>
// void fme::Selections::update<fme::ConfigKey::StarterField>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::StarterField), starter_field);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::FF8Path>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::FF8Path), path);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::WindowWidth>(Configuration &config) const
// {

//      config->insert_or_assign(key_to_string(ConfigKey::WindowWidth), window_width);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::WindowHeight>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::WindowHeight), window_height);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::Palette>(Configuration &config) const
// {

//      config->insert_or_assign(key_to_string(ConfigKey::Palette), palette & 0xFU);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::Bpp>(Configuration &config) const
// {

//      config->insert_or_assign(key_to_string(ConfigKey::Bpp), bpp.raw() & 3U);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::DrawMode>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::DrawMode), std::to_underlying(draw));
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::Coo>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::Coo), std::to_underlying(coo));
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::ImportSelectedTile>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::ImportSelectedTile), selected_tile);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::DrawDisableBlending>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::DrawDisableBlending), draw_disable_blending);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::DrawGrid>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::DrawGrid), draw_grid);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::DrawPalette>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::DrawPalette), draw_palette);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::DrawSwizzle>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::DrawSwizzle), draw_swizzle);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::RenderImportedImage>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::RenderImportedImage), render_imported_image);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::DrawTexturePageGrid>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::DrawTexturePageGrid), draw_texture_page_grid);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::DrawTileConflictRects>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::DrawTileConflictRects), draw_tile_conflict_rects);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::DisplayBatchWindow>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::DisplayBatchWindow), display_batch_window);
// }

// // TODO fix import window
// // template<>
// // void fme::Selections::update<fme::ConfigKey::DisplayImportImageWindow>(Configuration &config) const
// // {
// //      config->insert_or_assign(key_to_string(ConfigKey::DisplayImportImageWindow), display_import_image_window);
// // }

// template<>
// void fme::Selections::update<fme::ConfigKey::DisplayImportImage>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::DisplayImportImage), display_import_image);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::ForceReloadingOfTextures>(Configuration &config) const
// {

//      config->insert_or_assign(key_to_string(ConfigKey::ForceReloadingOfTextures), force_reloading_of_textures);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::ForceRenderingOfMap>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::ForceRenderingOfMap), force_rendering_of_map);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::ImportImageGrid>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::ImportImageGrid), import_image_grid);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::ImportLoadImageDirectory>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::ImportLoadImageDirectory), import_load_image_directory);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::TileSizeValue>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::TileSizeValue), std::to_underlying(tile_size_value));
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::DisplayHistoryWindow>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::DisplayHistoryWindow), display_history_window);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::DisplayControlPanelWindow>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::DisplayControlPanelWindow), display_control_panel_window);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::DisplayDrawWindow>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::DisplayDrawWindow), display_draw_window);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::DisplayCustomPathsWindow>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::DisplayCustomPathsWindow), display_custom_paths_window);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::DisplayFieldFileWindow>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::DisplayFieldFileWindow), display_field_file_window);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::OutputSwizzlePattern>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::OutputSwizzlePattern), get<ConfigKey::OutputSwizzlePattern>());
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::OutputDeswizzlePattern>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::OutputDeswizzlePattern), get<ConfigKey::OutputDeswizzlePattern>());
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::OutputMapPatternForSwizzle>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::OutputMapPatternForSwizzle), get<ConfigKey::OutputMapPatternForSwizzle>());
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::OutputMapPatternForDeswizzle>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::OutputMapPatternForDeswizzle), get<ConfigKey::OutputMapPatternForDeswizzle>());
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::CurrentPattern>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::CurrentPattern), std::to_underlying(get<ConfigKey::CurrentPattern>()));
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::CurrentPatternIndex>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::CurrentPatternIndex), current_pattern_index);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::BatchInputType>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::BatchInputType), std::to_underlying(batch_input_type));
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::BatchInputRootPathType>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::BatchInputRootPathType), std::to_underlying(batch_input_root_path_type));
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::BatchOutputType>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::BatchOutputType), std::to_underlying(batch_output_type));
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::BatchOutputRootPathType>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::BatchOutputRootPathType), std::to_underlying(batch_output_root_path_type));
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::BatchMapListEnabled>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::BatchMapListEnabled), get<ConfigKey::BatchMapListEnabled>());
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::BackgroundColor>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::BackgroundColor), std::bit_cast<std::uint32_t>(background_color));
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::BatchInputPath>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::BatchInputPath), std::string(batch_input_path.data()));
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::BatchOutputPath>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::BatchOutputPath), std::string(batch_output_path.data()));
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::BatchInputLoadMap>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::BatchInputLoadMap), batch_input_load_map);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::BatchOutputSaveMap>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::BatchOutputSaveMap), batch_output_save_map);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::PathPatternsWithPaletteAndTexturePage>(Configuration &config) const
// {
//      config.update_array(
//        key_to_string(ConfigKey::PathPatternsWithPaletteAndTexturePage), get<ConfigKey::PathPatternsWithPaletteAndTexturePage>());
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::PathPatternsWithPalette>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::PathPatternsWithPalette), get<ConfigKey::PathPatternsWithPalette>());
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::PathPatternsWithTexturePage>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::PathPatternsWithTexturePage), get<ConfigKey::PathPatternsWithTexturePage>());
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::PathPatternsWithPupuID>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::PathPatternsWithPupuID), get<ConfigKey::PathPatternsWithPupuID>());
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::PatternsBase>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::PatternsBase), get<ConfigKey::PatternsBase>());
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::PatternsCommonPrefixes>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::PatternsCommonPrefixes), get<ConfigKey::PatternsCommonPrefixes>());
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::PatternsCommonPrefixesForMaps>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::PatternsCommonPrefixesForMaps), get<ConfigKey::PatternsCommonPrefixesForMaps>());
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::FF8DirectoryPaths>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::FF8DirectoryPaths), ff8_directory_paths);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::ExternalTexturesDirectoryPaths>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::ExternalTexturesDirectoryPaths), external_textures_directory_paths);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::ExternalMapsDirectoryPaths>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::ExternalMapsDirectoryPaths), external_maps_directory_paths);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::SwizzlePath>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::SwizzlePath), swizzle_path);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::DeswizzlePath>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::DeswizzlePath), deswizzle_path);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::OutputImagePath>(Configuration &config) const
// {

//      config->insert_or_assign(key_to_string(ConfigKey::OutputImagePath), output_image_path);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::OutputMimPath>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::OutputMimPath), output_mim_path);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::OutputMapPath>(Configuration &config) const
// {
//      config->insert_or_assign(key_to_string(ConfigKey::OutputMapPath), output_map_path);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::CacheTexturePaths>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::CacheTexturePaths), cache_texture_paths);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::CacheSwizzlePathsEnabled>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::CacheSwizzlePathsEnabled), cache_swizzle_paths_enabled);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::CacheSwizzleAsOneImagePathsEnabled>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::CacheSwizzleAsOneImagePathsEnabled), cache_swizzle_as_one_image_paths_enabled);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::CacheDeswizzlePathsEnabled>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::CacheDeswizzlePathsEnabled), cache_deswizzle_paths_enabled);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::CacheMapPaths>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::CacheMapPaths), cache_map_paths);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::CacheMapPathsEnabled>(Configuration &config) const
// {
//      config.update_array(key_to_string(ConfigKey::CacheMapPathsEnabled), cache_map_paths_enabled);
// }

// template<>
// void fme::Selections::update<fme::ConfigKey::All>(Configuration &config) const
// {
//      [&]<std::size_t... Is>(std::index_sequence<Is...>) {
//           (
//             [&]<ConfigKey Key>() {
//                  if constexpr (!SelectionUseFFNXConfig<Key>::value)
//                       update<Key>(config);
//             }.template operator()<static_cast<ConfigKey>(Is)>(),
//             ...);
//      }(std::make_index_sequence<static_cast<std::size_t>(fme::ConfigKey::All)>{});
// }

fme::Selections::Selections(const Configuration config)
  : m_selections_array(load_selections_array(config))
{
     const auto start_time  = std::chrono::system_clock::now();

     // load<ConfigKey::All>(Configuration{});

     //     refresh_ffnx_paths();

     const auto end_time    = std::chrono::system_clock::now();
     const auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
     spdlog::info("Configuration Load Time: {:.3f} ms", static_cast<float>(duration_us) / 1000.0F);
}

std::optional<fme::Configuration> fme::Selections::get_ffnx_config() const
{
     return get_ffnx_config(get<ConfigKey::FF8Path>());
}

std::optional<fme::Configuration> fme::Selections::get_ffnx_config(const std::filesystem::path &ff8_path) const
{
     const auto      ffnx_settings_toml = ff8_path / "FFNx.toml";
     std::error_code error_code         = {};
     bool            exists             = std::filesystem::exists(ffnx_settings_toml, error_code);
     if (error_code)
     {
          spdlog::warn("{}:{} - {}: {} path: \"{}\"", __FILE__, __LINE__, error_code.value(), error_code.message(), ffnx_settings_toml);
          error_code.clear();
     }

     if (exists)
     {
          return ffnx_settings_toml;
     }
     return std::nullopt;
}

// void fme::Selections::refresh_ffnx_paths(const std::filesystem::path &ff8_path)
// {
//      const auto      ffnx_settings_toml = ff8_path / "FFNx.toml";
//      std::error_code error_code         = {};
//      bool            exists             = std::filesystem::exists(ffnx_settings_toml, error_code);
//      if (error_code)
//      {
//           spdlog::warn("{}:{} - {}: {} path: \"{}\"", __FILE__, __LINE__, error_code.value(), error_code.message(), ffnx_settings_toml);
//           error_code.clear();
//      }

//      if (exists)
//      {
//           const auto ffnx_config = Configuration(ffnx_settings_toml);
//           ffnx_mod_path          = ffnx_config["mod_path"].value_or("mods/Textures"s);
//           ffnx_override_path     = ffnx_config["override_path"].value_or("override"s);
//           ffnx_direct_mode_path  = ffnx_config["direct_mode_path"].value_or("direct"s);
//      }
// }

// void fme::Selections::update_configuration() const
// {
//      Configuration config{};
//      update<ConfigKey::All>(config);
//      config.save();
// }


bool fme::Selections::has_balanced_braces([[maybe_unused]] const std::string_view s)
{
     int balance = 0;
     for (const char c : s)
     {
          if (c == '{')
          {
               ++balance;
          }
          else if (c == '}')
          {
               --balance;
               if (balance < 0)
               {
                    spdlog::error("Unmatched closing brace in input: \"{}\" (note: literal braces shown as {{ and }})", s);
                    return false;
               }
          }
     }

     if (balance != 0)
     {
          spdlog::error("Mismatched brace count in input: \"{}\" ({} unmatched opening brace{{}})", s, balance);
          return false;
     }

     return true;
}