#ifndef B24A24CC_B3E6_4D3F_AFB1_C86F174A1902
#define B24A24CC_B3E6_4D3F_AFB1_C86F174A1902
#include <algorithm>
#include <concepts>
#include <filesystem>
#include <ranges>
#include <tuple>
#include <variant>
#include <vector>

namespace fme
{

template<typename R>
concept erasable_range = std::ranges::range<R> && requires(R &r) {
     {
          r.erase(std::ranges::begin(r), std::ranges::end(r))
     } -> std::same_as<typename R::iterator>;
};

template<erasable_range... R>
constexpr inline bool sort_and_remove_duplicates(R &...ranges) noexcept
{
     bool       changed = false;
     const auto projection
       = [](const auto &values) { return std::get<0>(values); };
     auto zip_view = std::ranges::views::zip(ranges...);
     if (!std::ranges::is_sorted(zip_view, {}, projection))
     {
          std::ranges::sort(zip_view, {}, projection);
          changed = true;
     }
     if (std::ranges::adjacent_find(zip_view, {}, projection) != zip_view.end())
     {
          const auto unique_range
            = std::ranges::unique(zip_view, {}, projection);

          const auto unique_begin_offset
            = std::ranges::distance(zip_view.begin(), unique_range.begin());

          (ranges.erase(
             std::ranges::next(ranges.begin(), unique_begin_offset),
             ranges.end()),
           ...);
          changed = true;
     }
     return changed;
}

template<erasable_range... R>
constexpr inline bool remove_empty_values(R &...ranges)
{
     const auto projection
       = [](const auto &values) -> const auto & { return std::get<0>(values); };
     auto zip_view = std::ranges::views::zip(ranges...);
     auto it       = std::ranges::remove_if(
                 zip_view,
                 [](const auto &val)
                 {
                      if constexpr (std::ranges::range<
                                            std::remove_cvref_t<decltype(val)>>)
                      {
                           return std::ranges::empty(val);
                      }
                      else
                      {
                           return false;
                      }
                 },
                 projection)
                 .begin();

     const auto offset = std::ranges::distance(zip_view.begin(), it);
     if (offset < std::ranges::distance(zip_view))
     {
          (ranges.erase(
             std::ranges::next(ranges.begin(), offset), ranges.end()),
           ...);
          return true;
     }
     return false;
}

#include <filesystem>

template<erasable_range... R>
inline bool remove_nonexistent_paths(
  const bool remove_on_error,
  R &...ranges)
{
     auto zip_view = std::ranges::views::zip(ranges...);
     auto it       = std::ranges::remove_if(
                 zip_view,
                 [remove_on_error](const auto &group)
                 {
                      const auto &path = std::get<0>(group);
                      if constexpr (std::same_as<
                                            std::remove_cvref_t<decltype(path)>,
                                            std::filesystem::path>)
                      {
                           try
                           {
                                return !std::filesystem::exists(path);
                           }
                           catch (const std::filesystem::filesystem_error &)
                           {
                                return remove_on_error;// true treats errors as
                                                       // "path does not exist"
                                // true might not be desired as you take a disc
                                // out of the drive or something it would get
                                // removed.
                           }
                      }
                      else
                      {
                           return false;
                      }
                 })
                 .begin();

     const auto offset = std::ranges::distance(zip_view.begin(), it);
     if (offset < std::ranges::distance(zip_view))
     {
          (ranges.erase(
             std::ranges::next(ranges.begin(), offset), ranges.end()),
           ...);
          return true;
     }
     return false;
}


bool has_balanced_braces([[maybe_unused]] const std::string_view s);
bool has_balanced_braces([[maybe_unused]] const std::u8string_view s);

// using the template here is to avoid implicit conversions. converting the path
// to u8string to pass to the u8string_view overload.
template<typename T>
     requires(std::same_as<
              std::remove_cvref_t<T>,
              std::filesystem::path>)
static inline bool has_balanced_braces([[maybe_unused]] const T &s)
{
     return has_balanced_braces(s.u8string());
}

template<std::ranges::range R>
     requires(
       (std::convertible_to<
          std::ranges::range_value_t<R>,
          std::string_view>
        || std::convertible_to<
          std::ranges::range_value_t<R>,
          std::u8string_view>
        || std::same_as<
          std::remove_cvref_t<std::ranges::range_value_t<R>>,
          std::filesystem::path>)
       && !std::same_as<
          std::remove_cvref_t<R>,
          std::filesystem::path>)
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

template<std::ranges::sized_range rangeT>
constexpr inline auto generate_combinations_more(const rangeT &pupu_ids)
{
     size_t n              = std::ranges::size(pupu_ids);
     using difference_type = std::ranges::range_difference_t<std::vector<bool>>;
     // Create a view that generates all combinations
     return std::ranges::iota_view(size_t{ 1 }, size_t{ 1ull << n })
            | std::views::transform(
              [n, &pupu_ids, mask = std::vector<bool>(n, false)](size_t) mutable
              {
                   const auto increment_mask = [&]() -> bool
                   {
                        for (auto &&[index, bit] : std::views::enumerate(mask))
                        {
                             if (!bit)
                             {
                                  bit = true;
                                  std::fill(
                                    mask.begin(),
                                    mask.begin()
                                      + static_cast<difference_type>(index),
                                    false);
                                  return true;
                             }
                        }
                        return false;// All 1s, no more increments possible
                   };
                   // Increment the mask to get the next combination
                   (void)increment_mask();

                   // Generate the combination based on the current mask
                   return pupu_ids | std::views::enumerate
                          | std::views::filter(
                            [&mask](const auto &tuple)
                            {
                                 const auto &[idx, _] = tuple;
                                 return mask[static_cast<std::size_t>(idx)];
                            })
                          | std::views::transform(
                            [](const auto &tuple)
                            {
                                 const auto &[_, id] = tuple;
                                 return id;
                            });
              });
}

template<std::ranges::sized_range rangeT>
constexpr inline auto generate_combinations_64bit(const rangeT &pupu_ids)
{
     size_t n     = std::ranges::size(pupu_ids);
     size_t total = 1ull << n;

     return std::views::iota(1ull, total)
            | std::views::transform(
              [n, &pupu_ids](size_t mask)
              {
                   return pupu_ids | std::views::enumerate
                          | std::views::filter(
                            [mask](const auto &tuple)
                            {
                                 const auto &[idx, _] = tuple;
                                 return mask & (1ull << idx);
                            })
                          | std::views::transform(
                            [](const auto &tuple)
                            {
                                 const auto &[_, id] = tuple;
                                 return id;
                            });
              });
}

template<std::ranges::sized_range rangeT>
constexpr inline auto generate_combinations(const rangeT &pupu_ids)
{
     // Define the variant type to hold both possible range types
     using CombinationRange = std::variant<
       decltype(generate_combinations_64bit(
         std::declval<std::remove_cvref_t<decltype(pupu_ids)>>())),
       decltype(generate_combinations_more(
         std::declval<std::remove_cvref_t<decltype(pupu_ids)>>()))>;
     if (std::ranges::size(pupu_ids) >= 64ull)
     {
          return CombinationRange{ generate_combinations_more(pupu_ids) };
     }
     return CombinationRange{ generate_combinations_64bit(pupu_ids) };
}

}// namespace fme

#endif /* B24A24CC_B3E6_4D3F_AFB1_C86F174A1902 */
