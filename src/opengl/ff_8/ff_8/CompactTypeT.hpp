#ifndef F9B6101F_23D3_4195_B3F1_1697180FC40A
#define F9B6101F_23D3_4195_B3F1_1697180FC40A
#include <cstdint>
#include <fmt/format.h>
#include <string_view>
namespace ff_8
{
enum struct CompactTypeT : std::uint8_t
{
     rows,
     all,
     move_only_conflicts,
     map_order,
     map_order_ffnx
};
}

template<>
struct fmt::formatter<ff_8::CompactTypeT> : fmt::formatter<std::string_view>
{// parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(
       ff_8::CompactTypeT in_compact_type,
       FormatContext     &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (in_compact_type)
          {
               case ff_8::CompactTypeT::map_order_ffnx:
                    name = "map order ffnx"sv;
                    break;
               case ff_8::CompactTypeT::map_order:
                    name = "map order"sv;
                    break;
               case ff_8::CompactTypeT::move_only_conflicts:
                    name = "move only conflicts"sv;
                    break;
               case ff_8::CompactTypeT::rows:
                    name = "rows"sv;
                    break;
               case ff_8::CompactTypeT::all:
                    name = "all"sv;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};
#endif /* F9B6101F_23D3_4195_B3F1_1697180FC40A */
