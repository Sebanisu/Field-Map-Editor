//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_MAPDESWIZZLE_HPP
#define FIELD_MAP_EDITOR_MAPDESWIZZLE_HPP
#include "Map.hpp"
#include <ff_8/TileOperations.hpp>
namespace ff_8
{
struct TileFunctionsDeswizzle
{
     using X              = TileOperations::X;
     using Y              = TileOperations::Y;
     using TexturePage    = TileOperations::TextureId::DefaultValue;
     using UseTexturePage = std::true_type;
     using UseBlending    = std::true_type;
     static constexpr const char *const label = "Map (Deswizzle)";
};
using MapDeswizzle = Map<TileFunctionsDeswizzle>;
static_assert(glengine::Renderable<MapDeswizzle>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPDESWIZZLE_HPP
