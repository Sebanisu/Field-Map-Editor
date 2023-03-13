//
// Created by pcvii on 11/15/2021.
//

#ifndef FIELD_MAP_EDITOR_GENERIC_COMBO_HPP
#define FIELD_MAP_EDITOR_GENERIC_COMBO_HPP
#include "gui/gui_labels.hpp"
#include "scope_guard.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <imgui.h>
#include <ranges>
#include <spdlog/spdlog.h>
#include <utility>
namespace fme
{
template<typename T>
concept returns_range_concept = requires(std::remove_cvref_t<T> callable) {
                                     {
                                          callable()
                                     } -> std::ranges::range;
                                };
template<typename T>
concept filter_concept = requires(std::remove_cvref_t<T> filter) {
                              {
                                   filter.enabled()
                              } -> std::convertible_to<bool>;
                              {
                                   filter.update(filter.value())
                              } -> std::convertible_to<T>;
                              {
                                   filter.enable()
                              } -> std::convertible_to<T>;
                              {
                                   filter.disable()
                              } -> std::convertible_to<T>;
                         };
template<typename T>
concept returns_filter_concept = requires(std::remove_cvref_t<T> callable) {
                                      {
                                           callable()
                                      } -> filter_concept;
                                 };
template<
  returns_range_concept  value_lambdaT,
  returns_range_concept  string_lambdaT,
  returns_range_concept  tool_tip_lambda_t,
  returns_filter_concept filter_lambdaT>
inline static bool generic_combo(
  int                &imgui_id,
  std::string_view    name,
  value_lambdaT     &&value_lambda,
  string_lambdaT    &&string_lambda,
  tool_tip_lambda_t &&tool_tip_lambda,
  filter_lambdaT    &&filter_lambda)
{
     bool                                                     changed     = false;
     const auto                                              &values      = std::invoke(std::forward<value_lambdaT>(value_lambda));
     auto                                                   &&strings     = std::invoke(std::forward<string_lambdaT>(string_lambda));
     auto                                                   &&tooltips    = std::invoke(std::forward<tool_tip_lambda_t>(tool_tip_lambda));
     auto                                                    &filter      = std::invoke(std::forward<filter_lambdaT>(filter_lambda));
     bool                                                     checked     = filter.enabled();
     const ImGuiStyle                                        &style       = ImGui::GetStyle();
     const float                                              spacing     = style.ItemInnerSpacing.x;
     static std::ranges::range_difference_t<decltype(values)> current_idx = {};
     if (const auto found = std::find(values.begin(), values.end(), filter.value()); found != values.end())
     {
          current_idx = std::ranges::distance(std::ranges::cbegin(values), found);
     }
     else
     {
          current_idx = 0;
          if (!std::ranges::empty(values))
          {
               filter.update(values.front());
          }
          changed = true;
     }
     if (std::ranges::empty(values) || std::ranges::empty(strings))
     {
          if (checked)
          {
               filter.disable();
               return true;
          }
          return false;
     }
     const auto next = [](const auto &range, const auto &idx) {
          // sometimes the types are different. So I had to static cast to silence
          // warning.
          return std::ranges::next(
            std::ranges::cbegin(range), static_cast<std::iter_difference_t<decltype(std::ranges::cbegin(range))>>(idx));
     };
     const auto current_item = next(strings, current_idx);
     //  static constexpr auto pattern      = "{}: \t{} \t{}";
     {
          const auto pop_id = scope_guard{ &ImGui::PopID };
          ImGui::PushID(++imgui_id);
          if (ImGui::Checkbox("", &checked))
          {
               if (checked)
               {
                    filter.enable();
                    //        spdlog::info(
                    //          pattern, gui_labels::enable, name, *next(values,
                    //          current_idx));
               }
               else
               {
                    filter.disable();
                    //        spdlog::info(
                    //          pattern, gui_labels::disable, name, *next(values,
                    //          current_idx));
               }
               changed = true;
          }
     }
     ImGui::SameLine(0, spacing);
     const auto old = current_idx;
     {
          const auto pop_id = scope_guard{ &ImGui::PopID };
          ImGui::PushID(++imgui_id);
          const auto &string_like  = *current_item;

          const float width        = ImGui::CalcItemWidth();
          const float button_size  = ImGui::GetFrameHeight();
          const float button_count = 3.0F;
          ImGui::PushItemWidth(width - spacing * button_count - button_size * button_count);
          const auto pop_item_width = scope_guard(&ImGui::PopItemWidth);
          if (ImGui::BeginCombo("##Empty", string_like.data(), ImGuiComboFlags_HeightLarge))
          // The second parameter is the label previewed
          // before opening the combo.
          {
               std::ranges::for_each(strings, [&, index = 0U](const auto &string) mutable {
                    const bool is_selected = (string_like == string);
                    const auto iterate     = scope_guard([&]() { ++index; });
                    // You can store your selection however you
                    // want, outside or inside your objects
                    {
                         const auto pop_id_2 = scope_guard{ &ImGui::PopID };
                         ImGui::PushID(++imgui_id);
                         if (ImGui::Selectable(string.data(), is_selected))
                         {
                              current_idx = index;// std::distance(std::ranges::data(strings), &string);
                              changed     = true;
                         }
                    }
                    if (is_selected)
                    {
                         ImGui::SetItemDefaultFocus();
                         // You may set the initial focus when
                         // opening the combo (scrolling + for
                         // keyboard navigation support)
                    }
                    if (!ImGui::IsItemHovered())
                    {
                         return;
                    }
                    const auto &tooltip = *next(tooltips, index);
                    if (std::ranges::empty(tooltip))
                    {
                         return;
                    }
                    const auto pop_tool_tip = scope_guard{ &ImGui::EndTooltip };
                    ImGui::BeginTooltip();
                    format_imgui_text("{}", tooltip);
               });
               //      if (old != current_idx)
               //      {
               //        spdlog::info(pattern, gui_labels::set, name, *next(values,
               //        current_idx));
               //      }
               ImGui::EndCombo();
          }
          {

               const auto pop_id_left = scope_guard{ &ImGui::PopID };
               ImGui::SameLine(0, spacing);
               ImGui::PushID(++imgui_id);
               const bool disabled =
                 std::cmp_less_equal(current_idx, 0) || std::cmp_greater_equal(current_idx - 1, std::ranges::size(values));
               ImGui::BeginDisabled(disabled);
               if (ImGui::ArrowButton("##l", ImGuiDir_Left))
               {
                    --current_idx;
                    changed = true;
               }
               ImGui::EndDisabled();
          }
          {
               const auto pop_id_right = scope_guard{ &ImGui::PopID };
               ImGui::PushID(++imgui_id);
               ImGui::SameLine(0, spacing);
               const bool disabled = std::cmp_greater_equal(current_idx + 1, std::ranges::size(values));
               ImGui::BeginDisabled(disabled);
               if (ImGui::ArrowButton("##r", ImGuiDir_Right))
               {
                    ++current_idx;
                    changed = true;
               }
               ImGui::EndDisabled();
          }
          ImGui::SameLine(0, spacing);
          format_imgui_text("{}", name);
     }
     auto      &update        = filter.update(*next(values, current_idx));
     const auto index_changed = old != current_idx;
     if (index_changed)
     {
          update.enable();
     }
     return (update.enabled() && index_changed) || changed;
}
template<returns_range_concept value_lambdaT, typename string_lambdaT, typename valueT>
// requires
// std::same_as<std::decay<std::ranges::range_value_t<std::invoke_result_t<value_lambdaT>>>,valueT>
inline static bool
  generic_combo(int &imgui_id, std::string_view name, value_lambdaT &&value_lambda, string_lambdaT &&string_lambda, valueT &value)
{
     bool                                                     changed     = false;
     auto                                                   &&values      = std::invoke(std::forward<value_lambdaT>(value_lambda));
     auto                                                   &&strings     = std::invoke(std::forward<string_lambdaT>(string_lambda));
     const ImGuiStyle                                        &style       = ImGui::GetStyle();
     const float                                              spacing     = style.ItemInnerSpacing.x;
     // auto        strings = string_lambda() | std::ranges::views::all;
     static std::ranges::range_difference_t<decltype(values)> current_idx = {};
     {
          if (const auto found = std::find(values.begin(), values.end(), value); found != values.end())
          {
               current_idx = std::ranges::distance(std::ranges::cbegin(values), found);
          }
          else
          {
               current_idx = 0;
               if (!std::empty(values))
               {
                    value = values.front();
               }
               changed = true;
          }
     }
     if (std::empty(values) || std::empty(strings))
     {
          return false;
     }
     const auto next = [](const auto &range, const auto &idx) {
          // sometimes the types are different. So I had to static cast to silence
          // warning.
          return std::ranges::next(
            std::ranges::cbegin(range), static_cast<std::iter_difference_t<decltype(std::ranges::cbegin(range))>>(idx));
     };
     const auto           &current_item = *next(strings, current_idx);
     static constexpr auto pattern      = "{}: \t{}\t{}\t{}";
     const auto            old          = current_idx;
     {
          const auto pop_id = scope_guard{ &ImGui::PopID };
          ImGui::PushID(++imgui_id);
          const float width        = ImGui::CalcItemWidth();
          const float button_size  = ImGui::GetFrameHeight();
          const float button_count = 2.0F;
          ImGui::PushItemWidth(width - spacing * button_count - button_size * button_count);
          const auto pop_item_width = scope_guard(&ImGui::PopItemWidth);
          if (ImGui::BeginCombo("##Empty", current_item.data(), ImGuiComboFlags_HeightLarge))
          // The second parameter is the label previewed
          // before opening the combo.
          {
               std::ranges::for_each(strings, [&](const auto &string) {
                    const bool  is_selected = (current_item == string);
                    // You can store your selection however you
                    // want, outside or inside your objects
                    const char *c_str_value = std::ranges::data(string);
                    {
                         const auto pop_id_2 = scope_guard{ &ImGui::PopID };
                         ImGui::PushID(++imgui_id);
                         if (ImGui::Selectable(c_str_value, is_selected))
                         {
                              for (current_idx = 0; const auto &temp : strings)
                              {
                                   if (std::ranges::equal(temp, string))
                                   {
                                        changed = true;
                                        break;
                                   }
                                   ++current_idx;
                              }
                              //            current_idx =
                              //            std::distance(std::ranges::data(strings), &string);
                              //            changed     = true;
                         }
                    }
                    if (is_selected)
                    {
                         ImGui::SetItemDefaultFocus();
                         // You may set the initial focus when
                         // opening the combo (scrolling + for
                         // keyboard navigation support)
                    }
               });
               if (old != current_idx)
               {
                    spdlog::info(pattern, gui_labels::set, name, *next(values, current_idx), *next(strings, current_idx));
               }
               ImGui::EndCombo();
          }
          {

               const auto pop_id_left = scope_guard{ &ImGui::PopID };
               ImGui::SameLine(0, spacing);
               ImGui::PushID(++imgui_id);
               const bool disabled =
                 std::cmp_less_equal(current_idx, 0) || std::cmp_greater_equal(current_idx - 1, std::ranges::size(values));
               ImGui::BeginDisabled(disabled);
               if (ImGui::ArrowButton("##l", ImGuiDir_Left))
               {
                    --current_idx;
                    changed = true;
               }
               ImGui::EndDisabled();
          }
          {
               const auto pop_id_right = scope_guard{ &ImGui::PopID };
               ImGui::PushID(++imgui_id);
               ImGui::SameLine(0, spacing);
               const bool disabled = std::cmp_greater_equal(current_idx + 1, std::ranges::size(values));
               ImGui::BeginDisabled(disabled);
               if (ImGui::ArrowButton("##r", ImGuiDir_Right))
               {
                    ++current_idx;
                    changed = true;
               }
               ImGui::EndDisabled();
          }
          ImGui::SameLine(0, spacing);
          format_imgui_text("{}", name);
     }
     value = *next(values, current_idx);
     return old != current_idx || changed;
}
}// namespace fme
#endif// FIELD_MAP_EDITOR_GENERIC_COMBO_HPP
