//
// Created by pcvii on 6/2/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPHISTORY_HPP
#define FIELD_MAP_EDITOR_MAPHISTORY_HPP
#include "normalized_source_tile.hpp"
#include "PupuID.hpp"
#include "scope_guard.hpp"
#include "source_tile_conflicts.hpp"
#include "UniquifyPupu.hpp"
#include <map>
#include <open_viii/graphics/background/Map.hpp>
#include <spdlog/spdlog.h>
/**
 * @namespace ff_8
 * @brief Contains classes and utilities specific to the Final Fantasy VIII field map editor.
 */
namespace ff_8
{
/**
 * @class MapHistory
 * @brief Tracks and manages the history of changes to original and working map states.
 *
 * This class provides functionality to manage undo and redo operations,
 * track changes, and associate descriptions with each modification to the map states.
 */
class [[nodiscard]] MapHistory
{
   public:
     /**
      * @enum pushed
      * @brief Specifies whether a map state belongs to the original or working state.
      */
     enum class pushed : std::uint8_t
     {
          unknown,
          original,
          working
     };

     /**
      * @typedef map_t
      * @brief Alias for the `Map` type from `open_viii::graphics::background`.
      */
     using map_t    = open_viii::graphics::background::Map;
     using nst_map  = std::map<open_viii::graphics::background::normalized_source_tile, std::uint8_t>;
     using nsat_map = std::map<open_viii::graphics::background::normalized_source_animated_tile, std::uint8_t>;

   private:
     /**
      * @brief Indicates whether the original state has been changed.
      *
      * This boolean flag tracks whether changes have been made to the original tile data.
      * It is set to `true` before changes are made to the original state, and may be set to
      * `false` after changes are committed. However, changes may still be in progress when
      * it is set to `false`, so it is important to check the state before performing any further operations.
      *
      * @note This flag is used to track modifications to the original tile data.
      */
     mutable bool                  m_original_changed         = { false };

     /**
      * @brief Indicates whether the working state has been changed.
      *
      * This boolean flag tracks whether changes have been made to the working tile data.
      * It is set to `true` before making changes to the working state, and may be set to
      * `false` after changes are completed. As with the original flag, the state may be in
      * the middle of a change when it is set to `false`.
      *
      * @note This flag is used to track modifications to the working tile data.
      */
     mutable bool                  m_working_changed          = { false };


     // Current states
     /**
      * @brief The active original map state.
      */
     map_t                         m_original                 = {};


     /**
      * @brief The active working map state.
      */
     map_t                         m_working                  = {};

     // Corresponding PupuIDs
     /**
      * @brief PupuID list corresponding to the original map state.
      */
     mutable std::vector<PupuID>   m_original_pupu            = {};

     /**
      * @brief PupuID list corresponding to the working map state.
      */
     mutable std::vector<PupuID>   m_working_pupu             = {};


     // Corresponding PupuIDs
     /**
      * @brief Unique PupuID list corresponding to the original map state.
      */
     mutable std::vector<PupuID>   m_original_unique_pupu     = {};

     /**
      * @brief Unique PupuID list corresponding to the working map state.
      */
     mutable std::vector<PupuID>   m_working_unique_pupu      = {};


     // Corresponding Source Conflicts
     /**
      * @brief Source Conflict list corresponding to the original map state.
      */
     mutable source_tile_conflicts m_original_conflicts       = {};

     /**
      * @brief Source Conflict list corresponding to the working map state.
      */
     mutable source_tile_conflicts m_working_conflicts        = {};


     /**
      * @brief Made from Source Conflict list corresponding to the working map state. m_working_conflicts.  We normalize a tile and then see
      * what the count is. We're using this to know if a tile is being used in more than one location. Mostly just to high light it
      * differently. Or provide a user with information.
      */
     mutable nst_map               m_working_similar_counts   = {};

     mutable nsat_map              m_working_animation_counts = {};


     // Consolidated history and tracking
     /**
      * @brief Unified undo history for both original and working states.
      */
     std::vector<map_t>            m_undo_history             = {};
     /**
      * @brief Tracks whether a history entry belongs to the original or working state.
      */
     std::vector<pushed>           m_undo_original_or_working = {};

     // Redo history and tracking
     /**
      * @brief Unified redo history for both original and working states.
      */
     std::vector<map_t>            m_redo_history             = {};

     /**
      * @brief Tracks redo states for original or working states.
      */
     std::vector<pushed>           m_redo_original_or_working = {};

     // New vectors for tracking descriptions of changes
     /**
      * @brief Descriptions of undo changes corresponding to `m_undo_original_or_working`.
      */
     std::vector<std::string>      m_undo_change_descriptions = {};

     /**
      * @brief Descriptions of redo changes corresponding to `m_redo_original_or_working`.
      */
     std::vector<std::string>      m_redo_change_descriptions = {};

     /**
      * @brief Debug utility to print the current map history count.
      *
      * Outputs the count of maps in history along with the file name and line number for debugging purposes.
      *
      * @return A `scope_guard` object that automatically logs debug information when it goes out of scope.
      */
     auto                          debug_count_print() const
     {
          return scope_guard([&]() { spdlog::debug("Map History Count: {}\n\t{}:{}", count() + 2U, __FILE__, __LINE__); });
     }

     /**
      * @brief Retrieve and process a tile from the original map at a specific offset.
      *
      * Invokes a lambda function on a tile in the original map at the specified position. Throws an exception if the position is out of
      * bounds.
      *
      * @tparam TileT The type of tile to retrieve.
      * @tparam PosT The type of the position (integral type).
      * @tparam LambdaT The type of the lambda function to process the tile.
      * @param pos The position of the tile in the original map.
      * @param lambda The lambda function to invoke on the tile.
      * @return The result of invoking the lambda function on the tile.
      * @throws std::exception If the position is out of bounds.
      */
     template<typename TileT, std::integral PosT, typename LambdaT>
     [[nodiscard]] auto original_get_tile_at_offset(const PosT pos, LambdaT &&lambda) const
     {
          return original().visit_tiles([&](auto &tiles) {
               if constexpr (std::is_same_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(tiles)>>, TileT>)
               {
                    auto front_tile = tiles.cbegin();
                    if (std::cmp_less(pos, 0) || std::cmp_greater_equal(pos, std::ranges::size(tiles)))
                    {
                         spdlog::error("{}:{} pos in original to be 0 < {} < {} ", __FILE__, __LINE__, pos, std::ranges::size(tiles));
                         throw std::exception();
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

     /**
      * @brief Retrieve and process a tile from the working map at a specific offset.
      *
      * Invokes a lambda function on a tile in the working map at the specified position.
      *
      * @tparam TileT The type of tile to retrieve.
      * @tparam PosT The type of the position (integral type).
      * @tparam LambdaT The type of the lambda function to process the tile.
      * @param pos The position of the tile in the working map.
      * @param lambda The lambda function to invoke on the tile.
      * @return The result of invoking the lambda function on the tile.
      */
     template<open_viii::graphics::background::is_tile TileT, std::integral PosT, typename LambdaT>
     [[nodiscard]] auto working_get_tile_at_offset(const PosT pos, LambdaT &&lambda) const
     {
          return working().visit_tiles([&](auto &tiles) {
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
                         using t = std::remove_cvref_t<decltype(lambda(v))>;
                         return t{};
                    }
               }
          });
     }

     /**
      * @brief Regenerates the PupuID data for the original map.
      */
     void refresh_original_pupu() const;

     /**
      * @brief Regenerates the PupuID data for the working map.
      */
     void refresh_working_pupu() const;


     /**
      * @brief Regenerates the Source Conflicts data for the original map.
      */
     void refresh_original_conflicts() const;

     /**
      * @brief Regenerates the Source Conflicts data for the working map.
      */
     void refresh_working_conflicts() const;

   public:
     /**
      * @brief Default constructor.
      *
      * Initializes an empty `MapHistory` instance with no original or working map states.
      */
     MapHistory() = default;

     /**
      * @brief Constructs a `MapHistory` instance with an initial map state.
      *
      * @param map The initial map state to set as both the original and working map states.
      *
      * This constructor sets the provided `map` as the starting state for both the
      * original and working maps, initializing the undo/redo history accordingly.
      */
     explicit MapHistory(map_t map);

     /**
      * @brief Retrieves the current original map.
      * @return A constant reference to the original map.
      */
     const map_t                               &original() const;

     /**
      * @brief Retrieves the current working map as a constant reference.
      * @return A constant reference to the working map.
      */
     const map_t                               &const_working() const;

     /**
      * @brief Retrieves the current working map.
      * @return A constant reference to the working map.
      */
     const map_t                               &working() const;

     /**
      * @brief Provides a mutable reference to the working map for modifications.
      * @return A mutable reference to the working map.
      */
     map_t                                     &working();
     /**
      * @brief Creates a copy of the current working map and logs the planned change.
      *
      * This function captures the current state of the working map for undo purposes
      * and associates it with a description of the change being planned. The copied
      * state represents the map prior to applying the change, while the description
      * provides context for the change.
      *
      * @param description A string describing the planned change to the working map.
      *                    This description is logged and stored for use in the undo history UI.
      *
      * @return A mutable reference to the copy of the working map.
      *
      * @details
      * - The copied state is stored in the undo history along with the provided description.
      * - This function is integral to the undo system, which stores the map's state before changes are applied.
      */
     map_t                                     &copy_working(std::string description);


     /**
      * @brief Pushes the current original map to the undo history and sets it as the new original state.
      *
      * This function records the current original map state in the undo history, associates it with a
      * provided description, and clears the redo history. It updates the original map to the earliest
      * state in the undo history and marks it as changed. The function is used to establish a new
      * original state while preserving the ability to undo the operation.
      *
      * @param description A string describing the change for the undo history.
      * @return A const reference to the original map state.
      * @note The `debug_count_print` function is called for debugging purposes.
      * @note The redo history is cleared as part of this operation.
      * @pre The `m_undo_history`, `m_undo_original_or_working`, and `m_undo_change_descriptions` containers must be properly initialized.
      * @post The original map is updated to the earliest undo history state, `m_original_changed` is set to true, and the redo history is
      * cleared.
      * @throws None (assumes container operations and `debug_count_print` do not throw).
      */
     const map_t                               &first_to_original(std::string description);
     
     /**
      * @brief Pushes the current working map to the undo history and sets it as the new working state.
      *
      * This function records the current working map state in the undo history, associates it with a
      * provided description, and clears the redo history. It updates the working map to the earliest
      * state in the undo history and marks it as changed. The function is used to establish a new
      * working state while preserving the ability to undo the operation.
      *
      * @param description A string describing the change for the undo history.
      * @return A const reference to the current working map state.
      * @note The `debug_count_print` function is called for debugging purposes.
      * @note The redo history is cleared as part of this operation.
      * @pre The `m_undo_history`, `m_undo_original_or_working`, and `m_undo_change_descriptions` containers must be properly initialized.
      * @post The working map is updated to the earliest undo history state, `m_working_changed` is set to true, and the redo history is
      * cleared.
      * @throws None (assumes container operations and `debug_count_print` do not throw).
      */
     const map_t                               &first_to_working(std::string description);

     /**
      * @brief Copies the current working map to the original map and logs the reason for the operation.
      *
      * This function is used to make changes in the working map permanent by copying its state to the original map.
      * It is commonly used during save operations. Additionally, the provided description explains the reason for
      * making the changes permanent.
      *
      * @param description A string describing the reason for making the changes permanent.
      *
      * @return A constant reference to the updated original map.
      *
      * @details
      * - This function updates the original map to match the working map, effectively committing the changes.
      * - After calling this function, other components (e.g., texture loaders) may need updates to reflect
      *   the new state of the map, as changes could affect rendering or other operations.
      */
     const map_t                               &copy_working_to_original(std::string description);

     /**
      * @brief Checks whether undo operations are possible.
      * @return True if undo is enabled, false otherwise.
      */
     bool                                       undo_enabled() const;

     /**
      * @brief Checks whether redo operations are possible.
      * @return True if redo is enabled, false otherwise.
      */
     bool                                       redo_enabled() const;

     /**
      * @brief Performs all available redo operations.
      */
     void                                       redo_all();

     /**
      * @brief Performs all available undo operations.
      */
     void                                       undo_all();

     /**
      * @brief Performs a single redo operation.
      * @return True if a redo was successfully performed, false otherwise.
      */
     bool                                       redo();

     /**
      * @brief Performs a single undo operation.
      * @param skip_redo If true, skips saving the undone change for redo.
      * @return True if an undo was successfully performed, false otherwise.
      */
     bool                                       undo(bool skip_redo = false);

     /**
      * @brief Removes duplicate entries from the undo history.
      * @return True if duplicates were found and removed, false otherwise.
      */
     bool                                       remove_duplicate();

     /**
      * @brief Retrieves the total number of changes in the undo history.
      * @return The size of the undo history.
      */
     size_t                                     count() const;

     /**
      * @brief Regenerates all PupuID data and Source Conflicts data for the maps in the original state.
      *
      * This function regenerates the entire set of PupuID data and source conflicts for the
      * maps in their original, unmodified state. It can be called to refresh the data,
      * either for the first time or after significant changes have been made.
      *
      * @param force If `true`, forces a regeneration even if the data is considered up-to-date.
      *              If `false` (default), the regeneration occurs only if necessary.
      */
     void                                       refresh_original_all(bool force = false) const;

     /**
      * @brief Regenerates all PupuID data and Source Conflicts data for the maps in the working state.
      *
      * This function regenerates the entire set of PupuID data and source conflicts for the
      * maps in their working state, which reflects any changes or modifications made.
      * It is useful for updating the working state data after changes have occurred.
      *
      * @param force If `true`, forces a regeneration even if the data is considered up-to-date.
      *              If `false` (default), the regeneration occurs only if necessary.
      */
     void                                       refresh_working_all(bool force = false) const;


     /**
      * @brief Retrieves the PupuIDs for the original map.
      * @return A constant reference to the vector of PupuIDs for the original map.
      */
     [[nodiscard]] const std::vector<PupuID>   &original_pupu() const noexcept;

     /**
      * @brief Retrieves the PupuIDs for the working map.
      * @return A constant reference to the vector of PupuIDs for the working map.
      */
     [[nodiscard]] const std::vector<PupuID>   &working_pupu() const noexcept;


     /**
      * @brief Retrieves the unique PupuIDs for the original state of the map.
      *
      * This function provides a constant reference to a vector of unique `PupuID` values
      * for the tiles in their original, unmodified state. These IDs are used to prevent
      * conflicts or overlapping tiles in the unswizzled or rendered output for the original
      * map data.
      *
      * @return A constant reference to a vector of unique `PupuID` values for the original state.
      * @note The returned vector is guaranteed to contain only unique values.
      */
     const std::vector<ff_8::PupuID>           &original_unique_pupu() const noexcept;

     /**
      * @brief Retrieves the unique PupuIDs for the working state of the map.
      *
      * This function provides a constant reference to a vector of unique `PupuID` values
      * for the tiles in their working state. The working state reflects any modifications
      * made to the tiles or their layout. These IDs are used to ensure consistency and
      * prevent conflicts in the unswizzled or rendered output for the working map data.
      *
      * @return A constant reference to a vector of unique `PupuID` values for the working state.
      * @note The returned vector is guaranteed to contain only unique values.
      */
     const std::vector<ff_8::PupuID>           &working_unique_pupu() const noexcept;


     /**
      * @brief Retrieves the current (working) tile conflicts.
      *
      * This function returns a constant reference to the current `source_tile_conflicts` object,
      * which holds the conflicts between tiles in their current (working) state. The working
      * conflicts represent the tile data that is actively being used or modified.
      *
      * @return A constant reference to the current `source_tile_conflicts` object.
      */
     [[nodiscard]] const source_tile_conflicts &working_conflicts() const noexcept;

     /**
      * @brief Returns a constant reference to the map holding the count of similar tiles.
      *
      * This getter provides access to the `m_working_similar_counts` map, which contains the frequency of normalized
      * tiles in the working map state. The map is of type `std::map<normalized_source_tile, std::uint8_t>`, where
      * the key represents a normalized tile and the value represents how many times it appears.
      *
      * The function is marked as `noexcept`, meaning it does not throw any exceptions, and is marked as `[[nodiscard]]`
      * to indicate that the return value should not be ignored.
      *
      * @return A constant reference to the map containing the count of similar tiles.
      */
     [[nodiscard]] const nst_map               &working_similar_counts() const noexcept;

     [[nodiscard]] const nsat_map              &working_animation_counts() const noexcept;

     /**
      * @brief Retrieves the original tile conflicts.
      *
      * This function returns a constant reference to the original `source_tile_conflicts` object,
      * which holds the conflicts between tiles in their initial, unmodified state. The original
      * conflicts are typically used to compare against the working conflicts or as a reference.
      *
      * @return A constant reference to the original `source_tile_conflicts` object.
      */
     [[nodiscard]] const source_tile_conflicts &original_conflicts() const noexcept;


     /**
      * @brief Retrieves the total number of redo operations available.
      * @return The size of the redo history.
      */
     size_t                                     redo_count() const;

     /**
      * @brief Clears all redo history.
      */
     void                                       clear_redo();

     /**
      * @brief Retrieves the PupuID corresponding to the given working tile.
      * @tparam TileT Type of the tile.
      * @param tile Reference to the working tile.
      * @return The corresponding PupuID.
      */
     template<open_viii::graphics::background::is_tile TileT>
     [[nodiscard]] PupuID get_pupu_from_working(const TileT &tile) const;

     /**
      * @brief Calculates the offset of the given tile in the working map.
      * @tparam TileT Type of the tile.
      * @param tile Reference to the tile.
      * @return The offset of the tile.
      */
     template<open_viii::graphics::background::is_tile TileT>
     [[nodiscard]] std::vector<TileT>::difference_type get_offset_from_working(const TileT &tile) const;


     /**
      * @brief Copies the working map and applies a lambda to retrieve a new tile.
      * @tparam TileT Type of the tile.
      * @tparam LambdaT Type of the lambda function.
      * @param tile Reference to the working tile.
      * @param lambda Lambda function to apply.
      * @return The new tile after the operation.
      */
     template<open_viii::graphics::background::is_tile TileT, typename LambdaT>
     auto copy_working_and_get_new_tile(const TileT &tile, LambdaT &&lambda)
     {
          const auto pos = get_offset_from_working(tile);
          (void)copy_working();
          return working_get_tile_at_offset<TileT>(pos, std::forward<LambdaT>(lambda));
     }


     /**
      * @brief Copies the working map and performs operations on specific tile indexes.
      * @tparam TileT Type of the tile.
      * @tparam LambdaT Type of the lambda function.
      * @param indexes Vector of tile indexes.
      * @param lambda Lambda function to apply.
      */
     template<open_viii::graphics::background::is_tile TileT, typename LambdaT>
     void copy_working_perform_operation(const std::vector<std::intmax_t> &indexes, LambdaT &&lambda)
     {
          (void)copy_working();
          for (const auto i : indexes)
          {
               working_get_tile_at_offset<TileT>(i, lambda);
          }
     }


     /**
      * @brief Copies the working map and performs operations on tiles filtered by a lambda.
      * @tparam TileT Type of the tile.
      * @tparam FilterLambdaT Type of the filter lambda.
      * @tparam LambdaT Type of the operation lambda.
      * @param filter Lambda function for filtering tiles.
      * @param lambda Lambda function to apply to filtered tiles.
      */
     template<open_viii::graphics::background::is_tile TileT, typename FilterLambdaT, typename LambdaT>
          requires(std::is_invocable_r_v<bool, FilterLambdaT, const TileT &>)
     void copy_working_perform_operation(FilterLambdaT &&filter, LambdaT &&lambda)
     {
          (void)copy_working();
          working().visit_tiles([&](auto &tiles) {
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

     /**
      * @brief Retrieves the original version of a working tile using a lambda.
      * @tparam TileT Type of the tile.
      * @tparam LambdaT Type of the lambda function.
      * @param tile Reference to the working tile.
      * @param lambda Lambda function to apply.
      * @return The corresponding original tile after the operation.
      */
     template<open_viii::graphics::background::is_tile TileT, typename LambdaT>
     auto get_original_version_of_working_tile(const TileT &tile, LambdaT &&lambda) const
     {
          return original_get_tile_at_offset<TileT>(get_offset_from_working(tile), std::forward<LambdaT>(lambda));
     }


     /**
      * @brief Retrieves the description of the most recent undoable change.
      *
      * This function provides a string view of the description for the last change
      * stored in the undo history, if available.
      *
      * @return A string view representing the description of the most recent undoable change.
      *         If there are no undoable changes, an empty string view is returned.
      */
     [[nodiscard]] std::string_view current_undo_description() const;

     /**
      * @brief Retrieves the description of the most recent redoable change.
      *
      * This function provides a string view of the description for the last change
      * stored in the redo history, if available.
      *
      * @return A string view representing the description of the most recent redoable change.
      *         If there are no redoable changes, an empty string view is returned.
      */
     [[nodiscard]] std::string_view current_redo_description() const;
     /**
      * @brief Retrieves the type of the most recent undoable change.
      *
      * This function indicates whether the last change in the undo history
      * corresponds to the original or working map, if available.
      *
      * @return A `pushed` value representing the type of the most recent undoable change.
      *         If there are no undoable changes, it returns `pushed::unknown`.
      */
     [[nodiscard]] pushed           current_undo_pushed() const;

     /**
      * @brief Retrieves the type of the most recent redoable change.
      *
      * This function indicates whether the last change in the redo history
      * corresponds to the original or working map, if available.
      *
      * @return A `pushed` value representing the type of the most recent redoable change.
      *         If there are no redoable changes, it returns `pushed::unknown`.
      */
     [[nodiscard]] pushed           current_redo_pushed() const;

     [[nodiscard]] auto             undo_history() const
     {
          return std::ranges::views::zip(
            std::ranges::views::iota(std::size_t{}, std::ranges::size(m_undo_original_or_working)),
            m_undo_original_or_working,
            m_undo_change_descriptions);
     }
     [[nodiscard]] auto redo_history() const
     {
          return std::ranges::views::zip(
            std::ranges::views::iota(std::size_t{}, std::ranges::size(m_redo_original_or_working)),
            m_redo_original_or_working,
            m_redo_change_descriptions);
     }
};
}// namespace ff_8

template<>
struct fmt::formatter<ff_8::MapHistory::pushed> : fmt::formatter<std::string_view>
{
     // tile_sizes::default_size, tile_sizes::x_2_size, tile_sizes::x_4_size, tile_sizes::x_8_size, tile_sizes::x_16_size
     //  parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(ff_8::MapHistory::pushed pushed, FormatContext &ctx) const
     {
          using namespace open_viii::graphics::background;
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (pushed)
          {
               case ff_8::MapHistory::pushed::unknown:
                    name = "Unknown"sv;
                    break;
               case ff_8::MapHistory::pushed::original:
                    name = "Original"sv;
                    break;
               case ff_8::MapHistory::pushed::working:
                    name = "Working"sv;
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};
#endif// FIELD_MAP_EDITOR_MAPHISTORY_HPP
