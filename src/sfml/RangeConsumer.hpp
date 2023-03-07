//
// Created by pcvii on 3/1/2023.
//

#ifndef FIELD_MAP_EDITOR_RANGECONSUMER_HPP
#define FIELD_MAP_EDITOR_RANGECONSUMER_HPP
#include <ranges>
#include <vector>
template<std::ranges::range range_t>
class RangeConsumer
{
   public:
     using value_t = std::ranges::range_value_t<range_t>;

   private:
     range_t m_range;

   public:
     using begin_t = std::remove_cvref_t<decltype(std::ranges::begin(m_range))>;
     using end_t   = std::remove_cvref_t<decltype(std::ranges::end(m_range))>;

   private:
     mutable begin_t pos{};
     end_t           end_pos{};

   public:
     explicit RangeConsumer(range_t in_range)
       : m_range(std::move(in_range))
       , pos(std::ranges::begin(m_range))
       , end_pos(std::ranges::end(m_range))
     {
     }
     [[nodiscard]] std::size_t size() const
     {
          return std::ranges::size(m_range);
     }
     [[nodiscard]] bool empty() const
     {
          return std::ranges::empty(m_range);
     }
     [[nodiscard]] bool done() const
     {
          return pos == end_pos;
     }
     const auto &operator++() const
     {
          ++pos;
          return *this;
     }
     auto &operator++()
     {
          ++pos;
          return *this;
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
          return !done();
     }
};

template<std::ranges::range range_t>
class FutureOfFutureConsumer
{
   private:
     using range_value_t  = std::ranges::range_value_t<range_t>;
     using future_value_t = std::remove_cvref_t<decltype(range_value_t{}.get())>;

     RangeConsumer<range_t>      m_range{};
     std::vector<future_value_t> m_out{};


   public:
     explicit FutureOfFutureConsumer(range_t in_range)
       : m_range(std::move(in_range))
     {
          m_out.reserve(std::ranges::size(m_range));
     }
     const FutureOfFutureConsumer<range_t> &operator++()
     {
          if (!done())
          {
               auto &item = *m_range;
               ++m_range;
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
     [[nodiscard]] decltype(auto) get_holder()
     {
          return RangeConsumer{ std::move(m_out) };
     }
     [[nodiscard]] decltype(auto) get_future_of_future_holder()
     {
          return FutureOfFutureConsumer{ std::move(m_out) };
     }
};
#endif// FIELD_MAP_EDITOR_RANGECONSUMER_HPP
