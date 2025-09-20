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
     // blend is only 0,1,2,3,4. So 3 bits
     static constexpr std::uint32_t blend_offset           = 20U;
     // animation id and state is 8 bits
     static constexpr std::uint32_t animation_offset       = 12U;
     static constexpr std::uint32_t animation_state_offset = 4U;
     // leaves 3 bits for offset markers and 3 bits for offset increment
     constexpr PupuID(
       const open_viii::graphics::background::is_tile auto &tile,
       std::uint8_t                                         offset = 0U)
       : PupuID(
           std::uint32_t{
             ((static_cast<std::uint32_t>(tile.layer_id()) & 0x7FU)
              << layer_offset)
             | ((static_cast<std::uint32_t>(tile.blend_mode()) & 0x07U) << blend_offset)
             | static_cast<std::uint32_t>(
               tile.animation_id() << animation_offset)
             | static_cast<std::uint32_t>(
               tile.animation_state() << animation_state_offset)
             | static_cast<std::uint32_t>(offset & 0xF) })
     {
          if (tile.x() % 16 != 0)
          {
               m_raw |= 0x8000'0000U;
          }
          if (tile.y() % 16 != 0)
          {
               m_raw |= 0x0080'0000U;
          }
     }
     constexpr auto &operator+=(std::uint32_t right)
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
     constexpr auto &operator|=(std::uint32_t right)
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
     constexpr auto &operator+=(PupuID right)
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
struct fmt::formatter<PupuID> : fmt::formatter<std::string>
{
     // Formats value using the parsed format specification stored in this
     // formatter and writes the output to ctx.out().
     auto format(
       const PupuID   &value,
       format_context &ctx) const -> format_context::iterator
     {
          return fmt::format_to(ctx.out(), "{:08X}", value.raw());
     }
};
#endif// FIELD_MAP_EDITOR_PUPUID_HPP
