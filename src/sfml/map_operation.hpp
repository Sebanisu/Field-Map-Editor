//
// Created by pcvii on 2/28/2023.
//

#ifndef FIELD_MAP_EDITOR_MAP_OPERATION_HPP
#define FIELD_MAP_EDITOR_MAP_OPERATION_HPP
#include "map_group.hpp"
namespace ff_8
{
void flatten_bpp(map_group::Map &map);
void flatten_palette(map_group::Map &map);
void compact_map_order(map_group::Map &map);
void compact_rows(map_group::Map &map);
void compact_all(map_group::Map &map);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAP_OPERATION_HPP
