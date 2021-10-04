//
// Created by pcvii on 10/4/2021.
//

#ifndef MYPROJECT_FILTER_HPP
#define MYPROJECT_FILTER_HPP
#include "open_viii/graphics/BPPT.hpp"
#include <cstdint>
#include <utility>
template<typename T>
struct filter
{
private:
  T    m_value   = {};
  bool m_enabled = { false };

public:
  filter() = default;
  explicit filter(T value, bool enabled = false)
    : m_value(std::move(value)), m_enabled(enabled)
  {}
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
  explicit operator T() const { return m_value; }
};
struct filters
{
  filter<std::uint8_t>              palette = {};
  filter<open_viii::graphics::BPPT> bpp     = []() {
    using namespace open_viii::graphics::literals;
    return filter<open_viii::graphics::BPPT>(4_bpp);
  }();
};
#endif// MYPROJECT_FILTER_HPP
