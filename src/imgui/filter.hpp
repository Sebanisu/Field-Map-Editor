//
// Created by pcvii on 10/4/2021.
//

#ifndef MYPROJECT_FILTER_HPP
#define MYPROJECT_FILTER_HPP
#include <cstdint>
#include <utility>
template<typename T>
struct filter
{
private:
  T    m_value   = {};
  bool m_enabled = { false };

public:
  template<typename U>
  filter &update(U &&value)
  {
    m_value = std::forward<U>(value);
    return *this;
  }
  [[nodiscard]] const T    &value() const { return m_value; }
  [[nodiscard]] const bool &enabled() const { return m_enabled; }
  filter                   &enable()
  {
    m_enabled = true;
    return *this;
  }
  filter &disable()
  {
    m_enabled = false;
    return *this;
  }
  explicit operator T() const
  {
    return m_value;
  }
};
struct filters
{
  filter<std::uint8_t> palette;
};
#endif// MYPROJECT_FILTER_HPP
