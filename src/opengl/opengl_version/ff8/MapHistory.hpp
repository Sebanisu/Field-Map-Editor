//
// Created by pcvii on 6/2/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPHISTORY_HPP
#define FIELD_MAP_EDITOR_MAPHISTORY_HPP
#include "MouseToTilePos.h"
#include "SimilarAdjustments.hpp"
#include <ScopeGuard.hpp>
namespace ff_8
{
// template<typename TileT>
// struct [[nodiscard]] PairOfTiles
//{
//   PairOfTiles() = default;
//   PairOfTiles(const TileT &front, TileT &back)
//     : m_front_tile(&front)
//     , m_back_tile(&back)
//   {
//   }
//   [[nodiscard]] const TileT &front_tile() const noexcept
//   {
//     return *m_front_tile;
//   }
//   [[nodiscard]] TileT &back_tile() noexcept
//   {
//     return *m_back_tile;
//   }
//   [[nodiscard]] const TileT &back_tile() const noexcept
//   {
//     return *m_back_tile;
//   }
//
// private:
//   const TileT *m_front_tile;
//   TileT       *m_back_tile;
// };
class [[nodiscard]] MapHistory
{
  enum class Pushed : std::uint8_t
  {
    Front,
    Back
  };
  using MapT                          = open_viii::graphics::background::Map;
  mutable MapT                m_front = {};
  mutable MapT                m_back  = {};
  mutable std::vector<MapT>   m_front_history      = {};
  mutable std::vector<MapT>   m_back_history       = {};
  mutable std::vector<MapT>   m_redo_history       = {};
  mutable std::vector<Pushed> m_front_or_back      = {};
  mutable std::vector<Pushed> m_redo_front_or_back = {};
  // mutable std::vector<std::string> m_changes       = {};
  mutable bool                preemptive_copy_mode = false;
  /**
   * Should only be called by undo() pops back
   * @return returns new back
   */
  [[nodiscard]] MapT         &redo_back() const
  {
    m_back_history.emplace_back(std::move(m_back));
    m_back = std::move(m_redo_history.back());
    m_redo_history.pop_back();
    return back();
  }
  /**
   * Should only be called by undo() pops front
   * @return returns new front
   */
  [[nodiscard]] const MapT &redo_front() const
  {
    m_front_history.emplace_back(std::move(m_front));
    m_front = std::move(m_redo_history.back());
    m_redo_history.pop_back();
    return front();
  }
  /**
   * Should only be called by undo() pops back
   * @return returns new back
   */
  [[nodiscard]] MapT &undo_back() const
  {
    m_redo_history.emplace_back(std::move(m_back));
    m_back = std::move(m_back_history.back());
    m_back_history.pop_back();
    return back();
  }
  /**
   * Should only be called by undo() pops front
   * @return returns new front
   */
  [[nodiscard]] const MapT &undo_front() const
  {
    m_redo_history.emplace_back(std::move(m_front));
    m_front = std::move(m_front_history.back());
    m_front_history.pop_back();
    return front();
  }
  const auto debug_count_print(
    std::source_location source_location =
      std::source_location::current()) const
  {
    return glengine::ScopeGuardCaptures([=, this]() {
      spdlog::debug(
        "Map History Count: {}\n\t{}:{}",
        m_back_history.size() + m_front_history.size() + 2U,
        source_location.file_name(),
        source_location.line());
    });
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

        //        spdlog::debug(
        //          "{}:{} pos in front to be 0 < {} < {} ",
        //          __FILE__,
        //          __LINE__,
        //          pos,
        //          std::ranges::size(tiles));

        if (pos < 0 || std::cmp_greater_equal(pos, std::ranges::size(tiles)))
        {
          spdlog::error(
            "{}:{} pos in front to be 0 < {} < {} ",
            __FILE__,
            __LINE__,
            pos,
            std::ranges::size(tiles));
          throw std::exception();
          //          if constexpr (!requires(TileT tile_t) {
          //                           {
          //                             lambda(tile_t)
          //                             } -> std::same_as<void>;
          //                         })
          //          {
          //            return typename std::remove_cvref_t<
          //              std::invoke_result_t<decltype(lambda), TileT>>{};
          //          }
          //          else
          //          {
          //            return;
          //          }
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
  template<typename TileT, std::integral PosT, typename LambdaT>
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
          return typename std::remove_cvref_t<
            std::invoke_result_t<decltype(lambda), TileT>>{};
        }
      }
    });
  }

public:
  MapHistory() = default;
  explicit MapHistory(MapT map)
    : m_front(std::move(map))
    , m_back(m_front)
  {
  }
  [[nodiscard]] std::size_t count() const
  {
    return m_front_history.size() + m_back_history.size();
  }
  [[nodiscard]] std::size_t redo_count() const
  {
    return m_redo_history.size();
  }
  [[nodiscard]] const MapT &front() const
  {
    return m_front;
  }
  [[nodiscard]] MapT &back() const
  {
    return m_back;
  }
  template<typename TileT, typename LambdaT>
  auto copy_back_and_get_new_tile(const TileT &tile, LambdaT &&lambda) const
  {
    const auto pos = get_offset_from_back(tile);
    (void)copy_back();
    return back_get_tile_at_offset<TileT>(pos, std::forward<LambdaT>(lambda));
  }
  template<typename TileT, typename FilterLambdaT, typename LambdaT>
  void
    copy_back_perform_operation(FilterLambdaT &&filter, LambdaT &&lambda) const
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
  void copy_back_perform_operation(
    const TileT              &tile,
    const SimilarAdjustments &similar,
    LambdaT                 &&lambda) const
  {
    if (similar)
    {
      copy_back_perform_operation<TileT>(
        similar(tile), std::forward<LambdaT>(lambda));
    }
    else
    {
      copy_back_and_get_new_tile<TileT>(tile, std::forward<LambdaT>(lambda));
    }
  }

  template<typename TileT, typename LambdaT>
  auto get_front_version_of_back_tile(const TileT &tile, LambdaT &&lambda) const
  {
    return front_get_tile_at_offset<TileT>(
      get_offset_from_back(tile), std::forward<LambdaT>(lambda));
  }
  //  /**
  //   * For when a change could happen. we make a copy ahead of time.
  //   * @return back map
  //   */
  //  [[nodiscard]] MapT &copy_back_preemptive(
  //    std::source_location source_location =
  //      std::source_location::current()) const
  //  {
  //    if (!preemptive_copy_mode)
  //    {
  //      auto &temp           = copy_back();
  //      preemptive_copy_mode = true;
  //      spdlog::debug(
  //        "Map History preemptive_copy_mode: {}\n\t{}:{}",
  //        preemptive_copy_mode,
  //        source_location.file_name(),
  //        source_location.line());
  //      return temp;
  //    }
  //    return back();
  //  }
  /**
   * After copy_mode is returned to normal copy_back_preemptive will resume
   * making copies.
   */
  void end_preemptive_copy_mode(
    std::source_location source_location =
      std::source_location::current()) const
  {
    if (preemptive_copy_mode)
    {
      preemptive_copy_mode = false;
      spdlog::debug(
        "Map History preemptive_copy_mode: {}\n\t{}:{}",
        preemptive_copy_mode,
        source_location.file_name(),
        source_location.line());
    }
  }
  [[nodiscard]] MapT &copy_back(
    std::source_location source_location =
      std::source_location::current()) const
  {
    const auto count = debug_count_print();
    if (preemptive_copy_mode)
    {// someone already copied
      end_preemptive_copy_mode(source_location);
      return back();
    }
    m_redo_history.clear();
    m_redo_front_or_back.clear();
    m_front_or_back.push_back(Pushed::Back);
    return m_back_history.emplace_back(back());
  }
  //  [[nodiscard]] const MapT &copy_back_to_front() const
  //  {
  //    m_redo_history.clear();
  //    m_redo_front_or_back.clear();
  //    const auto count = debug_count_print();
  //    m_maps.insert(m_maps.begin(), back());
  //    m_front_or_back.push_back(Pushed::Front);
  //    return front();
  //  }
  //  [[nodiscard]] const MapT &copy_front() const
  //  {
  //    m_redo_history.clear();
  //    m_redo_front_or_back.clear();
  //    const auto count = debug_count_print();
  //    m_maps.insert(m_maps.begin(), front());
  //    m_front_or_back.push_back(Pushed::Front);
  //    return front();
  //  }

  /**
   * Deletes the most recent back or front
   * @return
   */
  [[nodiscard]] bool
    redo(std::source_location source_location = std::source_location::current())
      const
  {
    const auto count = debug_count_print(source_location);
    if (!redo_enabled())
    {
      return false;
    }
    Pushed last = m_redo_front_or_back.back();
    m_front_or_back.push_back(last);
    m_redo_front_or_back.pop_back();
    if (last == Pushed::Back)
    {
      (void)redo_back();
      return true;
    }
    (void)redo_front();
    return true;
  }
  /**
   * Deletes the most recent back or front
   * @return
   */
  [[nodiscard]] bool
    undo(std::source_location source_location = std::source_location::current())
      const
  {
    const auto count = debug_count_print(source_location);
    if (!undo_enabled())
    {
      return false;
    }
    Pushed last = m_front_or_back.back();
    m_redo_front_or_back.push_back(last);
    m_front_or_back.pop_back();
    if (last == Pushed::Back)
    {
      (void)undo_back();
      return true;
    }
    (void)undo_front();
    return true;
  }
  void undo_all(
    std::source_location source_location =
      std::source_location::current()) const
  {
    while (undo(source_location))
    {
    }
  }
  void redo_all(
    std::source_location source_location =
      std::source_location::current()) const
  {
    while (redo(source_location))
    {
    }
  }
  [[nodiscard]] bool redo_enabled() const
  {
    return !m_redo_history.empty();
  }
  [[nodiscard]] bool undo_enabled() const
  {
    return count() != 0U;
  }
  //  [[nodiscard]] auto visit_both(auto &&function) const
  //  {
  //    return visit_both(function, std::identity{});
  //  }
  //  [[nodiscard]] auto visit_both(auto &&function, auto &&filter) const
  //  {
  //    return front().visit_tiles(
  //      [this, &function, &filter](
  //        const std::ranges::contiguous_range auto &front_tiles) {
  //        back().visit_tiles([&front_tiles, &function, &filter](
  //                             std::ranges::contiguous_range auto
  //                             &&back_tiles) {
  //          using TileT  =
  //          std::ranges::range_value_t<decltype(front_tiles)>; using BTileT
  //          = std::ranges::range_value_t<decltype(back_tiles)>; if constexpr
  //          (!std::is_same_v<TileT, BTileT>)
  //          {
  //            std::vector<PairOfTiles<TileT>> temp_mux        = {};
  //            auto                            temp_mux_filter =
  //            filter(temp_mux); return function(temp_mux_filter);
  //          }
  //          else
  //          {
  //            std::vector<PairOfTiles<TileT>> temp_mux = {};
  //            temp_mux.reserve(std::ranges::size(front_tiles));
  //            std::ranges::transform(
  //              front_tiles,
  //              back_tiles,
  //              std::back_inserter(temp_mux),
  //              [](const auto &front_tile, auto &back_tile) {
  //                return PairOfTiles<TileT>(front_tile, back_tile);
  //              });
  //            auto temp_mux_filter = filter(temp_mux);
  //            return function(temp_mux_filter);
  //          }
  //        });
  //      });
  //  }
  //  [[nodiscard]] bool visit_both_tiles(
  //    std::invocable auto &&function,
  //    std::invocable auto &&filter = std::identity{})
  //  {
  //    return visit_both(
  //      [&function](std::ranges::contiguous_range auto &&mux_tiles) -> bool
  //      {
  //        bool changed = false;
  //        for (auto &pair : mux_tiles)
  //        {
  //          if (function(std::as_const(pair.front_tile), pair.back_tile))
  //          {
  //            changed = true;
  //          }
  //        }
  //        return changed;
  //      },
  //      filter);
  //  }
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPHISTORY_HPP
