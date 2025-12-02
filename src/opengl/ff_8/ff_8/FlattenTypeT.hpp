#ifndef A2A1F775_0956_4BD9_91AA_C281C4D68D79
#define A2A1F775_0956_4BD9_91AA_C281C4D68D79
#include <cstdint>
#include <fmt/format.h>
#include <string_view>
namespace ff_8
{
enum struct FlattenTypeT : std::uint8_t
{
     bpp,
     palette,
     both,
};
}

template<>
struct fmt::formatter<ff_8::FlattenTypeT> : fmt::formatter<std::string_view>
{
     // parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(
       ff_8::FlattenTypeT in_flatten_type,
       FormatContext     &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (in_flatten_type)
          {
               case ff_8::FlattenTypeT::bpp:
                    name = "bpp"sv;
                    break;
               case ff_8::FlattenTypeT::palette:
                    name = "palette"sv;
                    break;
               case ff_8::FlattenTypeT::both:
                    name = "both"sv;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};
#endif /* A2A1F775_0956_4BD9_91AA_C281C4D68D79 */
