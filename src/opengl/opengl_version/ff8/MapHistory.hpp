//
// Created by pcvii on 6/2/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPHISTORY_HPP
#define FIELD_MAP_EDITOR_MAPHISTORY_HPP
namespace ff_8
{
template<typename TileT>
struct [[nodiscard]] PairOfTiles
{
  PairOfTiles() = default;
  PairOfTiles(const TileT &front, TileT &back)
    : m_front_tile(&front)
    , m_back_tile(&back)
  {
  }
  [[nodiscard]] const TileT &front_tile() const noexcept
  {
    return *m_front_tile;
  }
  [[nodiscard]] TileT &back_tile() noexcept
  {
    return *m_back_tile;
  }
  [[nodiscard]] const TileT &back_tile() const noexcept
  {
    return *m_back_tile;
  }

private:
  const TileT *m_front_tile;
  TileT       *m_back_tile;
};
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
  MapHistory() = default;
  MapHistory(MapT map)
  {
    m_maps.emplace_back(std::move(map));
    m_maps.push_back(m_maps.front());
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
    spdlog::debug("Map History Count: {}", m_maps.size());
    m_front_or_back.push_back(pushed_back);
    return m_maps.emplace_back(back());
  }
  [[nodiscard]] const MapT &copy_back_to_front() const
  {
    spdlog::debug("Map History Count: {}", m_maps.size());
    m_maps.insert(m_maps.begin(), back());
    m_front_or_back.push_back(pushed_front);
    return front();
  }
  [[nodiscard]] const MapT &copy_front() const
  {
    spdlog::debug("Map History Count: {}", m_maps.size());
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
    spdlog::debug("Map History Count: {}", m_maps.size());
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
  [[nodiscard]] auto visit_both(auto &&function) const
  {
    return visit_both(function, std::identity{});
  }
  [[nodiscard]] auto visit_both(auto &&function, auto &&filter) const
  {
    return front().visit_tiles(
      [this, &function, &filter](
        const std::ranges::contiguous_range auto &front_tiles) {
        back().visit_tiles([&front_tiles, &function, &filter](
                             std::ranges::contiguous_range auto &&back_tiles) {
          using TileT  = std::ranges::range_value_t<decltype(front_tiles)>;
          using BTileT = std::ranges::range_value_t<decltype(back_tiles)>;
          if constexpr (!std::is_same_v<TileT, BTileT>)
          {
            std::vector<PairOfTiles<TileT>> temp_mux        = {};
            auto                            temp_mux_filter = filter(temp_mux);
            return function(temp_mux_filter);
          }
          else
          {
            std::vector<PairOfTiles<TileT>> temp_mux = {};
            temp_mux.reserve(std::ranges::size(front_tiles));
            std::ranges::transform(
              front_tiles,
              back_tiles,
              std::back_inserter(temp_mux),
              [](const auto &front_tile, auto &back_tile) {
                return PairOfTiles<TileT>(front_tile, back_tile);
              });
            auto temp_mux_filter = filter(temp_mux);
            return function(temp_mux_filter);
          }
        });
      });
  }
  [[nodiscard]] bool visit_both_tiles(
    std::invocable auto &&function,
    std::invocable auto &&filter = std::identity{})
  {
    return visit_both(
      [&function](std::ranges::contiguous_range auto &&mux_tiles) -> bool {
        bool changed = false;
        for (auto &pair : mux_tiles)
        {
          if (function(std::as_const(pair.front_tile), pair.back_tile))
          {
            changed = true;
          }
        }
        return changed;
      },
      filter);
  }
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPHISTORY_HPP
