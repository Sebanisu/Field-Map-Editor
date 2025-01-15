
#include "source_tile_conflicts.hpp"
#include <boost/ut.hpp>// single header
#include <ranges>
constexpr auto sum(auto... args)
{
     return (args + ...);
}
int main()
{
     using namespace boost::ut;

     static constexpr auto t_range = std::ranges::views::iota(std::uint8_t{ 0 }, std::uint8_t{ ff_8::source_tile_conflicts::T_SIZE });
     static constexpr auto x_range =
       std::ranges::views::iota(std::uint8_t{ 0 }, std::uint8_t{ ff_8::source_tile_conflicts::X_SIZE })
       | std::ranges::views::transform([](const auto i) { return std::uint8_t{ i * ff_8::source_tile_conflicts::X_SIZE }; });
     static constexpr auto y_range =
       std::ranges::views::iota(std::uint8_t{ 0 }, std::uint8_t{ ff_8::source_tile_conflicts::Y_SIZE })
       | std::ranges::views::transform([](const auto i) { return std::uint8_t{ i * ff_8::source_tile_conflicts::X_SIZE }; });
     // Test case to validate the reverse mapping
     test("Reverse index validation") = [&] {
          const ff_8::source_tile_conflicts stc{};
          for (const auto t : t_range)
          {
               for (const auto y : y_range)
               {
                    for (const auto x : x_range)
                    {
                         const auto   index = ff_8::source_tile_conflicts::calculate_index(x, y, t);
                         std::uint8_t x_out, y_out, t_out;
                         ff_8::source_tile_conflicts::reverse_index(index, x_out, y_out, t_out);

                         // Assert that input matches output
                         expect(x == x_out) << "Mismatch in x: input " << +x << ", output " << +x_out;
                         expect(y == y_out) << "Mismatch in y: input " << +y << ", output " << +y_out;
                         expect(t == t_out) << "Mismatch in t: input " << +t << ", output " << +t_out;
                         expect(index < std::ranges::size(stc.m_grid))
                           << "Index out of range: t=" << +t << ", y=" << +y << ", x=" << +x << ", index=" << +index
                           << ", size=" << std::ranges::size(stc.m_grid);

                         if (x != x_out || y != y_out || t != t_out)
                         {
                              return;
                         }
                    }
               }
          }
     };
}