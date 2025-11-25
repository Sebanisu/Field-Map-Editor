//
// Created by pcvii on 11/15/2021.
//

#ifndef FIELD_MAP_EDITOR_FORMATTERS_HPP
#define FIELD_MAP_EDITOR_FORMATTERS_HPP
#include "gui/BackgroundSettings.hpp"
#include "gui/compact_type.hpp"
#include "gui/draw_mode.hpp"
#include "gui/gui_labels.hpp"
#include "tile_sizes.hpp"
#include <ff_8/Formatters.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <functional>
#include <glengine/Formatters.hpp>
#include <open_viii/graphics/background/BlendModeT.hpp>
#include <open_viii/graphics/background/Map.hpp>
#include <open_viii/graphics/BPPT.hpp>
#include <open_viii/strings/LangCommon.hpp>

template<>
struct fmt::formatter<tile_sizes> : fmt::formatter<std::string_view>
{
     // tile_sizes::default_size, tile_sizes::x_2_size, tile_sizes::x_4_size,
     // tile_sizes::x_8_size, tile_sizes::x_16_size
     //  parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(
       tile_sizes     tile_sizes_t,
       FormatContext &ctx) const
     {
          using namespace open_viii::graphics::background;
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (tile_sizes_t)
          {
               case tile_sizes::default_size:
                    name = "1x   16 px"sv;
                    break;
               case tile_sizes::x_2_size:
                    name = "2x   32 px"sv;
                    break;
               case tile_sizes::x_4_size:
                    name = "4x   64 px"sv;
                    break;
               case tile_sizes::x_8_size:
                    name = "8x  128 px"sv;
                    break;
               case tile_sizes::x_16_size:
                    name = "16x 256 px"sv;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};

template<>
struct fmt::formatter<draw_mode> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(
       draw_mode      draw_mode_t,
       FormatContext &ctx) const
     {
          using namespace open_viii::graphics::background;
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (draw_mode_t)
          {
               case draw_mode::draw_mim:
                    name = open_viii::graphics::background::Mim::EXT;
                    break;
               case draw_mode::draw_map:
                    name = open_viii::graphics::background::Map::EXT;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};

template<>
struct fmt::formatter<fme::FailOverLevels> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(
       fme::FailOverLevels level,
       FormatContext      &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (level)
          {
               case fme::FailOverLevels::EN:
                    name = open_viii::LangCommon::ENFULL;
                    break;
               case fme::FailOverLevels::FR:
                    name = open_viii::LangCommon::FRFULL;
                    break;
               case fme::FailOverLevels::DE:
                    name = open_viii::LangCommon::DEFULL;
                    break;
               case fme::FailOverLevels::IT:
                    name = open_viii::LangCommon::ITFULL;
                    break;
               case fme::FailOverLevels::ES:
                    name = open_viii::LangCommon::ESFULL;
                    break;
               case fme::FailOverLevels::JP:
                    name = open_viii::LangCommon::JPFULL;
                    break;
               case fme::FailOverLevels::Generic:
                    name = "Generic"sv;
                    break;
               case fme::FailOverLevels::All:
                    name = "All"sv;
                    break;
               case fme::FailOverLevels::Loaded:
                    name = "Loaded"sv;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};


template<>
struct fmt::formatter<fme::BackgroundSettings>
{
     // parse is default; no custom format parsing needed
     template<typename ParseContext>
     constexpr auto parse(ParseContext &ctx)
     {
          return ctx.begin();
     }

     template<typename FormatContext>
     auto format(
       fme::BackgroundSettings bs,
       FormatContext          &ctx) const
     {
          using enum fme::BackgroundSettings;

          if (bs == Default)
          {
               return fmt::format_to(
                 ctx.out(), "Default (OneColor | Checkerboard)");
          }

          bool       first      = true;

          const auto write_part = [&](std::string_view part)
          {
               if (!first)
                    fmt::format_to(ctx.out(), " | ");
               fmt::format_to(ctx.out(), "{}", part);
               first = false;
          };

          if (HasFlag(bs, TwoColors))
               write_part("TwoColors");
          else
               write_part("OneColor");

          if (HasFlag(bs, Solid))
               write_part("Solid");
          else
               write_part("Checkerboard");

          return ctx.out();
     }
};


// template<>
// struct fmt::formatter<fme::color>
// {
//      // Parses format specs; in this case, we don't support any custom
//      // formatting
//      constexpr auto parse(format_parse_context &ctx)
//      {
//           return ctx.begin();// no custom formatting, so just return the end
//      }

//      // Formats the color as "(r,g,b,a)"
//      template<typename FormatContext>
//      auto format(
//        const fme::color &c,
//        FormatContext    &ctx) const
//      {
//           return fmt::format_to(
//             ctx.out(), "({:>3},{:>3},{:>3},{:>3})", c.r, c.g, c.b, c.a);
//      }
// };

template<>
struct fmt::formatter<fme::root_path_types> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(
       fme::root_path_types in_root_path_types,
       FormatContext       &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (in_root_path_types)
          {
               case fme::root_path_types::selected_path:
                    name = fme::gui_labels::selected_path;
                    break;
               case fme::root_path_types::ff8_path:
                    name = fme::gui_labels::ff8_path;
                    break;
               case fme::root_path_types::current_path:
                    name = fme::gui_labels::current_path;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};

template<>
struct fmt::formatter<fme::PatternSelector> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(
       fme::PatternSelector pattern_selector,
       FormatContext       &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (pattern_selector)
          {
               case fme::PatternSelector::OutputSwizzlePattern:
                    name = "Output swizzle pattern"sv;
                    break;
               case fme::PatternSelector::OutputSwizzleAsOneImagePattern:
                    name = "Output Swizzle as One Image Pattern"sv;
                    break;
               case fme::PatternSelector::OutputDeswizzlePattern:
                    name = "Output deswizzle pattern"sv;
                    break;
               case fme::PatternSelector::OutputFullFileNamePattern:
                    name = "Output Full Filename Pattern (.toml)"sv;
                    break;
               case fme::PatternSelector::OutputTomlPattern:
                    name = "Output '.toml' Pattern"sv;
                    break;
               case fme::PatternSelector::OutputMapPatternForSwizzle:
                    name = "Output map pattern for swizzle"sv;
                    break;
               case fme::PatternSelector::OutputMapPatternForDeswizzle:
                    name = "Output map pattern for deswizzle"sv;
                    break;
               case fme::PatternSelector::OutputMapPatternForFullFileName:
                    name = "Output map pattern for Full Filename (.toml)"sv;
                    break;
               case fme::PatternSelector::PatternsCommonPrefixes:
                    name = "Path Patterns Common Swizzle"sv;
                    break;
               case fme::PatternSelector::PatternsCommonPrefixesForMaps:
                    name = "Path Patterns Common Swizzle For Maps"sv;
                    break;
               case fme::PatternSelector::PatternsBase:
                    name = "Path Patterns No Palette And Texture Page"sv;
                    break;
               case fme::PatternSelector::PathPatternsWithPaletteAndTexturePage:
                    name = "Path Patterns With Palette And Texture Page"sv;
                    break;
               case fme::PatternSelector::PathPatternsWithPalette:
                    name = "Path Patterns With Palette"sv;
                    break;
               case fme::PatternSelector::PathPatternsWithTexturePage:
                    name = "Path Patterns With Texture Page"sv;
                    break;
               case fme::PatternSelector::PathPatternsWithPupuID:
                    name = "Path Patterns With PupuID"sv;
                    break;
               case fme::PatternSelector::PathPatternsWithFullFileName:
                    name = "Path Patterns With Full Filename (.toml)"sv;
                    break;
               case fme::PatternSelector::End:
                    name = "End";
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};


template<>
struct fmt::formatter<fme::input_types> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(
       fme::input_types in_input_type,
       FormatContext   &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (in_input_type)
          {
               case fme::input_types::mim:
                    name = fme::gui_labels::mim;
                    break;
               case fme::input_types::deswizzle:
                    name = fme::gui_labels::deswizzle;
                    break;
               case fme::input_types::swizzle:
                    name = fme::gui_labels::swizzle;
                    break;
               case fme::input_types::swizzle_as_one_image:
                    name = fme::gui_labels::swizzle_as_one_image;
                    break;
               // case fme::input_types::deswizzle_generate_toml:
               //      name = fme::gui_labels::deswizzle_generate_toml;
               //      break;
               case fme::input_types::deswizzle_full_filename:
                    name = fme::gui_labels::deswizzle_full_filename_input;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};

template<>
struct fmt::formatter<fme::input_map_types> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(
       fme::input_map_types in_input_type,
       FormatContext       &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (in_input_type)
          {
               case fme::input_map_types::native:
                    name = fme::gui_labels::input_map_short_strings
                      [std::to_underlying(fme::input_map_types::native)];
                    break;
               case fme::input_map_types::loaded_same_input_path:
                    name = fme::gui_labels::input_map_short_strings
                      [std::to_underlying(
                        fme::input_map_types::loaded_same_input_path)];
                    break;
               case fme::input_map_types::loaded_different_input_path:
                    name = fme::gui_labels::input_map_short_strings
                      [std::to_underlying(
                        fme::input_map_types::loaded_different_input_path)];
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};

template<>
struct fmt::formatter<fme::output_types> : fmt::formatter<std::string_view>
{// parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(
       fme::output_types in_output_type,
       FormatContext    &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (in_output_type)
          {
               case fme::output_types::deswizzle:
                    name = fme::gui_labels::deswizzle;
                    break;
               case fme::output_types::swizzle:
                    name = fme::gui_labels::swizzle;
                    break;
               case fme::output_types::swizzle_as_one_image:
                    name = fme::gui_labels::swizzle_as_one_image;
                    break;
               case fme::output_types::deswizzle_generate_toml:
                    name = fme::gui_labels::deswizzle_generate_toml;
                    break;
               case fme::output_types::deswizzle_full_filename:
                    name = fme::gui_labels::deswizzle_full_filename;
                    break;
               case fme::output_types::csv:
                    name = ".csv";
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};
#endif// FIELD_MAP_EDITOR_FORMATTERS_HPP
