//
// Created by pcvii on 6/2/2022.
//

#include "MapHistory.hpp"

using namespace open_viii::graphics::background;
using map_t = Map;

/**
 * @brief Namespace for Final Fantasy VIII-related functionality.
 *
 * The `ff_8` namespace contains utilities, classes, and functions related to
 * handling tile data, conflicts, and other game-specific operations for Final Fantasy VIII.
 */
namespace ff_8
{

/**
 * @brief Calculates the Pupu IDs for the tiles in a given map.
 *
 * This function processes all tiles in the provided map and generates a unique
 * `PupuID` (an unsigned 64-bit integer) for each tile. The `PupuID` is used to
 * prevent conflicts or overlapping tiles in the unswizzled or rendered images,
 * allowing modders to modify and re-import tiles without conflicts.
 *
 * The `UniquifyPupu` logic performs the following:
 * - Initially, a `PupuID` is generated for each tile, which includes 4 offset bits.
 * - After the initial generation, a second pass is made where overlapping tiles are detected.
 * - For overlapping tiles, the `PupuID` is incremented to resolve the conflict, ensuring each tile
 *   has a unique ID even when they occupy the same location.
 *
 * The `PupuID` is designed to wrap around the unsigned 64-bit integer, allowing you to reverse
 * the ID back to the original values, much like `source_tile_conflicts` handles the swizzle or input locations.
 *
 * The generated `PupuID`s can be used to:
 * - Dump unswizzled images with unique filenames containing the raw hex value of each `PupuID`.
 * - Allow modders to upscale or modify the images.
 * - Re-import the modified images back into the system without conflicts.
 *
 * @param map The map containing the tiles to process.
 * @return A vector of `PupuID` objects corresponding to the tiles in the map.
 */
static std::vector<PupuID> calculate_pupu(const map_t &map)
{
     return map.visit_tiles([](const auto &tiles) {
          std::vector<PupuID> pupu_ids = {};
          UniquifyPupu const  pupu_map = {};
          pupu_ids.reserve(std::ranges::size(tiles));

          std::ranges::transform(tiles | Map::filter_view_invalid(), std::back_insert_iterator(pupu_ids), pupu_map);
          return pupu_ids;
     });
}

/**
 * @brief Generates a vector of unique PupuIDs from the input vector.
 *
 * This function processes a vector of `PupuID` values, removing any duplicate entries
 * while preserving the order of unique elements (after sorting). The steps include:
 * - Sorting the input vector to group duplicates together.
 * - Removing consecutive duplicates using `std::ranges::unique`.
 * - Erasing the duplicates from the resulting vector to ensure all elements are unique.
 *
 * @param input A vector of `PupuID` values that may contain duplicates.
 * @return A new vector containing only the unique `PupuID` values from the input.
 *
 * @note The function operates on a copy of the input, leaving the original vector unmodified.
 *       The output is sorted due to the use of `std::ranges::sort` prior to deduplication.
 */
static std::vector<PupuID> make_unique_pupu(const std::vector<PupuID> &input)
{
     // Copies the input
     std::vector<PupuID> output = input;

     // Sorts the vector to prepare for unique removal
     std::ranges::sort(output);

     // Removes consecutive duplicates
     const auto last = std::ranges::unique(output);

     // Erases the duplicates from the vector
     output.erase(last.begin(), last.end());
     return output;
}


/**
 * @brief Calculates source tile location conflicts for a given map.
 *
 * This function identifies and tracks conflicts between tiles in the provided map.
 * A conflict is defined as multiple tiles occupying the same grid location. The result
 * is a `source_tile_conflicts` object, which stores information about conflicting tiles
 * and their locations.
 *
 * @param map The map containing the tiles to process.
 * @return A `source_tile_conflicts` object representing the tile conflicts in the map.
 */
[[maybe_unused]] static source_tile_conflicts calculate_conflicts(const map_t &map)
{
     return map.visit_tiles([](const auto &tiles) {
          source_tile_conflicts stc{};
          std::ranges::for_each(tiles | Map::filter_view_invalid(), [&](const auto &tile) {
               stc(tile).push_back(std::ranges::distance(&tiles.front(), &tile));
          });
          return stc;
     });
}
}// namespace ff_8

ff_8::MapHistory::MapHistory(map_t map)
  : m_original(std::move(map))
  , m_working(m_original)
  , m_original_pupu(calculate_pupu(m_original))
  , m_working_pupu(m_original_pupu)
  , m_original_unique_pupu(make_unique_pupu(m_original_pupu))
  , m_working_unique_pupu(m_original_unique_pupu)
  , m_original_conflicts(calculate_conflicts(m_original))
  , m_working_conflicts(calculate_conflicts(m_original))
{
}

void ff_8::MapHistory::refresh_original_all(bool force) const
{
     if (!m_original_changed && !force)
     {
          return;
     }
     refresh_original_pupu();
     refresh_original_conflicts();
     m_original_changed = false;
}

void ff_8::MapHistory::refresh_working_all(bool force) const
{
     if (!m_working_changed && !force)
     {
          return;
     }
     refresh_working_conflicts();
     refresh_working_pupu();
     m_working_changed = false;
}

void ff_8::MapHistory::refresh_original_pupu() const
{
     m_original_pupu        = calculate_pupu(m_original);
     m_original_unique_pupu = make_unique_pupu(m_original_pupu);
}

void ff_8::MapHistory::refresh_working_pupu() const
{
     m_working_pupu        = calculate_pupu(m_working);
     m_working_unique_pupu = make_unique_pupu(m_working_pupu);
}


void ff_8::MapHistory::refresh_original_conflicts() const
{
     m_original_conflicts = calculate_conflicts(m_original);
}

void ff_8::MapHistory::refresh_working_conflicts() const
{
     m_working_conflicts = calculate_conflicts(m_working);
}

const std::vector<ff_8::PupuID> &ff_8::MapHistory::original_pupu() const noexcept
{
     return m_original_pupu;
}

const std::vector<ff_8::PupuID> &ff_8::MapHistory::working_pupu() const noexcept
{
     return m_working_pupu;
}

const std::vector<ff_8::PupuID> &ff_8::MapHistory::original_unique_pupu() const noexcept
{
     return m_original_unique_pupu;
}

const std::vector<ff_8::PupuID> &ff_8::MapHistory::working_unique_pupu() const noexcept
{
     return m_working_unique_pupu;
}

const ff_8::source_tile_conflicts &ff_8::MapHistory::original_conflicts() const noexcept
{
     return m_original_conflicts;
}

const ff_8::source_tile_conflicts &ff_8::MapHistory::working_conflicts() const noexcept
{
     return m_working_conflicts;
}

std::size_t ff_8::MapHistory::count() const
{
     return m_undo_history.size();
}

std::size_t ff_8::MapHistory::redo_count() const
{
     return m_redo_history.size();
}

const map_t &ff_8::MapHistory::original() const
{
     return m_original;
}

map_t &ff_8::MapHistory::working()
{
     return m_working;
}

const map_t &ff_8::MapHistory::working() const
{
     return m_working;
}

map_t &ff_8::MapHistory::copy_working(std::string description)
{
     (void)debug_count_print();
     m_undo_original_or_working.push_back(pushed::working);
     m_undo_history.push_back(working());
     m_undo_change_descriptions.push_back(std::move(description));
     m_working_changed = true;
     return working();
}

void ff_8::MapHistory::clear_redo()
{
     m_redo_history.clear();
     m_redo_original_or_working.clear();
}

bool ff_8::MapHistory::remove_duplicate()
{
     bool ret = false;
     while (!undo_enabled() &&
         ((m_undo_original_or_working.back() == pushed::working
           && m_undo_history.back() == m_working)
          || (m_undo_original_or_working.back() == pushed::original
              && m_undo_history.back() == m_original)))
     {
          (void)undo(true);
          ret = true;
     }
     return ret;
}

const map_t &ff_8::MapHistory::copy_working_to_original(std::string description)
{
     clear_redo();
     const auto count = debug_count_print();
     m_undo_history.push_back(original());
     m_undo_original_or_working.push_back(pushed::original);
     m_undo_change_descriptions.push_back(std::move(description));
     // todo do we want to recalculate the pupu?
     m_original         = working();
     m_original_changed = true;
     return original();
}

bool ff_8::MapHistory::redo()
{
     const auto count = debug_count_print();
     if (!redo_enabled())
     {
          return false;
     }
     const pushed last = m_redo_original_or_working.back();
     m_undo_original_or_working.push_back(last);
     m_redo_original_or_working.pop_back();

     (void)m_undo_change_descriptions.emplace_back(std::move(m_redo_change_descriptions.back()));
     m_redo_change_descriptions.pop_back();
     if (last == pushed::working)
     {
          (void)m_undo_history.emplace_back(std::move(m_working));
          m_working         = std::move(m_redo_history.back());
          m_working_changed = true;
          m_redo_history.pop_back();
          return true;
     }
     (void)m_undo_history.emplace_back(std::move(m_original));
     m_original         = std::move(m_redo_history.back());
     m_original_changed = true;
     m_redo_history.pop_back();
     return true;
}

bool ff_8::MapHistory::undo(bool skip_redo)
{
     const auto count = debug_count_print();
     if (!undo_enabled())
     {
          return false;
     }
     const pushed last = m_undo_original_or_working.back();
     if (!skip_redo)
     {
          m_redo_original_or_working.push_back(last);
          (void)m_redo_change_descriptions.emplace_back(std::move(m_undo_change_descriptions.back()));
     }
     m_undo_original_or_working.pop_back();
     m_undo_change_descriptions.pop_back();
     if (last == pushed::working)
     {
          if (!skip_redo)
          {
               m_redo_history.emplace_back(std::move(m_working));
          }
          m_working         = std::move(m_undo_history.back());
          m_working_changed = true;
          m_undo_history.pop_back();
          return true;
     }
     if (!skip_redo)
     {
          m_redo_history.emplace_back(std::move(m_original));
     }
     m_original         = std::move(m_undo_history.back());
     m_original_changed = true;
     m_undo_history.pop_back();
     return true;
}
void ff_8::MapHistory::undo_all()
{
     while (undo())
     {
     }
}
void ff_8::MapHistory::redo_all()
{
     while (redo())
     {
     }
}
bool ff_8::MapHistory::redo_enabled() const
{
     return !std::ranges::empty(m_redo_history);
}

bool ff_8::MapHistory::undo_enabled() const
{
     return !std::ranges::empty(m_undo_history);
}

const map_t &ff_8::MapHistory::const_working() const
{
     return m_working;
}

template<is_tile TileT>
[[nodiscard]] ff_8::PupuID ff_8::MapHistory::get_pupu_from_working(const TileT &tile) const
{
     return m_original_pupu[static_cast<std::size_t>(get_offset_from_working(tile))];
}

// Explicit instantiation for Tiles
template ff_8::PupuID ff_8::MapHistory::get_pupu_from_working(const Tile1 &) const;
template ff_8::PupuID ff_8::MapHistory::get_pupu_from_working(const Tile2 &) const;
template ff_8::PupuID ff_8::MapHistory::get_pupu_from_working(const Tile3 &) const;

template<open_viii::graphics::background::is_tile TileT>
std::vector<TileT>::difference_type ff_8::MapHistory::get_offset_from_working(const TileT &tile) const
{
     return working().visit_tiles([&](const auto &tiles) -> std::vector<TileT>::difference_type {
          if constexpr (std::is_same_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(tiles)>>, TileT>)
          {
               return static_cast<std::vector<TileT>::difference_type>(std::ranges::distance(&tiles.front(), &tile));
          }
          else
          {
               return {};
          }
     });
}

template std::vector<Tile1>::difference_type ff_8::MapHistory::get_offset_from_working(const Tile1 &) const;
template std::vector<Tile2>::difference_type ff_8::MapHistory::get_offset_from_working(const Tile2 &) const;
template std::vector<Tile3>::difference_type ff_8::MapHistory::get_offset_from_working(const Tile3 &) const;


std::string_view                             ff_8::MapHistory::current_undo_description() const
{
     if (std::ranges::empty(m_undo_change_descriptions))
     {
          return {};
     }
     return m_undo_change_descriptions.back();
}

std::string_view ff_8::MapHistory::current_redo_description() const
{
     if (std::ranges::empty(m_redo_change_descriptions))
     {
          return {};
     }
     return m_redo_change_descriptions.back();
}

[[nodiscard]] ff_8::MapHistory::pushed ff_8::MapHistory::current_undo_pushed() const
{
     if (std::ranges::empty(m_undo_original_or_working))
     {
          return {};
     }
     return m_undo_original_or_working.back();
}

[[nodiscard]] ff_8::MapHistory::pushed ff_8::MapHistory::current_redo_pushed() const
{
     if (std::ranges::empty(m_redo_original_or_working))
     {
          return {};
     }
     return m_redo_original_or_working.back();
}