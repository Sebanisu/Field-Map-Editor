//
// Created by pcvii on 1/3/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPSWIZZLE_HPP
#define FIELD_MAP_EDITOR_MAPSWIZZLE_HPP
#include "Map.hpp"
#include <ff_8/TileOperations.hpp>
namespace ff_8
{
struct TileFunctionsSwizzle
{
     using X                                  = TileOperations::SourceX;
     using Y                                  = TileOperations::SourceY;
     using TexturePage                        = TileOperations::TextureId;
     using UseTexturePage                     = std::false_type;
     using UseBlending                        = std::false_type;
     static constexpr const char *const label = "Map (Swizzle)";
};
using MapSwizzle = Map<TileFunctionsSwizzle>;
static_assert(glengine::Renderable<MapSwizzle>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPSWIZZLE_HPP
