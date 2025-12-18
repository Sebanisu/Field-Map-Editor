//
// Created by pcvii on 6/2/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPHISTORY_HPP
#define FIELD_MAP_EDITOR_MAPHISTORY_HPP
#include "MouseToTilePos.h"
#include "SimilarAdjustments.hpp"
#include <ff_8/UniquifyPupu.hpp>
#include <glengine/ScopeGuard.hpp>
#include <source_location>
namespace ff_8
{
class [[nodiscard]] MapHistory
{
     enum class pushed : std::uint8_t
     {
          Front,
          Back
     };
     using map_t                         = open_viii::graphics::background::Map;
     mutable map_t               m_front = {};
     mutable map_t               m_back  = {};
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
     [[nodiscard]] map_t        &redo_working() const;

     /**
      * Should only be called by undo() pops front
      * @return returns new front
      */
     [[nodiscard]] const map_t  &redo_original() const;

     /**
      * Should only be called by undo() pops back
      * @return returns new back
      */
     [[nodiscard]] map_t        &undo_working(bool skip_redo = false) const;

     /**
      * Should only be called by undo() pops front
      * @return returns new front
      */
     [[nodiscard]] const map_t  &undo_original(bool skip_redo = false) const;

     auto                        debug_count_print(
                              std::source_location source_location
                              = std::source_location::current()) const
     {
          return glengine::ScopeGuard(
            [=, this]()
            {
                 spdlog::debug(
                   "Map History Count: {}\n\t{}:{}",
                   m_back_history.size() + m_front_history.size() + 2U,
                   source_location.file_name(),
                   source_location.line());
            });
     }

     template<
       typename TileT,
       std::integral PosT,
       typename LambdaT>
     [[nodiscard]] auto original_get_tile_at_offset(
       const PosT pos,
       LambdaT  &&lambda) const
     {
          return front().visit_tiles(
            [&](auto &tiles)
            {
                 if constexpr (std::is_same_v<
                                 std::ranges::range_value_t<
                                   std::remove_cvref_t<decltype(tiles)>>,
                                 TileT>)
                 {
                      auto front_tile = tiles.cbegin();

                      if (
                        pos < 0
                        || std::cmp_greater_equal(
                          pos, std::ranges::size(tiles)))
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
     [[nodiscard]] auto working_get_tile_at_offset(
       const PosT pos,
       LambdaT  &&lambda) const
     {
          return back().visit_tiles(
            [&](auto &tiles)
            {
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
                           using T = std::remove_cvref_t<decltype(lambda(v))>;
                           return T{};
                      }
                 }
            });
     }

     static std::vector<PupuID> calculate_pupu(const map_t &map);

   public:
     MapHistory() = default;

     explicit MapHistory(map_t map);

     template<typename TileT>
     [[nodiscard]] PupuID get_pupu_from_working(const TileT &tile) const
     {
          return m_front_pupu[static_cast<std::size_t>(
            get_offset_from_working(tile))];
     }

     [[nodiscard]] const std::vector<PupuID> &pupu() const noexcept;

     template<typename TileT>
     [[nodiscard]] auto get_offset_from_working(const TileT &tile) const
     {
          return back().visit_tiles(
            [&](const auto &tiles)
            {
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

     [[nodiscard]] std::size_t  count() const;
     [[nodiscard]] std::size_t  redo_count() const;
     [[nodiscard]] const map_t &front() const;
     [[nodiscard]] map_t       &back() const;

     template<
       typename TileT,
       typename LambdaT>
     auto copy_working_and_get_new_tile(
       const TileT &tile,
       LambdaT    &&lambda) const
     {
          const auto pos = get_offset_from_working(tile);
          (void)copy_working();
          return working_get_tile_at_offset<TileT>(
            pos, std::forward<LambdaT>(lambda));
     }

     template<
       typename TileT,
       typename LambdaT>
     void copy_working_perform_operation(
       const std::vector<std::intmax_t> &indexes,
       LambdaT                         &&lambda) const
     {
          (void)copy_working();
          for (const auto i : indexes)
          {
               working_get_tile_at_offset<TileT>(i, lambda);
          }
     }

     template<
       typename TileT,
       typename FilterLambdaT,
       typename LambdaT>
          requires(std::is_invocable_r_v<
                   bool,
                   FilterLambdaT,
                   const TileT &>)
     void copy_working_perform_operation(
       FilterLambdaT &&filter,
       LambdaT       &&lambda) const
     {
          (void)copy_working();
          back().visit_tiles(
            [&](auto &tiles)
            {
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

     template<
       typename TileT,
       typename LambdaT>
     void copy_working_perform_operation(
       const TileT              &tile,
       const SimilarAdjustments &similar,
       LambdaT                 &&lambda) const
     {
          if (similar)
          {
               copy_working_perform_operation<TileT>(
                 similar(tile), std::forward<LambdaT>(lambda));
          }
          else
          {
               copy_working_and_get_new_tile<TileT>(
                 tile, std::forward<LambdaT>(lambda));
          }
     }

     template<
       typename TileT,
       typename LambdaT>
     auto get_original_version_of_working_tile(
       const TileT &tile,
       LambdaT    &&lambda) const
     {
          return original_get_tile_at_offset<TileT>(
            get_offset_from_working(tile), std::forward<LambdaT>(lambda));
     }

     /**
      * For when a change could happen. we make a copy ahead of time.
      * @return back map
      */
     [[nodiscard]] map_t &copy_back_preemptive(
       std::source_location source_location
       = std::source_location::current()) const;

     /**
      * After copy_mode is returned to normal copy_back_preemptive will resume
      * making copies.
      */
     void end_preemptive_copy_mode(
       std::source_location source_location
       = std::source_location::current()) const;

     [[nodiscard]] map_t &copy_working() const;

     [[nodiscard]] map_t &safe_copy_working() const;

     void                 clear_redo() const;

     [[nodiscard]] bool   remove_duplicate() const;

     /**
      * Deletes the most recent back or front
      * @return
      */
     [[nodiscard]] bool   redo(
         std::source_location source_location
         = std::source_location::current()) const;

     /**
      * Deletes the most recent back or front
      * @return
      */
     [[nodiscard]] bool undo(
       bool                 skip_redo = false,
       std::source_location source_location
       = std::source_location::current()) const;

     void undo_all(
       std::source_location source_location
       = std::source_location::current()) const;
     void redo_all(
       std::source_location source_location
       = std::source_location::current()) const;
     [[nodiscard]] bool redo_enabled() const;
     [[nodiscard]] bool undo_enabled() const;
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPHISTORY_HPP
