//
// Created by pcvii on 6/2/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPHISTORY_HPP
#define FIELD_MAP_EDITOR_MAPHISTORY_HPP
#include "MouseToTilePos.h"
#include "SimilarAdjustments.hpp"
#include "UniquifyPupu.hpp"
#include <ScopeGuard.hpp>
#include <source_location>
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
     enum class pushed : std::uint8_t
     {
          Front,
          Back
     };
     using map_t                                      = open_viii::graphics::background::Map;
     mutable map_t               m_front              = {};
     mutable map_t               m_back               = {};
     mutable std::vector<PupuID> m_front_pupu         = {};
     mutable std::vector<map_t>  m_front_history      = {};
     mutable std::vector<map_t>  m_back_history       = {};
     mutable std::vector<map_t>  m_redo_history       = {};
     mutable std::vector<pushed> m_front_or_back      = {};
     mutable std::vector<pushed> m_redo_front_or_back = {};
     // mutable std::vector<std::string> m_changes       = {};
     mutable bool                preemptive_copy_mode = false;
     /**
      * Should only be called by undo() pops back
      * @return returns new back
      */
     [[nodiscard]] map_t        &redo_working() const
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
     [[nodiscard]] const map_t &redo_original() const
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
     [[nodiscard]] map_t &undo_working(bool skip_redo = false) const
     {
          if (!skip_redo)
          {
               m_redo_history.emplace_back(std::move(m_back));
          }
          m_back = std::move(m_back_history.back());
          m_back_history.pop_back();
          return back();
     }
     /**
      * Should only be called by undo() pops front
      * @return returns new front
      */
     [[nodiscard]] const map_t &undo_original(bool skip_redo = false) const
     {
          if (!skip_redo)
          {
               m_redo_history.emplace_back(std::move(m_front));
          }
          m_front = std::move(m_front_history.back());
          m_front_history.pop_back();
          return front();
     }
     auto debug_count_print(std::source_location source_location = std::source_location::current()) const
     {
          return glengine::ScopeGuard([=, this]() {
               spdlog::debug(
                 "Map History Count: {}\n\t{}:{}",
                 m_back_history.size() + m_front_history.size() + 2U,
                 source_location.file_name(),
                 source_location.line());
          });
     }


     template<typename TileT, std::integral PosT, typename LambdaT>
     [[nodiscard]] auto original_get_tile_at_offset(const PosT pos, LambdaT &&lambda) const
     {
          return front().visit_tiles([&](auto &tiles) {
               if constexpr (std::is_same_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(tiles)>>, TileT>)
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
                         spdlog::error("{}:{} pos in front to be 0 < {} < {} ", __FILE__, __LINE__, pos, std::ranges::size(tiles));
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
                                       { lambda(tile_t) } -> std::same_as<void>;
                                  })
                    {
                         return typename std::remove_cvref_t<std::invoke_result_t<decltype(lambda), TileT>>{};
                    }
               }
          });
     }
     template<open_viii::graphics::background::is_tile TileT, std::integral PosT, typename LambdaT>
     [[nodiscard]] auto working_get_tile_at_offset(const PosT pos, LambdaT &&lambda) const
     {
          return back().visit_tiles([&](auto &tiles) {
               if constexpr (std::is_same_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(tiles)>>, TileT>)
               {
                    auto tile = tiles.begin();
                    std::ranges::advance(tile, pos);
                    return lambda(*tile);
               }
               else
               {
                    if constexpr (!requires(TileT tile_t) {
                                       { lambda(tile_t) } -> std::same_as<void>;
                                  })
                    {
                         TileT v{};
                         using T = std::remove_cvref_t<decltype(lambda(v))>;
                         return T{};
                    }
               }
          });
     }

     static std::vector<PupuID> calculate_pupu(const map_t &map)
     {
          return map.visit_tiles([](const auto &tiles) {
               std::vector<PupuID> pupu_ids = {};
               UniquifyPupu        pupu_map = {};
               pupu_ids.reserve(std::ranges::size(tiles));
               std::ranges::transform(tiles, std::back_insert_iterator(pupu_ids), pupu_map);
               return pupu_ids;
          });
     }

   public:
     MapHistory() = default;
     explicit MapHistory(map_t map)
       : m_front(std::move(map))
       , m_back(m_front)
       , m_front_pupu(calculate_pupu(m_front))
     {
     }
     template<typename TileT>
     [[nodiscard]] PupuID get_pupu_from_working(const TileT &tile) const
     {
          return m_front_pupu[static_cast<std::size_t>(get_offset_from_working(tile))];
     }
     [[nodiscard]] const auto &pupu() const noexcept
     {
          return m_front_pupu;
     }
     template<typename TileT>
     [[nodiscard]] auto get_offset_from_working(const TileT &tile) const
     {
          return back().visit_tiles([&](const auto &tiles) {
               if constexpr (std::is_same_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(tiles)>>, TileT>)
               {
                    return std::ranges::distance(&tiles.front(), &tile);
               }
               else
               {
                    return std::ranges::range_difference_t<std::remove_cvref_t<decltype(tiles)>>{};
               }
          });
     }
     [[nodiscard]] std::size_t count() const
     {
          return m_front_history.size() + m_back_history.size();
     }
     [[nodiscard]] std::size_t redo_count() const
     {
          return m_redo_history.size();
     }
     [[nodiscard]] const map_t &front() const
     {
          return m_front;
     }
     [[nodiscard]] map_t &back() const
     {
          return m_back;
     }
     template<typename TileT, typename LambdaT>
     auto copy_working_and_get_new_tile(const TileT &tile, LambdaT &&lambda) const
     {
          const auto pos = get_offset_from_working(tile);
          (void)copy_working();
          return working_get_tile_at_offset<TileT>(pos, std::forward<LambdaT>(lambda));
     }
     template<typename TileT, typename LambdaT>
     void copy_working_perform_operation(const std::vector<std::intmax_t> &indexes, LambdaT &&lambda) const
     {
          (void)copy_working();
          for (const auto i : indexes)
          {
               working_get_tile_at_offset<TileT>(i, lambda);
          }
     }
     template<typename TileT, typename FilterLambdaT, typename LambdaT>
          requires(std::is_invocable_r_v<bool, FilterLambdaT, const TileT &>)
     void copy_working_perform_operation(FilterLambdaT &&filter, LambdaT &&lambda) const
     {
          (void)copy_working();
          back().visit_tiles([&](auto &tiles) {
               if constexpr (std::is_same_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(tiles)>>, TileT>)
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
     void copy_working_perform_operation(const TileT &tile, const SimilarAdjustments &similar, LambdaT &&lambda) const
     {
          if (similar)
          {
               copy_working_perform_operation<TileT>(similar(tile), std::forward<LambdaT>(lambda));
          }
          else
          {
               copy_working_and_get_new_tile<TileT>(tile, std::forward<LambdaT>(lambda));
          }
     }

     template<typename TileT, typename LambdaT>
     auto get_original_version_of_working_tile(const TileT &tile, LambdaT &&lambda) const
     {
          return original_get_tile_at_offset<TileT>(get_offset_from_working(tile), std::forward<LambdaT>(lambda));
     }
     /**
      * For when a change could happen. we make a copy ahead of time.
      * @return back map
      */
     [[nodiscard]] map_t &copy_back_preemptive(std::source_location source_location = std::source_location::current()) const
     {
          if (!preemptive_copy_mode)
          {
               auto &temp           = safe_copy_working();
               preemptive_copy_mode = true;
               spdlog::debug(
                 "Map History preemptive_copy_mode: {}\n\t{}:{}",
                 preemptive_copy_mode,
                 source_location.file_name(),
                 source_location.line());
               return temp;
          }
          return back();
     }
     /**
      * After copy_mode is returned to normal copy_back_preemptive will resume
      * making copies.
      */
     void end_preemptive_copy_mode(std::source_location source_location = std::source_location::current()) const
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
     [[nodiscard]] map_t &copy_working() const
     {
          auto &temp = safe_copy_working();
          if (!preemptive_copy_mode)
          {
               clear_redo();
          }
          return temp;
     }
     [[nodiscard]] map_t &safe_copy_working() const
     {
          (void)debug_count_print();
          if (!preemptive_copy_mode)
          {
               m_front_or_back.push_back(pushed::Back);
               m_back_history.push_back(back());
          }
          return back();
     }
     void clear_redo() const
     {
          m_redo_history.clear();
          m_redo_front_or_back.clear();
     }
     [[nodiscard]] bool remove_duplicate() const
     {
          bool ret = false;
          while (!undo_enabled() &&
           ((m_front_or_back.back() == pushed::Back
             && m_back_history.back() == m_back)
            || (m_front_or_back.back() == pushed::Front
                && m_front_history.back() == m_front)))
          {
               (void)undo(true);
               ret = true;
          }
          return ret;
     }
     //  [[nodiscard]] const MapT &copy_working_to_original() const
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
     [[nodiscard]] bool redo(std::source_location source_location = std::source_location::current()) const
     {
          const auto count = debug_count_print(source_location);
          if (!redo_enabled())
          {
               return false;
          }
          pushed last = m_redo_front_or_back.back();
          m_front_or_back.push_back(last);
          m_redo_front_or_back.pop_back();
          if (last == pushed::Back)
          {
               (void)redo_working();
               return true;
          }
          (void)redo_original();
          return true;
     }
     /**
      * Deletes the most recent back or front
      * @return
      */
     [[nodiscard]] bool undo(bool skip_redo = false, std::source_location source_location = std::source_location::current()) const
     {
          const auto count = debug_count_print(source_location);
          if (!undo_enabled())
          {
               return false;
          }
          pushed last = m_front_or_back.back();
          if (!skip_redo)
          {
               m_redo_front_or_back.push_back(last);
          }
          m_front_or_back.pop_back();
          if (last == pushed::Back)
          {
               (void)undo_working(false);
               return true;
          }
          (void)undo_original(false);
          return true;
     }
     void undo_all(std::source_location source_location = std::source_location::current()) const
     {
          while (undo(false, source_location))
          {
          }
     }
     void redo_all(std::source_location source_location = std::source_location::current()) const
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
