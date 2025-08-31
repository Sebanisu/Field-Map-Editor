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
#include "utilities.hpp"
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
     TomlPaths,
     TomlPath,
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
     DrawPupuMask,
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
     DisplayFiltersWindow,
     OutputSwizzlePattern,
     OutputDeswizzlePattern,
     OutputFullFileNamePattern,
     OutputTomlPattern,
     OutputMapPatternForSwizzle,
     OutputMapPatternForDeswizzle,
     OutputMapPatternForFullFileName,
     CurrentPattern,
     CurrentPatternIndex,
     BatchInputType,
     BatchInputRootPathType,
     BatchOutputType,
     BatchOutputRootPathType,
     BatchMapListEnabled,
     BackgroundCheckerboardScale,
     BackgroundColor,
     BackgroundColor2,
     BackgroundSettings,
     BatchInputPath,
     BatchOutputPath,
     BatchInputLoadMap,
     BatchOutputSaveMap,
     BatchGenerateColorfulMask,
     BatchGenerateWhiteOnBlackMask,
     PathPatternsWithPaletteAndTexturePage,
     PathPatternsWithPalette,
     PathPatternsWithTexturePage,
     PathPatternsWithPupuID,
     PathPatternsWithFullFileName,
     PatternsBase,
     PatternsCommonPrefixes,
     PatternsCommonPrefixesForMaps,
     ExternalTexturesAndMapsDirectoryPaths,

     SwizzlePath,
     SwizzleAsOneImagePath,
     DeswizzlePath,
     FullFileNamePath,
     OutputImagePath,
     OutputMimPath,
     OutputMapPath,

     CacheTextureAndMapPaths,
     CacheSwizzlePathsEnabled,
     CacheSwizzleAsOneImagePathsEnabled,
     CacheDeswizzlePathsEnabled,
     CacheFullFileNamePathsEnabled,
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

template<ConfigKey... Keys>
consteval bool has_duplicate_keys()
{
     std::array<ConfigKey, sizeof...(Keys)> arr = { Keys... };

     if (std::ranges::any_of(arr, [](ConfigKey k) { return k == ConfigKey::All; }) && sizeof...(Keys) > 1U)
     {
          // if we need to match keys above All we could adjust the function to allow that.
          return true;// ConfigKey::All and something else present
     }

     std::ranges::sort(arr);
     return std::ranges::adjacent_find(arr) != arr.end();
}

template<ConfigKey... Keys>
consteval bool has_valid_keys()
{
     using UT                            = std::underlying_type_t<ConfigKey>;
     std::array<UT, sizeof...(Keys)> arr = { std::to_underlying(Keys)... };

     if constexpr (std::signed_integral<UT>)
     {
          const auto minmax_value = std::ranges::minmax_element(arr);
          const auto &[min, max]  = minmax_value;
          return *min >= UT{} && *max <= std::to_underlying(ConfigKey::All);
     }
     else
     {
          const auto max_value = std::ranges::max_element(arr);
          return *max_value <= std::to_underlying(ConfigKey::All);
     }
}


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
struct SelectionInfo<ConfigKey::TomlPath>
{
     using value_type                     = std::filesystem::path;
     static constexpr std::string_view id = "TomlPath";
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
struct SelectionInfo<ConfigKey::DrawPupuMask>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DrawPupuMask";
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
     static constexpr value_type       default_value()
     {
          return true;
     }
};
template<>
struct SelectionInfo<ConfigKey::DisplayDrawWindow>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DisplayDrawWindow";
     static constexpr value_type       default_value()
     {
          return true;
     }
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
struct SelectionInfo<ConfigKey::DisplayFiltersWindow>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "DisplayFiltersWindow";
};

template<>
struct SelectionInfo<ConfigKey::OutputSwizzlePattern>
{
     using value_type                     = std::string;
     static constexpr std::string_view id = "OutputSwizzlePattern";
     static inline value_type          default_value()
     {
          using namespace std::string_literals;
          return "{selected_path}\\{demaster}"s;
     }
};
template<>
struct SelectionInfo<ConfigKey::OutputDeswizzlePattern>
{
     using value_type                     = std::string;
     static constexpr std::string_view id = "OutputDeswizzlePattern";
     static inline value_type          default_value()
     {
          using namespace std::string_literals;
          return "{selected_path}\\deswizzle\\{demaster}"s;
     }
};

template<>
struct SelectionInfo<ConfigKey::OutputFullFileNamePattern>
{
     using value_type                     = std::string;
     static constexpr std::string_view id = "OutputFullFileNamePattern";
     static inline value_type          default_value()
     {
          using namespace std::string_literals;
          return "{selected_path}\\full_filename\\{demaster_full}"s;
     }
};

template<>
struct SelectionInfo<ConfigKey::OutputTomlPattern>
{
     using value_type                     = std::string;
     static constexpr std::string_view id = "OutputTomlPattern";
     static inline value_type          default_value()
     {
          using namespace std::string_literals;
          return R"({{current_toml_path}?:{current_path}\res\deswizzle.toml:?})"s;
     }
};

template<>
struct SelectionInfo<ConfigKey::OutputMapPatternForSwizzle>
{
     using value_type                     = std::string;
     static constexpr std::string_view id = "OutputMapPatternForSwizzle";
     static inline value_type          default_value()
     {
          using namespace std::string_literals;
          return "{selected_path}\\{demaster}"s;
     }
};
template<>
struct SelectionInfo<ConfigKey::OutputMapPatternForDeswizzle>
{
     using value_type                     = std::string;
     static constexpr std::string_view id = "OutputMapPatternForDeswizzle";
     static inline value_type          default_value()
     {
          using namespace std::string_literals;
          return "{selected_path}\\deswizzle\\{demaster}"s;
     }
};
template<>
struct SelectionInfo<ConfigKey::OutputMapPatternForFullFileName>
{
     using value_type                     = std::string;
     static constexpr std::string_view id = "OutputMapPatternForFullFileName";
     static inline value_type          default_value()
     {
          using namespace std::string_literals;
          return "{selected_path}\\full_filename\\{demaster}"s;
     }
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
struct SelectionInfo<ConfigKey::BackgroundCheckerboardScale>
{
     using value_type                     = std::uint16_t;
     static constexpr std::string_view id = "BackgroundCheckerboardScale";
     static inline value_type          default_value()
     {
          return 4U;
     }
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
struct SelectionInfo<ConfigKey::BackgroundColor2>
{
     using value_type                     = color;
     static constexpr std::string_view id = "BackgroundColor2";
     static inline value_type          default_value()
     {
          return fme::colors::White;
     }
};
template<>
struct SelectionInfo<ConfigKey::BackgroundSettings>
{
     using value_type                     = BackgroundSettings;
     static constexpr std::string_view id = "BackgroundSettings";
     static inline value_type          default_value()
     {
          return BackgroundSettings::Default;
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
     static constexpr value_type       default_value()
     {
          return true;
     }
};

template<>
struct SelectionInfo<ConfigKey::BatchGenerateColorfulMask>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "BatchGenerateColorfulMask";
     static constexpr value_type       default_value()
     {
          return true;
     }
};

template<>
struct SelectionInfo<ConfigKey::BatchGenerateWhiteOnBlackMask>
{
     using value_type                     = bool;
     static constexpr std::string_view id = "BatchGenerateWhiteOnBlackMask";
     static constexpr value_type       default_value()
     {
          return false;
     }
};

template<>
struct SelectionInfo<ConfigKey::BatchCompactEnabled>
{
     static constexpr std::string_view id = ff_8::ConfigKeys<ff_8::FilterTag::Compact>::enabled_key_name;
};
template<>
struct SelectionInfo<ConfigKey::BatchCompactType>
{
     using value_type                     = ff_8::filter_old<ff_8::FilterTag::Compact>;
     static constexpr std::string_view id = ff_8::ConfigKeys<ff_8::FilterTag::Compact>::key_name;
     static inline value_type          default_value(const Configuration &config)
     {
          return { true, config };
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
     using value_type                     = ff_8::filter_old<ff_8::FilterTag::Flatten>;
     static constexpr std::string_view id = ff_8::ConfigKeys<ff_8::FilterTag::Flatten>::key_name;
     static inline value_type          default_value(const Configuration &config)
     {
          return { true, config };
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
          return { "{selected_path}/{field_name}_{_{2_letter_lang}_{pupu_id}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}_{_{2_letter_lang}_{pupu_id}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}_{_{2_letter_lang}_{pupu_id}{ext}"s

                   "{selected_path}/{field_name}_{pupu_id}{ext}"s,
                   "{selected_path}/{field_name}/{field_name}_{pupu_id}{ext}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}_{pupu_id}{ext}"s };
     }
     static void post_load_operation([[maybe_unused]] const value_type &value)
     {
          assert(has_balanced_braces(value));
     }
};

template<>
struct SelectionInfo<ConfigKey::PathPatternsWithFullFileName>
{
     using value_type                     = std::vector<std::string>;
     static constexpr std::string_view id = "PathPatternsWithFullFileName";
     static value_type                 expensive_default_value()
     {
          using namespace std::string_literals;
          return { "{selected_path}/{full_filename}"s,
                   "{selected_path}/{field_name}/{full_filename}"s,
                   "{selected_path}/{field_prefix}/{field_name}/{full_filename}"s };
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
struct SelectionInfo<ConfigKey::TomlPaths>
{
     using value_type                     = std::vector<std::filesystem::path>;
     static constexpr std::string_view id = "TomlPaths";
     // static value_type                 expensive_default_value()
     // {
     //      const auto &default_paths = open_viii::Paths::get();
     //      return { default_paths.begin(), default_paths.end() };
     // }
     // static void post_load_operation([[maybe_unused]] const value_type &value)
     // {
     //      assert(has_balanced_braces(value));
     // }
};

template<>
struct SelectionInfo<ConfigKey::ExternalTexturesAndMapsDirectoryPaths>
{
     using value_type                     = std::vector<std::filesystem::path>;
     static constexpr std::string_view id = "ExternalTexturesAndMapsDirectoryPaths";
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
struct SelectionInfo<ConfigKey::SwizzlePath>
{
     using value_type                                     = std::filesystem::path;
     static constexpr std::string_view id                 = "SwizzlePath";
     static constexpr ConfigKey        default_value_copy = ConfigKey::FF8Path;
};
template<>
struct SelectionInfo<ConfigKey::SwizzleAsOneImagePath>
{
     using value_type                                     = std::filesystem::path;
     static constexpr std::string_view id                 = "SwizzleAsOneImagePath";
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
struct SelectionInfo<ConfigKey::FullFileNamePath>
{
     using value_type                                     = std::filesystem::path;
     static constexpr std::string_view id                 = "FullFileNamePath";
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
struct SelectionInfo<ConfigKey::CacheTextureAndMapPaths>
{
     using value_type                     = std::vector<std::filesystem::path>;
     static constexpr std::string_view id = "CacheTextureAndMapPaths";
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
struct SelectionInfo<ConfigKey::CacheFullFileNamePathsEnabled>
{
     using value_type                     = std::vector<bool>;
     static constexpr std::string_view id = "CacheFullFileNamePathsEnabled";
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
          else if constexpr (glengine::is_std_vector<ValueT>)
          {
               if constexpr (std::is_enum_v<glengine::vector_elem_type_t<ValueT>>)
               {
                    return fme::Configuration::
                      load_array<glengine::vector_elem_type_t<ValueT>, std::underlying_type_t<glengine::vector_elem_type_t<ValueT>>>(
                        config, id, value);
               }
               else if constexpr (std::same_as<glengine::vector_elem_type_t<ValueT>, ff_8::PupuID>)
               {
                    return fme::Configuration::load_array<glengine::vector_elem_type_t<ValueT>, std::uint32_t>(config, id, value);
               }
               else
               {
                    return fme::Configuration::load_array<glengine::vector_elem_type_t<ValueT>>(config, id, value);
               }
          }
          else
          {
               value = config[id].value_or(value);
          }
          if constexpr (std::ranges::range<ValueT>)
          {
               if (std::ranges::empty(value))
               {
                    return false;
               }
          }
          return true;
     }
};

// For filters that are constructed with full context and do not support default init
template<ff_8::FilterTag Tag>
struct SelectionLoadStrategy<ff_8::filter_old<Tag>>
{
     // No loading: object is fully initialized elsewhere
     static bool load(auto &&...) noexcept
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
               Configuration::update_array(config, id, value);
          }
          else if constexpr (std::same_as<ValueT, std::vector<bool>>)
          {
               Configuration::update_array(config, id, value);
          }
          else if constexpr (std::same_as<ValueT, std::vector<std::filesystem::path>>)
          {
               Configuration::update_array(config, id, value);
          }
          else
          {
               spdlog::info("selection<{}>: {}", id, value);
               config->insert_or_assign(id, value);
          }
     }
};

// Skip updating for filters — they update themselves
template<ff_8::FilterTag Tag>
struct SelectionUpdateStrategy<ff_8::filter_old<Tag>>
{
     static void update(auto &&...) noexcept
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
          else
          {
               value = get_default_value(nullptr);
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
;
consteval inline auto load_selections_id_array()
{
     return []<std::size_t... Is>(std::index_sequence<Is...>) constexpr {
          return std::array<std::string_view, static_cast<std::size_t>(fme::ConfigKey::All)>{
               SelectionInfo<static_cast<ConfigKey>(Is)>::id...
          };
     }(std::make_index_sequence<static_cast<std::size_t>(fme::ConfigKey::All)>{});
}

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
     SelectionsArrayT                                                      m_selections_array;

     static inline constexpr std::array<std::string_view, SelectionsSizeT> s_selections_id_array = load_selections_id_array();

     SelectionsArrayT                                                      load_selections_array(const Configuration &config)
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

     ///**
     // * @brief Refreshes FFNx-related paths based on the current FF8 path.
     // *
     // * This function must be rerun if the FF8 path changes, as the presence and location
     // * of FFNx components are path-dependent. It reads configuration from "FFNx.toml".
     // */
     // void                         refresh_ffnx_paths(const std::filesystem::path &ff8_path);
     std::optional<Configuration> get_ffnx_config() const;
     std::optional<Configuration> get_ffnx_config(const std::filesystem::path &ff8_path) const;

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
          requires(has_valid_keys<Key>())
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

     const auto get_id(ConfigKey key)
     {
          assert(
            static_cast<std::size_t>(key) < std::ranges::size(s_selections_id_array)
            && "Key out of range, must be less than ConfigKey::All");
          return s_selections_id_array[static_cast<std::size_t>(key)];
     }

     template<ConfigKey... Keys>
          requires(!has_duplicate_keys<Keys...>() && has_valid_keys<Keys...>())
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
          requires(!has_duplicate_keys<Keys...>() && has_valid_keys<Keys...>())
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
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_SELECTIONS_HPP
