//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_MAPDESWIZZLE_HPP
#define FIELD_MAP_EDITOR_MAPDESWIZZLE_HPP
#include "Map.hpp"
#include <open_viii/graphics/background/TileOperations.hpp>
namespace ff_8
{
struct TileFunctionsDeswizzle
{
     using X           = open_viii::graphics::background::tile_operations::X;
     using Y           = open_viii::graphics::background::tile_operations::Y;
     using TexturePage = open_viii::graphics::background::tile_operations::
       TextureIdDefaultValue;
     using UseTexturePage                     = std::true_type;
     using UseBlending                        = std::true_type;
     static constexpr const char *const label = "Map (Deswizzle)";
};
using MapDeswizzle = Map<TileFunctionsDeswizzle>;
static_assert(glengine::Renderable<MapDeswizzle>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPDESWIZZLE_HPP
