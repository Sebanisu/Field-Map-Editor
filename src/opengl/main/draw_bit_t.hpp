//
// Created by pcvii on 1/1/2025.
//

#ifndef FIELD_MAP_EDITOR_DRAW_BIT_T_HPP
#define FIELD_MAP_EDITOR_DRAW_BIT_T_HPP
#include <utility>
namespace ff_8
{
enum struct draw_bitT
{
     all      = 0,
     disabled = 1,
     enabled  = 2,
};

static inline constexpr bool operator==(draw_bitT left, bool right) noexcept
{
     return (std::to_underlying(left) == std::to_underlying(draw_bitT::all))
            || (std::to_underlying(left) == std::to_underlying(draw_bitT::disabled) && !right)
            || (std::to_underlying(left) == std::to_underlying(draw_bitT::enabled) && right);
}
static inline constexpr bool operator==(bool left, draw_bitT right) noexcept
{
     return operator==(right, left);
}
static inline constexpr bool operator==(draw_bitT lhs, draw_bitT rhs) noexcept
{
     return std::to_underlying(lhs) == std::to_underlying(draw_bitT::all) || std::to_underlying(rhs) == std::to_underlying(draw_bitT::all)
            || std::to_underlying(lhs) == std::to_underlying(rhs);
}

};// namespace ff_8
#endif