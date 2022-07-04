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
  bool source_xy = { false };
  template<typename TileT>
  auto operator()(const TileT &tile) const
  {
    return [=, this](const TileT &other_tile) -> bool {
      const auto compare = [&](bool toggle, auto &&op) -> bool {
        return !toggle || op(tile) == op(other_tile);
      };
      using namespace tile_operations;
      return std::ranges::all_of(
        std::array{ compare(source_xy, SourceXY{}) }, std::identity{});
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
