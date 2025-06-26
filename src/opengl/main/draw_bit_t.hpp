//
// Created by pcvii on 1/1/2025.
//

#ifndef FIELD_MAP_EDITOR_DRAW_BIT_T_HPP
#define FIELD_MAP_EDITOR_DRAW_BIT_T_HPP
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
     return (left == draw_bitT::all) || (left == draw_bitT::disabled && !right) || (left == draw_bitT::enabled && right);
}
static inline constexpr bool operator==(bool left, draw_bitT right) noexcept
{
     return operator==(right, left);
}
};
#endif