//
// Created by pcvii on 11/30/2021.
//

#include "Fields.hpp"
#include "ImGuiDisabled.hpp"
#include "ImGuiPushID.hpp"
#include "ImGuiPushItemWidth.hpp"

namespace ff8
{
static int current_index = {};
}

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
  bool        shift        = false;
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
  bool changed = { false };
  starttime    = std::chrono::steady_clock::now();
  if (m_archive.OnImGuiUpdate())
  {
    m_map_data = m_archive.Fields().map_data();
    m_field    = load_field();
    changed    = true;
    endtime    = std::chrono::steady_clock::now();
    fmt::print("time to load fields = {:%S} seconds\n", endtime - starttime);
  }
  const ImGuiStyle &style   = ImGui::GetStyle();
  const float       spacing = style.ItemInnerSpacing.x;
  {
    const float w         = ImGui::CalcItemWidth();
    const float button_sz = ImGui::GetFrameHeight();
    const auto  popwidth =
      glengine::ImGuiPushItemWidth(w - spacing * 2.0f - button_sz * 2.0f);
    const auto disabled =
      glengine::ImGuiDisabled(std::ranges::empty(m_map_data));
    if (ImGui::BeginCombo(
          "##Field", Map_Name().c_str(), ImGuiComboFlags_HeightLargest))
    {
      const auto end = glengine::scope_guard{ &ImGui::EndCombo };
      for (int i{}; const std::string &map : m_map_data)
      {
        const bool is_selected = i == current_index;
        const auto pop         = glengine::ImGuiPushID();
        if (ImGui::Selectable(map.c_str(), is_selected))
        {
          current_index = i;
          changed       = true;
          m_field       = load_field();
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
    const auto pop = glengine::ImGuiPushID();
    ImGui::SameLine(0, spacing);
    const auto disabled =
      glengine::ImGuiDisabled(std::cmp_less_equal(current_index, 0));
    if (ImGui::ArrowButton("##l", ImGuiDir_Left))
    {
      --current_index;
      changed = true;
      m_field = load_field();
    }
  }
  {
    const auto pop = glengine::ImGuiPushID();
    ImGui::SameLine(0, spacing);
    const auto disabled = glengine::ImGuiDisabled(
      std::cmp_greater_equal(current_index + 1, std::ranges::size(m_map_data)));
    if (ImGui::ArrowButton("##r", ImGuiDir_Right))
    {
      ++current_index;
      changed = true;
      m_field = load_field();
    }
  }
  ImGui::SameLine(0, spacing);
  ImGui::Text("%s", "Field");
  return changed;
}

open_viii::archive::FIFLFS<false> ff8::Fields::load_field() const
{
  open_viii::archive::FIFLFS<false> archive{};
  if (!m_map_data.empty() && std::cmp_less(current_index, m_map_data.size()))
  {
    m_archive.Fields().execute_with_nested(
      { Map_Name() },
      [&archive](auto &&field) {
        archive = std::forward<decltype(field)>(field);
      },
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
      current_index,
      m_map_data.size());
  }
  return archive;
}

std::string_view ff8::Fields::Coo() const
{
  return m_archive.Coo();
}

const std::string &ff8::Fields::Map_Name() const
{
  if (std::cmp_less(current_index, std::ranges::size(m_map_data)))
  {
    return m_map_data[static_cast<std::size_t>(current_index)];
  }
  const static auto tmp = std::string("");
  return tmp;
}

ff8::Fields::Fields()
  : m_map_data(m_archive.Fields().map_data())
  , m_field(load_field())
{
  fmt::print("time to load fields = {:%S} seconds\n", endtime - starttime);
}

const open_viii::archive::FIFLFS<false> &ff8::Fields::Field() const
{
  return m_field;
}
