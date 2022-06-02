//
// Created by pcvii on 6/2/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPHISTORY_HPP
#define FIELD_MAP_EDITOR_MAPHISTORY_HPP
namespace ff8
{
class [[nodiscard]] MapHistory
{
  using MapT                       = open_viii::graphics::background::Map;
  mutable std::vector<MapT> m_maps = {};
  mutable std::vector<bool> m_front_or_back      = {};
  // mutable std::vector<std::string> m_changes       = {};
  static constexpr bool     pushed_front         = false;
  static constexpr bool     pushed_back          = false;
  mutable bool              preemptive_copy_mode = false;
  /**
   * Should only be called by undo() pops back
   * @return returns new back
   */
  [[nodiscard]] MapT       &pop_back() const
  {
    m_maps.pop_back();
    return back();
  }
  /**
   * Should only be called by undo() pops front
   * @return returns new front
   */
  [[nodiscard]] const MapT &pop_front() const
  {
    m_maps.erase(m_maps.begin());
    return front();
  }

public:
  MapHistory(MapT map)
  {
    m_maps.push_back(map);
    m_maps.emplace_back(std::move(map));
  }
  [[nodiscard]] const MapT &front() const
  {
    return m_maps.front();
  }
  [[nodiscard]] MapT &back() const
  {
    return m_maps.back();
  }
  /**
   * For when a change could happen. we make a copy ahead of time.
   * @return back map
   */
  [[nodiscard]] MapT &copy_back_preemptive() const
  {
    if (!preemptive_copy_mode)
    {
      preemptive_copy_mode = true;
      return copy_back();
    }
    return back();
  }
  /**
   * After copy_mode is returned to normal copy_back_preemptive will resume
   * making copies.
   */
  void end_preemptive_copy_mode() const
  {
    preemptive_copy_mode = false;
  }
  [[nodiscard]] MapT &copy_back() const
  {
    if (preemptive_copy_mode)
    {// someone already copied
      end_preemptive_copy_mode();
      return back();
    }
    m_front_or_back.push_back(pushed_back);
    return m_maps.emplace_back(back());
  }
  [[nodiscard]] const MapT &copy_back_to_front() const
  {
    m_maps.insert(m_maps.begin(), back());
    m_front_or_back.push_back(pushed_front);
    return front();
  }
  [[nodiscard]] const MapT &copy_front() const
  {
    m_maps.insert(m_maps.begin(), front());
    m_front_or_back.push_back(pushed_front);
    return front();
  }
  /**
   * Deletes the most recent back or front
   * @return
   */
  [[nodiscard]] bool undo() const
  {
    if (!undo_enabled())
    {
      return false;
    }
    bool last = m_front_or_back.back();
    m_front_or_back.pop_back();
    if (last == pushed_back)
    {
      (void)pop_back();
      return true;
    }
    (void)pop_front();
    return true;
  }
  [[nodiscard]] bool undo_enabled() const
  {
    return m_maps.size() > 2U;
  }
  [[nodiscard]] auto VisitBoth(std::invocable auto &&function)
  {
    return front().visit_tiles(
      [this, &function](const std::ranges::contiguous_range auto &front_tiles) {
        back().visit_tiles([&front_tiles, &function](
                             std::ranges::contiguous_range auto &&back_tiles) {
          return function(
            front_tiles, std::forward<decltype(back_tiles)>(back_tiles));
        });
      });
  }
  [[nodiscard]] bool VisitBothTiles(std::invocable auto &&function)
  {
    return VisitBoth(
      [&function](
        const std::ranges::contiguous_range auto &front_tiles,
        std::ranges::contiguous_range auto      &&back_tiles) -> bool {
        bool       changed     = false;
        auto       front_begin = std::ranges::cbegin(front_tiles);
        auto       back_begin  = std::ranges::begin(back_tiles);
        const auto front_end   = std::ranges::cend(front_tiles);
        const auto back_end    = std::ranges::end(back_tiles);
        for (; front_begin != front_end && back_begin != back_end;
             (void)++front_begin, ++back_begin)
        {
          if (function(*front_begin, *back_begin))
          {
            changed = true;
          }
        }
        return changed;
      });
  }
};
}// namespace ff8
#endif// FIELD_MAP_EDITOR_MAPHISTORY_HPP
