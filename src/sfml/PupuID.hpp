//
// Created by pcvii on 10/19/2021.
//

#ifndef FIELD_MAP_EDITOR_PUPUID_HPP
#define FIELD_MAP_EDITOR_PUPUID_HPP
#include "open_viii/graphics/background/Map.hpp"
#include <compare>
#include <fmt/format.h>
struct PupuID
{
     constexpr PupuID() = default;
     constexpr explicit PupuID(std::uint32_t raw)
       : m_raw(raw)
     {
     }
     // layer is 7 bits
     static constexpr std::uint32_t layer_offset           = 24U;
     static constexpr std::uint32_t layer_mask             = 0x7FU;
     // blend is only 0,1,2,3,4. So 3 bits
     static constexpr std::uint32_t blend_offset           = 20U;
     static constexpr std::uint32_t blend_mask             = 0x07U;
     // animation id and state is 8 bits
     static constexpr std::uint32_t animation_offset       = 12U;
     static constexpr std::uint32_t animation_state_offset = 4U;
     static constexpr std::uint32_t offset_mask            = 0xFU;
     static constexpr std::uint32_t x_not_aligned_to_grid  = 0x8000'0000;
     static constexpr std::uint32_t y_not_aligned_to_grid  = 0x0080'0000;
     static constexpr std::int8_t   tile_grid_size         = 16;
     // You are using all 32 bits without overlap.
     // The layer field uses 7 bits, which are stored in bits 24-30 of the m_raw value.
     // The blend field uses only 3 bits, which are stored in bits 20-22 of the m_raw value.
     // The animation id and animation state fields are 8 bits each, which are stored in bits 12-19 and 4-11 respectively.
     // The offset field uses 4 bits, which are stored in bits 0-3 of the m_raw value.
     // Finally, you use two more bits (31 and 23) for the x_not_aligned_to_grid and y_not_aligned_to_grid flags, respectively.
     // Therefore, all 32 bits are used without any overlap or waste of space.
     constexpr PupuID(const open_viii::graphics::background::is_tile auto &tile, std::uint8_t offset = 0U)
       : PupuID(std::uint32_t{ ((static_cast<std::uint32_t>(tile.layer_id()) & layer_mask) << layer_offset)
                               | ((static_cast<std::uint32_t>(tile.blend_mode()) & blend_mask) << blend_offset)
                               | static_cast<std::uint32_t>(tile.animation_id() << animation_offset)
                               | static_cast<std::uint32_t>(tile.animation_state() << animation_state_offset) | (offset & offset_mask) })
     {
          if (tile.x() % tile_grid_size != 0)
          {
               m_raw |= x_not_aligned_to_grid;
          }
          if (tile.y() % tile_grid_size != 0)
          {
               m_raw |= y_not_aligned_to_grid;
          }
     }
     [[nodiscard]] constexpr std::uint8_t layer_id() const
     {
          return static_cast<std::uint8_t>((m_raw >> PupuID::layer_offset) & PupuID::layer_mask);
     }

     [[nodiscard]] constexpr std::uint8_t blend_mode() const
     {
          return static_cast<std::uint8_t>((m_raw >> PupuID::blend_offset) & PupuID::blend_mask);
     }

     [[nodiscard]] constexpr std::uint8_t animation_id() const
     {
          return static_cast<std::uint8_t>((m_raw >> PupuID::animation_offset) & std::numeric_limits<std::uint8_t>::max());
     }

     [[nodiscard]] constexpr std::uint8_t animation_state() const
     {
          return static_cast<std::uint8_t>((m_raw >> PupuID::animation_state_offset) & std::numeric_limits<std::uint8_t>::max());
     }

     [[nodiscard]] constexpr std::uint8_t offset() const
     {
          return static_cast<std::uint8_t>(m_raw & PupuID::offset_mask);
     }

     [[nodiscard]] constexpr bool is_x_not_aligned_to_grid() const
     {
          return (m_raw & PupuID::x_not_aligned_to_grid) != 0;
     }

     [[nodiscard]] constexpr bool is_y_not_aligned_to_grid() const
     {
          return (m_raw & PupuID::y_not_aligned_to_grid) != 0;
     }
     [[nodiscard]] std::string create_summary() const
     {
          return fmt::format(
            "Layer ID: {}\nBlend Mode: {}\nAnimation ID: {}\nAnimation State: {}\nOffset: {}\nX not aligned: {}\nY not aligned: {}",
            layer_id(),
            blend_mode(),
            animation_id(),
            animation_state(),
            offset(),
            is_x_not_aligned_to_grid(),
            is_y_not_aligned_to_grid());
     }

     constexpr auto operator+=(std::uint32_t right)
     {
          m_raw += right;
          return *this;
     }
     [[nodiscard]] constexpr auto operator+(std::uint32_t right) const
     {
          auto cpy = *this;
          cpy += right;
          return cpy;
     }
     constexpr auto operator|=(std::uint32_t right)
     {
          m_raw |= right;
          return *this;
     }
     [[nodiscard]] constexpr auto operator|(std::uint32_t right) const
     {
          auto cpy = *this;
          cpy |= right;
          return cpy;
     }
     constexpr auto operator+=(PupuID right)
     {
          m_raw += right.raw();
          return *this;
     }
     [[nodiscard]] constexpr auto operator+(PupuID right) const
     {
          auto cpy = *this;
          cpy += right.raw();
          return cpy;
     }
     [[nodiscard]] constexpr std::uint32_t raw() const
     {
          return m_raw;
     }
     [[nodiscard]] bool constexpr same_base(PupuID right) const
     {
          constexpr auto mask = 0xFFFF'FFF0U;
          return (m_raw & mask) == (right.raw() & mask);
     }

     constexpr auto operator<=>(const PupuID &) const = default;

   private:
     std::uint32_t m_raw{};
};
// PupuID = uint32_t(0U + (tile.layer_id() <<
// 24U)+(static_cast<std::uint32_t>(tile.blend_mode()) << 20U) +
// (tile.animation_id() <<12U)+ (tile.animation_state()<<4U))

// std::transform(Sprites.cbegin(), Sprites.cend() - 1, Sprites.cbegin() + 1,
// Sprites.begin()+1, [](const Sprite& first, Sprite second) {
//     static constexpr auto mask = 0xFFFFFFF0U;
//     if ((first.ID & mask) == (second.ID & mask))
//     {
//       second.ID = first.ID + 1;
//     }
//     return second;
//   });

// IDs.reserve(Sprites.size());
// std::transform(Sprites.cbegin(), Sprites.cend(), std::back_inserter(IDs),
// [](const Sprite& sprite) {return sprite.ID; }); std::sort(IDs.begin(),
// IDs.end()); auto it = std::unique(IDs.begin(), IDs.end()); IDs.erase(it,
// IDs.end());
template<>
struct fmt::formatter<PupuID>
{
     // Presentation format: 'f' - fixed, 'e' - exponential.
     char           presentation = 'f';

     // Parses format specifications of the form ['f' | 'e'].
     constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin())
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
               throw format_error("invalid format");

          // Return an iterator past the end of the parsed range:
          return it;
     }

     // Formats the BPPT p using the parsed format specification (presentation)
     // stored in this formatter.
     template<typename FormatContext>
     auto format(const PupuID &p, FormatContext &ctx) -> decltype(ctx.out())
     {
          // ctx.out() is an output iterator to write to.
          return fmt::format_to(ctx.out(), "{:08X}", p.raw());
     }
};
#endif// FIELD_MAP_EDITOR_PUPUID_HPP
