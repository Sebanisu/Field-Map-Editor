//
// Created by pcvii on 3/1/2023.
//

#ifndef FIELD_MAP_EDITOR_RANGEHOLDER_HPP
#define FIELD_MAP_EDITOR_RANGEHOLDER_HPP
#include <ranges>
#include <vector>
template<std::ranges::range range_t>
class RangeHolder
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
     explicit RangeHolder(range_t in_range)
       : m_range(std::move(in_range))
       , pos(std::ranges::begin(m_range))
       , end_pos(std::ranges::end(m_range))
     {
     }
     const value_t &operator++() const
     {
          return *++pos;
     }
     value_t &operator++()
     {
          return *++pos;
     }
     const value_t &operator*() const
     {
          return *pos;
     }
     value_t &operator*()
     {
          return *pos;
     }
     explicit operator bool() const
     {
          return pos != end_pos;
     }
};

template<std::ranges::range range_t>
class FutureOfFutureHolder
{
   private:
     RangeHolder<range_t> m_range;
     using range_value_t  = std::ranges::range_value_t<range_t>;
     using future_value_t = std::remove_cvref_t<decltype(range_value_t{}.get())>;
     std::vector<future_value_t> m_out{};

   public:
     explicit FutureOfFutureHolder(range_t in_range)
     {
          m_out.reserve(in_range);
          m_range = std::move(in_range);
     }
     const FutureOfFutureHolder<range_t> &operator++()
     {
          if (!done())
          {
               auto &item = ++m_range;
               m_out.push_back(item.get());
          }

          return *this;
     }
     [[nodiscard]] bool done() const
     {
          return !static_cast<bool>(m_range);
     }
     [[nodiscard]] bool empty() const
     {
          return std::ranges::empty(m_out);
     }
     [[nodiscard]] auto get_holder() const
     {
          return RangeHolder{std::move(m_out)};
     }
     [[nodiscard]] auto get_future_of_future_holder() const
     {
          return FutureOfFutureHolder{std::move(m_out)};
     }

};
#endif// FIELD_MAP_EDITOR_RANGEHOLDER_HPP
