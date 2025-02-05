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
}// namespace fme
#endif// FIELD_MAP_EDITOR_COMPACT_TYPE_HPP
