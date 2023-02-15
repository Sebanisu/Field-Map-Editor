//
// Created by pcvii on 10/4/2021.
//

#ifndef FIELD_MAP_EDITOR_FILTER_HPP
#define FIELD_MAP_EDITOR_FILTER_HPP
#include "open_viii/graphics/background/BlendModeT.hpp"
#include "open_viii/graphics/BPPT.hpp"
#include "PupuID.hpp"
#include <cstdint>
#include <filesystem>
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
    : m_value(std::move(value))
    , m_enabled(enabled)
  {
  }
  template<typename U>
  filter &update(U &&value)
  {
    m_value = std::forward<U>(value);
    return *this;
  }
  [[nodiscard]] const T &value() const
  {
    return m_value;
  }
  [[nodiscard]] const bool &enabled() const
  {
    return m_enabled;
  }
  filter &enable()
  {
    m_enabled = true;
    return *this;
  }
  filter &disable()
  {
    m_enabled = false;
    return *this;
  }
  [[nodiscard]] bool operator==(const T &cmp) const
  {
    return m_value == cmp;
  }
  [[nodiscard]] bool operator!=(const T &cmp) const
  {
    return m_value != cmp;
  }
  [[nodiscard]] operator bool() const
  {
    return m_enabled;
  }
  explicit operator T() const
  {
    return m_value;
  }
};
enum struct draw_bitT
{
  all,
  disabled,
  enabled,
};
struct filters
{
  filter<draw_bitT>                                   draw_bit        = {};
  filter<PupuID>                                      pupu            = {};
  filter<std::uint16_t>                               z               = {};
  filter<std::uint8_t>                                palette         = {};
  filter<std::uint8_t>                                animation_id    = {};
  filter<std::uint8_t>                                animation_frame = {};
  filter<std::uint8_t>                                layer_id        = {};
  filter<std::uint8_t>                                texture_page_id = {};
  filter<open_viii::graphics::background::BlendModeT> blend_mode      = {};
  filter<std::uint8_t>                                blend_other     = {};
  filter<open_viii::graphics::BPPT>                   bpp             = []() {
    using namespace open_viii::graphics::literals;
    return filter<open_viii::graphics::BPPT>(4_bpp);
  }();
  filter<std::filesystem::path> upscale   = {};
  filter<std::filesystem::path> deswizzle = {};
};
#endif// FIELD_MAP_EDITOR_FILTER_HPP
