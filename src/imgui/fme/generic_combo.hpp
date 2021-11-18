//
// Created by pcvii on 11/15/2021.
//

#ifndef MYPROJECT_GENERIC_COMBO_HPP
#define MYPROJECT_GENERIC_COMBO_HPP
#include "gui_labels.hpp"
#include "scope_guard.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <imgui.h>
#include <ranges>
#include <utility>
namespace fme
{
template<typename T>
concept returns_range_concept = requires(std::decay_t<T> t)
{
  {
    t()
    } -> std::ranges::range;
};
template<typename T>
concept filter_concept = requires(std::decay_t<T> t)
{
  {
    t.enabled()
    } -> std::convertible_to<bool>;
  {
    t.update(t.value())
    } -> std::convertible_to<T>;
  {
    t.enable()
    } -> std::convertible_to<T>;
  {
    t.disable()
    } -> std::convertible_to<T>;
};
template<typename T>
concept returns_filter_concept = requires(std::decay_t<T> t)
{
  {
    t()
    } -> filter_concept;
};
template<
  returns_range_concept  value_lambdaT,
  returns_range_concept  string_lambdaT,
  returns_filter_concept filter_lambdaT>
inline static bool
  generic_combo(
    int             &id,
    std::string_view name,
    value_lambdaT  &&value_lambda,
    string_lambdaT &&string_lambda,
    filter_lambdaT &&filter_lambda)
{
  bool        changed = false;
  const auto &values  = value_lambda();
  const auto &strings = string_lambda();
  auto       &filter  = filter_lambda();
  bool        checked = filter.enabled();
  static std::ranges::range_difference_t<decltype(values)> current_idx = {};
  if (const auto it = std::find(values.begin(), values.end(), filter.value());
      it != values.end())
  {
    current_idx = std::ranges::distance(std::ranges::cbegin(values), it);
  }
  else
  {
    current_idx = 0;
    if (!std::empty(values))
    {
      filter.update(values.front());
    }
    changed = true;
  }
  if (std::empty(values) || std::empty(strings))
  {
    if (checked)
    {
      filter.disable();
      return true;
    }
    return false;
  }
  const auto next = [](const auto &r, const auto &idx)
  {
    // sometimes the types are different. So I had to static cast to silence
    // warning.
    return std::ranges::next(
      std::ranges::cbegin(r),
      static_cast<std::iter_difference_t<decltype(std::ranges::cbegin(r))>>(
        idx));
  };
  const auto            current_item = next(strings, current_idx);
  static constexpr auto pattern      = "{}: \t{} \t{}\n";
  {
    const auto sc = scope_guard{ &ImGui::PopID };
    ImGui::PushID(++id);
    if (ImGui::Checkbox("", &checked))
    {
      if (checked)
      {
        filter.enable();
        fmt::print(
          pattern, gui_labels::enable, name, *next(values, current_idx));
      }
      else
      {
        filter.disable();
        fmt::print(
          pattern, gui_labels::disable, name, *next(values, current_idx));
      }
      changed = true;
    }
  }
  ImGui::SameLine();
  const auto old = current_idx;
  {
    const auto sc = scope_guard{ &ImGui::PopID };
    ImGui::PushID(++id);
    if (ImGui::BeginCombo(
          name.data(), current_item->data(), ImGuiComboFlags_HeightLarge))
    // The second parameter is the label previewed
    // before opening the combo.
    {
      std::ranges::for_each(
        strings,
        [&](const auto &string)
        {
          const bool  is_selected = (*current_item == string);
          // You can store your selection however you
          // want, outside or inside your objects
          const char *v           = string.data();
          {
            const auto sc = scope_guard{ &ImGui::PopID };
            ImGui::PushID(++id);
            if (ImGui::Selectable(v, is_selected))
            {
              current_idx = std::distance(std::ranges::data(strings), &string);
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
        });
      if (old != current_idx)
      {
        fmt::print(pattern, gui_labels::set, name, *next(values, current_idx));
      }
      ImGui::EndCombo();
    }
  }
  changed = (filter.update(*next(values, current_idx)).enabled()
             && (old != current_idx))
            || changed;
  return changed;
}
template<
  returns_range_concept value_lambdaT,
  returns_range_concept string_lambdaT,
  typename valueT>
requires requires(value_lambdaT v)
{
  {
    *(v().begin())
    } -> std::convertible_to<valueT>;
  {
    *(v().begin())
    } -> std::equality_comparable_with<valueT>;
}
inline static bool
  generic_combo(
    int             &id,
    std::string_view name,
    value_lambdaT  &&value_lambda,
    string_lambdaT &&string_lambda,
    valueT          &value)
{
  bool        changed = false;
  const auto &values  = value_lambda();
  const auto &strings = string_lambda();
  static std::ranges::range_difference_t<decltype(values)> current_idx = {};
  {
    if (const auto it = std::find(values.begin(), values.end(), value);
        it != values.end())
    {
      current_idx = std::ranges::distance(std::ranges::cbegin(values), it);
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
  const auto next = [](const auto &r, const auto &idx)
  {
    // sometimes the types are different. So I had to static cast to silence
    // warning.
    return std::ranges::next(
      std::ranges::cbegin(r),
      static_cast<std::iter_difference_t<decltype(std::ranges::cbegin(r))>>(
        idx));
  };
  const auto            current_item = next(strings, current_idx);
  static constexpr auto pattern      = "{}: \t{} \t{}\n";
  const auto            old          = current_idx;
  {
    const auto sc = scope_guard{ &ImGui::PopID };
    ImGui::PushID(++id);
    if (ImGui::BeginCombo(
          name.data(), current_item->data(), ImGuiComboFlags_HeightLarge))
    // The second parameter is the label previewed
    // before opening the combo.
    {
      std::ranges::for_each(
        strings,
        [&](const auto &string)
        {
          const bool  is_selected = (*current_item == string);
          // You can store your selection however you
          // want, outside or inside your objects
          const char *c_str_value = std::ranges::data(string);
          {
            const auto sc = scope_guard{ &ImGui::PopID };
            ImGui::PushID(++id);
            if (ImGui::Selectable(c_str_value, is_selected))
            {
              current_idx = std::distance(std::ranges::data(strings), &string);
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
        });
      if (old != current_idx)
      {
        fmt::print(pattern, gui_labels::set, name, *next(values, current_idx));
      }
      ImGui::EndCombo();
    }
  }
  value = *next(values, current_idx);
  return old != current_idx || changed;
}
}// namespace fme
#endif// MYPROJECT_GENERIC_COMBO_HPP
