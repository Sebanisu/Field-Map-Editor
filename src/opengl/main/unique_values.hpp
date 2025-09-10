//
// Created by pcvii on 10/5/2021.
//

#ifndef FIELD_MAP_EDITOR_UNIQUE_VALUES_HPP
#define FIELD_MAP_EDITOR_UNIQUE_VALUES_HPP
#include "formatters.hpp"
#include "open_viii/graphics/background/BlendModeT.hpp"
#include "open_viii/graphics/BPPT.hpp"
#include "PupuID.hpp"
#include "tile_operations.hpp"
#include "UniquifyPupu.hpp"
#include <concepts>
#include <cstdint>
#include <fmt/format.h>
#include <map>
#include <ranges>
#include <string>
#include <utility>
#include <vector>
namespace ff_8
{
template<typename T>
struct unique_values_and_strings
{
   private:
     static constexpr auto default_filter_lambda = [](auto &&) { return true; };
     static constexpr auto kite                  = [](auto &&, auto &&ret) { return std::forward<decltype(ret)>(ret); };

   public:
     using value_type            = T;
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
       : m_values(second_pass(
           tiles,
           get_values(
             tiles,
             lambda,
             sort,
             filter)))
       , m_strings(get_strings(m_values))
     {
     }
     explicit unique_values_and_strings(
       std::vector<value_type>  in_values,
       std::vector<std::string> in_strings)
       : m_values(std::move(in_values))
       , m_strings(std::move(in_strings))
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

     [[nodiscard]] auto zip() const
     {
          return std::views::zip(m_values, m_strings);
     }

   private:
     std::vector<value_type>                m_values{};
     std::vector<std::string>               m_strings{};

     [[nodiscard]] std::vector<std::string> get_strings(const std::vector<T> &data) const
     {
          std::vector<std::string> vector;
          vector.reserve(std::size(data));
          std::ranges::transform(data, std::back_inserter(vector), [](const T &t_value) { return fmt::format("{}", t_value); });
          return vector;
     }
     template<
       typename tilesT,
       typename lambdaT,
       typename sortT,
       typename filterT>
     [[nodiscard]] std::vector<T> get_values(
       const tilesT &tiles,
       lambdaT     &&lambda,
       sortT       &&sort,
       filterT     &&filter) const
     {
          std::vector<T> ret{};
          if (std::ranges::empty(tiles))
          {
               return ret;
          }
          using namespace open_viii::graphics::background;
          auto filtered = tiles | Map::filter_view_invalid();
          std::ranges::transform(filtered | std::views::filter(filter), std::back_inserter(ret), lambda);
          std::ranges::sort(ret, sort);
          auto last = std::ranges::unique(ret.begin(), ret.end());
          ret.erase(last.begin(), last.end());

          return ret;
     }
};
// Concept to check for values(), strings(), and zip()
template<typename T>
concept HasValuesAndStringsAndZip = requires(T obj) {
     { obj.values() } -> std::ranges::range; // std::convertible_to<const std::vector<typename std::remove_cvref_t<T>::value_type> &>;
     { obj.strings() } -> std::ranges::range;// std::convertible_to<const std::vector<std::string> &>;
     { obj.zip() } -> std::ranges::range;
};

struct all_unique_values_and_strings
{
   public:
     all_unique_values_and_strings() = default;
     explicit all_unique_values_and_strings(std::monostate /*unused*/) {}
     template<std::ranges::range tilesT>
     explicit all_unique_values_and_strings(const tilesT &tiles)
       : m_z(
           tiles,
           ff_8::tile_operations::Z{},
           std::greater<>{})
       , m_layer_id(
           tiles,
           ff_8::tile_operations::LayerId{})
       , m_texture_page_id(
           tiles,
           ff_8::tile_operations::TextureId{})
       , m_animation_id(
           tiles,
           ff_8::tile_operations::AnimationId{})
       , m_blend_other(
           tiles,
           ff_8::tile_operations::Blend{})
       , m_blend_mode(
           tiles,
           ff_8::tile_operations::BlendMode{})
       , m_bpp(
           tiles,
           ff_8::tile_operations::Depth{})
       , m_animation_frame(
           get_map<
             std::uint8_t,
             std::uint8_t>(
             tiles,
             m_animation_id,
             ff_8::tile_operations::AnimationState{},
             {},
             [](
               const auto  key,
               const auto &tile) -> bool { return ff_8::tile_operations::AnimationIdMatch{ key } == tile; }))
       , m_palette(
           get_map<
             std::uint8_t,
             open_viii::graphics::BPPT>(
             tiles,
             m_bpp,
             ff_8::tile_operations::PaletteId{},
             {},
             [](
               const auto  key,
               const auto &tile) -> bool { return ff_8::tile_operations::DepthMatch{ tile } == key; }))
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
     [[nodiscard]] const auto &animation_state() const
     {
          return m_animation_frame;
     }
     [[nodiscard]] const auto &palette() const
     {
          return m_palette;
     }

   private:
     unique_values_and_strings<std::uint16_t>                                     m_z                   = {};
     unique_values_and_strings<std::uint8_t>                                      m_layer_id            = {};
     unique_values_and_strings<std::uint8_t>                                      m_texture_page_id     = {};
     unique_values_and_strings<std::uint8_t>                                      m_animation_id        = {};
     unique_values_and_strings<std::uint8_t>                                      m_blend_other         = {};
     unique_values_and_strings<open_viii::graphics::background::BlendModeT>       m_blend_mode          = {};
     unique_values_and_strings<open_viii::graphics::BPPT>                         m_bpp                 = {};
     std::map<std::uint8_t, unique_values_and_strings<std::uint8_t>>              m_animation_frame     = {};
     std::map<open_viii::graphics::BPPT, unique_values_and_strings<std::uint8_t>> m_palette             = {};

     static constexpr auto                                                        default_filter_lambda = [](auto &&) { return true; };
     template<
       typename valT,
       typename keyT,
       typename tilesT,
       typename lambdaT,
       typename sortT   = std::less<>,
       typename filterT = decltype(default_filter_lambda)>
     static std::map<
       keyT,
       unique_values_and_strings<valT>>
       get_map(
         const tilesT                          &tiles,
         const unique_values_and_strings<keyT> &keys,
         lambdaT                              &&lambda,
         sortT                                &&sort   = {},
         filterT                              &&filter = {})
     {
          std::map<keyT, unique_values_and_strings<valT>> ret = {};
          for (const auto &key : keys.values())
          {
               ret.emplace(key, unique_values_and_strings<valT>(tiles, lambda, sort, [&key, &filter](const auto &tile) {
                                return filter(key, tile);
                           }));
          }
          return ret;
     }
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_UNIQUE_VALUES_HPP
