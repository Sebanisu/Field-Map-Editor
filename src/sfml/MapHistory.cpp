//
// Created by pcvii on 6/2/2022.
//

#include "MapHistory.hpp"

using namespace open_viii::graphics::background;
using map_t = Map;

namespace ff_8
{
static std::vector<PupuID> calculate_pupu(const map_t &map)
{
     return map.visit_tiles([](const auto &tiles) {
          std::vector<PupuID> pupu_ids = {};
          UniquifyPupu const  pupu_map = {};
          pupu_ids.reserve(std::ranges::size(tiles));
          std::ranges::transform(tiles, std::back_insert_iterator(pupu_ids), pupu_map);
          return pupu_ids;
     });
}
}// namespace ff_8

ff_8::MapHistory::MapHistory(map_t map)
  : m_original(std::move(map))
  , m_working(m_original)
  , m_original_pupu(calculate_pupu(m_original))
  , m_working_pupu(m_original_pupu)
{
}

void ff_8::MapHistory::refresh_all_pupu()
{
     refresh_original_pupu();
     refresh_working_pupu();
}

void ff_8::MapHistory::refresh_original_pupu()
{
     m_original_pupu = calculate_pupu(m_original);
}

void ff_8::MapHistory::refresh_working_pupu()
{
     m_working_pupu = calculate_pupu(m_working);
}

const std::vector<PupuID> &ff_8::MapHistory::original_pupu() const noexcept
{
     return m_original_pupu;
}

const std::vector<PupuID> &ff_8::MapHistory::working_pupu() const noexcept
{
     return m_working_pupu;
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
     m_original = working();
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
          m_working = std::move(m_redo_history.back());
          m_redo_history.pop_back();
          return true;
     }
     (void)m_undo_history.emplace_back(std::move(m_original));
     m_original = std::move(m_redo_history.back());
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
          m_working = std::move(m_undo_history.back());
          m_undo_history.pop_back();
          return true;
     }
     if (!skip_redo)
     {
          m_redo_history.emplace_back(std::move(m_original));
     }
     m_original = std::move(m_undo_history.back());
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
[[nodiscard]] PupuID ff_8::MapHistory::get_pupu_from_working(const TileT &tile) const
{
     return m_original_pupu[static_cast<std::size_t>(get_offset_from_working(tile))];
}

// Explicit instantiation for Tiles
template PupuID ff_8::MapHistory::get_pupu_from_working(const Tile1 &) const;
template PupuID ff_8::MapHistory::get_pupu_from_working(const Tile2 &) const;
template PupuID ff_8::MapHistory::get_pupu_from_working(const Tile3 &) const;

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


std::string_view                             ff_8::MapHistory::undo_description() const
{
     if (std::ranges::empty(m_undo_change_descriptions))
     {
          return {};
     }
     return m_undo_change_descriptions.back();
}

std::string_view ff_8::MapHistory::redo_description() const
{
     if (std::ranges::empty(m_redo_change_descriptions))
     {
          return {};
     }
     return m_redo_change_descriptions.back();
}

[[nodiscard]] ff_8::MapHistory::pushed ff_8::MapHistory::undo_pushed() const
{
     if (std::ranges::empty(m_undo_original_or_working))
     {
          return {};
     }
     return m_undo_original_or_working.back();
}

[[nodiscard]] ff_8::MapHistory::pushed ff_8::MapHistory::redo_pushed() const
{
     if (std::ranges::empty(m_redo_original_or_working))
     {
          return {};
     }
     return m_redo_original_or_working.back();
}