//
// Created by pcvii on 6/2/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPHISTORY_HPP
#define FIELD_MAP_EDITOR_MAPHISTORY_HPP
#include "PupuID.hpp"
#include "scope_guard.hpp"
#include "UniquifyPupu.hpp"
#include <open_viii/graphics/background/Map.hpp>
#include <spdlog/spdlog.h>
namespace ff_8
{
class [[nodiscard]] MapHistory
{
  enum class pushed : std::uint8_t
  {
    front,
    back
  };
  using map_t                          = open_viii::graphics::background::Map;
  map_t                m_front         = {};
  map_t                m_back          = {};
  std::vector<PupuID>  m_front_pupu    = {};
  std::vector<map_t>   m_front_history = {};
  std::vector<map_t>   m_back_history  = {};
  std::vector<map_t>   m_redo_history  = {};
  std::vector<pushed>  m_front_or_back = {};
  std::vector<pushed>  m_redo_front_or_back = {};
  mutable bool         preemptive_copy_mode = false;
  [[nodiscard]] map_t &redo_back();
  [[nodiscard]] const map_t &redo_front();
  [[nodiscard]] map_t &undo_back(bool skip_redo = false);
  [[nodiscard]] const map_t &undo_front(bool skip_redo = false);
  auto debug_count_print() const
  {
    return scope_guard([=, this]() {
      spdlog::debug(
        "Map History Count: {}\n\t{}:{}",
        m_back_history.size() + m_front_history.size() + 2U,
        __FILE__,
        __LINE__);
    });
  }
  template<typename TileT, std::integral PosT, typename LambdaT>
  [[nodiscard]] auto
    front_get_tile_at_offset(const PosT pos, LambdaT &&lambda) const
  {
    return front().visit_tiles([&](auto &tiles) {
      if constexpr (std::is_same_v<
                      std::ranges::range_value_t<
                        std::remove_cvref_t<decltype(tiles)>>,
                      TileT>)
      {
        auto front_tile = tiles.cbegin();
        if (pos < 0 || std::cmp_greater_equal(pos, std::ranges::size(tiles)))
        {
          spdlog::error(
            "{}:{} pos in front to be 0 < {} < {} ",
            __FILE__,
            __LINE__,
            pos,
            std::ranges::size(tiles));
          throw std::exception();
        }
        std::ranges::advance(front_tile, pos);
        return lambda(*front_tile);
      }
      else
      {
        if constexpr (!requires(TileT tile_t) {
                         {
                           lambda(tile_t)
                         } -> std::same_as<void>;
                       })
        {
          return typename std::remove_cvref_t<
            std::invoke_result_t<decltype(lambda), TileT>>{};
        }
      }
    });
  }
  template<
    open_viii::graphics::background::is_tile TileT,
    std::integral                            PosT,
    typename LambdaT>
  [[nodiscard]] auto
    back_get_tile_at_offset(const PosT pos, LambdaT &&lambda) const
  {
    return back().visit_tiles([&](auto &tiles) {
      if constexpr (std::is_same_v<
                      std::ranges::range_value_t<
                        std::remove_cvref_t<decltype(tiles)>>,
                      TileT>)
      {
        auto tile = tiles.begin();
        std::ranges::advance(tile, pos);
        return lambda(*tile);
      }
      else
      {
        if constexpr (!requires(TileT tile_t) {
                         {
                           lambda(tile_t)
                         } -> std::same_as<void>;
                       })
        {
          TileT v{};
          using t = std::remove_cvref_t<decltype(lambda(v))>;
          return t{};
        }
      }
    });
  }
public:
  MapHistory() = default;
  explicit MapHistory(map_t map);
  const map_t               &front() const;
  const map_t               &const_back() const;
  const map_t               &back() const;
  map_t                     &back();
  map_t                     &safe_copy_back();
  map_t                     &copy_back();
  const map_t               &copy_back_to_front();
  bool                       undo_enabled() const;
  bool                       redo_enabled() const;
  void                       redo_all();
  void                       undo_all();
  bool                       redo();
  bool                       undo(bool skip_redo = false);
  bool                       remove_duplicate();
  void                       end_preemptive_copy_mode() const;
  map_t                     &copy_back_preemptive();
  size_t                     count() const;
  const std::vector<PupuID> &pupu() const noexcept;
  size_t                     redo_count() const;
  void                       clear_redo();

  template<typename TileT>
  [[nodiscard]] PupuID get_pupu_from_back(const TileT &tile) const
  {
    return m_front_pupu[static_cast<std::size_t>(get_offset_from_back(tile))];
  }
  template<typename TileT>
  [[nodiscard]] auto get_offset_from_back(const TileT &tile) const
  {
    return back().visit_tiles([&](const auto &tiles) {
      if constexpr (std::is_same_v<
                      std::ranges::range_value_t<
                        std::remove_cvref_t<decltype(tiles)>>,
                      TileT>)
      {
        return std::ranges::distance(&tiles.front(), &tile);
      }
      else
      {
        return std::ranges::range_difference_t<
          std::remove_cvref_t<decltype(tiles)>>{};
      }
    });
  }
  template<typename TileT, typename LambdaT>
  auto copy_back_and_get_new_tile(const TileT &tile, LambdaT &&lambda)
  {
    const auto pos = get_offset_from_back(tile);
    (void)copy_back();
    return back_get_tile_at_offset<TileT>(pos, std::forward<LambdaT>(lambda));
  }
  template<typename TileT, typename LambdaT>
  void copy_back_perform_operation(
    const std::vector<std::intmax_t> &indexes,
    LambdaT                         &&lambda)
  {
    (void)copy_back();
    for (const auto i : indexes)
    {
      back_get_tile_at_offset<TileT>(i, lambda);
    }
  }
  template<typename TileT, typename FilterLambdaT, typename LambdaT>
    requires(std::is_invocable_r_v<bool, FilterLambdaT, const TileT &>)
  void copy_back_perform_operation(FilterLambdaT &&filter, LambdaT &&lambda)
  {
    (void)copy_back();
    back().visit_tiles([&](auto &tiles) {
      if constexpr (std::is_same_v<
                      std::ranges::range_value_t<
                        std::remove_cvref_t<decltype(tiles)>>,
                      TileT>)
      {
        auto filtered_tiles = tiles | std::views::filter(filter);
        for (auto &tile : filtered_tiles)
        {
          lambda(tile);
        }
      }
    });
  }
  template<typename TileT, typename LambdaT>
  auto get_front_version_of_back_tile(const TileT &tile, LambdaT &&lambda) const
  {
    return front_get_tile_at_offset<TileT>(
      get_offset_from_back(tile), std::forward<LambdaT>(lambda));
  }
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPHISTORY_HPP
