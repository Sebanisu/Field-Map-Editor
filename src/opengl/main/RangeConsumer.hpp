//
// Created by pcvii on 3/1/2023.
//

#ifndef FIELD_MAP_EDITOR_RANGECONSUMER_HPP
#define FIELD_MAP_EDITOR_RANGECONSUMER_HPP
#include <cstddef>
#include <future>
#include <iterator>
#include <ranges>
#include <spdlog/spdlog.h>
#include <stacktrace>
#include <utility>
#include <vector>

template<std::ranges::input_range range_t>
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
     explicit RangeConsumer(value_t in_value)
       : RangeConsumer(
           [&]
           {
                range_t ret{};
                ret.emplace_back(std::move(in_value));
                return ret;
           }())
     {
     }
     void restart()
          requires(std::ranges::forward_range<range_t>)
     {
          pos     = std::ranges::begin(m_range);
          end_pos = std::ranges::end(m_range);
          m_stop  = false;
     }
     RangeConsumer<range_t> &operator=(range_t new_value)
     {
          m_range = std::move(new_value);
          pos     = std::ranges::begin(m_range);
          end_pos = std::ranges::end(m_range);
          m_stop  = false;
          return *this;
     }
     RangeConsumer<range_t> &operator+=(range_t &&new_value)
          requires(std::ranges::forward_range<range_t> && requires(range_t t) {
               std::back_inserter(t);
          })
     {
          const auto offset
            = std::ranges::distance(std::ranges::begin(m_range), pos);
          m_range = std::ranges::move(new_value, std::back_inserter(m_range));
          if constexpr (requires(range_t t) { t.clear(); })
          {
               new_value.clear();
          }
          restart();
          std::ranges::advance(pos, offset);
          return *this;
     }
     [[nodiscard]] auto distance_from_begin() const
          requires std::ranges::sized_range<range_t>
     {
          return std::ranges::distance(std::ranges::begin(*m_range), pos);
     }
     [[nodiscard]] std::size_t size() const
          requires std::ranges::sized_range<range_t>
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
          requires(std::ranges::input_range<range_t>)
     {
          ++pos;
          return *this;
     }
     decltype(auto) operator*() const
          requires(std::ranges::input_range<range_t>)
     {
          return *pos;
     }
     decltype(auto) operator*()
          requires(std::ranges::input_range<range_t>)
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


template<std::ranges::input_range range_t>
class RangeConsumerView
{
   public:
     using value_t = std::ranges::range_value_t<range_t>;

   private:
     range_t *m_range = nullptr;// Non-owning pointer

   public:
     using begin_t
       = std::remove_cvref_t<decltype(std::ranges::begin(*m_range))>;
     using end_t = std::remove_cvref_t<decltype(std::ranges::end(*m_range))>;

   private:
     begin_t pos{};
     end_t   end_pos{};
     bool    m_stop = false;

   public:
     RangeConsumerView() = default;

     explicit RangeConsumerView(range_t &in_range)
       : m_range(&in_range)
       , pos(std::ranges::begin(in_range))
       , end_pos(std::ranges::end(in_range))
     {
     }

     void reset(range_t &in_range)
     {
          m_range = &in_range;
          if (!m_range)
               return;
          pos     = std::ranges::begin(*m_range);
          end_pos = std::ranges::end(*m_range);
          m_stop  = false;
     }

     void restart()
          requires(std::ranges::forward_range<range_t>)
     {
          if (!m_range)
               return;
          pos     = std::ranges::begin(*m_range);
          end_pos = std::ranges::end(*m_range);
          m_stop  = false;
     }

     [[nodiscard]] std::ranges::range_difference_t<range_t>
       distance_from_begin() const
          requires std::ranges::sized_range<range_t>
     {
          if (!m_range)
               return {};
          return std::ranges::distance(std::ranges::begin(*m_range), pos);
     }

     [[nodiscard]] std::size_t size() const
          requires std::ranges::sized_range<range_t>
     {
          if (!m_range)
               return {};
          return std::ranges::size(*m_range);
     }

     [[nodiscard]] bool empty() const
     {
          return !m_range || std::ranges::empty(*m_range);
     }

     [[nodiscard]] bool done() const
     {
          return m_stop || !m_range || pos == end_pos;
     }

     auto &operator++()
          requires(std::ranges::input_range<range_t>)
     {
          ++pos;
          return *this;
     }

     decltype(auto) operator*() const
          requires(std::ranges::input_range<range_t>)
     {
          return *pos;
     }
     decltype(auto) operator*()
          requires(std::ranges::input_range<range_t>)
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
     bool                                m_stop{};

     void                                compact()
     {
          const auto remove_range = std::ranges::remove_if(
            m_ranges, [](const auto &range) { return range.done(); });
          m_ranges.erase(remove_range.begin(), remove_range.end());
     }

     template<typename GetHandler>
     void consume_one(GetHandler &&handler)
     {
          compact();
          if (done())
               return;

          auto &range = m_ranges.front();
          auto &item  = *range;

          try
          {
               if (item.valid())
               {
                    const auto state = item.wait_for(std::chrono::seconds(0));
                    if (
                      state != std::future_status::ready
                      && state != std::future_status::deferred)
                         return;

                    std::invoke(std::forward<GetHandler>(handler), item);
               }
               else
               {
                    spdlog::warn(
                      "Skipping future: no valid state (possibly moved-from or "
                      "default-constructed), ranges_size {}, front_size {}  : "
                      "{}:{}",
                      m_ranges.size(),
                      m_ranges.front().size(),
                      __FILE__,
                      __LINE__);
               }

               ++range;
          }
          catch (const std::future_error &e)
          {
               spdlog::error("Future error in wait_for: {}", e.what());
               spdlog::error("{}", std::stacktrace::current());
          }
          catch (const std::exception &e)
          {
               spdlog::error("Unexpected exception in wait_for: {}", e.what());
               spdlog::error("{}", std::stacktrace::current());
          }
          catch (...)
          {
               spdlog::error("Unknown exception occurred in wait_for");
               spdlog::error("{}", std::stacktrace::current());
          }
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
          m_ranges = std::exchange(other.m_ranges, {});
     }

     FutureConsumer &operator=(FutureConsumer &&other) noexcept
     {
          if (this != &other)
          {
               consume_now();
               m_ranges = std::exchange(other.m_ranges, {});
          }
          return *this;
     }

     void clear()
     {
          m_ranges.clear();// will block until async tasks finish
     }

     void clear_detached()
     {
          std::thread(
            [old = std::move(std::move(m_ranges))]() mutable
            {
                 // let futures destruct asynchronously
                 old.clear();
            })
            .detach();
          m_ranges.clear();
     }

     void stop()
     {
          m_stop = true;
     }

     auto consume_now()
     {
          using result_t = decltype(std::declval<range_value_t>().get());
          if constexpr (std::is_void_v<result_t>)
          {
               for (auto &range : m_ranges)
               {
                    for (; !range.done(); ++range)
                    {
                         auto &item = *range;
                         if (item.valid())
                              item.get();
                    }
               }
          }
          else
          {
               std::vector<result_t> results;
               for (auto &range : m_ranges)
               {
                    for (; !range.done(); ++range)
                    {
                         auto &item = *range;
                         if (item.valid())
                              results.push_back(item.get());
                    }
               }
               return results;
          }
     }

     FutureConsumer<range_t> &operator=(range_t &&new_value)
     {
          consume_now();
          m_ranges.clear();
          m_ranges.emplace_back(std::move(new_value));
          if constexpr (requires(range_t t) { t.clear(); })
          {
               new_value.clear();
          }
          m_stop = false;
          return *this;
     }

     FutureConsumer<range_t> &operator+=(range_value_t &&new_value)
     {
          m_ranges.emplace_back(std::move(new_value));
          m_stop = false;
          return *this;
     }

     FutureConsumer<range_t> &operator+=(range_t &&new_value)
     {
          m_ranges.emplace_back(std::move(new_value));
          if constexpr (requires(range_t t) { t.clear(); })
          {
               new_value.clear();
          }
          m_stop = false;
          return *this;
     }

     FutureConsumer<range_t> &operator+=(FutureConsumer<range_t> &&other)
     {
          std::ranges::move(other.m_ranges, std::back_inserter(m_ranges));
          if constexpr (requires(FutureConsumer<range_t> t) {
                             t.m_ranges.clear();
                        })
          {
               other.m_ranges.clear();
          }
          m_stop = false;
          return *this;
     }

     const FutureConsumer<range_t> &operator++()
          requires(
            std::is_void_v<decltype(std::declval<range_value_t>().get())>)
     {
          consume_one([](auto &fut) { fut.get(); });
          return *this;
     }

     template<typename Callback = std::nullptr_t>
     void consume_one_with_callback(Callback &&callback = nullptr)
     {
          consume_one(
            [&callback](auto &fut)
            {
                 if constexpr (std::is_void_v<
                                 decltype(std::declval<range_value_t>().get())>)
                 {
                      fut.get();
                      if constexpr (!std::is_same_v<Callback, std::nullptr_t>)
                      {
                           std::invoke(callback);
                      }
                 }
                 else
                 {
                      if constexpr (!std::is_same_v<Callback, std::nullptr_t>)
                      {
                           std::invoke(callback, fut.get());
                      }
                      else
                      {
                           std::ignore = fut.get();
                      }
                 }
            });
     }


     [[nodiscard]] bool done() const
     {
          return m_stop
                 || std::ranges::all_of(
                   m_ranges, [](const auto &range) { return range.done(); });
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
     using range_value_t = std::ranges::range_value_t<range_t>;
     using future_value_t
       = std::remove_cvref_t<decltype(range_value_t{}.get())>;

     std::vector<RangeConsumer<range_t>> m_ranges{};
     std::vector<future_value_t>         m_out{};

     void                                compact()
     {
          const auto remove_range = std::ranges::remove_if(
            m_ranges, [](const auto &range) { return range.done(); });
          m_ranges.erase(remove_range.begin(), remove_range.end());
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
          consume_now();
          m_ranges = std::exchange(other.m_ranges, {});
          m_out    = std::exchange(other.m_out, {});
     }

     FutureOfFutureConsumer &operator=(FutureOfFutureConsumer &&other) noexcept
     {
          if (this != &other)
          {
               consume_now();
               m_ranges = std::exchange(other.m_ranges, {});
               m_out    = std::exchange(other.m_out, {});
          }
          return *this;
     }

     void consume_now()
     {
          for (auto &&range : m_ranges)
          {
               for (; !range.done(); ++range)
               {
                    range_value_t &item = *range;
                    if (item.valid())
                    {
                         auto inner = item.get();
                         if (inner.valid())
                         {
                              m_out.emplace_back(std::move(inner));
                         }
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
          m_out.reserve(std::ranges::size(m_ranges.front()));
          return *this;
     }

     FutureOfFutureConsumer<range_t> &operator+=(range_value_t in_value)
     {
          m_ranges.emplace_back(std::move(in_value));
          return *this;
     }

     FutureOfFutureConsumer<range_t> &operator+=(range_t in_range)
     {
          m_ranges.emplace_back(std::move(in_range));
          return *this;
     }

     FutureOfFutureConsumer<range_t> &
       operator+=(FutureOfFutureConsumer<range_t> &&other)
     {
          std::ranges::move(other.m_ranges, std::back_inserter(m_ranges));
          std::ranges::move(other.m_out, std::back_inserter(m_out));
          if constexpr (requires(FutureConsumer<range_t> t) {
                             t.m_ranges.clear();
                             t.m_out.clear();
                        })
          {
               other.m_ranges.clear();
               other.m_out.clear();
          }
          return *this;
     }

     const FutureOfFutureConsumer<range_t> &operator++()
     {
          compact();
          if (!done())
          {
               RangeConsumer<range_t> &range = m_ranges.front();
               range_value_t          &item  = *range;
               try
               {
                    if (item.valid())
                    {
                         const auto state
                           = item.wait_for(std::chrono::seconds(0));
                         if (
                           state != std::future_status::ready
                           && state != std::future_status::deferred)
                         {
                              // launch policy is async and the future is not
                              // yet ready, so skip it
                              return *this;
                         }

                         auto inner = item.get();
                         if (inner.valid())
                         {
                              m_out.emplace_back(std::move(inner));
                         }
                    }
                    else
                    {
                         spdlog::warn(
                           "Skipping future: no valid state (possibly "
                           "moved-from or default-constructed), {}:{}",
                           __FILE__,
                           __LINE__);
                    }
                    ++range;
               }
               catch (const std::future_error &e)
               {
                    spdlog::error("Future error in wait_for: {}", e.what());
                    spdlog::error("{}", std::stacktrace::current());
               }
               catch (const std::exception &e)
               {
                    spdlog::error(
                      "Unexpected exception in wait_for: {}", e.what());
                    spdlog::error("{}", std::stacktrace::current());
               }
               catch (...)
               {
                    spdlog::error("Unknown exception occurred in wait_for");
                    spdlog::error("{}", std::stacktrace::current());
               }
          }
          return *this;
     }

     [[nodiscard]] bool done() const
     {
          return std::ranges::all_of(
            m_ranges, [](const auto &range) { return range.done(); });
     }

     [[nodiscard]] bool consumer_ready() const
     {
          return !std::ranges::empty(m_out);
     }

     [[nodiscard]] FutureConsumer<std::vector<future_value_t>> get_consumer()
     {
          return FutureConsumer{ std::exchange(m_out, {}) };
     }

     // [[nodiscard]] FutureOfFutureConsumer<std::vector<future_value_t>>
     // get_future_of_future_consumer()
     // {
     //      return FutureOfFutureConsumer{ std::exchange(m_out, {}) };
     // }
     ~FutureOfFutureConsumer()
     {
          consume_now();
     }
};
#endif// FIELD_MAP_EDITOR_RANGECONSUMER_HPP
