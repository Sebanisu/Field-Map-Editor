#ifndef CA95268C_7560_47D3_A991_A641D0AED050
#define CA95268C_7560_47D3_A991_A641D0AED050
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stacktrace>

template<>
struct fmt::formatter<std::stacktrace>
{
     constexpr auto parse(fmt::format_parse_context &ctx)
     {
          return ctx.begin();// no custom format specifiers
     }

     template<typename FormatContext>
     auto format(
       const std::stacktrace &st,
       FormatContext         &ctx) const
     {
          auto out = ctx.out();
          fmt::format_to(out, "Stacktrace ({} frames):\n", st.size());
          for (const auto &[i, frame] : st | std::views::enumerate)
          {
               fmt::format_to(
                 out,
                 "{:>3}: {} [{}:{}]\n",
                 i,
                 frame.description(),
                 frame.source_file(),
                 frame.source_line());
          }
          return out;
     }
};


#endif /* CA95268C_7560_47D3_A991_A641D0AED050 */
