//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_MAPUPSCALEDESWIZZLE_HPP
#define FIELD_MAP_EDITOR_MAPUPSCALEDESWIZZLE_HPP
#include "Map.hpp"
#include <open_viii/graphics/background/TileOperations.hpp>
namespace ff_8
{
struct TileFunctionsUpscaleDeswizzle
{
     using X           = open_viii::graphics::background::tile_operations::X;
     using Y           = open_viii::graphics::background::tile_operations::Y;
     using TexturePage = open_viii::graphics::background::tile_operations::
       TextureIdDefaultValue;
     using UseTexturePage                     = std::true_type;
     using UseBlending                        = std::true_type;
     static constexpr const char *const label = "Map (Swizzle Deswizzle)";
};
using MapUpscaleDeswizzle = Map<TileFunctionsUpscaleDeswizzle>;
static_assert(glengine::Renderable<MapUpscaleDeswizzle>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPUPSCALEDESWIZZLE_HPP
