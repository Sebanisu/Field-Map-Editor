//
// Created by pcvii on 1/3/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPUPSCALESWIZZLE_HPP
#define FIELD_MAP_EDITOR_MAPUPSCALESWIZZLE_HPP
#include "Map.hpp"
#include <open_viii/graphics/background/TileOperations.hpp>
namespace ff_8
{
struct TileFunctionsUpscaleSwizzle
{
     using X = open_viii::graphics::background::tile_operations::SourceX;
     using Y = open_viii::graphics::background::tile_operations::SourceY;
     using TexturePage
       = open_viii::graphics::background::tile_operations::TextureId;
     using UseTexturePage                     = std::false_type;
     using UseBlending                        = std::false_type;
     static constexpr const char *const label = "Map (Swizzle Swizzle)";
};
using MapUpscaleSwizzle = Map<TileFunctionsUpscaleSwizzle>;
static_assert(glengine::Renderable<MapUpscaleSwizzle>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPUPSCALESWIZZLE_HPP
