//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_FIELDS_HPP
#define MYPROJECT_FIELDS_HPP
#include "Archive.hpp"
#include "fmt/format.h"
#include <open_viii/graphics/background/Map.hpp>
#include <open_viii/graphics/background/Mim.hpp>
namespace ff8
{
class Fields
{
public:
  friend void
    OnUpdate(const Fields &, float);
  friend void
    OnRender(const Fields &);
  friend bool
    OnImGuiRender(const Fields &);
  [[nodiscard]] const open_viii::archive::FIFLFS<false> &
    Field() const
  {
    return m_field;
  }
  [[nodiscard]] const std::string &
    Map_Name() const
  {
    if (std::cmp_less(m_current, std::ranges::size(m_map_data)))
    {
      return m_map_data.at(static_cast<std::size_t>(m_current));
    }
    const static auto tmp = std::string("");
    return tmp;
  }
  [[nodiscard]] std::string_view
    Coo() const
  {
    return m_archive.Coo();
  }
  [[nodiscard]] open_viii::graphics::background::Mim
    Mim(std::string &out_path, bool &coo) const
  {
    std::size_t out_path_pos = {};
    auto        lang_name =
      fmt::format("_{}{}", Coo(), open_viii::graphics::background::Mim::EXT);

    auto buffer = m_field.get_entry_data(
      { std::string_view(lang_name),
        open_viii::graphics::background::Mim::EXT },
      &out_path,
      &out_path_pos);
    coo = out_path_pos == 0U;// if true then the coo was picked.
    if (!std::ranges::empty(buffer))
    {
      fmt::print("loaded: {}\n",out_path);
      auto mim =
        open_viii::graphics::background::Mim{ buffer, m_field.get_base_name() };
      return mim;
    }
    return {};
  }
  [[nodiscard]] open_viii::graphics::background::Map
    Map(
      const open_viii::graphics::background::Mim &mim,
      std::string                                &out_path,
      bool                                       &coo) const
  {
    bool        shift        = true;
    std::size_t out_path_pos = {};
    auto        lang_name =
      fmt::format("_{}{}", Coo(), open_viii::graphics::background::Map::EXT);
    auto buffer = m_field.get_entry_data(
      { std::string_view(lang_name),
        open_viii::graphics::background::Map::Map::EXT },
      &out_path,
      &out_path_pos);
    coo = out_path_pos == 0U;// if true then the coo was picked.
    if (!std::ranges::empty(buffer))
    {
      fmt::print("loaded: {}\n",out_path);
      auto map =
        open_viii::graphics::background::Map{ mim.mim_type(), buffer, shift };
      return map;
    }
    return {};
  }
  void
    init() const
  {
    m_archive.init();
    m_map_data = m_archive.Fields().map_data();
    m_field    = load_field();
  }

private:
  open_viii::archive::FIFLFS<false>
    load_field() const
  {
    open_viii::archive::FIFLFS<false> archive{};
    if (!m_map_data.empty() && std::cmp_less(m_current, m_map_data.size()))
    {
      m_archive.Fields().execute_with_nested(
        { Map_Name() },
        [&archive](auto &&field)
        { archive = std::forward<decltype(field)>(field); },
        {},
        [](auto &&) { return true; },
        true);
    }
    else
    {
      fmt::print(
        stderr,
        "{}:{} - Index out of range {} / {}\n",
        __FILE__,
        __LINE__,
        m_current,
        m_map_data.size());
    }
    return archive;
  }
  Archive                                   m_archive  = {};
  mutable std::vector<std::string>          m_map_data = {};
  mutable open_viii::archive::FIFLFS<false> m_field    = {};
  mutable int                               m_current  = {};
};
static_assert(test::Test<Fields>);

inline void
  OnUpdate(const Fields &, float)
{
}
inline void
  OnRender(const Fields &)
{
}
inline bool
  OnImGuiRender(const Fields &self)
{
  bool changed = { false };
  if (OnImGuiRender(self.m_archive))
  {
    self.m_map_data = self.m_archive.Fields().map_data();
    self.m_field    = self.load_field();
    changed         = true;
  }
  if (ImGui::BeginCombo("Field", self.Map_Name().c_str()))
  {
    int        id  = {};
    const auto end = scope_guard{ &ImGui::EndCombo };
    for (int i{}; const std::string &map : self.m_map_data)
    {
      const bool is_selected = i == self.m_current;
      const auto pop         = scope_guard{ &ImGui::PopID };
      ImGui::PushID(++id);
      if (ImGui::Selectable(map.c_str(), is_selected))
      {
        self.m_current = i;
        changed        = true;
        self.m_field   = self.load_field();
      }
      if (is_selected)
      {
        ImGui::SetItemDefaultFocus();
      }
      ++i;
    }
  }
  return changed;
}
}// namespace ff8
#endif// MYPROJECT_FIELDS_HPP
