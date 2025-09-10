//
// Created by pcvii on 6/30/2022.
//

#ifndef FIELD_MAP_EDITOR_SIMILARADJUSTMENTS_HPP
#define FIELD_MAP_EDITOR_SIMILARADJUSTMENTS_HPP
#include "tile_operations.hpp"
namespace ff_8
{
struct SimilarAdjustments
{
     bool x               = { false };
     bool y               = { false };
     bool xy              = { false };
     bool z               = { false };
     bool xyz             = { false };
     bool source_x        = { false };
     bool source_y        = { false };
     bool source_xy       = { false };
     bool texture_id      = { false };
     bool blend_mode      = { false };
     bool blend           = { false };
     bool draw            = { false };
     bool depth           = { false };
     bool layer_id        = { false };
     bool palette_id      = { false };
     bool animation_id    = { false };
     bool animation_state = { false };
     template<typename TileT>
     auto operator()(const TileT &tile) const
     {
          using namespace tile_operations;
          static constexpr auto xy_f = XY{};
          static constexpr auto z_f  = Z{};
          return [=, this](const TileT &other_tile) -> bool {
               const auto compare = [&](bool toggle, auto &&op) -> bool { return !toggle || op(tile) == op(other_tile); };
               return std::ranges::all_of(
                 std::array{ compare(x, X{}),
                             compare(y, Y{}),
                             compare(xy, xy_f),
                             compare(z, z_f),
                             compare(xyz, [](const TileT &t) { return std::make_pair(xy_f(t), z_f(t)); }),
                             compare(source_x, SourceX{}),
                             compare(source_y, SourceY{}),
                             compare(source_xy, SourceXY{}),
                             compare(texture_id, TextureId{}),
                             compare(blend_mode, BlendMode{}),
                             compare(blend, Blend{}),
                             compare(draw, Draw{}),
                             compare(depth, Depth{}),
                             compare(layer_id, LayerId{}),
                             compare(palette_id, PaletteId{}),
                             compare(animation_id, AnimationId{}),
                             compare(animation_state, AnimationState{}) },
                 std::identity{});
          };
     }
     operator bool() const
     {
          return std::ranges::any_of(std::bit_cast<std::array<bool, sizeof(SimilarAdjustments)>>(*this), std::identity{});
     }
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_SIMILARADJUSTMENTS_HPP
