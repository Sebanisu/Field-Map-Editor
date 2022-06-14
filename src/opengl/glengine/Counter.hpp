//
// Created by pcvii on 5/30/2022.
//

#ifndef FIELD_MAP_EDITOR_COUNTER_HPP
#define FIELD_MAP_EDITOR_COUNTER_HPP
#include <cstdint>
#include <compare>
namespace glengine
{
inline namespace impl
{
  /**
   * The Counter class is assigned a number automatically on creation.
   * And it increments a static counter.
   */
  class Counter
  {
  public:
    Counter() = default;
    explicit Counter(std::uint32_t);
    explicit operator std::uint32_t() const noexcept;
    auto operator<=>(const Counter&) const = default;

  private:
    std::uint32_t                         m_counter = ++s_counter;
    static inline constinit std::uint32_t s_counter = {};
  };
}// namespace impl
}// namespace glengine
#endif// FIELD_MAP_EDITOR_COUNTER_HPP
