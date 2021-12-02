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
[[nodiscard]] open_viii::graphics::background::Mim
  LoadMim(
    open_viii::archive::FIFLFS<false> field,
    std::string_view                  coo,
    std::string                      &out_path,
    bool                             &coo_was_used);
[[nodiscard]] open_viii::graphics::background::Map
  LoadMap(
    open_viii::archive::FIFLFS<false>           field,
    std::string_view                            coo,
    const open_viii::graphics::background::Mim &mim,
    std::string                                &out_path,
    bool                                       &coo_was_used);
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
  inline static int                         m_current  = {};
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
  int  id      = {};
  bool changed = { false };
  if (OnImGuiRender(self.m_archive))
  {
    self.m_map_data = self.m_archive.Fields().map_data();
    self.m_field    = self.load_field();
    changed         = true;
  }
  const ImGuiStyle &style   = ImGui::GetStyle();
  const float       spacing = style.ItemInnerSpacing.x;
  {
    const float w         = ImGui::CalcItemWidth();
    const float button_sz = ImGui::GetFrameHeight();
    ImGui::PushItemWidth(w - spacing * 2.0f - button_sz * 2.0f);
    const auto popwidth = scope_guard{ &ImGui::PopItemWidth };
    const auto disabled = scope_guard{ &ImGui::EndDisabled };
    ImGui::BeginDisabled(std::ranges::empty(self.m_map_data));
    if (ImGui::BeginCombo(
          "##Field", self.Map_Name().c_str(), ImGuiComboFlags_HeightLargest))
    {
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
  }
  {
    const auto pop = scope_guard{ &ImGui::PopID };
    ImGui::PushID(++id);
    ImGui::SameLine(0, spacing);
    const auto disabled = scope_guard{ &ImGui::EndDisabled };
    ImGui::BeginDisabled(std::cmp_less_equal(self.m_current, 0));
    if (ImGui::ArrowButton("##l", ImGuiDir_Left))
    {
      --self.m_current;
      changed      = true;
      self.m_field = self.load_field();
    }
  }
  {
    const auto pop = scope_guard{ &ImGui::PopID };
    ImGui::PushID(++id);
    ImGui::SameLine(0, spacing);
    const auto disabled = scope_guard{ &ImGui::EndDisabled };
    ImGui::BeginDisabled(std::cmp_greater_equal(
      self.m_current + 1, std::ranges::size(self.m_map_data)));
    if (ImGui::ArrowButton("##r", ImGuiDir_Right))
    {
      ++self.m_current;
      changed      = true;
      self.m_field = self.load_field();
    }
  }
  ImGui::SameLine(0, spacing);
  ImGui::Text("%s", "Field");
  return changed;
}
}// namespace ff8
#endif// MYPROJECT_FIELDS_HPP
