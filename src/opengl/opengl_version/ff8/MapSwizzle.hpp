//
// Created by pcvii on 1/3/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPSWIZZLE_HPP
#define FIELD_MAP_EDITOR_MAPSWIZZLE_HPP
#include "Fields.hpp"
#include "FrameBuffer.hpp"
#include "Map.hpp"
#include "Mim.hpp"
#include "Texture.hpp"
#include "tile_operations.hpp"
namespace ff_8
{
struct TileFunctionsSwizzle
{
  using X                                  = tile_operations::SourceX;
  using Y                                  = tile_operations::SourceY;
  using TexturePage                        = tile_operations::TextureId;
  using UseTexturePage                     = std::false_type;
  using UseBlending                        = std::false_type;
  static constexpr const char *const label = "Map (Swizzle)";
};
using MapSwizzle = Map<TileFunctionsSwizzle>;
static_assert(glengine::Renderable<MapSwizzle>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPSWIZZLE_HPP
