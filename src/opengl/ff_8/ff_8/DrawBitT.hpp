//
// Created by pcvii on 1/1/2025.
//

#ifndef FIELD_MAP_EDITOR_DRAW_BIT_T_HPP
#define FIELD_MAP_EDITOR_DRAW_BIT_T_HPP
#include <fmt/format.h>
#include <string_view>
#include <utility>
namespace ff_8
{
enum struct DrawBitT
{
     all      = 0,
     disabled = 1,
     enabled  = 2,
};

static inline constexpr bool operator==(
  DrawBitT left,
  bool     right) noexcept
{
     return (std::to_underlying(left) == std::to_underlying(DrawBitT::all))
            || (std::to_underlying(left) == std::to_underlying(DrawBitT::disabled) && !right)
            || (std::to_underlying(left) == std::to_underlying(DrawBitT::enabled) && right);
}
static inline constexpr bool operator==(
  bool     left,
  DrawBitT right) noexcept
{
     return operator==(right, left);
}
static inline constexpr bool operator==(
  DrawBitT lhs,
  DrawBitT rhs) noexcept
{
     return std::to_underlying(lhs) == std::to_underlying(DrawBitT::all)
            || std::to_underlying(rhs) == std::to_underlying(DrawBitT::all)
            || std::to_underlying(lhs) == std::to_underlying(rhs);
}

};// namespace ff_8

template<>
struct fmt::formatter<ff_8::DrawBitT> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(
       ff_8::DrawBitT draw_bit_t,
       FormatContext &ctx) const
     {
          using namespace open_viii::graphics::background;
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (draw_bit_t)
          {
               case ff_8::DrawBitT::all:
                    name = "all"sv;
                    break;
               case ff_8::DrawBitT::disabled:
                    name = "disabled"sv;
                    break;
               case ff_8::DrawBitT::enabled:
                    name = "enabled"sv;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};
#endif