#ifndef B24A24CC_B3E6_4D3F_AFB1_C86F174A1902
#define B24A24CC_B3E6_4D3F_AFB1_C86F174A1902
#include <algorithm>
#include <concepts>
#include <ranges>
#include <filesystem>
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

bool has_balanced_braces([[maybe_unused]] const std::string_view s);
bool has_balanced_braces([[maybe_unused]] const std::u8string_view s);

// using the template here is to avoid implicit conversions. converting the path to u8string to pass to the u8string_view overload.
template<typename T>
     requires(std::same_as<std::remove_cvref_t<T>, std::filesystem::path>)
static inline bool has_balanced_braces([[maybe_unused]] const T &s)
{
     return has_balanced_braces(s.u8string());
}

template<std::ranges::range R>
     requires(
       (std::convertible_to<std::ranges::range_value_t<R>, std::string_view>
        || std::convertible_to<std::ranges::range_value_t<R>, std::u8string_view>
        || std::same_as<std::remove_cvref_t<std::ranges::range_value_t<R>>, std::filesystem::path>)
       && !std::same_as<std::remove_cvref_t<R>, std::filesystem::path>)
static inline bool has_balanced_braces([[maybe_unused]] const R &r)
{
     for (const auto &s : r)
     {
          if (bool ok = has_balanced_braces(s); !ok)
          {
               return false;// found bad brace.
          }
     }
     return true;
}

}// namespace fme

#endif /* B24A24CC_B3E6_4D3F_AFB1_C86F174A1902 */
