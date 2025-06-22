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

     std::vector<RangeConsumer<range_t>> m_ranges{};

     void                                compact_front()
     {
          const auto new_begin = std::ranges::find_if(m_ranges, [](const auto &range) { return !range.done(); });
          m_ranges.erase(m_ranges.begin(), new_begin);
     }

   public:
     FutureConsumer() = default;
     explicit FutureConsumer(range_t in_range)
     {
          m_ranges.emplace_back(std::move(in_range));
     }

     FutureConsumer(const FutureConsumer &)            = delete;
     FutureConsumer &operator=(const FutureConsumer &) = delete;

     FutureConsumer(FutureConsumer &&other) noexcept
     {
          other.consume_now();
          std::swap(m_ranges, other.m_ranges);
     }

     FutureConsumer &operator=(FutureConsumer &&other) noexcept
     {
          if (this != &other)
          {
               consume_now();
               std::swap(m_ranges, other.m_ranges);
               other.m_ranges.clear();// optional but keeps 'other' in a clean state
          }
          return *this;
     }

     void consume_now()
     {
          for (auto &range : m_ranges)
          {
               for (; !range.done(); ++range)
               {
                    auto &item = *range;
                    if (item.valid())
                    {
                         item.get();
                    }
               }
          }
     }
     FutureConsumer<range_t> &operator=(range_t new_value)
     {
          consume_now();
          m_ranges.clear();
          m_ranges.emplace_back(std::move(new_value));
          return *this;
     }

     FutureConsumer<range_t> &operator+=(range_t new_value)
     {
          m_ranges.emplace_back(std::move(new_value));
          return *this;
     }

     FutureConsumer<range_t> &operator+=(FutureConsumer<range_t> &&other)
     {
          for (auto &range : other.m_ranges)
          {
               m_ranges.push_back(std::move(range));
          }
          other.m_ranges.clear();// optionally clear the moved-from consumer
          return *this;
     }

     const FutureConsumer<range_t> &operator++()
     {
          compact_front();
          if (!done())
          {
               auto      &range = m_ranges.front();
               auto      &item  = *range;
               const auto state = item.wait_for(std::chrono::seconds(0));
               if (std::future_status::ready != state && std::future_status::deferred != state)
               {
                    // launch policy is async and the future is not yet ready, so skip it
                    return *this;
               }
               if (item.valid())
               {
                    item.get();
               }
               ++range;
          }
          return *this;
     }
     [[nodiscard]] bool done() const
     {
          return std::ranges::all_of(m_ranges, [](const auto &range) { return range.done(); });
     }

     ~FutureConsumer()
     {
          consume_now();
     }
};
template<std::ranges::range range_t>
class [[nodiscard]] FutureOfFutureConsumer
{
   private:
     using range_value_t  = std::ranges::range_value_t<range_t>;
     using future_value_t = std::remove_cvref_t<decltype(range_value_t{}.get())>;

     std::vector<RangeConsumer<range_t>> m_ranges{};
     std::vector<future_value_t>         m_out{};

     void                                compact_front()
     {
          const auto new_begin = std::ranges::find_if(m_ranges, [](const auto &range) { return !range.done(); });
          m_ranges.erase(m_ranges.begin(), new_begin);
     }

   public:
     FutureOfFutureConsumer() = default;
     explicit FutureOfFutureConsumer(range_t in_range)
     {
          m_ranges.clear();
          m_ranges.emplace_back(std::move(in_range));
          m_out.reserve(std::ranges::size(m_ranges.front()));
     }

     FutureOfFutureConsumer(const FutureOfFutureConsumer &)            = delete;
     FutureOfFutureConsumer &operator=(const FutureOfFutureConsumer &) = delete;

     FutureOfFutureConsumer(FutureOfFutureConsumer &&other) noexcept
     {
          other.consume_now();
          std::swap(m_ranges, other.m_ranges);
     }

     FutureOfFutureConsumer &operator=(FutureOfFutureConsumer &&other) noexcept
     {
          if (this != &other)
          {
               consume_now();
               std::swap(m_ranges, other.m_ranges);
               other.m_ranges.clear();// optional but keeps 'other' in a clean state
          }
          return *this;
     }

     void consume_now()
     {
          for (auto &&range : m_ranges)
          {
               for (; !range.done(); ++range)
               {
                    auto &item = *range;
                    if (item.valid())
                    {
                         m_out.push_back(item.get());
                    }
               }
               auto frh = get_consumer();
               frh.consume_now();
          }
     }

     FutureOfFutureConsumer &operator=(range_t &&in_range)
     {
          consume_now();
          m_ranges.clear();
          m_ranges.emplace_back(std::move(in_range));
          m_out.clear();
          m_out.reserve(std::ranges::size(m_ranges.first()));
          return *this;
     }

     FutureOfFutureConsumer<range_t> &operator+=(range_t in_range)
     {
          m_ranges.emplace_back(std::move(in_range));
          return *this;
     }

     FutureOfFutureConsumer<range_t> &operator+=(FutureOfFutureConsumer<range_t> &&other)
     {
          std::ranges::move(other.m_ranges, std::back_inserter(m_ranges));
          other.m_ranges.clear();// optionally clear the moved-from consumer
          return *this;
     }

     const FutureOfFutureConsumer<range_t> &operator++()
     {
          if (!done())
          {
               compact_front();
               auto      &range = m_ranges.front();
               auto      &item  = *range;
               const auto state = item.wait_for(std::chrono::seconds(0));
               if (std::future_status::ready != state && std::future_status::deferred != state)
               {
                    // launch policy is async and the future is not yet ready, so skip it
                    return *this;
               }
               if (item.valid())
               {
                    m_out.push_back(item.get());
               }
               ++range;
          }
          return *this;
     }

     [[nodiscard]] bool done() const
     {
          return std::ranges::all_of(m_ranges, [](const auto &range) { return range.done(); });// && std::empty(m_out);
     }

     [[nodiscard]] bool consumer_ready() const
     {
          return !std::ranges::empty(m_out);
     }

     [[nodiscard]] FutureConsumer<std::vector<future_value_t>> get_consumer()
     {
          return FutureConsumer{ std::move(m_out) };
     }

     [[nodiscard]] FutureOfFutureConsumer<std::vector<future_value_t>> get_future_of_future_consumer()
     {
          return FutureOfFutureConsumer{ std::move(m_out) };
     }
     ~FutureOfFutureConsumer()
     {
          consume_now();
     }
};
#endif// FIELD_MAP_EDITOR_RANGECONSUMER_HPP
