//
// Created by pcvii on 6/2/2022.
//

#include "MapHistory.hpp"

using map_t = open_viii::graphics::background::Map;
namespace ff_8
{
static std::vector<PupuID> calculate_pupu(const map_t &map)
{
  return map.visit_tiles([](const auto &tiles) {
    std::vector<PupuID> pupu_ids = {};
    UniquifyPupu        const pupu_map = {};
    pupu_ids.reserve(std::ranges::size(tiles));
    std::ranges::transform(
      tiles, std::back_insert_iterator(pupu_ids), pupu_map);
    return pupu_ids;
  });
}
}// namespace ff_8

ff_8::MapHistory::MapHistory(map_t map)
  : m_front(std::move(map))
  , m_back(m_front)
  , m_front_pupu(calculate_pupu(m_front))
{
}
[[nodiscard]] const std::vector<PupuID> &ff_8::MapHistory::pupu() const noexcept
{
  return m_front_pupu;
}

[[nodiscard]] std::size_t ff_8::MapHistory::count() const
{
  return m_front_history.size() + m_back_history.size();
}
[[nodiscard]] std::size_t ff_8::MapHistory::redo_count() const
{
  return m_redo_history.size();
}
[[nodiscard]] const map_t &ff_8::MapHistory::front() const
{
  return m_front;
}
[[nodiscard]] map_t &ff_8::MapHistory::back()
{
  return m_back;
}

[[nodiscard]] const map_t &ff_8::MapHistory::back() const
{
  return m_back;
}
/**
 * For when a change could happen. we make a copy ahead of time.
 * @return back map
 */
[[nodiscard]] map_t &ff_8::MapHistory::copy_back_preemptive()
{
  if (!preemptive_copy_mode)
  {
    auto &temp           = safe_copy_back();
    preemptive_copy_mode = true;
    spdlog::debug(
      "Map History preemptive_copy_mode: {}\n\t{}:{}",
      preemptive_copy_mode,
      __FILE__,
      __LINE__);
    return temp;
  }
  return back();
}
/**
 * After copy_mode is returned to normal copy_back_preemptive will resume
 * making copies.
 */
void ff_8::MapHistory::end_preemptive_copy_mode() const
{
  if (preemptive_copy_mode)
  {
    preemptive_copy_mode = false;
    spdlog::debug(
      "Map History preemptive_copy_mode: {}\n\t{}:{}",
      preemptive_copy_mode,
      __FILE__,
      __LINE__);
  }
}
[[nodiscard]] map_t &ff_8::MapHistory::copy_back()
{
  auto &temp = safe_copy_back();
  if (!preemptive_copy_mode)
  {
    clear_redo();
  }
  return temp;
}
[[nodiscard]] map_t &ff_8::MapHistory::safe_copy_back()
{
  (void)debug_count_print();
  if (!preemptive_copy_mode)
  {
    m_front_or_back.push_back(pushed::back);
    m_back_history.push_back(back());
  }
  return back();
}
void ff_8::MapHistory::clear_redo()
{
  m_redo_history.clear();
  m_redo_front_or_back.clear();
}
[[nodiscard]] bool ff_8::MapHistory::remove_duplicate()
{
  bool ret = false;
  while (!undo_enabled() &&
         ((m_front_or_back.back() == pushed::back
           && m_back_history.back() == m_back)
          || (m_front_or_back.back() == pushed::front
              && m_front_history.back() == m_front)))
  {
    (void)undo(true);
    ret = true;
  }
  return ret;
}
[[nodiscard]] const map_t &ff_8::MapHistory::copy_back_to_front()
{
  if (!preemptive_copy_mode)
  {
    clear_redo();
  }
  const auto count = debug_count_print();
  m_front_history.push_back(front());
  m_front_or_back.push_back(pushed::front);
  // todo do we want to recalculate the pupu?
  m_front = back();
  return front();
}
/**
 * Deletes the most recent back or front
 * @return
 */
[[nodiscard]] bool ff_8::MapHistory::redo()
{
  const auto count = debug_count_print();
  if (!redo_enabled())
  {
    return false;
  }
  pushed const last = m_redo_front_or_back.back();
  m_front_or_back.push_back(last);
  m_redo_front_or_back.pop_back();
  if (last == pushed::back)
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
[[nodiscard]] bool ff_8::MapHistory::undo(bool skip_redo)
{
  const auto count = debug_count_print();
  if (!undo_enabled())
  {
    return false;
  }
  pushed const last = m_front_or_back.back();
  if (!skip_redo)
  {
    m_redo_front_or_back.push_back(last);
  }
  m_front_or_back.pop_back();
  if (last == pushed::back)
  {
    (void)undo_back(skip_redo);
    return true;
  }
  (void)undo_front(skip_redo);
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
[[nodiscard]] bool ff_8::MapHistory::redo_enabled() const
{
  return !m_redo_history.empty();
}
[[nodiscard]] bool ff_8::MapHistory::undo_enabled() const
{
  return count() != 0U;
}
const map_t &ff_8::MapHistory::const_back() const
{
  return m_back;
}
/**
* Should only be called by undo() pops front
* @return returns new front
*/
const map_t &ff_8::MapHistory::undo_front(bool skip_redo)
{
  if (!skip_redo)
  {
    m_redo_history.emplace_back(std::move(m_front));
  }
  m_front = std::move(m_front_history.back());
  m_front_history.pop_back();
  return front();
}
/**
   * Should only be called by undo() pops back
   * @return returns new back
 */
map_t &ff_8::MapHistory::undo_back(bool skip_redo)
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
const map_t &ff_8::MapHistory::redo_front()
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
map_t &ff_8::MapHistory::redo_back()
{
  m_back_history.emplace_back(std::move(m_back));
  m_back = std::move(m_redo_history.back());
  m_redo_history.pop_back();
  return back();
}
