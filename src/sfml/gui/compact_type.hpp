//
// Created by pcvii on 12/21/2022.
//

#ifndef FIELD_MAP_EDITOR_COMPACT_TYPE_HPP
#define FIELD_MAP_EDITOR_COMPACT_TYPE_HPP
#include <cstdint>
#include <optional>
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
[[nodiscard]] constexpr static inline std::optional<output_types> to_output_type(input_types input) noexcept
{
     switch (input)
     {
          case input_types::swizzle:
               return output_types::swizzle;
          case input_types::deswizzle:
               return output_types::deswizzle;
          default:
               return std::nullopt;// input_types::mim has no mapping
     }
}

[[nodiscard]] constexpr static inline std::optional<input_types> to_input_type(output_types output) noexcept
{
     switch (output)
     {
          case output_types::swizzle:
               return input_types::swizzle;
          case output_types::deswizzle:
               return input_types::deswizzle;
          default:
               return std::nullopt;// input_types::mim has no mapping
     }
}
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

enum class PatternSelector : std::uint8_t
{
     OutputSwizzlePattern,
     OutputDeswizzlePattern,
     OutputMapPatternForSwizzle,
     OutputMapPatternForDeswizzle,
};

}// namespace fme
#endif// FIELD_MAP_EDITOR_COMPACT_TYPE_HPP
