//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_MAPUPSCALEDESWIZZLE_HPP
#define FIELD_MAP_EDITOR_MAPUPSCALEDESWIZZLE_HPP
#include "Map.hpp"
#include <ff_8/TileOperations.hpp>
namespace ff_8
{
struct TileFunctionsUpscaleDeswizzle
{
     using X              = TileOperations::X;
     using Y              = TileOperations::Y;
     using TexturePage    = TileOperations::TextureId::DefaultValue;
     using UseTexturePage = std::true_type;
     using UseBlending    = std::true_type;
     static constexpr const char *const label = "Map (Swizzle Deswizzle)";
};
using MapUpscaleDeswizzle = Map<TileFunctionsUpscaleDeswizzle>;
static_assert(glengine::Renderable<MapUpscaleDeswizzle>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPUPSCALEDESWIZZLE_HPP
