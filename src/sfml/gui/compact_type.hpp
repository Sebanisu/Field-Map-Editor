//
// Created by pcvii on 12/21/2022.
//

#ifndef FIELD_MAP_EDITOR_COMPACT_TYPE_HPP
#define FIELD_MAP_EDITOR_COMPACT_TYPE_HPP
#include <cstdint>
#include <fmt/format.h>
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

template<>
struct fmt::formatter<compact_type> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(compact_type in_compact_type, FormatContext &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (in_compact_type)
          {
               case compact_type::map_order_ffnx:
                    name = "Map Order: FFNX"sv;
                    break;
               case compact_type::map_order:
                    name = "Map Order"sv;
                    break;
               case compact_type::rows:
                    name = "Rows"sv;
                    break;
               case compact_type::all:
                    name = "All"sv;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};

template<>
struct fmt::formatter<input_types> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(input_types in_input_type, FormatContext &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (in_input_type)
          {
               case input_types::mim:
                    name = "Mim"sv;
                    break;
               case input_types::deswizzle:
                    name = "Deswizzle"sv;
                    break;
               case input_types::swizzle:
                    name = "Swizzle"sv;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};

template<>
struct fmt::formatter<output_types> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(output_types in_output_type, FormatContext &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (in_output_type)
          {
               case output_types::deswizzle:
                    name = "Deswizzle"sv;
                    break;
               case output_types::swizzle:
                    name = "Swizzle"sv;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};
template<>
struct fmt::formatter<flatten_type> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(flatten_type in_flatten_type, FormatContext &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (in_flatten_type)
          {
               case flatten_type::bpp:
                    name = "BPP"sv;
                    break;
               case flatten_type::palette:
                    name = "Palette"sv;
                    break;
               case flatten_type::both:
                    name = "BPP & Palette"sv;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};
#endif// FIELD_MAP_EDITOR_COMPACT_TYPE_HPP
