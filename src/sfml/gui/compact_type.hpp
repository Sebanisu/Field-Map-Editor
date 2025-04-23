//
// Created by pcvii on 12/21/2022.
//

#ifndef FIELD_MAP_EDITOR_COMPACT_TYPE_HPP
#define FIELD_MAP_EDITOR_COMPACT_TYPE_HPP
#include <cstdint>
namespace fme
{
enum struct compact_type
{
     rows,
     all,
     move_only_conflicts,
     map_order,
     map_order_ffnx
};
enum struct input_types
{
     mim,
     deswizzle,
     swizzle,
};
enum struct output_types
{
     deswizzle,
     swizzle,
};
enum struct flatten_type : std::uint8_t
{
     bpp,
     palette,
     both,
};
enum struct root_path_types : std::uint8_t
{
     selected_path,
     ff8_path,
     current_path
};

}// namespace fme
#endif// FIELD_MAP_EDITOR_COMPACT_TYPE_HPP
