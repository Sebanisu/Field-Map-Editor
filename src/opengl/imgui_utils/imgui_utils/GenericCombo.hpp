//
// Created by pcvii on 1/25/2022.
//

#ifndef FIELD_MAP_EDITOR_GENERICCOMBO_HPP
#define FIELD_MAP_EDITOR_GENERICCOMBO_HPP
#include "imgui.h"
#include "ImGuiDisabled.hpp"
#include "ImGuiPushID.hpp"
#include "ImGuiPushItemWidth.hpp"
#include <ranges>
#include <spdlog/spdlog.h>
#include <utility>
namespace imgui_utils
{
template<std::ranges::random_access_range dataT>

     requires(
       std::is_same_v<
         std::remove_cvref_t<std::ranges::range_value_t<dataT>>,
         std::string>
       || std::is_same_v<
         std::remove_cvref_t<std::ranges::range_value_t<dataT>>,
         std::string_view>)

[[nodiscard]] inline bool GenericCombo(
  const char  *label,
  int         &current_index,
  const dataT &data,
  float        sub_width = 0.F)
{
     bool              changed = false;
     const ImGuiStyle &style   = ImGui::GetStyle();
     const float       spacing = style.ItemInnerSpacing.x;
     {
          const auto  pop_0       = imgui_utils::ImGuiPushId();
          const float width       = ImGui::CalcItemWidth() - sub_width;
          const float button_size = ImGui::GetFrameHeight();
          const auto  pop_width   = imgui_utils::ImGuiPushItemWidth(
            width - spacing * 2.0f - button_size * 2.0f);
          const auto disabled
            = imgui_utils::ImGuiDisabled(std::ranges::empty(data));

          static constexpr auto c_str = [](auto &&v)
          {
               using StrT = std::remove_cvref_t<decltype(v)>;
               if constexpr (std::is_same_v<StrT, std::string_view>)
               {
                    return std::ranges::data(v);
               }
               else
                    return v.c_str();
          };
          const char *current_string = [&]()
          {
               if (std::ranges::empty(data))
               {
                    return "";
               }
               auto b = std::ranges::cbegin(data);
               std::ranges::advance(b, current_index);

               const char *current_c_str = c_str(*b);
               const auto &current_str   = *b;
               const auto  pos           = current_str.find_last_of("\\/");
               auto        length        = std::string_view(*b).size();
               if (
                 std::cmp_not_equal(pos, std::string::npos)
                 && std::cmp_not_equal(pos, length - 1))
               {
                    // show only end of long paths.
                    return current_c_str + pos + 1;
               }
               return current_c_str;
          }();
          if (ImGui::BeginCombo(
                "##Empty", current_string, ImGuiComboFlags_HeightLargest))
          {
               const auto end = glengine::ScopeGuard{ &ImGui::EndCombo };
               for (int i{}; const auto &map : data)
               {
                    if (c_str(map) == nullptr)
                    {
                         spdlog::warn(
                           "GenericCombo: encountered null c_str entry, "
                           "skipping, at index: {}",
                           i);
                         continue;
                    }
                    if (std::ranges::empty(map))
                    {
                         spdlog::warn(
                           "GenericCombo: encountered empty string entry, "
                           "skipping, at index: {}",
                           i);
                         continue;
                    }
                    const bool is_selected = i == current_index;
                    const auto pop_1       = imgui_utils::ImGuiPushId();
                    if (ImGui::Selectable(c_str(map), is_selected))
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
          const auto pop = imgui_utils::ImGuiPushId();
          ImGui::SameLine(0, spacing);
          const auto disabled = imgui_utils::ImGuiDisabled(
            std::cmp_less_equal(current_index, 0)
            || std::cmp_greater_equal(
              current_index - 1, std::ranges::size(data)));
          if (ImGui::ArrowButton("##l", ImGuiDir_Left))
          {
               --current_index;
               changed = true;
          }
     }
     {
          const auto pop = imgui_utils::ImGuiPushId();
          ImGui::SameLine(0, spacing);
          const auto disabled = imgui_utils::ImGuiDisabled(
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
}// namespace imgui_utils
#endif// FIELD_MAP_EDITOR_GENERICCOMBO_HPP
