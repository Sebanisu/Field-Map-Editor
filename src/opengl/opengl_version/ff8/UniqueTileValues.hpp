//
// Created by pcvii on 2/7/2022.
//

#ifndef FIELD_MAP_EDITOR_UNIQUETILEVALUES_HPP
#define FIELD_MAP_EDITOR_UNIQUETILEVALUES_HPP
#include "TransformedSortedUniqueCopy.hpp"
#include "UniqueValues.hpp"
#include <open_viii/graphics/background/Map.hpp>
struct UniqueTileValues
{
private:
  auto filtered(const auto &tiles)
  {
    return tiles
           | std::views::filter(
             open_viii::graphics::background::Map::filter_invalid());
  };

  auto visit(const open_viii::graphics::background::Map &map, auto &&transform)
  {
    return map.visit_tiles([&](const auto &tiles) {
      return TransformedSortedUniqueCopy(
        filtered(tiles), std::forward<decltype(transform)>(transform));
    });
  }

public:
  static constexpr auto BLENDMODE_TO_STRING = [](open_viii::graphics::
                                                   background::BlendModeT
                                                     in_blend_mode) {
    using namespace std::string_literals;
    if (in_blend_mode == open_viii::graphics::background::BlendModeT::none)
      return "None"s;
    if (in_blend_mode == open_viii::graphics::background::BlendModeT::add)
      return "Add"s;
    if (
      in_blend_mode == open_viii::graphics::background::BlendModeT::quarter_add)
      return "Quarter Add"s;
    if (in_blend_mode == open_viii::graphics::background::BlendModeT::half_add)
      return "Half Add"s;
    if (in_blend_mode == open_viii::graphics::background::BlendModeT::subtract)
      return "Subtract"s;
    throw std::invalid_argument("Invalid blend mode!");
  };
  static constexpr auto BPP_TO_STRING = [](open_viii::graphics::BPPT in_bpp) {
    using namespace std::string_literals;
    if (in_bpp.bpp4())
      return "4"s;
    if (in_bpp.bpp8())
      return "8"s;
    if (in_bpp.bpp16())
      return "16"s;
    if (in_bpp.bpp24())
      return "24"s;
    throw std::invalid_argument("Must be 4, 8, 16, or 24 bpp");
  };
  UniqueTileValues() = default;
  UniqueTileValues(const open_viii::graphics::background::Map &map)
    : z(visit(map, [](const auto &tile) { return tile.z(); }))
    , layer_id(visit(map, [](const auto &tile) { return tile.layer_id(); }))
    , palette_id(visit(map, [](const auto &tile) { return tile.palette_id(); }))
    , texture_page_id(
        visit(map, [](const auto &tile) { return tile.texture_id(); }))
    , animation_id(
        visit(map, [](const auto &tile) { return tile.animation_id(); }))
    , animation_frame(
        visit(map, [](const auto &tile) { return tile.animation_state(); }))
    , blend_other(visit(map, [](const auto &tile) { return tile.blend(); }))
    , blend_mode(
        visit(map, [](const auto &tile) { return tile.blend_mode(); }),
        BLENDMODE_TO_STRING)
    , bpp(
        visit(map, [](const auto &tile) { return tile.depth(); }),
        BPP_TO_STRING)
  {
  }
  UniqueValues<std::uint16_t> z                                        = {};
  UniqueValues<std::uint8_t>  layer_id                                 = {};
  UniqueValues<std::uint8_t>  palette_id                               = {};
  UniqueValues<std::uint8_t>  texture_page_id                          = {};
  UniqueValues<std::uint8_t>  animation_id                             = {};
  UniqueValues<std::uint8_t>  animation_frame                          = {};
  UniqueValues<std::uint8_t>  blend_other                              = {};
  UniqueValues<open_viii::graphics::background::BlendModeT> blend_mode = {};
  UniqueValues<open_viii::graphics::BPPT>                   bpp        = {};
};
struct TilePossibleValues
{
  UniqueValues<open_viii::graphics::BPPT> bpp = {
    std::array{ open_viii::graphics::BPPT::BPP4_CONST(),
                open_viii::graphics::BPPT::BPP8_CONST(),
                open_viii::graphics::BPPT::BPP16_CONST(),
                open_viii::graphics::BPPT::BPP24_CONST() },
    UniqueTileValues::BPP_TO_STRING
  };
  UniqueValues<open_viii::graphics::background::BlendModeT> blend_mode = {
    std::array{ open_viii::graphics::background::BlendModeT::none,
                open_viii::graphics::background::BlendModeT::add,
                open_viii::graphics::background::BlendModeT::half_add,
                open_viii::graphics::background::BlendModeT::quarter_add,
                open_viii::graphics::background::BlendModeT::subtract },
    UniqueTileValues::BLENDMODE_TO_STRING
  };
  UniqueValues<std::uint8_t> palette_id = {
    std::views::iota(std::uint8_t{ 0 }, std::uint8_t{ 16 })
  };
};
#endif// FIELD_MAP_EDITOR_UNIQUETILEVALUES_HPP
