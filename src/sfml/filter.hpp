//
// Created by pcvii on 10/4/2021.
//

#ifndef FIELD_MAP_EDITOR_FILTER_HPP
#define FIELD_MAP_EDITOR_FILTER_HPP
#include "open_viii/graphics/background/BlendModeT.hpp"
#include "open_viii/graphics/BPPT.hpp"
#include "PupuID.hpp"
#include "tile_operations.hpp"
#include <cstdint>
#include <filesystem>
#include <fmt/format.h>
#include <map>
#include <string>
#include <utility>
#include <vector>
namespace ff_8
{
template<typename T>
struct filter_old
{
   private:
     T    m_value   = {};
     bool m_enabled = { false };

   public:
     filter_old() = default;
     explicit filter_old(T value, bool enabled = false)
       : m_value(std::move(value))
       , m_enabled(enabled)
     {
     }
     template<typename U>
     filter_old &update(U &&value)
     {
          m_value = std::forward<U>(value);
          return *this;
     }
     [[nodiscard]] const T &value() const
     {
          return m_value;
     }
     [[nodiscard]] const bool &enabled() const
     {
          return m_enabled;
     }
     filter_old &enable()
     {
          m_enabled = true;
          return *this;
     }
     filter_old &disable()
     {
          m_enabled = false;
          return *this;
     }
     [[nodiscard]] bool operator==(const T &cmp) const
     {
          return m_value == cmp;
     }
     [[nodiscard]] bool operator!=(const T &cmp) const
     {
          return m_value != cmp;
     }
     [[nodiscard]] explicit operator bool() const
     {
          return m_enabled;
     }
     explicit operator T() const
     {
          return m_value;
     }
};
template<typename T, typename OpT>
struct filter
{
   private:
     T    m_value     = {};
     bool m_enabled   = { false };
     OpT  m_operation = {};

   public:
     filter() = default;
     explicit filter(T value, bool enabled = false)
       : m_value(std::move(value))
       , m_enabled(enabled)
     {
     }
     template<typename U>
     filter &update(U &&value)
     {
          m_value = std::forward<U>(value);
          return *this;
     }
     [[nodiscard]] const T &value() const
     {
          return m_value;
     }
     [[nodiscard]] const bool &enabled() const
     {
          return m_enabled;
     }
     filter &enable()
     {
          m_enabled = true;
          return *this;
     }
     filter &disable()
     {
          m_enabled = false;
          return *this;
     }
     [[nodiscard]] bool operator==(const T &cmp) const
     {
          return m_value == cmp;
     }
     [[nodiscard]] bool operator!=(const T &cmp) const
     {
          return m_value != cmp;
     }
     [[nodiscard]] explicit operator bool() const
     {
          return m_enabled;
     }
     explicit operator T() const
     {
          return m_value;
     }
     template<open_viii::graphics::background::is_tile TileT>
     bool operator()(const TileT &tile) const
     {
          return !m_enabled || (m_value == std::invoke(m_operation, tile));
     }
};
enum struct draw_bitT
{
     all      = 0,
     disabled = 1,
     enabled  = 2,
};

static inline constexpr bool operator==(draw_bitT left, bool right) noexcept
{
     return (left == draw_bitT::all) || (left == draw_bitT::disabled && !right) || (left == draw_bitT::enabled && right);
}
static inline constexpr bool operator==(bool left, draw_bitT right) noexcept
{
     return operator==(right, left);
}
struct filters
{
     using TileT = open_viii::graphics::background::Tile1;
     filter_old<PupuID>                                                                           pupu            = {};
     filter_old<std::filesystem::path>                                                            upscale         = {};
     filter_old<std::filesystem::path>                                                            deswizzle       = {};
     filter<draw_bitT, ff_8::tile_operations::Draw>                                               draw_bit        = {};
     filter<ff_8::tile_operations::ZT<TileT>, ff_8::tile_operations::Z>                           z               = {};
     filter<ff_8::tile_operations::PaletteIdT<TileT>, ff_8::tile_operations::PaletteId>           palette         = {};
     filter<ff_8::tile_operations::AnimationIdT<TileT>, ff_8::tile_operations::AnimationId>       animation_id    = {};
     filter<ff_8::tile_operations::AnimationStateT<TileT>, ff_8::tile_operations::AnimationState> animation_frame = {};
     filter<ff_8::tile_operations::LayerIdT<TileT>, ff_8::tile_operations::LayerId>               layer_id        = {};

     filter<ff_8::tile_operations::TextureIdT<TileT>, ff_8::tile_operations::TextureId>           texture_page_id = {};
     filter<ff_8::tile_operations::BlendModeT<TileT>, ff_8::tile_operations::BlendMode>           blend_mode      = {};
     filter<ff_8::tile_operations::BlendT<TileT>, ff_8::tile_operations::Blend>                   blend_other     = {};
     filter<ff_8::tile_operations::DepthT<TileT>, ff_8::tile_operations::Depth>                   bpp =
       filter<ff_8::tile_operations::DepthT<TileT>, ff_8::tile_operations::Depth>{ open_viii::graphics::BPPT::BPP4_CONST() };
     template<open_viii::graphics::background::is_tile ThisTileT>
     bool operator()(const ThisTileT &tile) const
     {
          return [&tile](const auto &...operations) -> bool {
               return (std::invoke(operations, tile) && ...);
          }(draw_bit, z, palette, animation_id, animation_frame, layer_id, texture_page_id, blend_mode, blend_other, bpp);
     }
};
namespace tile_operations
{
     template<open_viii::graphics::background::is_tile tileT>
     bool fail_any_filters(const ff_8::filters &filters, const tileT &tile)
     {
          return !std::invoke(filters, tile);
     }
}// namespace tile_operations
}// namespace ff_8
template<>
struct fmt::formatter<ff_8::draw_bitT>
{
     // Presentation format: 'f' - fixed, 'e' - exponential.
     char           presentation = 'f';

     // Parses format specifications of the form ['f' | 'e'].
     constexpr auto parse(fmt::format_parse_context &ctx) -> decltype(ctx.begin())
     {
          // [ctx.begin(), ctx.end()) is a character range that contains a part of
          // the format string starting from the format specifications to be parsed,
          // e.g. in
          //
          //   fmt::format("{:f} - BPPT of interest", BPPT{1, 2});
          //
          // the range will contain "f} - BPPT of interest". The formatter should
          // parse specifiers until '}' or the end of the range. In this example
          // the formatter should parse the 'f' specifier and return an iterator
          // BPPTing to '}'.

          // Parse the presentation format and store it in the formatter:
          auto it  = ctx.begin();
          auto end = ctx.end();
          if (it != end && (*it == 'f' || *it == 'e'))
               presentation = *it++;

          // Check if reached the end of the range:
          if (it != end && *it != '}')
               throw fmt::format_error("invalid format");

          // Return an iterator past the end of the parsed range:
          return it;
     }

     // Formats the BPPT p using the parsed format specification (presentation)
     // stored in this formatter.
     template<typename FormatContext>
     auto format(const ff_8::draw_bitT &p, FormatContext &ctx) -> decltype(ctx.out())
     {
          // ctx.out() is an output iterator to write to.
          return fmt::format_to(ctx.out(), "{:08X}", static_cast<int>(p));
     }
};
#endif// FIELD_MAP_EDITOR_FILTER_HPP
