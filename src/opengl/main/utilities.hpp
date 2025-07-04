#ifndef B24A24CC_B3E6_4D3F_AFB1_C86F174A1902
#define B24A24CC_B3E6_4D3F_AFB1_C86F174A1902
#include <algorithm>
#include <concepts>
#include <ranges>
namespace fme
{
template<typename R>
concept erasable_range = std::ranges::range<R> && requires(R &r) {
     { r.erase(std::ranges::begin(r), std::ranges::end(r)) } -> std::same_as<typename R::iterator>;
};
template<erasable_range... R>
constexpr inline bool sort_and_remove_duplicates(R &...ranges) noexcept
{
     bool changed  = false;
     const auto projection = [](const auto &values) { return std::get<0>(values); };
     auto zip_view = std::ranges::views::zip(ranges...);
     if (!std::ranges::is_sorted(zip_view, {}, projection))
     {
          std::ranges::sort(zip_view, {}, projection);
          changed = true;
     }
     if (std::ranges::adjacent_find(zip_view, {}, projection) != zip_view.end())
     {
          const auto unique_range        = std::ranges::unique(zip_view, {}, projection);

          const auto unique_begin_offset = std::ranges::distance(zip_view.begin(), unique_range.begin());

          (ranges.erase(std::ranges::next(ranges.begin(), unique_begin_offset), ranges.end()), ...);
          changed = true;
     }
     return changed;
}
}// namespace fme

#endif /* B24A24CC_B3E6_4D3F_AFB1_C86F174A1902 */
