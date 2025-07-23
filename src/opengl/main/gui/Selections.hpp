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
     FF8DirectoryPaths,
     FF8Path,
     WindowWidth,
     WindowHeight,
     Palette,
     Bpp,
     DrawMode,
     Coo,
     ImportSelectedTile,
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
     ExternalTexturesDirectoryPaths,
     ExternalMapsDirectoryPaths,

     SwizzlePath,
     DeswizzlePath,
     OutputImagePath,
     OutputMimPath,
     OutputMapPath,

     CacheTexturePaths,
     CacheSwizzlePathsEnabled,
     CacheSwizzleAsOneImagePathsEnabled,
     CacheDeswizzlePathsEnabled,
     CacheMapPaths,
     CacheMapPathsEnabled,

     // FFNX can load from FFNX config but we're doing read only these. Usually only if we're changing the FF8 directory
     FFNXModPath,
     FFNXOverridePath,
     FFNXDirectPath,

     // Filters not required by update or load.
     BatchCompactType,
     BatchFlattenType,
     // All is used to map all values less than All.
     All,

     // Filters not required by update or load.
     BatchCompactEnabled,
     BatchFlattenEnabled,

};

template<ConfigKey Key>
struct SelectionInfo;

template<>
struct SelectionInfo<ConfigKey::StarterField>
{
     using value_type                     = std::string;
     static constexpr std::string_view id = "StarterField";
     static constexpr value_type       default_value()
     {
          return "ecenter3";
     }
};
template<>
struct SelectionInfo<ConfigKey::FF8Path>
{
     using value_type                     = std::filesystem::path;
     static constexpr std::string_view id = "FF8Path";
};
template<>
struct SelectionInfo<ConfigKey::WindowWidth>
{
     using value_type                     = std::int32_t;
     static constexpr std::string_view id = "WindowWidth";
     static constexpr value_type       default_value()
     {
          return 1280;
     }
};
template<>
struct SelectionInfo<ConfigKey::WindowHeight>
{
     using value_type                     = std::int32_t;
     static constexpr std::string_view id = "WindowHeight";
     static constexpr value_type       default_value()
     {
          return 720;
     }
};
template<>
struct SelectionInfo<ConfigKey::Palette>
{
     using value_type                     = std::uint8_t;
     static constexpr std::string_view id = "Palette";
     static constexpr void             post_load_operation(value_type &value)
     {
          value = value & 0xFU;
     }
};
template<>
struct SelectionInfo<ConfigKey::Bpp>
{
     using value_type                     = open_viii::graphics::BPPT;
     static constexpr std::string_view id = "Bpp";
};
template<>
struct SelectionInfo<ConfigKey::DrawMode>
{
     using value_type                     = draw_mode;
     static constexpr std::string_view id = "DrawMode";
     static constexpr value_type       default_value()
     {
          return draw_mode::draw_map;
     }
};
template<>
struct SelectionInfo<ConfigKey::Coo>
{
     using value_type                     = open_viii::LangT;
     static constexpr std::string_view id = "Coo";
};
template<>
struct SelectionInfo<ConfigKey::ImportSelectedTile>
{
     using value_type                     = std::int32_t;
     static constexpr std::string_view id = "ImportSelectedTile";
};
template<>
struct SelectionInfo<ConfigKey::DrawDisableBlending>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DrawDisableBlending";
};
template<>
struct SelectionInfo<ConfigKey::DrawGrid>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DrawGrid";
};
template<>
struct SelectionInfo<ConfigKey::DrawPalette>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DrawPalette";
};
template<>
struct SelectionInfo<ConfigKey::DrawSwizzle>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DrawSwizzle";
};
template<>
struct SelectionInfo<ConfigKey::RenderImportedImage>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "RenderImportedImage";
};
template<>
struct SelectionInfo<ConfigKey::DrawTexturePageGrid>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DrawTexturePageGrid";
};
template<>
struct SelectionInfo<ConfigKey::DrawTileConflictRects>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DrawTileConflictRects";
};
template<>
struct SelectionInfo<ConfigKey::DisplayBatchWindow>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DisplayBatchWindow";
};
// template<>
// struct SelectionInfo<ConfigKey::DisplayImportImageWindow>
// {
//      using value_type = bool;
//      static constexpr std::string_view id = "DisplayImportImageWindow";
//};
template<>
struct SelectionInfo<ConfigKey::DisplayImportImage>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DisplayImportImage";
};
template<>
struct SelectionInfo<ConfigKey::ForceReloadingOfTextures>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "ForceReloadingOfTextures";
};
template<>
struct SelectionInfo<ConfigKey::ForceRenderingOfMap>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "ForceRenderingOfMap";
};
template<>
struct SelectionInfo<ConfigKey::ImportImageGrid>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "ImportImageGrid";
};
template<>
struct SelectionInfo<ConfigKey::ImportLoadImageDirectory>
{
     using value_type                     = std::filesystem::path;
     static constexpr std::string_view id = "ImportLoadImageDirectory";
};
template<>
struct SelectionInfo<ConfigKey::TileSizeValue>
{
     using value_type                     = tile_sizes;
     static constexpr std::string_view id = "TileSizeValue";
};
template<>
struct SelectionInfo<ConfigKey::DisplayHistoryWindow>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DisplayHistoryWindow";
};
template<>
struct SelectionInfo<ConfigKey::DisplayControlPanelWindow>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DisplayControlPanelWindow";
};
template<>
struct SelectionInfo<ConfigKey::DisplayDrawWindow>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DisplayDrawWindow";
};
template<>
struct SelectionInfo<ConfigKey::DisplayCustomPathsWindow>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DisplayCustomPathsWindow";
};
template<>
struct SelectionInfo<ConfigKey::DisplayFieldFileWindow>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DisplayFieldFileWindow";
};
template<>
struct SelectionInfo<ConfigKey::OutputSwizzlePattern>
{
     using value_type                     = std::string;
     static constexpr std::string_view id = "OutputSwizzlePattern";
};
template<>
struct SelectionInfo<ConfigKey::OutputDeswizzlePattern>
{
     using value_type                     = std::string;
     static constexpr std::string_view id = "OutputDeswizzlePattern";
};
template<>
struct SelectionInfo<ConfigKey::OutputMapPatternForSwizzle>
{
     using value_type                     = std::string;
     static constexpr std::string_view id = "OutputMapPatternForSwizzle";
};
template<>
struct SelectionInfo<ConfigKey::OutputMapPatternForDeswizzle>
{
     using value_type                     = std::string;
     static constexpr std::string_view id = "OutputMapPatternForDeswizzle";
};
template<>
struct SelectionInfo<ConfigKey::CurrentPattern>
{
     using value_type                     = PatternSelector;
     static constexpr std::string_view id = "CurrentPattern";
};
template<>
struct SelectionInfo<ConfigKey::CurrentPatternIndex>
{
     using value_type                     = std::int32_t;
     static constexpr std::string_view id = "CurrentPatternIndex";
};
template<>
struct SelectionInfo<ConfigKey::BatchInputType>
{
     using value_type                     = input_types;
     static constexpr std::string_view id = "BatchInputType";
};
template<>
struct SelectionInfo<ConfigKey::BatchInputRootPathType>
{
     using value_type                     = root_path_types;
     static constexpr std::string_view id = "BatchInputRootPathType";
};
template<>
struct SelectionInfo<ConfigKey::BatchOutputType>
{
     using value_type                     = output_types;
     static constexpr std::string_view id = "BatchOutputType";
};
template<>
struct SelectionInfo<ConfigKey::BatchOutputRootPathType>
{
     using value_type                     = root_path_types;
     static constexpr std::string_view id = "BatchOutputRootPathType";
};
template<>
struct SelectionInfo<ConfigKey::BatchMapListEnabled>
{
     using value_type                     = std::vector<bool>;
     static constexpr std::string_view id = "BatchMapListEnabled";
};
template<>
struct SelectionInfo<ConfigKey::BackgroundColor>
{
     using value_type                     = color;
     static constexpr std::string_view id = "BackgroundColor";
     static inline value_type          default_value()
     {
          return fme::colors::White;
     }
};
template<>
struct SelectionInfo<ConfigKey::BatchInputPath>
{
     using value_type                     = std::string;
     static constexpr std::string_view id = "BatchInputPath";
};
template<>
struct SelectionInfo<ConfigKey::BatchOutputPath>
{
     using value_type                     = std::string;
     static constexpr std::string_view id = "BatchOutputPath";
};
template<>
struct SelectionInfo<ConfigKey::BatchInputLoadMap>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "BatchInputLoadMap";
};
template<>
struct SelectionInfo<ConfigKey::BatchOutputSaveMap>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "BatchOutputSaveMap";
};

template<>
struct SelectionInfo<ConfigKey::BatchCompactEnabled>
{
     static constexpr std::string_view id = ff_8::ConfigKeys<ff_8::FilterTag::Compact>::enabled_key_name;
};
template<>
struct SelectionInfo<ConfigKey::BatchCompactType>
{
     using value_type                     = ff_8::filter_old<compact_type, ff_8::FilterTag::Compact>;
     static constexpr std::string_view id = ff_8::ConfigKeys<ff_8::FilterTag::Compact>::key_name;
     static inline value_type          default_value(const Configuration &config)
     {
          return { static_cast<fme::compact_type>(config[id].value_or(std::to_underlying(fme::compact_type{}))),
                   ff_8::WithFlag(
                     ff_8::FilterSettings::Default,
                     ff_8::FilterSettings::Toggle_Enabled,
                     config[SelectionInfo<ConfigKey::BatchCompactEnabled>::id].value_or(false)) };
     }
};

template<>
struct SelectionInfo<ConfigKey::BatchFlattenEnabled>
{
     static constexpr std::string_view id = ff_8::ConfigKeys<ff_8::FilterTag::Flatten>::enabled_key_name;
};
template<>
struct SelectionInfo<ConfigKey::BatchFlattenType>
{
     using value_type                     = ff_8::filter_old<flatten_type, ff_8::FilterTag::Flatten>;
     static constexpr std::string_view id = ff_8::ConfigKeys<ff_8::FilterTag::Flatten>::key_name;
     static inline value_type          default_value(const Configuration &config)
     {
          return { static_cast<fme::flatten_type>(config[id].value_or(std::to_underlying(fme::flatten_type{}))),
                   ff_8::WithFlag(
                     ff_8::FilterSettings::Default,
                     ff_8::FilterSettings::Toggle_Enabled,
                     config[SelectionInfo<ConfigKey::BatchFlattenEnabled>::id].value_or(false)) };
     }
};
template<>
struct SelectionInfo<ConfigKey::PathPatternsWithPaletteAndTexturePage>
{
     using value_type                     = std::vector<std::string>;
     static constexpr std::string_view id = "PathPatternsWithPaletteAndTexturePage";
     static value_type                 expensive_default_value()
     {
          using namespace std::string_literals;
          return { "{selected_path}/{field_name}{_{2_letter_lang}}_0{texture_page}_0{palette}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}_0{palette}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}_0{palette}{ext}"s,

                   "{selected_path}/{field_name}_0{texture_page}_0{palette}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}_0{texture_page}_0{palette}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}_0{texture_page}_0{palette}{ext}"s,

                   "{selected_path}/{field_name}{_{2_letter_lang}}_{texture_page}_{palette}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}_{palette}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}_{palette}{ext}"s,

                   "{selected_path}/{field_name}_{texture_page}_{palette}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}_{texture_page}_{palette}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}_{texture_page}_{palette}{ext}"s };
     }
     static void post_load_operation([[maybe_unused]] const value_type &value)
     {
          assert(has_balanced_braces(value));
     }
};
template<>
struct SelectionInfo<ConfigKey::PathPatternsWithPalette>
{
     using value_type                     = std::vector<std::string>;
     static constexpr std::string_view id = "PathPatternsWithPalette";
     static value_type                 expensive_default_value()
     {
          using namespace std::string_literals;
          return { "{selected_path}/{field_name}{_{2_letter_lang}}_0{palette}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_0{palette}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_0{palette}{ext}"s,

                   "{selected_path}/{field_name}_0{palette}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}_0{palette}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}_0{palette}{ext}"s,

                   "{selected_path}/{field_name}{_{2_letter_lang}}_{palette}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_{palette}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_{palette}{ext}"s,

                   "{selected_path}/{field_name}_{palette}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}_{palette}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}_{palette}{ext}"s };
     }
     static void post_load_operation([[maybe_unused]] const value_type &value)
     {
          assert(has_balanced_braces(value));
     }
};
template<>
struct SelectionInfo<ConfigKey::PathPatternsWithTexturePage>
{
     using value_type                     = std::vector<std::string>;
     static constexpr std::string_view id = "PathPatternsWithTexturePage";
     static value_type                 expensive_default_value()
     {
          using namespace std::string_literals;
          return { "{selected_path}/{field_name}{_{2_letter_lang}}_0{texture_page}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}{ext}"s,

                   "{selected_path}/{field_name}_0{texture_page}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}_0{texture_page}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}_0{texture_page}{ext}"s,

                   "{selected_path}/{field_name}{_{2_letter_lang}}_{texture_page}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}{ext}"s,

                   "{selected_path}/{field_name}_{texture_page}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}_{texture_page}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}_{texture_page}{ext}"s };
     }
     static void post_load_operation([[maybe_unused]] const value_type &value)
     {
          assert(has_balanced_braces(value));
     }
};
template<>
struct SelectionInfo<ConfigKey::PathPatternsWithPupuID>
{
     using value_type                     = std::vector<std::string>;
     static constexpr std::string_view id = "PathPatternsWithPupuID";
     static value_type                 expensive_default_value()
     {
          using namespace std::string_literals;
          return { "{selected_path}/{field_name}_{pupu_id}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}_{pupu_id}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}_{pupu_id}{ext}"s };
     }
     static void post_load_operation([[maybe_unused]] const value_type &value)
     {
          assert(has_balanced_braces(value));
     }
};
template<>
struct SelectionInfo<ConfigKey::PatternsBase>
{
     using value_type                     = std::vector<std::string>;
     static constexpr std::string_view id = "PatternsBase";
     static value_type                 expensive_default_value()
     {
          using namespace std::string_literals;
          return { "{selected_path}/{field_name}{_{2_letter_lang}}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}{ext}"s,

                   "{selected_path}/{field_name}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}{ext}"s,

                   "{selected_path}/{demaster}"s,

                   "{selected_path}/{ffnx_multi_texture}"s,
                   "{selected_path}/{ffnx_single_texture}"s,
                   "{selected_path}/{ffnx_map}"s };
     }
     static void post_load_operation([[maybe_unused]] const value_type &value)
     {
          assert(has_balanced_braces(value));
     }
};
template<>
struct SelectionInfo<ConfigKey::PatternsCommonPrefixes>
{
     using value_type                     = std::vector<std::string>;
     static constexpr std::string_view id = "PatternsCommonPrefixes";
     static value_type                 expensive_default_value()
     {
          using namespace std::string_literals;
          return { "{selected_path}"s,

                   "{selected_path}/{ffnx_mod_path}/field/mapdata/"s,
                   "{selected_path}/mods/Textures"s,

                   "{selected_path}/{demaster_mod_path}/textures/field_bg"s,
                   "{selected_path}/field_bg"s,

                   "{selected_path}/textures/fields"s,
                   "{selected_path}/textures"s,

                   "{selected_path}/ff8/Data/{3_letter_lang}/field/mapdata"s,
                   "{selected_path}/ff8/Data/{3_letter_lang}/FIELD/mapdata"s,

                   "{selected_path}/ff8/Data/{eng}/field/mapdata"s,
                   "{selected_path}/ff8/Data/{eng}/FIELD/mapdata"s,

                   "{selected_path}/ff8/Data/{fre}/field/mapdata"s,
                   "{selected_path}/ff8/Data/{fre}/FIELD/mapdata"s,

                   "{selected_path}/ff8/Data/{ger}/field/mapdata"s,
                   "{selected_path}/ff8/Data/{ger}/FIELD/mapdata"s,

                   "{selected_path}/ff8/Data/{ita}/field/mapdata"s,
                   "{selected_path}/ff8/Data/{ita}/FIELD/mapdata"s,

                   "{selected_path}/ff8/Data/{spa}/field/mapdata"s,
                   "{selected_path}/ff8/Data/{spa}/FIELD/mapdata"s,

                   "{selected_path}/ff8/Data/{jp}/field/mapdata"s,
                   "{selected_path}/ff8/Data/{jp}/FIELD/mapdata"s,

                   "{selected_path}/ff8/Data/{x}/field/mapdata"s,
                   "{selected_path}/ff8/Data/{x}/FIELD/mapdata"s };
     }
     static void post_load_operation([[maybe_unused]] const value_type &value)
     {
          assert(has_balanced_braces(value));
     }
};
template<>
struct SelectionInfo<ConfigKey::PatternsCommonPrefixesForMaps>
{
     using value_type                     = std::vector<std::string>;
     static constexpr std::string_view id = "PatternsCommonPrefixesForMaps";
     static value_type                 expensive_default_value()
     {
          using namespace std::string_literals;
          return { "{selected_path}/{ffnx_direct_mode_path}/field/mapdata/"s };
     }
     static void post_load_operation([[maybe_unused]] const value_type &value)
     {
          assert(has_balanced_braces(value));
     }
};
template<>
struct SelectionInfo<ConfigKey::FF8DirectoryPaths>
{
     using value_type                     = std::vector<std::filesystem::path>;
     static constexpr std::string_view id = "FF8DirectoryPaths";
     static value_type                 expensive_default_value()
     {
          const auto &default_paths = open_viii::Paths::get();
          return { default_paths.begin(), default_paths.end() };
     }
     static void post_load_operation([[maybe_unused]] const value_type &value)
     {
          assert(has_balanced_braces(value));
     }
};
template<>
struct SelectionInfo<ConfigKey::ExternalTexturesDirectoryPaths>
{
     using value_type                     = std::vector<std::filesystem::path>;
     static constexpr std::string_view id = "ExternalTexturesDirectoryPaths";
     static value_type                 expensive_default_value()
     {
          using namespace std::string_literals;
          return { R"(D:\Angelwing-Ultima_Remastered_v1-0-a)"s, R"(D:\dev\Field-Map-Editor\bin\RelWithDebInfo\deswizzle)"s };
     }
     static void post_load_operation([[maybe_unused]] const value_type &value)
     {
          assert(has_balanced_braces(value));
     }
};
template<>
struct SelectionInfo<ConfigKey::ExternalMapsDirectoryPaths>
{
     using value_type                     = std::vector<std::filesystem::path>;
     static constexpr std::string_view id = "ExternalMapsDirectoryPaths";
};
template<>
struct SelectionInfo<ConfigKey::SwizzlePath>
{
     using value_type                                     = std::filesystem::path;
     static constexpr std::string_view id                 = "SwizzlePath";
     static constexpr ConfigKey        default_value_copy = ConfigKey::FF8Path;
};
template<>
struct SelectionInfo<ConfigKey::DeswizzlePath>
{
     using value_type                                     = std::filesystem::path;
     static constexpr std::string_view id                 = "DeswizzlePath";
     static constexpr ConfigKey        default_value_copy = ConfigKey::FF8Path;
};
template<>
struct SelectionInfo<ConfigKey::OutputImagePath>
{
     using value_type                                     = std::filesystem::path;
     static constexpr std::string_view id                 = "OutputImagePath";
     static constexpr ConfigKey        default_value_copy = ConfigKey::FF8Path;
};
template<>
struct SelectionInfo<ConfigKey::OutputMimPath>
{
     using value_type                                     = std::filesystem::path;
     static constexpr std::string_view id                 = "OutputMimPath";
     static constexpr ConfigKey        default_value_copy = ConfigKey::FF8Path;
};
template<>
struct SelectionInfo<ConfigKey::OutputMapPath>
{
     using value_type                                     = std::filesystem::path;
     static constexpr std::string_view id                 = "OutputMapPath";
     static constexpr ConfigKey        default_value_copy = ConfigKey::FF8Path;
};
template<>
struct SelectionInfo<ConfigKey::CacheTexturePaths>
{
     using value_type                     = std::vector<std::filesystem::path>;
     static constexpr std::string_view id = "CacheTexturePaths";
};
template<>
struct SelectionInfo<ConfigKey::CacheSwizzlePathsEnabled>
{
     using value_type                     = std::vector<bool>;
     static constexpr std::string_view id = "CacheSwizzlePathsEnabled";
};
template<>
struct SelectionInfo<ConfigKey::CacheSwizzleAsOneImagePathsEnabled>
{
     using value_type                     = std::vector<bool>;
     static constexpr std::string_view id = "CacheSwizzleAsOneImagePathsEnabled";
};
template<>
struct SelectionInfo<ConfigKey::CacheDeswizzlePathsEnabled>
{
     using value_type                     = std::vector<bool>;
     static constexpr std::string_view id = "CacheDeswizzlePathsEnabled";
};
template<>
struct SelectionInfo<ConfigKey::CacheMapPaths>
{
     using value_type                     = std::vector<std::filesystem::path>;
     static constexpr std::string_view id = "CacheMapPaths";
};
template<>
struct SelectionInfo<ConfigKey::CacheMapPathsEnabled>
{
     using value_type                     = std::vector<bool>;
     static constexpr std::string_view id = "CacheMapPathsEnabled";
};


template<>
struct SelectionInfo<ConfigKey::FFNXModPath>
{
     using value_type                     = std::filesystem::path;
     static constexpr std::string_view id = "FFNXModPath";
     static value_type                 default_value()
     {
          return "mods/Textures";
     }
};
template<>
struct SelectionInfo<ConfigKey::FFNXOverridePath>
{
     using value_type                     = std::filesystem::path;
     static constexpr std::string_view id = "FFNXOverridePath";
     static value_type                 default_value()
     {
          return "override";
     }
};
template<>
struct SelectionInfo<ConfigKey::FFNXDirectPath>
{
     using value_type                     = std::filesystem::path;
     static constexpr std::string_view id = "FFNXDirectPath";
     static value_type                 default_value()
     {
          return "direct";
     }
};

template<ConfigKey Key>
struct SelectionUseFFNXConfig : std::false_type
{
};
template<>
struct SelectionUseFFNXConfig<ConfigKey::FFNXModPath> : std::true_type
{
};
template<>
struct SelectionUseFFNXConfig<ConfigKey::FFNXOverridePath> : std::true_type
{
};
template<>
struct SelectionUseFFNXConfig<ConfigKey::FFNXDirectPath> : std::true_type
{
};


template<typename ValueT>
struct SelectionLoadStrategy
{
     static bool load(const Configuration &config, std::string_view id, ValueT &value)
     {
          if (!config->contains(id))
          {
               return false;
          }
          if constexpr (std::same_as<ValueT, std::filesystem::path>)
          {
               value = config[id].value_or(value.u8string());
          }
          else if constexpr (std::convertible_to<ValueT, fme::color>)
          {
               value = std::bit_cast<fme::color>(config[id].value_or(std::bit_cast<std::uint32_t>(value)));
          }
          else if constexpr (requires { std::declval<ValueT>().raw(); })
          {
               value = ValueT{ config[id].value_or(value.raw()) };
          }
          else if constexpr (std::is_enum_v<ValueT>)
          {
               value = static_cast<ValueT>(config[id].value_or(std::to_underlying(value)));
          }
          else if constexpr (std::same_as<ValueT, std::vector<std::string>>)
          {
               return config.load_array(id, value);
          }
          else if constexpr (std::same_as<ValueT, std::vector<bool>>)
          {
               return config.load_array(id, value);
          }
          else if constexpr (std::same_as<ValueT, std::vector<std::filesystem::path>>)
          {
               return config.load_array(id, value);
          }
          else
          {
               value = config[id].value_or(value);
          }
          return true;
     }
};

// For filters that are constructed with full context and do not support default init
template<typename T, ff_8::FilterTag Tag>
struct SelectionLoadStrategy<ff_8::filter_old<T, Tag>>
{
     // No loading: object is fully initialized elsewhere
     static bool load(...)
     {
          return true;// We're returning true to prevent fall back logic from triggering.
     }
};

template<typename ValueT>
struct SelectionUpdateStrategy
{
     static void update(Configuration &config, std::string_view id, const ValueT &value)
     {
          if constexpr (std::same_as<ValueT, std::filesystem::path>)
          {
               std::u8string str_val = value.u8string();
               std::ranges::replace(str_val, u8'\\', u8'/');// normalize to forward slashes
               spdlog::info("selection<{}>: \"{}\"", id, std::filesystem::path(str_val).string());
               config->insert_or_assign(id, str_val);
          }
          else if constexpr (std::convertible_to<ValueT, fme::color>)
          {
               spdlog::info("selection<{}>: {}", id, value);
               config->insert_or_assign(id, std::bit_cast<std::uint32_t>(value));
          }
          else if constexpr (requires { std::declval<ValueT>().raw(); })
          {
               spdlog::info("selection<{}>: {}", id, value);
               config->insert_or_assign(id, value.raw());
          }
          else if constexpr (std::is_enum_v<ValueT>)
          {
               spdlog::info("selection<{}>: {}", id, value);
               config->insert_or_assign(id, std::to_underlying(value));
          }
          else if constexpr (std::same_as<ValueT, std::vector<std::string>>)
          {
               config.update_array(id, value);
          }
          else if constexpr (std::same_as<ValueT, std::vector<bool>>)
          {
               config.update_array(id, value);
          }
          else if constexpr (std::same_as<ValueT, std::vector<std::filesystem::path>>)
          {
               config.update_array(id, value);
          }
          else
          {
               spdlog::info("selection<{}>: {}", id, value);
               config->insert_or_assign(id, value);
          }
     }
};

// Skip updating for filters — they update themselves
template<typename T, ff_8::FilterTag Tag>
struct SelectionUpdateStrategy<ff_8::filter_old<T, Tag>>
{
     static void update(...)
     {
          // No-op: filter manages its own update
     }
};

struct SelectionBase
{
     virtual ~SelectionBase() = default;
};

template<ConfigKey Key>
struct Selection : SelectionBase
{
     using value_type = typename SelectionInfo<Key>::value_type;

     value_type value;
     Selection([[maybe_unused]] const Configuration &config, [[maybe_unused]] const std::optional<Configuration> &ffnx_config)
       : value([&]() {
            if constexpr (SelectionUseFFNXConfig<Key>::value)
            {
                 if (ffnx_config.has_value())
                 {
                      return get_default_value(&ffnx_config.value());
                 }
                 return get_default_value(nullptr);
            }
            else
            {
                 return get_default_value(&config);
            }
       }())
     {
          if constexpr (SelectionUseFFNXConfig<Key>::value)
          {
               if (ffnx_config.has_value())
               {
                    load(ffnx_config.value());
               }
          }
          else
          {
               load(config);
          }
     }

   private:
     static constexpr value_type get_default_value([[maybe_unused]] const Configuration *config)
     {
          if constexpr (requires(const Configuration &c) { SelectionInfo<Key>::default_value(c); })
          {
               if (config != nullptr)
               {
                    return SelectionInfo<Key>::default_value(*config);
               }
               else
               {
                    if constexpr (std::default_initializable<value_type>)
                    {
                         return {};// default-constructed
                    }
                    else
                    {
                         throw std::runtime_error("Selection not initialized and not default-initializable");
                    }
               }
          }
          else if constexpr (requires { SelectionInfo<Key>::default_value(); })
          {
               return SelectionInfo<Key>::default_value();
          }
          else if constexpr (std::default_initializable<value_type>)
          {
               return {};// default-constructed
          }
          else
          {
               throw std::runtime_error("Selection not initialized and not default-initializable");
          }
     }

     void fail_to_load([[maybe_unused]] const Configuration &config)
     {
          if constexpr (requires { SelectionInfo<Key>::expensive_default_value(); })
          {
               value = SelectionInfo<Key>::expensive_default_value();
          }
          else if constexpr (requires { SelectionInfo<Key>::default_value_copy; })
          {
               std::ignore =
                 SelectionLoadStrategy<value_type>::load(config, SelectionInfo<SelectionInfo<Key>::default_value_copy>::id, value);
          }
     }

     void load(const Configuration &config)
     {
          if (!SelectionLoadStrategy<value_type>::load(config, SelectionInfo<Key>::id, value))
          {
               fail_to_load(config);
          }
          if constexpr (requires(value_type &v) { SelectionInfo<Key>::post_load_operation(v); })
          {
               SelectionInfo<Key>::post_load_operation(value);
          }
     }

   public:
     // when we change directories we need to check for the ffnx config and refresh the values from that config.
     // go back to default value if ffnx_config not there.
     void refresh([[maybe_unused]] const std::optional<Configuration> &ffnx_config)
     {
          if constexpr (SelectionUseFFNXConfig<Key>::value)
          {
               if (ffnx_config.has_value())
               {
                    load(ffnx_config.value());
               }
               else
               {
                    value = get_default_value(nullptr);
               }
          }
     }

     // update skips over ffnx values as we're currently not writing to the ffnx config file.
     void update([[maybe_unused]] Configuration &config) const
     {
          if constexpr (!SelectionUseFFNXConfig<Key>::value)
          {
               SelectionUpdateStrategy<value_type>::update(config, SelectionInfo<Key>::id, value);
          }
     }
};

/**
 * @brief Manages various settings and selections for the application.
 *
 * This struct contains configuration options and runtime selections, such as window dimensions,
 * drawing modes, selected tiles, and rendering options.
 */
struct Selections
{
   private:
     static constexpr std::size_t SelectionsSizeT = static_cast<std::size_t>(fme::ConfigKey::All);
     using SelectionsArrayT                       = std::array<std::unique_ptr<SelectionBase>, SelectionsSizeT>;
     using SelectionsIDArrayT                     = std::array<std::string_view, SelectionsSizeT>;

     SelectionsArrayT load_selections_array(const Configuration &config)
     {
          // cache these values for use later on.
          std::optional<Configuration>         ffnx_config{};
          std::optional<std::filesystem::path> ff8_path{};
          return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> SelectionsArrayT {
               SelectionsArrayT result{};

               ((result[Is] = [&]<ConfigKey Key>() -> std::unique_ptr<Selection<Key>> {
                     if constexpr (ConfigKey::FF8Path == Key)
                     {
                          auto tmp    = std::make_unique<Selection<Key>>(config, ffnx_config);
                          ff8_path    = tmp->value;
                          ffnx_config = get_ffnx_config(tmp->value);
                          return std::move(tmp);
                     }
                     else
                     {
                          return std::make_unique<Selection<Key>>(config, ffnx_config);
                     }
                }.template operator()<static_cast<ConfigKey>(Is)>()),
                ...);
               return result;
          }(std::make_index_sequence<SelectionsSizeT>{});
     }
     SelectionsArrayT                    m_selections_array;

     static constexpr SelectionsIDArrayT load_selections_id_array()
     {
          return []<std::size_t... Is>(std::index_sequence<Is...>) constexpr {
               return SelectionsIDArrayT{ SelectionInfo<static_cast<ConfigKey>(Is)>::id... };
          }(std::make_index_sequence<SelectionsSizeT>{});
     }
     static constexpr SelectionsIDArrayT s_selections_id_array = load_selections_id_array();

     ///**
     // * @brief Refreshes FFNx-related paths based on the current FF8 path.
     // *
     // * This function must be rerun if the FF8 path changes, as the presence and location
     // * of FFNx components are path-dependent. It reads configuration from "FFNx.toml".
     // */
     // void                         refresh_ffnx_paths(const std::filesystem::path &ff8_path);
     std::optional<Configuration>        get_ffnx_config() const;
     std::optional<Configuration>        get_ffnx_config(const std::filesystem::path &ff8_path) const;

   public:
     /**
      * @brief Constructs a Selections object with default values.
      */
     Selections(const Configuration config = {});

     template<ConfigKey Key>
          requires(SelectionsSizeT > static_cast<std::size_t>(Key))
     auto &get()
     {
          static constexpr std::size_t index = static_cast<std::size_t>(Key);
          using ValueT                       = typename SelectionInfo<Key>::value_type;

          if (!m_selections_array[index])
          {
               if constexpr (std::default_initializable<ValueT>)
               {
                    throw std::runtime_error("Mutable access to default-initialized value is not supported");
               }
               else
               {
                    throw std::runtime_error("Selection not initialized and not default-initializable");
               }
          }
          Selection<Key> *selection = static_cast<Selection<Key> *>(m_selections_array[index].get());
          return selection->value;
     }

     template<ConfigKey Key>
          requires(SelectionsSizeT > static_cast<std::size_t>(Key))
     const auto &get() const
     {
          static constexpr std::size_t index = static_cast<std::size_t>(Key);
          using ValueT                       = typename SelectionInfo<Key>::value_type;

          if (!m_selections_array[index])
          {
               if constexpr (std::default_initializable<ValueT>)
               {
                    static const ValueT default_value{};
                    return default_value;
               }
               else
               {
                    throw std::runtime_error("Selection not initialized and not default-initializable");
               }
          }
          const Selection<Key> *selection = static_cast<const Selection<Key> *>(m_selections_array[index].get());
          return selection->value;
     }

     template<ConfigKey... Keys>
     void refresh()
     {
          if constexpr (sizeof...(Keys) == 1U && ((Keys == ConfigKey::All) && ...))
          {
               [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                    refresh<(static_cast<ConfigKey>(Is))...>();
               }(std::make_index_sequence<static_cast<std::size_t>(fme::ConfigKey::All)>{});
          }
          else
          {
               const std::optional<Configuration> ffnx_config = get_ffnx_config();
               (
                 [&]<ConfigKey Key> {
                      static constexpr std::size_t index = static_cast<std::size_t>(Key);
                      if (index >= std::ranges::size(m_selections_array) || !m_selections_array[index])
                      {
                           return;
                      }
                      Selection<Key> *selection = static_cast<Selection<Key> *>(m_selections_array[index].get());
                      selection->refresh(ffnx_config);
                 }.template operator()<Keys>(),
                 ...);
          }
     }


     template<ConfigKey... Keys>
     void update()
     {
          if constexpr (sizeof...(Keys) == 1U && ((Keys == ConfigKey::All) && ...))
          {
               [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                    update<(static_cast<ConfigKey>(Is))...>();
               }(std::make_index_sequence<static_cast<std::size_t>(fme::ConfigKey::All)>{});
          }
          else
          {
               Configuration config{};
               (
                 [&]<ConfigKey Key> {
                      static constexpr std::size_t index = static_cast<std::size_t>(Key);
                      if (index >= std::ranges::size(m_selections_array) || !m_selections_array[index])
                      {
                           return;
                      }
                      Selection<Key> *selection = static_cast<Selection<Key> *>(m_selections_array[index].get());
                      selection->update(config);
                 }.template operator()<Keys>(),
                 ...);
               config.save();
          }
     }

     void sort_paths();

     // void update_configuration() const;


     // template<ConfigKey... Keys>
     // void update_configuration_key() const
     // {
     //      Configuration config{};
     //      (
     //        [&]<ConfigKey Key> {
     //             if constexpr (!SelectionUseFFNXConfig<Key>::value)
     //                  update<Key>(config);
     //        }.template operator()<Keys>(),
     //        ...);
     //      config.save();
     // }


     //    private:
     //      template<ConfigKey K>
     //      void load(const Configuration &) = delete;// delete fine for load because all defined and used in Selections

     //      template<ConfigKey K>
     //           requires(static_cast<std::size_t>(K) <= static_cast<std::size_t>(ConfigKey::All))
     //      void update(Configuration &)
     //        const;// can't delete or else other cpp can't find the functions in Selections.cpp will get linker errors if missing.
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_SELECTIONS_HPP
