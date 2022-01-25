//
// Created by pcvii on 1/25/2022.
//

#ifndef FIELD_MAP_EDITOR_GENERICCOMBO_HPP
#define FIELD_MAP_EDITOR_GENERICCOMBO_HPP
#include "ImGuiDisabled.hpp"
#include "ImGuiPushID.hpp"
#include "ImGuiPushItemWidth.hpp"
namespace glengine
{
template<std::ranges::random_access_range dataT>
// clang-format off
requires(
  std::is_same_v<
    std::decay_t<std::ranges::range_value_t<dataT>>,
    std::string>)
  // clang-format on
  inline bool GenericCombo(
    const char  *label,
    int         &current_index,
    const dataT &data)
{
  bool              changed = false;
  const ImGuiStyle &style   = ImGui::GetStyle();
  const float       spacing = style.ItemInnerSpacing.x;
  {
    const auto  pop0        = glengine::ImGuiPushID();
    const float width       = ImGui::CalcItemWidth();
    const float button_size = ImGui::GetFrameHeight();
    const auto  pop_width =
      glengine::ImGuiPushItemWidth(width - spacing * 2.0f - button_size * 2.0f);
    const auto  disabled = glengine::ImGuiDisabled(std::ranges::empty(data));
    const char *current_string = [&]() {
      if (std::ranges::empty(data))
      {
        return "";
      }
      auto b = std::ranges::cbegin(data);
      std::ranges::advance(b, current_index);
      const char * current_c_str = b->c_str();
      const std::string & current_str = *b;
      const auto pos = current_str.find_last_of("\\/");
      if(pos != std::string::npos)
      {
        //show only end of long paths.
        return current_c_str + pos+1;
      }
      return current_c_str;
    }();
    if (ImGui::BeginCombo(
          "##Empty", current_string, ImGuiComboFlags_HeightLargest))
    {
      const auto end = glengine::scope_guard{ &ImGui::EndCombo };
      for (int i{}; const std::string &map : data)
      {
        const bool is_selected = i == current_index;
        const auto pop1        = glengine::ImGuiPushID();
        if (ImGui::Selectable(map.c_str(), is_selected))
        {
          current_index = i;
          changed       = true;
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
    }
  }
  {
    const auto pop = glengine::ImGuiPushID();
    ImGui::SameLine(0, spacing);
    const auto disabled = glengine::ImGuiDisabled(
      std::cmp_greater_equal(current_index + 1, std::ranges::size(data)));
    if (ImGui::ArrowButton("##r", ImGuiDir_Right))
    {
      ++current_index;
      changed = true;
    }
  }
  ImGui::SameLine(0, spacing);
  ImGui::Text("%s", label);
  return changed;
}
}// namespace glengine
#endif// FIELD_MAP_EDITOR_GENERICCOMBO_HPP
