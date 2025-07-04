//
// Created by pcvii on 5/30/2022.
//
#include "Counter.hpp"
namespace glengine
{
inline namespace impl
{
     Counter::Counter(std::uint32_t forced_value)
       : m_counter(std::move(forced_value))
     {
     }
     Counter::operator std::uint32_t() const noexcept
     {
          return m_counter;
     }
}// namespace impl
}// namespace glengine