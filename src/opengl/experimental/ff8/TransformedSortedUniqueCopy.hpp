//
// Created by pcvii on 2/7/2022.
//

#ifndef FIELD_MAP_EDITOR_TRANSFORMEDSORTEDUNIQUECOPY_HPP
#define FIELD_MAP_EDITOR_TRANSFORMEDSORTEDUNIQUECOPY_HPP

template<
  typename transformT      = std::identity,
  typename sort_compareT   = std::ranges::less,
  typename unique_compareT = std::ranges::equal_to,
  typename projectionT     = std::identity,
  std::ranges::range... rangeT>
     requires(
       std::indirect_equivalence_relation<
         unique_compareT,
         std::projected<
           std::ranges::iterator_t<rangeT>,
           transformT>>,
       ...)
static inline auto TransformedSortedUniqueCopy(
  transformT      transform    = {},
  sort_compareT   sort_compare = {},
  unique_compareT unique_comp  = {},
  projectionT     projection   = {},
  rangeT &&...range)
{

     std::vector<typename std::projected<
       std::ranges::iterator_t<glengine::first_t<rangeT...>>,
       transformT>::value_type>
       out = {};
     (
       [&]()
       {
            auto transformed_range
              = std::forward<rangeT>(range) | std::views::transform(transform);
            std::ranges::copy(transformed_range, std::back_inserter(out));
       }(),
       ...);
     std::ranges::sort(out, sort_compare, projection);
     const auto [first, last]
       = std::ranges::unique(out, unique_comp, projection);
     out.erase(first, last);
     return out;
}
#endif// FIELD_MAP_EDITOR_TRANSFORMEDSORTEDUNIQUECOPY_HPP
