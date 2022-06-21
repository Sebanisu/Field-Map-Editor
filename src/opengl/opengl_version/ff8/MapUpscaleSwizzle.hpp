//
// Created by pcvii on 1/3/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPUPSCALESWIZZLE_HPP
#define FIELD_MAP_EDITOR_MAPUPSCALESWIZZLE_HPP
#include "Fields.hpp"
#include "FrameBuffer.hpp"
#include "Map.hpp"
#include "Mim.hpp"
#include "Texture.hpp"
#include "tile_operations.hpp"
namespace ff_8
{
struct TileFunctionsUpscaleSwizzle
{
  using x                            = tile_operations::source_x;
  using y                            = tile_operations::source_y;
  using texture_page                 = tile_operations::texture_id;
  using use_texture_page             = std::false_type;
  using use_blending                 = std::false_type;
  static constexpr const char *Label = "Map (Upscale Swizzle)";
};
using MapUpscaleSwizzle = Map<TileFunctionsUpscaleSwizzle>;
static_assert(glengine::Renderable<MapUpscaleSwizzle>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPUPSCALESWIZZLE_HPP
