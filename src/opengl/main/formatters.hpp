//
// Created by pcvii on 11/15/2021.
//

#ifndef FIELD_MAP_EDITOR_FORMATTERS_HPP
#define FIELD_MAP_EDITOR_FORMATTERS_HPP
#include "draw_bit_t.hpp"
#include "gui/colors.hpp"
#include "gui/compact_type.hpp"
#include "gui/draw_mode.hpp"
#include "gui/gui_labels.hpp"
#include "normalized_source_tile.hpp"
#include "tile_sizes.hpp"
#include <filesystem>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <functional>
#include <open_viii/graphics/background/BlendModeT.hpp>
#include <open_viii/graphics/background/Map.hpp>
#include <open_viii/graphics/BPPT.hpp>
#include <open_viii/strings/LangCommon.hpp>

namespace ff_8
{
template<open_viii::graphics::background::is_tile tileT>
static constexpr auto to_hex(const tileT &tile)
{
     constexpr auto to_hex_operation = [](const std::uint8_t input_byte, const auto operation) -> char {
          constexpr std::uint8_t number_of_values_in_nibble = 16U;
          constexpr char         threshold_of_A_to_F        = 10;
          char const             half_transformed_char      = static_cast<char>(operation(input_byte, number_of_values_in_nibble));
          return static_cast<char>((
            half_transformed_char < threshold_of_A_to_F ? half_transformed_char + '0' : half_transformed_char - threshold_of_A_to_F + 'A'));
     };
     const auto                               raw_bytes = std::bit_cast<std::array<std::uint8_t, sizeof(tileT)>>(tile);
     std::array<char, sizeof(tileT) * 2U + 1> raw_hex{};
     raw_hex.back() = '\0';
     auto rhi       = raw_hex.begin();
     for (const std::uint8_t current_byte : raw_bytes)
     {
          *rhi = to_hex_operation(current_byte, std::divides{});
          std::advance(rhi, 1);
          *rhi = to_hex_operation(current_byte, std::modulus{});
          std::advance(rhi, 1);
     }
     return raw_hex;
}
}// namespace ff_8

template<>
struct fmt::formatter<tile_sizes> : fmt::formatter<std::string_view>
{
     // tile_sizes::default_size, tile_sizes::x_2_size, tile_sizes::x_4_size, tile_sizes::x_8_size, tile_sizes::x_16_size
     //  parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(tile_sizes tile_sizes_t, FormatContext &ctx) const
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
struct fmt::formatter<ff_8::draw_bitT> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(ff_8::draw_bitT draw_bit_t, FormatContext &ctx) const
     {
          using namespace open_viii::graphics::background;
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (draw_bit_t)
          {
               case ff_8::draw_bitT::all:
                    name = "all"sv;
                    break;
               case ff_8::draw_bitT::disabled:
                    name = "disabled"sv;
                    break;
               case ff_8::draw_bitT::enabled:
                    name = "enabled"sv;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};

template<>
struct fmt::formatter<open_viii::graphics::background::BlendModeT> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(open_viii::graphics::background::BlendModeT blend_mode_t, FormatContext &ctx) const
     {
          using namespace open_viii::graphics::background;
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (blend_mode_t)
          {
               case BlendModeT::add:
                    name = "add"sv;
                    break;
               case BlendModeT::half_add:
                    name = "half add"sv;
                    break;
               case BlendModeT::none:
                    name = "none"sv;
                    break;
               case BlendModeT::quarter_add:
                    name = "quarter add"sv;
                    break;
               case BlendModeT::subtract:
                    name = "subtract"sv;
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
     constexpr auto format(draw_mode draw_mode_t, FormatContext &ctx) const
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
struct fmt::formatter<open_viii::LangT> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(open_viii::LangT lang_t, FormatContext &ctx) const
     {
          using namespace open_viii::graphics::background;
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (lang_t)
          {
               case open_viii::LangT::en:
                    name = open_viii::LangCommon::ENFULL;
                    break;
               case open_viii::LangT::fr:
                    name = open_viii::LangCommon::FRFULL;
                    break;
               case open_viii::LangT::de:
                    name = open_viii::LangCommon::DEFULL;
                    break;
               case open_viii::LangT::it:
                    name = open_viii::LangCommon::ITFULL;
                    break;
               case open_viii::LangT::es:
                    name = open_viii::LangCommon::ESFULL;
                    break;
               case open_viii::LangT::jp:
                    name = open_viii::LangCommon::JPFULL;
                    break;
               case open_viii::LangT::end:
               case open_viii::LangT::generic:
                    name = "Generic"sv;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};

template<open_viii::Number numT>
struct fmt::formatter<open_viii::graphics::Point<numT>> : fmt::formatter<numT>
{
     // parse is inherited from formatter<std::underlying_type_t<tile_sizes>>.
     template<typename FormatContext>
     constexpr auto format(open_viii::graphics::Point<numT> point, FormatContext &ctx) const
     {
          fmt::format_to(ctx.out(), "{}", '(');
          fmt::formatter<numT>::format(point.x(), ctx);
          fmt::format_to(ctx.out(), "{}", ", ");
          fmt::formatter<numT>::format(point.y(), ctx);
          return fmt::format_to(ctx.out(), "{}", ')');
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
     auto format(fme::BackgroundSettings bs, FormatContext &ctx) const
     {
          using enum fme::BackgroundSettings;

          if (bs == Default)
          {
               return fmt::format_to(ctx.out(), "Default (OneColor | Checkerboard)");
          }

          bool       first      = true;

          const auto write_part = [&](std::string_view part) {
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

template<open_viii::Number numT>
struct fmt::formatter<open_viii::graphics::Rectangle<numT>> : fmt::formatter<open_viii::graphics::Point<numT>>
{
     // parse is inherited from formatter<std::underlying_type_t<tile_sizes>>.
     template<typename FormatContext>
     constexpr auto format(open_viii::graphics::Rectangle<numT> rectangle, FormatContext &ctx) const
     {
          fmt::formatter<open_viii::graphics::Point<numT>>::format(open_viii::graphics::Point<numT>{ rectangle.x(), rectangle.y() }, ctx);
          fmt::format_to(ctx.out(), "{}", ' ');
          return fmt::formatter<open_viii::graphics::Point<numT>>::format(
            open_viii::graphics::Point<numT>{ rectangle.width(), rectangle.height() }, ctx);
     }
};

template<>
struct fmt::formatter<open_viii::graphics::BPPT> : fmt::formatter<std::uint32_t>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(open_viii::graphics::BPPT bppt, FormatContext &ctx) const
     {
          using namespace open_viii::graphics;
          using namespace std::string_view_literals;

          if (bppt.bpp8())
          {
               return fmt::formatter<std::uint32_t>::format(BPPT::BPP8, ctx);
          }
          if (bppt.bpp16())
          {
               return fmt::formatter<std::uint32_t>::format(BPPT::BPP16, ctx);
          }
          if (bppt.bpp24())
          {
               return fmt::formatter<std::uint32_t>::format(BPPT::BPP24, ctx);
          }
          return fmt::formatter<std::uint32_t>::format(BPPT::BPP4, ctx);
     }
};


template<open_viii::graphics::background::is_tile tileT>
struct fmt::formatter<tileT> : fmt::formatter<std::string>
{
     // parse is inherited from formatter<string>.
     template<typename FormatContext>
     constexpr auto format(const tileT &tile, FormatContext &ctx) const
     {
          const auto array   = ff_8::to_hex(tile);
          const auto hexview = std::string_view(array.data(), array.size() - 1);
          return fmt::format_to(
            ctx.out(),
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}",
            fme::gui_labels::hex,
            hexview,
            fme::gui_labels::source,
            tile.source_rectangle(),
            fme::gui_labels::destination,
            tile.output_rectangle(),
            fme::gui_labels::z,
            tile.z(),
            fme::gui_labels::bpp,
            tile.depth(),
            fme::gui_labels::palette,
            tile.palette_id(),
            fme::gui_labels::texture_page,
            tile.texture_id(),
            fme::gui_labels::layer_id,
            tile.layer_id(),
            fme::gui_labels::blend_mode,
            tile.blend_mode(),
            fme::gui_labels::blend_other,
            tile.blend(),
            fme::gui_labels::animation_id,
            tile.animation_id(),
            fme::gui_labels::animation_state,
            tile.animation_state(),
            fme::gui_labels::draw,
            tile.draw());
     }
};

template<>
struct fmt::formatter<open_viii::graphics::background::normalized_source_tile> : fmt::formatter<std::string>
{
     // parse is inherited from formatter<string>.
     template<typename FormatContext>
     constexpr auto format(const open_viii::graphics::background::normalized_source_tile &tile, FormatContext &ctx) const
     {
          const auto array   = ff_8::to_hex(tile);
          const auto hexview = std::string_view(array.data(), array.size() - 1);
          return fmt::format_to(
            ctx.out(),
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}\n"
            "{}: {}",
            fme::gui_labels::hex,
            hexview,
            fme::gui_labels::source,
            tile.m_source_xy,
            fme::gui_labels::bpp,
            tile.m_tex_id_buffer.depth(),
            fme::gui_labels::palette,
            tile.m_palette_id.id(),
            fme::gui_labels::texture_page,
            tile.m_tex_id_buffer.id(),
            fme::gui_labels::layer_id,
            tile.m_layer_id.id(),
            fme::gui_labels::blend_mode,
            tile.m_blend_mode,
            fme::gui_labels::blend_other,
            tile.m_tex_id_buffer.blend(),
            fme::gui_labels::animation_id,
            tile.m_animation_id,
            fme::gui_labels::animation_state,
            tile.m_animation_state,
            fme::gui_labels::draw,
            tile.m_tex_id_buffer.draw);
     }
};

template<typename range_t>
concept tile_range = std::ranges::range<range_t> && open_viii::graphics::background::is_tile<std::ranges::range_value_t<range_t>>;

template<tile_range TileRange>
struct fmt::is_range<TileRange, char> : std::false_type
{
};


// Specialization for ranges of tiles
template<tile_range TileRange>
struct fmt::formatter<TileRange> : fmt::formatter<std::string>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(const TileRange &tiles, FormatContext &ctx) const
     {
          const auto count = std::ranges::distance(tiles);


          return fmt::format_to(ctx.out(), "Total Tiles: {}\n", count);
     }
};

template<>
struct fmt::formatter<fme::color>
{
     // Parses format specs; in this case, we don't support any custom formatting
     constexpr auto parse(format_parse_context &ctx)
     {
          return ctx.begin();// no custom formatting, so just return the end
     }

     // Formats the color as "(r,g,b,a)"
     template<typename FormatContext>
     auto format(const fme::color &c, FormatContext &ctx) const
     {
          return fmt::format_to(ctx.out(), "({},{},{},{})", c.r, c.g, c.b, c.a);
     }
};

template<>
struct fmt::formatter<fme::root_path_types> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(fme::root_path_types in_root_path_types, FormatContext &ctx) const
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
     constexpr auto format(fme::PatternSelector pattern_selector, FormatContext &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (pattern_selector)
          {
               case fme::PatternSelector::OutputSwizzlePattern:
                    name = "Output swizzle pattern"sv;
                    break;
               case fme::PatternSelector::OutputDeswizzlePattern:
                    name = "Output deswizzle pattern"sv;
                    break;
               case fme::PatternSelector::OutputMapPatternForSwizzle:
                    name = "Output map pattern for swizzle"sv;
                    break;
               case fme::PatternSelector::OutputMapPatternForDeswizzle:
                    name = "Output map pattern for deswizzle"sv;
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
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};


template<>
struct fmt::formatter<fme::compact_type> : fmt::formatter<std::string_view>
{// parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(fme::compact_type in_compact_type, FormatContext &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (in_compact_type)
          {
               case fme::compact_type::map_order_ffnx:
                    name = fme::gui_labels::compact_map_order_ffnx;
                    break;
               case fme::compact_type::map_order:
                    name = fme::gui_labels::map_order;
                    break;
               case fme::compact_type::move_only_conflicts:
                    name = fme::gui_labels::move_conflicts;
                    break;
               case fme::compact_type::rows:
                    name = fme::gui_labels::rows;
                    break;
               case fme::compact_type::all:
                    name = fme::gui_labels::all;
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
     constexpr auto format(fme::input_types in_input_type, FormatContext &ctx) const
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
               // case fme::input_types::deswizzle_combined_toml:
               //      name = fme::gui_labels::deswizzle_combined_toml;
               //      break;
               case fme::input_types::deswizzle_combined_images:
                    name = fme::gui_labels::deswizzle_combined_images;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};

template<>
struct fmt::formatter<fme::output_types> : fmt::formatter<std::string_view>
{// parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(fme::output_types in_output_type, FormatContext &ctx) const
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
               case fme::output_types::deswizzle_combined_toml:
                    name = fme::gui_labels::deswizzle_combined_toml;
                    break;
               case fme::output_types::deswizzle_combined_images:
                    name = fme::gui_labels::deswizzle_combined_images;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};

template<>
struct fmt::formatter<fme::flatten_type> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(fme::flatten_type in_flatten_type, FormatContext &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (in_flatten_type)
          {
               case fme::flatten_type::bpp:
                    name = fme::gui_labels::bpp;
                    break;
               case fme::flatten_type::palette:
                    name = fme::gui_labels::palette;
                    break;
               case fme::flatten_type::both:
                    name = fme::gui_labels::bpp_and_palette;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};
#endif// FIELD_MAP_EDITOR_FORMATTERS_HPP
