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
  using MapT = open_viii::graphics::background::Map;
  static auto filtered(const auto &tiles)
  {
    return tiles | std::views::filter(MapT::filter_invalid());
  };
  static auto visit(const MapT &map, auto &&transform)
  {
    return map.visit_tiles([&](const auto &tiles) {
      return TransformedSortedUniqueCopy(
        filtered(tiles), std::forward<decltype(transform)>(transform));
    });
  }
  static auto gen_z(const MapT &map)
  {
    return visit(map, [](const auto &tile) { return tile.z(); });
  }
  static auto gen_layer_id(const MapT &map)
  {
    return visit(map, [](const auto &tile) { return tile.layer_id(); });
  }
  static auto gen_palette_id(const MapT &map)
  {
    return visit(map, [](const auto &tile) { return tile.palette_id(); });
  }
  static auto gen_texture_page_id(const MapT &map)
  {
    return visit(map, [](const auto &tile) { return tile.texture_id(); });
  }
  static auto gen_animation_id(const MapT &map)
  {
    return visit(map, [](const auto &tile) { return tile.animation_id(); });
  }
  static auto gen_animation_frame(const MapT &map)
  {
    return visit(map, [](const auto &tile) { return tile.animation_state(); });
  }
  static auto gen_blend_other(const MapT &map)
  {
    return visit(map, [](const auto &tile) { return tile.blend(); });
  }
  static auto gen_blend_mode(const MapT &map)
  {
    return UniqueValues<open_viii::graphics::background::BlendModeT>(
      visit(map, [](const auto &tile) { return tile.blend_mode(); }),
      blendmode_to_string);
  }
  static auto gen_bpp(const MapT &map)
  {
    return UniqueValues<open_viii::graphics::BPPT>(
      visit(map, [](const auto &tile) { return tile.depth(); }), bpp_to_string);
  }
  static void refresh(const MapT &map, auto &old, auto &&func)
  {
    std::remove_cvref_t<decltype(old)> new_value = func(map);
    new_value.update(old);
    old = new_value;
  }

public:
  static constexpr auto blendmode_to_string =
    [](open_viii::graphics::background::BlendModeT in_blend_mode)
    -> const std::string & {
    using namespace std::string_literals;
    if (in_blend_mode == open_viii::graphics::background::BlendModeT::none)
    {
      const static auto str = "None"s;
      return str;
    }
    if (in_blend_mode == open_viii::graphics::background::BlendModeT::add)
    {
      const static auto str = "Add"s;
      return str;
    }
    if (
      in_blend_mode == open_viii::graphics::background::BlendModeT::quarter_add)
    {
      const static auto str = "Quarter Add"s;
      return str;
    }
    if (in_blend_mode == open_viii::graphics::background::BlendModeT::half_add)
    {
      const static auto str = "Half Add"s;
      return str;
    }
    if (in_blend_mode == open_viii::graphics::background::BlendModeT::subtract)
    {
      const static auto str = "Subtract"s;
      return str;
    }
    throw std::invalid_argument("Invalid blend mode!");
  };
  static constexpr auto bpp_to_string =
    [](open_viii::graphics::BPPT in_bpp) -> const std::string & {
    using namespace std::string_literals;
    if (in_bpp.bpp4())
    {
      const static auto str = "4"s;
      return str;
    }
    if (in_bpp.bpp8())
    {
      const static auto str = "8"s;
      return str;
    }
    if (in_bpp.bpp16())
    {
      const static auto str = "16"s;
      return str;
    }
    if (in_bpp.bpp24())
    {
      const static auto str = "24"s;
      return str;
    }
    throw std::invalid_argument("Must be 4, 8, 16, or 24 bpp");
  };
  UniqueTileValues() = default;
  UniqueTileValues(const MapT &map)
    : z(gen_z(map))
    , layer_id(gen_layer_id(map))
    , palette_id(gen_palette_id(map))
    , texture_page_id(gen_texture_page_id(map))
    , animation_id(gen_animation_id(map))
    , animation_frame(gen_animation_frame(map))
    , blend_other(gen_blend_other(map))
    , blend_mode(gen_blend_mode(map))
    , bpp(gen_bpp(map))
  {
  }
  void refresh_z(const MapT &map)
  {
    refresh(map, z, &gen_z);
  }
  void refresh_layer_id(const MapT &map)
  {
    refresh(map, layer_id, &gen_layer_id);
  }
  void refresh_palette_id(const MapT &map)
  {
    refresh(map, palette_id, &gen_palette_id);
  }
  void refresh_texture_page_id(const MapT &map)
  {
    refresh(map, texture_page_id, &gen_texture_page_id);
  }
  void refresh_animation_id(const MapT &map)
  {
    refresh(map, animation_id, &gen_animation_id);
  }
  void refresh_animation_frame(const MapT &map)
  {
    refresh(map, animation_frame, &gen_animation_frame);
  }
  void refresh_blend_other(const MapT &map)
  {
    refresh(map, blend_other, &gen_blend_other);
  }
  void refresh_blend_mode(const MapT &map)
  {
    refresh(map, blend_mode, &gen_blend_mode);
  }
  void refresh_bpp(const MapT &map)
  {
    refresh(map, bpp, &gen_bpp);
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
    UniqueTileValues::bpp_to_string
  };
  UniqueValues<open_viii::graphics::background::BlendModeT> blend_mode = {
    std::array{ open_viii::graphics::background::BlendModeT::none,
                open_viii::graphics::background::BlendModeT::add,
                open_viii::graphics::background::BlendModeT::half_add,
                open_viii::graphics::background::BlendModeT::quarter_add,
                open_viii::graphics::background::BlendModeT::subtract },
    UniqueTileValues::blendmode_to_string
  };
  UniqueValues<std::uint8_t> palette_id = {
    std::views::iota(std::uint8_t{ 0 }, std::uint8_t{ 16 })
  };
};
#endif// FIELD_MAP_EDITOR_UNIQUETILEVALUES_HPP
