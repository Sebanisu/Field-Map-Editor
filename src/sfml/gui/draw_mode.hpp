//
// Created by pcvii on 3/13/2023.
//

#ifndef FIELD_MAP_EDITOR_DRAW_MODE_HPP
#define FIELD_MAP_EDITOR_DRAW_MODE_HPP
#include <cstdint>
enum struct draw_mode : std::uint8_t
{
     draw_mim, // render raw mim textures.
     draw_map, // render map in output_draw_mode
};


enum struct output_draw_mode : std::uint8_t
{
     output_deswizzle,// Use Destination X, Y, Z to arrange tiles.
     output_swizzle,// Use Source X,Y, Texture Page to arrange tiles.
     output_horizontal_tile_index_swizzle// for FFNX use Tile Index to arrange tiles. MAX 256 px height not up-scaled. (16 tiles high) width
                                         // is number of tiles as pixels. Might skip the invalid tile at the end.
};
#endif// FIELD_MAP_EDITOR_DRAW_MODE_HPP
