
#include <boost/ut.hpp>// single header
#include <ff_8/SourceTileConflicts.hpp>
#include <ranges>
static constexpr auto sum(auto... args)
{
     return (args + ...);
}
int main()
{
     using namespace boost::ut;

     static constexpr auto t_range = std::ranges::views::iota(
       std::uint8_t{},
       static_cast<std::uint8_t>(ff_8::SourceTileConflicts::T_SIZE));
     static constexpr auto x_range
       = std::ranges::views::iota(
           std::uint8_t{},
           static_cast<std::uint8_t>(ff_8::SourceTileConflicts::X_SIZE))
         | std::ranges::views::transform(
           [](const std::uint8_t i)
           {
                return static_cast<std::uint8_t>(
                  i * ff_8::SourceTileConflicts::X_SIZE);
           });
     static constexpr auto y_range
       = std::ranges::views::iota(
           std::uint8_t{},
           static_cast<std::uint8_t>(ff_8::SourceTileConflicts::Y_SIZE))
         | std::ranges::views::transform(
           [](const std::uint8_t i)
           {
                return static_cast<std::uint8_t>(
                  i * ff_8::SourceTileConflicts::Y_SIZE);
           });
     // Test case to validate the reverse mapping
     test("Reverse index validation") = [&]
     {
          const ff_8::SourceTileConflicts stc{};
          for (const auto t : t_range)
          {
               for (const auto y : y_range)
               {
                    for (const auto x : x_range)
                    {
                         const auto index
                           = ff_8::SourceTileConflicts::calculate_index(
                             x, y, t);

                         const auto l
                           = ff_8::SourceTileConflicts::reverse_index(index);

                         // Assert that input matches output
                         expect(x == l.x) << "Mismatch in x: input " << +x
                                          << ", output " << +l.x;
                         expect(y == l.y) << "Mismatch in y: input " << +y
                                          << ", output " << +l.y;
                         expect(t == l.t) << "Mismatch in t: input " << +t
                                          << ", output " << +l.t;
                         expect(
                           index < ff_8::SourceTileConflicts::X_SIZE
                                     * ff_8::SourceTileConflicts::Y_SIZE
                                     * ff_8::SourceTileConflicts::T_SIZE)
                           << "Index out of range: t=" << +t << ", y=" << +y
                           << ", x=" << +x << ", index=" << +index << ", size="
                           << ff_8::SourceTileConflicts::X_SIZE
                                * ff_8::SourceTileConflicts::Y_SIZE
                                * ff_8::SourceTileConflicts::T_SIZE;

                         if (x != l.x || y != l.y || t != l.t)
                         {
                              return;
                         }
                    }
               }
          }
     };
}