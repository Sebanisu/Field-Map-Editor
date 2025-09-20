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
     swizzle_as_one_image,
     // deswizzle_generate_toml,
     deswizzle_full_filename,// only have one combined step for input that takes
                             // images and toml
};
enum struct output_types
{
     deswizzle,
     swizzle,
     swizzle_as_one_image,
     deswizzle_generate_toml,
     deswizzle_full_filename,
     csv,
};
[[nodiscard]] constexpr static inline std::optional<output_types>
  to_output_type(input_types input) noexcept
{
     switch (input)
     {
          case input_types::swizzle:
               return output_types::swizzle;
          case input_types::deswizzle:
               return output_types::deswizzle;
          case input_types::swizzle_as_one_image:
               return output_types::swizzle_as_one_image;
          // case input_types::deswizzle_generate_toml:
          //      return output_types::deswizzle_generate_toml;
          case input_types::deswizzle_full_filename:
               return output_types::deswizzle_full_filename;
          default:
               return std::nullopt;// input_types::mim has no mapping
     }
}

[[nodiscard]] constexpr static inline std::optional<input_types>
  to_input_type(output_types output) noexcept
{
     switch (output)
     {
          case output_types::swizzle:
               return input_types::swizzle;
          case output_types::deswizzle:
               return input_types::deswizzle;
          case output_types::swizzle_as_one_image:
               return input_types::swizzle_as_one_image;
          // case output_types::deswizzle_generate_toml:
          //      return input_types::deswizzle_generate_toml;
          case output_types::deswizzle_full_filename:
               return input_types::deswizzle_full_filename;
          // case output_types::csv:
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


enum struct VectorOrString : std::uint8_t
{
     unknown,
     vector,
     string,
};


enum class PatternSelector : std::uint8_t
{
     OutputSwizzlePattern,
     OutputSwizzleAsOneImagePattern,
     OutputDeswizzlePattern,
     OutputFullFileNamePattern,
     OutputTomlPattern,
     OutputMapPatternForSwizzle,
     OutputMapPatternForDeswizzle,
     OutputMapPatternForFullFileName,
     PatternsCommonPrefixes,
     PatternsCommonPrefixesForMaps,
     PatternsBase,
     PathPatternsWithPaletteAndTexturePage,
     PathPatternsWithPalette,
     PathPatternsWithTexturePage,
     PathPatternsWithPupuID,
     PathPatternsWithFullFileName,
     End,
};

enum struct FailOverLevels : std::int8_t
{
     All     = -1,
     Loaded  = 0,
     Generic = 1,
     EN      = 2,
     FR      = 3,
     DE      = 4,
     IT      = 5,
     ES      = 6,
     JP      = 7,
     Begin   = All,
     End     = JP,
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_COMPACT_TYPE_HPP
