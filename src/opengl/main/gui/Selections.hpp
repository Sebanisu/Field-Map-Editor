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
     FF8DirectoryPaths,
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

     // All is used to map all values less than All.
     All,

     // Filters not required by update or load.
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
          case ConfigKey::DrawMode:
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
          case ConfigKey::ImportSelectedTile:
               return "selections_selected_tile"sv;
          case ConfigKey::SelectionsPath:
               return "selections_path"sv;
          case ConfigKey::StarterField:
               return "starter_field"sv;
          case ConfigKey::SwizzlePath:
               return "selections_swizzle_path"sv;
          case ConfigKey::TileSizeValue:
               return "selections_tile_size_value"sv;
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

template<ConfigKey Key>
struct SelectionInfo;

template<>
struct SelectionInfo<ConfigKey::StarterField>
{
     using value_type = std::string;
     static constexpr value_type default_value()
     {
          return "ecenter3";
     }
};
template<>
struct SelectionInfo<ConfigKey::SelectionsPath>
{
     using value_type = std::int32_t;
};
template<>
struct SelectionInfo<ConfigKey::WindowWidth>
{
     using value_type = std::int32_t;
     static constexpr value_type default_value()
     {
          return 1280;
     }
};
template<>
struct SelectionInfo<ConfigKey::WindowHeight>
{
     using value_type = std::int32_t;
     static constexpr value_type default_value()
     {
          return 720;
     }
};
template<>
struct SelectionInfo<ConfigKey::Palette>
{
     using value_type = std::uint8_t;
     static constexpr void post_load_operation(value_type &value)
     {
          value = value & 0xFU;
     }
};
template<>
struct SelectionInfo<ConfigKey::Bpp>
{
     using value_type = open_viii::graphics::BPPT;
};
template<>
struct SelectionInfo<ConfigKey::DrawMode>
{
     using value_type = draw_mode;
     static constexpr value_type default_value()
     {
          return draw_mode::draw_map;
     }
};
template<>
struct SelectionInfo<ConfigKey::Coo>
{
     using value_type = std::int32_t;
};
template<>
struct SelectionInfo<ConfigKey::ImportSelectedTile>
{
     using value_type = std::int32_t;
};
template<>
struct SelectionInfo<ConfigKey::DrawDisableBlending>
{
     using value_type = bool;
};
template<>
struct SelectionInfo<ConfigKey::DrawGrid>
{
     using value_type = bool;
};
template<>
struct SelectionInfo<ConfigKey::DrawPalette>
{
     using value_type = bool;
};
template<>
struct SelectionInfo<ConfigKey::DrawSwizzle>
{
     using value_type = bool;
};
template<>
struct SelectionInfo<ConfigKey::RenderImportedImage>
{
     using value_type = std::int32_t;
};
template<>
struct SelectionInfo<ConfigKey::DrawTexturePageGrid>
{
     using value_type = bool;
};
template<>
struct SelectionInfo<ConfigKey::DrawTileConflictRects>
{
     using value_type = bool;
};
template<>
struct SelectionInfo<ConfigKey::DisplayBatchWindow>
{
     using value_type = bool;
};
// template<>
// struct SelectionInfo<ConfigKey::DisplayImportImageWindow>
// {
//      using value_type = bool;
// };
template<>
struct SelectionInfo<ConfigKey::DisplayImportImage>
{
     using value_type = bool;
};
template<>
struct SelectionInfo<ConfigKey::ForceReloadingOfTextures>
{
     using value_type = std::int32_t;
};
template<>
struct SelectionInfo<ConfigKey::ForceRenderingOfMap>
{
     using value_type = std::int32_t;
};
template<>
struct SelectionInfo<ConfigKey::ImportImageGrid>
{
     using value_type = std::int32_t;
};
template<>
struct SelectionInfo<ConfigKey::ImportLoadImageDirectory>
{
     using value_type = std::int32_t;
};
template<>
struct SelectionInfo<ConfigKey::TileSizeValue>
{
     using value_type = std::int32_t;
};
template<>
struct SelectionInfo<ConfigKey::DisplayHistoryWindow>
{
     using value_type = bool;
};
template<>
struct SelectionInfo<ConfigKey::DisplayControlPanelWindow>
{
     using value_type = bool;
};
template<>
struct SelectionInfo<ConfigKey::DisplayDrawWindow>
{
     using value_type = bool;
};
template<>
struct SelectionInfo<ConfigKey::DisplayCustomPathsWindow>
{
     using value_type = bool;
};
template<>
struct SelectionInfo<ConfigKey::DisplayFieldFileWindow>
{
     using value_type = bool;
};
template<>
struct SelectionInfo<ConfigKey::OutputSwizzlePattern>
{
     using value_type = std::string;
};
template<>
struct SelectionInfo<ConfigKey::OutputDeswizzlePattern>
{
     using value_type = std::string;
};
template<>
struct SelectionInfo<ConfigKey::OutputMapPatternForSwizzle>
{
     using value_type = std::string;
};
template<>
struct SelectionInfo<ConfigKey::OutputMapPatternForDeswizzle>
{
     using value_type = std::string;
};
template<>
struct SelectionInfo<ConfigKey::CurrentPattern>
{
     using value_type = PatternSelector;
};
template<>
struct SelectionInfo<ConfigKey::CurrentPatternIndex>
{
     using value_type = std::int32_t;
};
template<>
struct SelectionInfo<ConfigKey::BatchInputType>
{
     using value_type = input_types;
};
template<>
struct SelectionInfo<ConfigKey::BatchInputRootPathType>
{
     using value_type = root_path_types;
};
template<>
struct SelectionInfo<ConfigKey::BatchOutputType>
{
     using value_type = output_types;
};
template<>
struct SelectionInfo<ConfigKey::BatchOutputRootPathType>
{
     using value_type = root_path_types;
};
template<>
struct SelectionInfo<ConfigKey::BatchMapListEnabled>
{
     using value_type = std::vector<bool>;
};
template<>
struct SelectionInfo<ConfigKey::BackgroundColor>
{
     using value_type = color;
};
template<>
struct SelectionInfo<ConfigKey::BatchInputPath>
{
     using value_type = std::string;
};
template<>
struct SelectionInfo<ConfigKey::BatchOutputPath>
{
     using value_type = std::string;
};
template<>
struct SelectionInfo<ConfigKey::BatchInputLoadMap>
{
     using value_type = bool;
};
template<>
struct SelectionInfo<ConfigKey::BatchOutputSaveMap>
{
     using value_type = bool;
};
template<>
struct SelectionInfo<ConfigKey::BatchCompactType>
{
     using value_type = ff_8::filter_old<compact_type, ff_8::FilterTag::Compact>;
     static inline value_type default_value(const Configuration &config)
     {
          return { static_cast<fme::compact_type>(
                     config[key_to_string(ConfigKey::BatchCompactType)].value_or(std::to_underlying(fme::compact_type{}))),
                   ff_8::WithFlag(
                     ff_8::FilterSettings::Default,
                     ff_8::FilterSettings::Toggle_Enabled,
                     config[key_to_string(ConfigKey::BatchCompactEnabled)].value_or(false)) };
     }
};
template<>
struct SelectionInfo<ConfigKey::BatchFlattenType>
{
     using value_type = ff_8::filter_old<flatten_type, ff_8::FilterTag::Flatten>;
     static inline value_type default_value(const Configuration &config)
     {
          return { static_cast<fme::flatten_type>(
                     config[key_to_string(ConfigKey::BatchFlattenType)].value_or(std::to_underlying(fme::flatten_type{}))),
                   ff_8::WithFlag(
                     ff_8::FilterSettings::Default,
                     ff_8::FilterSettings::Toggle_Enabled,
                     config[key_to_string(ConfigKey::BatchFlattenEnabled)].value_or(false)) };
     }
};
template<>
struct SelectionInfo<ConfigKey::PathPatternsWithPaletteAndTexturePage>
{
     using value_type = std::string;
};
template<>
struct SelectionInfo<ConfigKey::PathPatternsWithPalette>
{
     using value_type = std::string;
};
template<>
struct SelectionInfo<ConfigKey::PathPatternsWithTexturePage>
{
     using value_type = std::string;
};
template<>
struct SelectionInfo<ConfigKey::PathPatternsWithPupuID>
{
     using value_type = std::string;
};
template<>
struct SelectionInfo<ConfigKey::PatternsBase>
{
     using value_type = std::vector<std::string>;
};
template<>
struct SelectionInfo<ConfigKey::PatternsCommonPrefixes>
{
     using value_type = std::vector<std::string>;
};
template<>
struct SelectionInfo<ConfigKey::PatternsCommonPrefixesForMaps>
{
     using value_type = std::vector<std::string>;
};
template<>
struct SelectionInfo<ConfigKey::FF8DirectoryPaths>
{
     using value_type = std::vector<std::filesystem::path>;
};
template<>
struct SelectionInfo<ConfigKey::ExternalTexturesDirectoryPaths>
{
     using value_type = std::vector<std::filesystem::path>;
};
template<>
struct SelectionInfo<ConfigKey::ExternalMapsDirectoryPaths>
{
     using value_type = std::vector<std::filesystem::path>;
};
template<>
struct SelectionInfo<ConfigKey::SwizzlePath>
{
     using value_type = std::filesystem::path;
};
template<>
struct SelectionInfo<ConfigKey::DeswizzlePath>
{
     using value_type = std::filesystem::path;
};
template<>
struct SelectionInfo<ConfigKey::OutputImagePath>
{
     using value_type = std::filesystem::path;
};
template<>
struct SelectionInfo<ConfigKey::OutputMimPath>
{
     using value_type = std::filesystem::path;
};
template<>
struct SelectionInfo<ConfigKey::OutputMapPath>
{
     using value_type = std::filesystem::path;
};
template<>
struct SelectionInfo<ConfigKey::CacheTexturePaths>
{
     using value_type = std::vector<std::filesystem::path>;
};
template<>
struct SelectionInfo<ConfigKey::CacheSwizzlePathsEnabled>
{
     using value_type = std::vector<bool>;
};
template<>
struct SelectionInfo<ConfigKey::CacheSwizzleAsOneImagePathsEnabled>
{
     using value_type = std::vector<bool>;
};
template<>
struct SelectionInfo<ConfigKey::CacheDeswizzlePathsEnabled>
{
     using value_type = std::vector<bool>;
};
template<>
struct SelectionInfo<ConfigKey::CacheMapPaths>
{
     using value_type = std::vector<std::filesystem::path>;
};
template<>
struct SelectionInfo<ConfigKey::CacheMapPathsEnabled>
{
     using value_type = std::vector<bool>;
};


template<>
struct SelectionInfo<ConfigKey::FFNXModPath>
{
     using value_type = std::filesystem::path;
};
template<>
struct SelectionInfo<ConfigKey::FFNXOverridePath>
{
     using value_type = std::filesystem::path;
};
template<>
struct SelectionInfo<ConfigKey::FFNXDirectPath>
{
     using value_type = std::filesystem::path;
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
     static bool load(const Configuration &config, std::string_view config_key, ValueT &value)
     {
          if constexpr (std::convertible_to<ValueT, std::filesystem::path>)
          {
               return config[config_key].value_or(value.u8string());
          }
          else if constexpr (requires { std::declval<ValueT>().raw(); })
          {
               return config[config_key].value_or(value.raw());
          }
          else if constexpr (std::is_enum_v<ValueT>)
          {
               value = static_cast<ValueT>(config[config_key].value_or(std::to_underlying(value)));
          }
          else
          {
               value = config[config_key].value_or(value);
          }
          return true;
     }
};

template<typename ValueT>
struct SelectionUpdateStrategy
{
     static void update(Configuration &config, std::string_view config_key, const ValueT &value)
     {
          if constexpr (std::convertible_to<ValueT, std::filesystem::path>)
          {
               std::u8string str_val = value.u8string();
               std::ranges::replace(str_val, u8'\\', u8'/');// normalize to forward slashes
               spdlog::info("selection<{}>: \"{}\"", config_key, std::filesystem::path(str_val).string());
               config->insert_or_assign(config_key, str_val);
          }
          else if constexpr (requires { std::declval<ValueT>().raw(); })
          {
               spdlog::info("selection<{}>: {}", config_key, value);
               config->insert_or_assign(config_key, value.raw());
          }
          else if constexpr (std::is_enum_v<ValueT>)
          {
               spdlog::info("selection<{}>: {}", config_key, value);
               config->insert_or_assign(config_key, std::to_underlying(value));
          }
          else
          {
               spdlog::info("selection<{}>: {}", config_key, value);
               config->insert_or_assign(config_key, value);
          }
     }
};


template<ConfigKey Key>
struct Selection
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
       })
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
          }
          else if constexpr (requires { SelectionInfo<Key>::default_value(); })
          {
               return SelectionInfo<Key>::default_value();
          }
          else
          {
               return {};// default-constructed
          }
     }

     static constexpr value_type get_expensive_default_value()
     {
          if constexpr (requires { SelectionInfo<Key>::expensive_default_value(); })
          {
               return SelectionInfo<Key>::expensive_default_value();
          }
          else
          {
               return {};// default-constructed
          }
     }

     void load(const Configuration &config)
     {
          if (!SelectionLoadStrategy<value_type>::load(config, config_key, value))
          {
               value = get_expensive_default_value();
          }
          if constexpr (requires(value_type &v) { SelectionInfo<Key>::post_load_operation(v); })
          {
               SelectionInfo<Key>::post_load_operation(value);
          }
     }

   public:
     // when we change directories we need to check for the ffnx config and refresh the values from that config.
     // go back to default value if ffnx_config not there.
     void refresh([[maybe_unused]] const std::optional<Configuration> &ffnx_config) const
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
               SelectionUpdateStrategy<value_type>::update(config, config_key, value);
          }
     }

     constexpr static std::string_view config_key = key_to_string(Key);
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
          (
            [&]<ConfigKey Key> {
                 if constexpr (!SelectionUseFFNXConfig<Key>::value)
                      update<Key>(config);
            }.template operator()<Keys>(),
            ...);
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
          requires(static_cast<std::size_t>(K) <= static_cast<std::size_t>(ConfigKey::All))
     void update(Configuration &)
       const;// can't delete or else other cpp can't find the functions in Selections.cpp will get linker errors if missing.
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_SELECTIONS_HPP
