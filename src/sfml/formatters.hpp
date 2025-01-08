//
// Created by pcvii on 11/15/2021.
//

#ifndef FIELD_MAP_EDITOR_FORMATTERS_HPP
#define FIELD_MAP_EDITOR_FORMATTERS_HPP
#include "draw_bit_t.hpp"
#include "gui/draw_mode.hpp"
#include "tile_sizes.hpp"
#include <filesystem>
#include <fmt/format.h>
#include <open_viii/graphics/background/BlendModeT.hpp>
#include <open_viii/graphics/BPPT.hpp>
#include <open_viii/strings/LangCommon.hpp>
#include "tile_sizes.hpp"


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
     static constexpr auto to_hex(const tileT &tile)
     {
          constexpr auto to_hex_operation = [](const std::uint8_t input_byte, const auto operation) -> char {
               constexpr std::uint8_t number_of_values_in_nibble = 16U;
               constexpr char         threshold_of_A_to_F        = 10;
               char const             half_transformed_char      = static_cast<char>(operation(input_byte, number_of_values_in_nibble));
               return static_cast<char>(
                 (half_transformed_char < threshold_of_A_to_F ? half_transformed_char + '0'
                                                              : half_transformed_char - threshold_of_A_to_F + 'A'));
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
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(const tileT &tile, FormatContext &ctx) const
     {
          const auto array = to_hex(tile);
          return fmt::format_to(
            ctx.out(),
            "Hex: {}\n"
            "Source: {}\n"
            "Output: {}\n"
            "Z: {}\n"
            "Depth: {}\n"
            "Palette ID: {}\n"
            "Texture ID: {}\n"
            "Layer ID: {}\n"
            "Blend Mode: {}\n"
            "Blend Other: {}\n"
            "Animation ID: {}\n"
            "Animation State: {}\n"
            "Draw: {}",
            std::string_view(array.data(), array.size() - 1),
            tile.source_rectangle(),
            tile.output_rectangle(),
            tile.z(),
            tile.depth(),
            tile.palette_id(),
            tile.texture_id(),
            tile.layer_id(),
            tile.blend_mode(),
            tile.blend(),
            tile.animation_id(),
            tile.animation_state(),
            tile.draw());
     }
};
#endif// FIELD_MAP_EDITOR_FORMATTERS_HPP
