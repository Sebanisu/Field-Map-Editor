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

namespace ff_8
{
struct TileFunctionsUpscaleDeswizzle
{
  using X                            = tile_operations::X;
  using Y                            = tile_operations::Y;
  using TexturePage                  = tile_operations::TextureIdDefaultValue;
  using UseTexturePage               = std::true_type;
  using UseBlending                  = std::true_type;
  static constexpr const char *label = "Map (Upscale Deswizzle)";
};
using MapUpscaleDeswizzle = Map<TileFunctionsUpscaleDeswizzle>;
static_assert(glengine::Renderable<MapUpscaleDeswizzle>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPUPSCALEDESWIZZLE_HPP
