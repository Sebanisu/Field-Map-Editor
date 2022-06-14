//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_MAPUPSCALEDESWIZZLE_HPP
#define FIELD_MAP_EDITOR_MAPUPSCALEDESWIZZLE_HPP
#include "Fields.hpp"
#include "FrameBuffer.hpp"
#include "Map.hpp"
#include "Mim.hpp"
#include "Texture.hpp"
#include "tile_operations.hpp"

namespace ff8
{
struct TileFunctionsUpscaleDeswizzle
{
  using x                = tile_operations::x;
  using y                = tile_operations::y;
  using texture_page     = tile_operations::texture_id_default_value;
  using use_texture_page = std::true_type;
  using use_blending     = std::true_type;
  static constexpr const char *Label = "Map (Upscale Deswizzle)";
};
using MapUpscaleDeswizzle = Map<TileFunctionsUpscaleDeswizzle>;
static_assert(glengine::Renderable<MapUpscaleDeswizzle>);
}// namespace ff8
#endif// FIELD_MAP_EDITOR_MAPDESWIZZLE_HPP
