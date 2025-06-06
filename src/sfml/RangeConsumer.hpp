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
     begin_t pos;
     end_t   end_pos;
     bool    m_stop = { false };

   public:
     RangeConsumer()
       : RangeConsumer(range_t{})
     {
     }
     explicit RangeConsumer(range_t in_range)
       : m_range(std::move(in_range))
       , pos(std::ranges::begin(m_range))
       , end_pos(std::ranges::end(m_range))
     {
     }
     void restart()
     {
          pos     = std::ranges::begin(m_range);
          end_pos = std::ranges::end(m_range);
          m_stop  = false;
     }
     RangeConsumer<range_t> &operator=(range_t new_value)
     {
          m_range = std::move(new_value);
          restart();
          return *this;
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
          return m_stop || pos == end_pos;
     }
     auto &operator++()
     {
          pos = std::next(pos);
          return *this;
     }
     decltype(auto) operator*() const
     {
          return *pos;
     }
     decltype(auto) operator*()
     {
          return *pos;
     }
     explicit operator bool() const
     {
          return !done();
     }
     void stop()
     {
          m_stop = true;
     }
};
template<std::ranges::range range_t>
class [[nodiscard]] FutureConsumer
{
   private:
     using range_value_t = std::ranges::range_value_t<range_t>;

     RangeConsumer<range_t> m_range{};


   public:
     FutureConsumer() = default;
     explicit FutureConsumer(range_t in_range)
       : m_range(std::move(in_range))
     {
     }
     void                   consume_now()
     {
         for (; !done(); ++m_range)
         {
             auto& item = *m_range;
             if (item.valid()) {
                 item.get();
             }
         }
     }
     FutureConsumer<range_t> &operator=(range_t new_value)
     {
          consume_now();
          m_range = std::move(new_value);
          return *this;
     }
     const FutureConsumer<range_t> &operator++()
     {
          //if (!done())
          //{
          //     auto &item = *m_range;
          //     if (item.valid())
          //     {
          //          item.get();
          //     }
          //     ++m_range;
          //}
          if (!done()) {
              auto& item = *m_range;
              const auto state = item.wait_for(std::chrono::seconds(0));
              if (std::future_status::ready != state && std::future_status::deferred != state) {
                  // launch policy is async and the future is not yet ready, so skip it
                  return *this;
              }
              if (item.valid()) {
                  item.get();
              }
              ++m_range;
          }
          return *this;
     }
     [[nodiscard]] bool done() const
     {
          return m_range.done();
     }
};
template<std::ranges::range range_t>
class [[nodiscard]] FutureOfFutureConsumer
{
   private:
     using range_value_t  = std::ranges::range_value_t<range_t>;
     using future_value_t = std::remove_cvref_t<decltype(range_value_t{}.get())>;

     RangeConsumer<range_t>      m_range{};
     std::vector<future_value_t> m_out{};


   public:

     FutureOfFutureConsumer() = default;
     explicit FutureOfFutureConsumer(range_t in_range)
       : m_range(std::move(in_range))
     {
          m_out.reserve(std::ranges::size(m_range));
     }

     void                        consume_now()
     {
         for (; !done(); ++m_range)
         {
             auto& item = *m_range;
             if (item.valid()) {
                 m_out.push_back(item.get());
             }
         }
         auto frh = get_consumer();
         frh.consume_now();
     }
     FutureOfFutureConsumer &operator=(range_t && in_range)
     {
          if (!std::ranges::empty(m_range))
          {
               consume_now();
          }
          m_range = std::move(in_range);
          m_out.clear();
          m_out.reserve(std::ranges::size(m_range));
          return *this;
     }
     const FutureOfFutureConsumer<range_t> &operator++()
     {
          //if (!done())
          //{
          //     auto &item = *m_range;
          //     ++m_range;
          //     if (item.valid())
          //     {
          //          m_out.push_back(item.get());
          //     }
          //}
          if (!done()) {
              auto& item = *m_range;
              const auto state = item.wait_for(std::chrono::seconds(0));
              if (std::future_status::ready != state && std::future_status::deferred != state) {
                  // launch policy is async and the future is not yet ready, so skip it
                  return *this;
              }
              if (item.valid()) {
                  m_out.push_back(item.get());
              }
              ++m_range;
          }
          return *this;
     }
     [[nodiscard]] bool done() const
     {
          return m_range.done();
     }
     [[nodiscard]] bool output_empty() const
     {
          return std::ranges::empty(m_out);
     }
     [[nodiscard]] decltype(auto) get_consumer()
     {
          return FutureConsumer{ std::move(m_out) };
     }
     [[nodiscard]] decltype(auto) get_future_of_future_consumer()
     {
          return FutureOfFutureConsumer{ std::move(m_out) };
     }
};
#endif// FIELD_MAP_EDITOR_RANGECONSUMER_HPP
