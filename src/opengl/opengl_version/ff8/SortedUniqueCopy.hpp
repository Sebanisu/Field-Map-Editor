//
// Created by pcvii on 2/7/2022.
//

#ifndef FIELD_MAP_EDITOR_SORTEDUNIQUECOPY_HPP
#define FIELD_MAP_EDITOR_SORTEDUNIQUECOPY_HPP
template<
  std::ranges::range rangeT,
  typename projectionT = std::identity,
  std::indirect_equivalence_relation<
    std::projected<std::ranges::iterator_t<rangeT>, projectionT>> compareT =
    std::ranges::equal_to>
static inline constexpr auto SortUnique(rangeT &&range, projectionT proj = {}, compareT comp = {})
{
  auto projected_range = range | std::views::transform(proj);
  std::vector<std::ranges::range_value_t<decltype(projected_range)>> out = {
    projected_range.begin(), projected_range.end()
  };
  std::ranges::sort(out);
  const auto [first, last] = std::ranges::unique(out, comp);
  out.erase(first, last);
  return out;
}
#endif// FIELD_MAP_EDITOR_SORTEDUNIQUECOPY_HPP
