//
// Created by pcvii on 3/1/2023.
//

#ifndef FIELD_MAP_EDITOR_RANGE_HOLDER_HPP
#define FIELD_MAP_EDITOR_RANGE_HOLDER_HPP
#include <ranges>
template<std::ranges::range range_t>
class range_holder
{
   public:
     using value_t = std::ranges::range_value_t<range_t>;

   private:
     range_t m_range;

   public:
     using begin_t = decltype(std::ranges::begin(m_range));
     using end_t   = decltype(std::ranges::end(m_range));

   private:
     mutable begin_t pos{};
     end_t           end_pos{};

   public:
     explicit range_holder(range_t in_range)
       : m_range(std::move(in_range))
       , pos(std::ranges::begin(m_range))
       , end_pos(std::ranges::end(m_range))
     {
     }
     const value_t &operator++() const
     {
          return *++pos;
     }
     const value_t &operator++(int) const
     {
          return *pos++;
     }
     const value_t &operator*() const
     {
          return *pos;
     }
     operator bool() const
     {
          return pos != end_pos;
     }
};
#endif// FIELD_MAP_EDITOR_RANGE_HOLDER_HPP
