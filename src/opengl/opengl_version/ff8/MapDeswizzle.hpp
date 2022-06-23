//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_MAPDESWIZZLE_HPP
#define FIELD_MAP_EDITOR_MAPDESWIZZLE_HPP
#include "Fields.hpp"
#include "FrameBuffer.hpp"
#include "Map.hpp"
#include "Mim.hpp"
#include "Texture.hpp"
#include "tile_operations.hpp"

namespace ff_8
{
struct TileFunctionsDeswizzle
{
  using X                            = tile_operations::X;
  using Y                            = tile_operations::Y;
  using TexturePage                  = tile_operations::TextureIdDefaultValue;
  using UseTexturePage               = std::true_type;
  using UseBlending                  = std::true_type;
  static constexpr const char *label = "Map (Deswizzle)";
};
using MapDeswizzle = Map<TileFunctionsDeswizzle>;
static_assert(glengine::Renderable<MapDeswizzle>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPDESWIZZLE_HPP
