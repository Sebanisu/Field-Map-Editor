//
// Created by pcvii on 10/19/2021.
//

#ifndef FIELD_MAP_EDITOR_PUPUID_HPP
#define FIELD_MAP_EDITOR_PUPUID_HPP
#include "open_viii/graphics/background/BlendModeT.hpp"
#include <compare>
#include <fmt/format.h>
struct PupuID
{
  constexpr PupuID() = default;
  constexpr explicit PupuID(std::uint32_t raw)
    : m_raw(raw)
  {
  }
  static constexpr std::uint32_t layer_offset           = 24U;
  static constexpr std::uint32_t blend_offset           = 20U;
  static constexpr std::uint32_t animation_offset       = 12U;
  static constexpr std::uint32_t animation_state_offset = 4U;
  constexpr PupuID(
    std::uint16_t                               layer_id,
    open_viii::graphics::background::BlendModeT blend_mode,
    std::uint8_t                                animation_id,
    std::uint8_t                                animation_state,
    std::uint8_t                                offset = 0U)
    : PupuID(std::uint32_t{
      static_cast<std::uint32_t>(layer_id << layer_offset)
      + (static_cast<std::uint32_t>(blend_mode) << blend_offset)
      + static_cast<std::uint32_t>(animation_id << animation_offset)
      + static_cast<std::uint32_t>(animation_state << animation_state_offset)
      + offset })
  {
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
    constexpr auto mask = 0xFFFFFFF0U;
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
    return format_to(ctx.out(), "{:08X}", p.raw());
  }
};
#endif// FIELD_MAP_EDITOR_PUPUID_HPP
