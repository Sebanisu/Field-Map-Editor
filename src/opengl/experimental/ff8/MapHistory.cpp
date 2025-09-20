//
// Created by pcvii on 6/2/2022.
//

#include "MapHistory.hpp"
namespace ff_8
{
/**
 * Should only be called by undo() pops back
 * @return returns new back
 */
[[nodiscard]] MapHistory::map_t &MapHistory::redo_working() const
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
[[nodiscard]] const MapHistory::map_t &MapHistory::redo_original() const
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
[[nodiscard]] MapHistory::map_t &MapHistory::undo_working(bool skip_redo) const
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
[[nodiscard]] const MapHistory::map_t &
  MapHistory::undo_original(bool skip_redo) const
{
     if (!skip_redo)
     {
          m_redo_history.emplace_back(std::move(m_front));
     }
     m_front = std::move(m_front_history.back());
     m_front_history.pop_back();
     return front();
}


std::vector<PupuID> MapHistory::calculate_pupu(const map_t &map)
{
     return map.visit_tiles(
       [](const auto &tiles)
       {
            std::vector<PupuID> pupu_ids = {};
            UniquifyPupu        pupu_map = {};
            pupu_ids.reserve(std::ranges::size(tiles));
            std::ranges::transform(
              tiles, std::back_insert_iterator(pupu_ids), pupu_map);
            return pupu_ids;
       });
}

MapHistory::MapHistory(map_t map)
  : m_front(std::move(map))
  , m_back(m_front)
  , m_front_pupu(calculate_pupu(m_front))
{
}
[[nodiscard]] const std::vector<PupuID> &MapHistory::pupu() const noexcept
{
     return m_front_pupu;
}
[[nodiscard]] std::size_t MapHistory::count() const
{
     return m_front_history.size() + m_back_history.size();
}
[[nodiscard]] std::size_t MapHistory::redo_count() const
{
     return m_redo_history.size();
}
[[nodiscard]] const MapHistory::map_t &MapHistory::front() const
{
     return m_front;
}
[[nodiscard]] MapHistory::map_t &MapHistory::back() const
{
     return m_back;
}

/**
 * For when a change could happen. we make a copy ahead of time.
 * @return back map
 */
[[nodiscard]] MapHistory::map_t &
  MapHistory::copy_back_preemptive(std::source_location source_location) const
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
void MapHistory::end_preemptive_copy_mode(
  std::source_location source_location) const
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
[[nodiscard]] MapHistory::map_t &MapHistory::copy_working() const
{
     auto &temp = safe_copy_working();
     if (!preemptive_copy_mode)
     {
          clear_redo();
     }
     return temp;
}
[[nodiscard]] MapHistory::map_t &MapHistory::safe_copy_working() const
{
     (void)debug_count_print();
     if (!preemptive_copy_mode)
     {
          m_front_or_back.push_back(pushed::Back);
          m_back_history.push_back(back());
     }
     return back();
}
void MapHistory::clear_redo() const
{
     m_redo_history.clear();
     m_redo_front_or_back.clear();
}
[[nodiscard]] bool MapHistory::remove_duplicate() const
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

/**
 * Deletes the most recent back or front
 * @return
 */
[[nodiscard]] bool MapHistory::redo(std::source_location source_location) const
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
[[nodiscard]] bool MapHistory::undo(
  bool                 skip_redo,
  std::source_location source_location) const
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
void MapHistory::undo_all(std::source_location source_location) const
{
     while (undo(false, source_location))
     {
     }
}
void MapHistory::redo_all(std::source_location source_location) const
{
     while (redo(source_location))
     {
     }
}
[[nodiscard]] bool MapHistory::redo_enabled() const
{
     return !m_redo_history.empty();
}
[[nodiscard]] bool MapHistory::undo_enabled() const
{
     return count() != 0U;
}
}// namespace ff_8