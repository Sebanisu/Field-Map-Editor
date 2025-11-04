//
// Created by pcvii on 6/30/2022.
//

#ifndef FIELD_MAP_EDITOR_SIMILARADJUSTMENTS_HPP
#define FIELD_MAP_EDITOR_SIMILARADJUSTMENTS_HPP
#include <ff_8/TileOperations.hpp>
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

          static constexpr auto xy_f = TileOperations::XY{};
          static constexpr auto z_f  = TileOperations::Z{};
          return [=, this](const TileT &other_tile) -> bool
          {
               const auto compare = [&](bool toggle, auto &&op) -> bool
               { return !toggle || op(tile) == op(other_tile); };
               return std::ranges::all_of(
                 std::array{
                   compare(x, TileOperations::X{}),
                   compare(y, TileOperations::Y{}), compare(xy, xy_f),
                   compare(z, z_f),
                   compare(
                     xyz, [](const TileT &t)
                     { return std::make_pair(xy_f(t), z_f(t)); }),
                   compare(source_x, TileOperations::SourceX{}),
                   compare(source_y, TileOperations::SourceY{}),
                   compare(source_xy, TileOperations::SourceXY{}),
                   compare(texture_id, TileOperations::TextureId{}),
                   compare(blend_mode, TileOperations::BlendMode{}),
                   compare(blend, TileOperations::Blend{}),
                   compare(draw, TileOperations::Draw{}),
                   compare(depth, TileOperations::Depth{}),
                   compare(layer_id, TileOperations::LayerId{}),
                   compare(palette_id, TileOperations::PaletteId{}),
                   compare(animation_id, TileOperations::AnimationId{}),
                   compare(animation_state, TileOperations::AnimationState{}) },
                 std::identity{});
          };
     }
     operator bool() const
     {
          return std::ranges::any_of(
            std::bit_cast<std::array<bool, sizeof(SimilarAdjustments)>>(*this),
            std::identity{});
     }
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_SIMILARADJUSTMENTS_HPP
