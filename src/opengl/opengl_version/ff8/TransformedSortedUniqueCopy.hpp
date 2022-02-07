//
// Created by pcvii on 2/7/2022.
//

#ifndef FIELD_MAP_EDITOR_TRANSFORMEDSORTEDUNIQUECOPY_HPP
#define FIELD_MAP_EDITOR_TRANSFORMEDSORTEDUNIQUECOPY_HPP
template<
  std::ranges::range rangeT,
  typename transformT    = std::identity,
  typename sort_compareT = std::ranges::less,
  std::indirect_equivalence_relation<
    std::projected<std::ranges::iterator_t<rangeT>, transformT>>
    unique_compareT = std::ranges::equal_to>
static inline constexpr std::vector<typename std::projected<
  std::ranges::iterator_t<rangeT>,
  transformT>::value_type>
  TransformedSortedUniqueCopy(
    rangeT        &&range,
    transformT      transform    = {},
    sort_compareT   sort_compare = {},
    unique_compareT unique_comp  = {})
{
  auto projected_range = range | std::views::transform(transform);
  std::vector<typename std::projected<
    std::ranges::iterator_t<rangeT>,
    transformT>::value_type>
    out = { projected_range.begin(), projected_range.end() };
  std::ranges::sort(out, sort_compare);
  const auto [first, last] = std::ranges::unique(out, unique_comp);
  out.erase(first, last);
  return out;
}
#endif// FIELD_MAP_EDITOR_TRANSFORMEDSORTEDUNIQUECOPY_HPP
