//
// Created by pcvii on 11/30/2021.
//

#include "Fields.hpp"
[[nodiscard]] open_viii::graphics::background::Mim ff8::LoadMim(
  open_viii::archive::FIFLFS<false> field,
  std::string_view                  coo,
  std::string                      &out_path,
  bool                             &coo_was_used)
{
  std::size_t out_path_pos = {};
  auto        lang_name =
    fmt::format("_{}{}", coo, open_viii::graphics::background::Mim::EXT);

  auto buffer = field.get_entry_data(
    { std::string_view(lang_name), open_viii::graphics::background::Mim::EXT },
    &out_path,
    &out_path_pos);
  coo_was_used = out_path_pos == 0U;
  if (!std::ranges::empty(buffer))
  {
    fmt::print("loaded: {}\n", out_path);
    auto mim =
      open_viii::graphics::background::Mim{ buffer, field.get_base_name() };
    return mim;
  }
  return {};
}
[[nodiscard]] open_viii::graphics::background::Map ff8::LoadMap(
  open_viii::archive::FIFLFS<false>           field,
  std::string_view                            coo,
  const open_viii::graphics::background::Mim &mim,
  std::string                                &out_path,
  bool                                       &coo_was_used)
{
  bool        shift        = true;
  std::size_t out_path_pos = {};
  auto        lang_name =
    fmt::format("_{}{}", coo, open_viii::graphics::background::Map::EXT);
  auto buffer = field.get_entry_data(
    { std::string_view(lang_name),
      open_viii::graphics::background::Map::Map::EXT },
    &out_path,
    &out_path_pos);
  coo_was_used = out_path_pos == 0U;// if true then the coo was picked.
  if (!std::ranges::empty(buffer))
  {
    fmt::print("loaded: {}\n", out_path);
    auto map =
      open_viii::graphics::background::Map{ mim.mim_type(), buffer, shift };
    return map;
  }
  return {};
}


bool ff8::Fields::OnImGuiUpdate() const
{
  int  id      = {};
  bool changed = { false };
  if (m_archive.OnImGuiUpdate())
  {
    m_map_data = m_archive.Fields().map_data();
    m_field    = load_field();
    changed    = true;
  }
  const ImGuiStyle &style   = ImGui::GetStyle();
  const float       spacing = style.ItemInnerSpacing.x;
  {
    const float w         = ImGui::CalcItemWidth();
    const float button_sz = ImGui::GetFrameHeight();
    ImGui::PushItemWidth(w - spacing * 2.0f - button_sz * 2.0f);
    const auto popwidth = scope_guard{ &ImGui::PopItemWidth };
    const auto disabled = scope_guard{ &ImGui::EndDisabled };
    ImGui::BeginDisabled(std::ranges::empty(m_map_data));
    if (ImGui::BeginCombo(
          "##Field", Map_Name().c_str(), ImGuiComboFlags_HeightLargest))
    {
      const auto end = scope_guard{ &ImGui::EndCombo };
      for (int i{}; const std::string &map : m_map_data)
      {
        const bool is_selected = i == m_current;
        const auto pop         = scope_guard{ &ImGui::PopID };
        ImGui::PushID(++id);
        if (ImGui::Selectable(map.c_str(), is_selected))
        {
          m_current = i;
          changed   = true;
          m_field   = load_field();
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
    ImGui::BeginDisabled(std::cmp_less_equal(m_current, 0));
    if (ImGui::ArrowButton("##l", ImGuiDir_Left))
    {
      --m_current;
      changed = true;
      m_field = load_field();
    }
  }
  {
    const auto pop = scope_guard{ &ImGui::PopID };
    ImGui::PushID(++id);
    ImGui::SameLine(0, spacing);
    const auto disabled = scope_guard{ &ImGui::EndDisabled };
    ImGui::BeginDisabled(
      std::cmp_greater_equal(m_current + 1, std::ranges::size(m_map_data)));
    if (ImGui::ArrowButton("##r", ImGuiDir_Right))
    {
      ++m_current;
      changed = true;
      m_field = load_field();
    }
  }
  ImGui::SameLine(0, spacing);
  ImGui::Text("%s", "Field");
  return changed;
}