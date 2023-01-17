//
// Created by pcvii on 10/5/2021.
//

#ifndef FIELD_MAP_EDITOR_UNIQUE_VALUES_HPP
#define FIELD_MAP_EDITOR_UNIQUE_VALUES_HPP
#include "open_viii/graphics/background/BlendModeT.hpp"
#include "open_viii/graphics/BPPT.hpp"
#include "PupuID.hpp"
#include <concepts>
#include <cstdint>
#include <fmt/format.h>
#include <map>
#include <ranges>
#include <string>
#include <utility>
#include <vector>
#include "UniquifyPupu.hpp"
template<typename T>
struct unique_values_and_strings
{
private:
  static constexpr auto default_filter_lambda = [](auto &&) { return true; };
  static constexpr auto kite                  = [](auto &&, auto &&ret) {
    return std::forward<decltype(ret)>(ret);
  };

public:
  unique_values_and_strings() = default;
  template<
    std::ranges::range tilesT,
    typename lambdaT,
    typename sortT   = std::less<>,
    typename filterT = decltype(default_filter_lambda),
    typename secondT = decltype(kite)>
  explicit unique_values_and_strings(
    const tilesT &tiles,
    lambdaT     &&lambda,
    sortT       &&sort        = {},
    filterT     &&filter      = {},
    secondT     &&second_pass = {})
    : m_values(second_pass(tiles, get_values(tiles, lambda, sort, filter)))
    , m_strings(get_strings(m_values))
  {
  }
  [[nodiscard]] const auto &values() const
  {
    return m_values;
  }
  [[nodiscard]] const auto &strings() const
  {
    return m_strings;
  }

private:
  std::vector<T>           m_values{};
  std::vector<std::string> m_strings{};

  [[nodiscard]] std::vector<std::string>
    get_strings(const std::vector<T> &data) const
  {
    using namespace open_viii::graphics::background;
    using namespace open_viii::graphics;
    using namespace open_viii::graphics::literals;
    using namespace std::string_literals;
    std::vector<std::string> vector;
    vector.reserve(std::size(data));
    std::ranges::transform(data, std::back_inserter(vector), [](const T &t) {
      if constexpr (std::is_same_v<T, BlendModeT>)
      {
        switch (t)
        {
          case BlendModeT::quarter_add:
            return "quarter add"s;
          case BlendModeT::half_add:
            return "half add"s;
          case BlendModeT::add:
            return "add"s;
          default:
          case BlendModeT::none:
            return "none"s;
          case BlendModeT::subtract:
            return "subtract"s;
        }
      }
      else if constexpr (std::is_same_v<T, BPPT>)
      {
        if (t.bpp8())
          return "8"s;
        if (t.bpp16())
          return "16"s;
        return "4"s;
      }
      else
      {
        return fmt::format("{}", t);
      }
    });
    return vector;
  }
  template<typename tilesT, typename lambdaT, typename sortT, typename filterT>
  [[nodiscard]] std::vector<T> get_values(
    const tilesT &tiles,
    lambdaT     &&lambda,
    sortT       &&sort,
    filterT     &&filter) const
  {
    std::vector<T> ret{};
    if (!std::ranges::empty(tiles))
    {
      static constexpr auto filter_invalid0 =
        open_viii::graphics::background::Map::filter_invalid();
      std::ranges::transform(
        tiles | std::views::filter(filter_invalid0)
          | std::views::filter(filter),
        std::back_inserter(ret),
        lambda);
      std::ranges::sort(ret, sort);
      auto last = std::unique(ret.begin(), ret.end());
      ret.erase(last, ret.end());
    }
    return ret;
  }
};
struct all_unique_values_and_strings
{
public:
  all_unique_values_and_strings() = default;
  explicit all_unique_values_and_strings(std::monostate) {}
  template<std::ranges::range tilesT>
  explicit all_unique_values_and_strings(const tilesT &tiles)
    : m_pupu(
      tiles,
      m_pupu_map,
      [](const PupuID &left, const PupuID &right) {
        return left.raw() < right.raw();
      })
    , m_z(
        tiles,
        [](const auto &tile) { return tile.z(); },
        std::greater<>{})
    , m_layer_id(tiles, [](const auto &tile) { return tile.layer_id(); })
    , m_texture_page_id(
        tiles,
        [](const auto &tile) { return tile.texture_id(); })
    , m_animation_id(
        tiles,
        [](const auto &tile) { return tile.animation_id(); })
    , m_blend_other(tiles, [](const auto &tile) { return tile.blend(); })
    , m_blend_mode(tiles, [](const auto &tile) { return tile.blend_mode(); })
    , m_bpp(tiles, [](const auto &tile) { return tile.depth(); })
    , m_animation_frame(get_map<std::uint8_t, std::uint8_t>(
        tiles,
        m_animation_id,
        [](const auto &tile) { return tile.animation_state(); },
        {},
        [](const auto key, const auto &tile) {
          return key == tile.animation_id();
        }))
    , m_palette(get_map<std::uint8_t, open_viii::graphics::BPPT>(
        tiles,
        m_bpp,
        [](const auto &tile) { return tile.palette_id(); },
        {},
        [](const auto key, const auto &tile) { return key == tile.depth(); }))
  {
  }
  [[nodiscard]] const auto &z() const
  {
    return m_z;
  }
  [[nodiscard]] const auto &layer_id() const
  {
    return m_layer_id;
  }
  [[nodiscard]] const auto &texture_page_id() const
  {
    return m_texture_page_id;
  }
  [[nodiscard]] const auto &animation_id() const
  {
    return m_animation_id;
  }
  [[nodiscard]] const auto &blend_other() const
  {
    return m_blend_other;
  }
  [[nodiscard]] const auto &blend_mode() const
  {
    return m_blend_mode;
  }
  [[nodiscard]] const auto &bpp() const
  {
    return m_bpp;
  }
  [[nodiscard]] const auto &animation_frame() const
  {
    return m_animation_frame;
  }
  [[nodiscard]] const auto &palette() const
  {
    return m_palette;
  }
  [[nodiscard]] const auto &pupu() const
  {
    return m_pupu;
  }

private:
  UniquifyPupu                             m_pupu_map        = {};
  unique_values_and_strings<PupuID>        m_pupu            = {};
  unique_values_and_strings<std::uint16_t> m_z               = {};
  unique_values_and_strings<std::uint8_t>  m_layer_id        = {};
  unique_values_and_strings<std::uint8_t>  m_texture_page_id = {};
  unique_values_and_strings<std::uint8_t>  m_animation_id    = {};
  unique_values_and_strings<std::uint8_t>  m_blend_other     = {};
  unique_values_and_strings<open_viii::graphics::background::BlendModeT>
                                                       m_blend_mode = {};
  unique_values_and_strings<open_viii::graphics::BPPT> m_bpp        = {};
  std::map<std::uint8_t, unique_values_and_strings<std::uint8_t>>
    m_animation_frame = {};
  std::map<open_viii::graphics::BPPT, unique_values_and_strings<std::uint8_t>>
                        m_palette             = {};

  static constexpr auto default_filter_lambda = [](auto &&) { return true; };
  template<
    typename valT,
    typename keyT,
    typename tilesT,
    typename lambdaT,
    typename sortT   = std::less<>,
    typename filterT = decltype(default_filter_lambda)>
  static std::map<keyT, unique_values_and_strings<valT>> get_map(
    const tilesT                          &tiles,
    const unique_values_and_strings<keyT> &keys,
    lambdaT                              &&lambda,
    sortT                                &&sort   = {},
    filterT                              &&filter = {})
  {
    std::map<keyT, unique_values_and_strings<valT>> ret = {};
    for (const auto &key : keys.values())
    {
      ret.emplace(
        key,
        unique_values_and_strings<valT>(
          tiles, lambda, sort, [&key, &filter](const auto &tile) {
            return filter(key, tile);
          }));
    }
    return ret;
  }
};
#endif// FIELD_MAP_EDITOR_UNIQUE_VALUES_HPP
