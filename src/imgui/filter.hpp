//
// Created by pcvii on 10/4/2021.
//

#ifndef MYPROJECT_FILTER_HPP
#define MYPROJECT_FILTER_HPP
#include "open_viii/graphics/background/BlendModeT.hpp"
#include "open_viii/graphics/BPPT.hpp"
#include <cstdint>
#include <fmt/format.h>
#include <map>
#include <string>
#include <utility>
#include <vector>
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
  filter<std::uint8_t>                                palette         = {};
  filter<std::uint8_t>                                animation_id    = {};
  filter<std::uint8_t>                                animation_frame = {};
  filter<std::uint8_t>                                layer_id        = {};
  filter<std::uint8_t>                                texture_page_id = {};
  filter<open_viii::graphics::background::BlendModeT> blend_mode      = {};
  filter<open_viii::graphics::BPPT>                   bpp             = []() {
    using namespace open_viii::graphics::literals;
    return filter<open_viii::graphics::BPPT>(4_bpp);
  }();
};
//template<typename T>
//struct unique_values_and_strings
//{
//private:
//  std::vector<T>           m_values{};
//  std::vector<std::string> m_names{};
//
//public:
//  void push_back(T &&val)
//  {
//    m_values.emplace_back(std::move(val));
//  }
//  void sort()
//  {
//
//  }
//  void clone_to_str()
//  {
//    m_names.emplace_back(fmt::format("{}", val));
//  }
//  const auto &values() const { return m_values; }
//  const auto &names() const { return m_names; }
//};
//struct the_unique_values_and_strings
//{
//  unique_values_and_strings<std::uint8_t> animation_id = {};
//  std::map<std::uint8_t, unique_values_and_strings<std::uint8_t>>
//                                          animation_frame = {};
//  unique_values_and_strings<std::uint8_t> layer_id        = {};
//  unique_values_and_strings<std::uint8_t> texture_page_id = {};
//  unique_values_and_strings<open_viii::graphics::background::BlendModeT>
//                                                       blend_mode = {};
//  unique_values_and_strings<open_viii::graphics::BPPT> bpp        = {};
//  std::map<open_viii::graphics::BPPT, unique_values_and_strings<std::uint8_t>>
//    palette = {};
//};
#endif// MYPROJECT_FILTER_HPP
