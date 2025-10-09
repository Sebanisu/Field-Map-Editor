//
// Created by pcvii on 11/15/2021.
//

#ifndef FIELD_MAP_EDITOR_GENERIC_COMBO_HPP
#define FIELD_MAP_EDITOR_GENERIC_COMBO_HPP
#include "filter.hpp"
#include "format_imgui_text.hpp"
#include "gui/fa_icons.hpp"
#include "gui/gui_labels.hpp"
#include "gui/push_pop_id.hpp"
#include "gui/tool_tip.hpp"
#include "open_file_explorer.hpp"
#include "unique_values.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <functional>
#include <imgui.h>
#include <ranges>
#include <ScopeGuard.hpp>
#include <spdlog/spdlog.h>
#include <string_view>
#include <utility>
#include <vector>

namespace fme
{
struct generic_combo_settings
{
     int  num_columns         = 2;
     bool show_explore_button = false;
};

template<typename T>
concept filter_concept = requires(std::remove_cvref_t<T> filter) {
     { filter.enabled() } -> std::convertible_to<bool>;
     { filter.update(filter.value()) } -> std::convertible_to<T>;
     { filter.enable() } -> std::convertible_to<T>;
     { filter.disable() } -> std::convertible_to<T>;
};

template<typename R>
concept compatible_range = std::ranges::contiguous_range<R>;
// std::ranges::sized_range<R> && std::ranges::forward_range<R>;

template<
  typename InputValueT,
  typename StringT,
  typename ToolTipT,
  typename FilterT>
     requires requires(FilterT &f, InputValueT v) { f.update(v); }
class GenericComboWithFilter
{
   public:
     GenericComboWithFilter(
       std::string_view             name,
       std::span<const InputValueT> value_range,
       std::span<const StringT>     string_range,
       std::span<const ToolTipT>    tooltip_range,
       FilterT                     &filter,
       generic_combo_settings       settings = {})
       : name_(name)
       , values_(value_range)
       , strings_(string_range)
       , tool_tips_(tooltip_range)
       , filter_(filter)
       , settings_(settings)
       , current_idx_(0)
       , changed_(false)
       , spacing_(ImGui::GetStyle().ItemInnerSpacing.x)
     {
     }
     bool render() const
     {
          if (values_.empty() || strings_.empty())
          {
               return false;
          }

          updateCurrentIndex();

          const auto old_idx = current_idx_;
          renderCheckBox();
          renderComboBox();
          renderLeftButton();
          renderRightButton();
          renderExploreButton();
          renderTitle();

          if (old_idx != current_idx_)
          {
               static constexpr auto pattern = "{}: \t{}\t{}\t{}";
               spdlog::info(
                 pattern,
                 gui_labels::set,
                 name_,
                 *getNext(values_, current_idx_),
                 *getNext(strings_, current_idx_));
               filter_.get().update(*getNext(values_, current_idx_));
          }

          return old_idx != current_idx_ || changed_;
     }

   private:
     std::string_view                name_;
     std::span<const InputValueT>    values_;
     std::span<const StringT>        strings_;
     std::span<const ToolTipT>       tool_tips_;
     std::reference_wrapper<FilterT> filter_;
     generic_combo_settings          settings_;
     mutable std::ranges::range_difference_t<std::span<const InputValueT>>
                  current_idx_;
     mutable bool changed_;
     float        spacing_;

     void         updateCurrentIndex() const
     {
          const auto found = std::find_if(
            std::ranges::begin(values_),
            std::ranges::end(values_),
            [&](const auto &tmp)
            {
                 using FilterValueT
                   = std::remove_cvref_t<decltype(filter_.get().value())>;
                 using ValuesValueT = std::remove_cvref_t<
                   std::ranges::range_value_t<decltype(values_)>>;

                 if constexpr (
                   std::is_enum_v<FilterValueT> && std::is_enum_v<ValuesValueT>)
                 {
                      return std::to_underlying(filter_.get().value())
                             == std::to_underlying(tmp);
                 }
                 else
                 {
                      return filter_.get().value() == tmp;
                 }
            });
          if (found != std::ranges::end(values_))
          {
               current_idx_
                 = std::ranges::distance(std::ranges::begin(values_), found);
          }
          else
          {
               current_idx_ = 0;
               if (!values_.empty())
               {
                    filter_.get().update(values_.front());
               }
               changed_ = true;
          }
     }

     template<typename Range>
     auto getNext(
       const Range &range,
       const auto  &idx) const
     {
          return std::ranges::next(std::ranges::begin(range), idx);
     }
     void renderCheckBox() const
     {
          const auto _ = PushPopID();
          if (bool checked = filter_.get().enabled();
              ImGui::Checkbox("", &checked))
          {
               checked ? filter_.get().enable() : filter_.get().disable();
               changed_ = true;
          }
          ImGui::SameLine(0, spacing_);
     }

     void renderComboBox() const
     {
          const auto  _            = PushPopID();
          const float button_size  = ImGui::GetFrameHeight();
          const float button_count = [&]()
          {
               int count = 3;
               if (settings_.show_explore_button)
               {
                    ++count;
               }
               return static_cast<float>(count);
          }();
          ImGui::PushItemWidth(
            ImGui::CalcItemWidth() - spacing_ * button_count
            - button_size * button_count);
          const auto pop_item_width
            = glengine::ScopeGuard(&ImGui::PopItemWidth);

          const char *current_item_cstr = [&]() -> const char *
          {
               if (
                 std::ranges::empty(strings_)
                 || !std::cmp_less(current_idx_, std::ranges::size(strings_)))
               {
                    return "N/A";
               }
               return std::data(*getNext(strings_, current_idx_));
          }();

          if (ImGui::BeginCombo(
                "##Empty", current_item_cstr, ImGuiComboFlags_HeightLarge))
          // The second parameter is the label previewed
          // before opening the combo.
          {
               ImGui::Columns(settings_.num_columns, "##columns", false);
               for (const auto &[index_raw, string] :
                    std::views::enumerate(strings_))
               {
                    const auto index
                      = static_cast<decltype(current_idx_)>(index_raw);
                    const bool  is_selected = (index == current_idx_);
                    // You can store your selection however you
                    // want, outside or inside your objects
                    const char *c_str_value = std::ranges::data(string);
                    {
                         const auto pop_id = PushPopID();
                         const auto pop_column
                           = glengine::ScopeGuard{ &ImGui::NextColumn };
                         if (ImGui::Selectable(c_str_value, is_selected))
                         {
                              for (current_idx_ = 0;
                                   const auto &temp : strings_)
                              {
                                   if (std::ranges::equal(temp, string))
                                   {
                                        changed_ = true;
                                        filter_.get().enable();
                                        break;
                                   }
                                   ++current_idx_;
                              }
                         }
                    }
                    if (is_selected)
                    {
                         ImGui::SetItemDefaultFocus();
                         // You may set the initial focus when
                         // opening the combo (scrolling + for
                         // keyboard navigation support)
                    }
                    renderToolTip(index);
               }
               ImGui::Columns(1);
               ImGui::EndCombo();
          }
          renderToolTip(current_idx_);
     }

     void renderToolTip(const decltype(current_idx_) index) const
     {
          if (std::ranges::empty(tool_tips_))
          {
               return;
          }
          const auto &tooltip = *getNext(tool_tips_, index);
          tool_tip(tooltip);
     }

     auto size_of_values() const
     {
          return static_cast<std::ranges::range_difference_t<
            std::remove_cvref_t<decltype(values_)>>>(
            std::ranges::size(values_));
     }

     void renderLeftButton() const
     {
          const auto _ = PushPopID();
          ImGui::SameLine(0, spacing_);
          const auto check_valid = [&]()
          {
               return (current_idx_ <= decltype(current_idx_){})
                      || (current_idx_ - 1 >= size_of_values());
          };
          const bool disabled = check_valid();
          ImGui::BeginDisabled(disabled);
          if (ImGui::ArrowButton("##l", ImGuiDir_Left))
          {
               --current_idx_;
               changed_ = true;
               filter_.get().enable();
               while (std::ranges::empty(*getNext(strings_, current_idx_))
                      && !check_valid())
               {
                    --current_idx_;
               }
          }
          ImGui::EndDisabled();
     }

     void renderRightButton() const
     {
          const auto _ = PushPopID();
          ImGui::SameLine(0, spacing_);
          const auto check_valid
            = [&]() { return (current_idx_ + 1 >= size_of_values()); };
          const bool disabled = check_valid();
          ImGui::BeginDisabled(disabled);
          if (ImGui::ArrowButton("##r", ImGuiDir_Right))
          {
               ++current_idx_;
               changed_ = true;
               filter_.get().enable();
               while (std::ranges::empty(*getNext(strings_, current_idx_))
                      && !check_valid())
               {
                    ++current_idx_;
               }
          }
          ImGui::EndDisabled();
     }

     void renderExploreButton() const
     {
          if (!settings_.show_explore_button)
          {
               return;
          }
          ImGui::SameLine(0, spacing_);
          const float button_size = ImGui::GetFrameHeight();
          const auto  _           = PushPopID();
          if (ImGui::Button(
                ICON_FA_FOLDER_OPEN, ImVec2{ button_size, button_size }))
          {
               open_directory(*getNext(strings_, current_idx_));
          }
          tool_tip(gui_labels::explore_tooltip);
     }

     void renderTitle() const
     {
          if (std::ranges::empty(name_))
          {
               return;
          }
          ImGui::SameLine(0, spacing_);
          format_imgui_text("{}", name_);
     }
};

template<
  std::ranges::contiguous_range ValueRangeT,
  std::ranges::contiguous_range StringRangeT,
  std::ranges::contiguous_range ToolTipRangeT,
  typename FilterT>
     requires requires(
       FilterT                                &f,
       std::ranges::range_value_t<ValueRangeT> v) { f.update(v); }
GenericComboWithFilter(
  std::string_view,
  ValueRangeT &&,
  StringRangeT &&,
  ToolTipRangeT &&,
  FilterT &,
  generic_combo_settings = {})
  -> GenericComboWithFilter<
    std::ranges::range_value_t<ValueRangeT>,  // InputValueT
    std::ranges::range_value_t<StringRangeT>, // StringT
    std::ranges::range_value_t<ToolTipRangeT>,// ToolTipT
    FilterT                                   // FilterT
    >;


template<
  ff_8::HasValuesAndStringsAndZip HasValuesAndStringsAndZipT,
  ff_8::IsFilterOld               IsFilterOldT,
  std::invocable                  invocableT>
     requires(requires(IsFilterOldT &filter, HasValuesAndStringsAndZipT &pair) {
          typename std::remove_cvref_t<
            std::ranges::range_value_t<decltype(pair.values())>>;

          filter.update(
            std::declval<std::remove_cvref_t<
              std::ranges::range_value_t<decltype(pair.values())>>>());
     })
class GenericMenuWithFilter
{
   private:
     std::string_view             m_label;
     HasValuesAndStringsAndZipT &&m_pair;
     IsFilterOldT                &m_filter;
     invocableT                 &&m_lambda;

   public:
     GenericMenuWithFilter(
       std::string_view             label,
       HasValuesAndStringsAndZipT &&pair,
       IsFilterOldT                &filter,
       invocableT                 &&lambda)
       : m_label(label)
       , m_pair(std::forward<HasValuesAndStringsAndZipT>(pair))
       , m_filter(filter)
       , m_lambda(std::forward<invocableT>(lambda))
     {
     }

     void operator()()
     {
          if (ImGui::BeginMenu(m_label.data()))
          {
               const auto pop_menu1 = glengine::ScopeGuard(&ImGui::EndMenu);
               const auto process_element = [&](auto &value, auto &str)
               {
                    const bool selected = m_filter.value() == value;
                    bool       checked  = selected && m_filter.enabled();
                    if (ImGui::MenuItem(str.data(), nullptr, &checked))
                    {
                         if (selected)
                         {
                              if (m_filter.enabled())
                              {
                                   m_filter.disable();
                              }
                              else
                              {
                                   m_filter.enable();
                              }
                         }
                         else
                         {
                              m_filter.update(value);
                              m_filter.enable();
                         }
                         std::invoke(m_lambda);
                    }
               };
               const std::uint32_t cols = { 3U };
               std::uint32_t       i    = {};
               ImGui::BeginTable("##filter_menu_items", cols);
               for (auto &&t : m_pair.zip())
               {
                    if (i % cols == 0U)
                    {
                         ImGui::TableNextRow();
                    }
                    ImGui::TableNextColumn();
                    if constexpr (
                      std::tuple_size_v<std::decay_t<decltype(t)>> == 2)
                    {
                         auto &&[value, str] = t;
                         // Handle the case where tuple has 2 elements
                         process_element(value, str);
                    }
                    else if constexpr (
                      std::tuple_size_v<std::decay_t<decltype(t)>> == 3)
                    {
                         auto &&[value, str, tooltip_str] = t;
                         // Handle the case where tuple has 3 elements
                         process_element(value, str);
                         tool_tip(tooltip_str);
                    }
                    ++i;
               }
               ImGui::EndTable();
          }
     }
};


template<
  ff_8::HasValuesAndStringsAndZip HasValuesAndStringsAndZipT,
  ff_8::IsFilterOld               IsFilterOldT,
  std::invocable                  invocableT>
     requires(requires(IsFilterOldT &filter, HasValuesAndStringsAndZipT &pair) {
          typename std::remove_cvref_t<
            std::ranges::range_value_t<decltype(pair.values())>>;

          filter.update(
            std::declval<std::vector<std::remove_cvref_t<
              std::ranges::range_value_t<decltype(pair.values())>>>>());
     })
class GenericMenuWithMultiFilter
{
   private:
     std::string_view                  m_label;
     const HasValuesAndStringsAndZipT &m_pair;
     IsFilterOldT                     &m_filter;
     invocableT                      &&m_lambda;

   public:
     GenericMenuWithMultiFilter(
       std::string_view                  label,
       const HasValuesAndStringsAndZipT &pair,
       IsFilterOldT                     &filter,
       invocableT                      &&lambda)
       : m_label(label)
       , m_pair(pair)
       , m_filter(filter)
       , m_lambda(std::forward<invocableT>(lambda))
     {
     }

     void operator()()
     {
          if (ImGui::BeginMenu(m_label.data()))
          {
               const auto pop_menu1 = glengine::ScopeGuard(&ImGui::EndMenu);
               const auto process_element = [&](auto &value, auto &str)
               {
                    auto copy_value = m_filter.value();
                    auto it         = std::ranges::find_if(
                      copy_value,
                      [&](const auto &tmp) { return tmp == value; });
                    const bool selected = it != copy_value.end();

                    // ImGui::BeginGroup();

                    // Draw the checkmark icon or empty space for alignment
                    ImGui::TextUnformatted(
                      selected ? ICON_FA_CHECK
                               : "  ");// Keep spacing even if not selected

                    ImGui::SameLine();
                    if (ImGui::Selectable(
                          str.data(),
                          selected,
                          ImGuiSelectableFlags_DontClosePopups))
                    {
                         if (selected)
                         {
                              copy_value.erase(it);
                              m_filter.update(copy_value);
                              if (m_filter.enabled() && copy_value.empty())
                              {
                                   m_filter.disable();
                              }
                         }
                         else
                         {
                              copy_value.push_back(value);
                              std::ranges::sort(copy_value);
                              m_filter.update(copy_value);
                              m_filter.enable();
                         }
                         std::invoke(m_lambda);
                    }
                    // ImGui::EndGroup();
               };
               bool checked = m_filter.enabled();
               if (ImGui::Checkbox("Toggle Filter", &checked))
               {
                    if (checked)
                    {
                         m_filter.enable();
                    }
                    else
                    {
                         m_filter.disable();
                    }
               }
               const std::uint32_t cols = { 3U };
               std::uint32_t       i    = {};
               ImGui::BeginTable("##filter_menu_items", cols);
               for (auto &&t : m_pair.zip())
               {
                    if (i % cols == 0U)
                    {
                         ImGui::TableNextRow();
                    }
                    ImGui::TableNextColumn();
                    if constexpr (
                      std::tuple_size_v<std::decay_t<decltype(t)>> == 2)
                    {
                         auto &&[value, str] = t;
                         // Handle the case where tuple has 2 elements
                         process_element(value, str);
                    }
                    else if constexpr (
                      std::tuple_size_v<std::decay_t<decltype(t)>> == 3)
                    {
                         auto &&[value, str, tooltip_str] = t;
                         // Handle the case where tuple has 3 elements
                         process_element(value, str);
                         tool_tip(tooltip_str);
                    }
                    ++i;
               }
               ImGui::EndTable();
          }
     }
};

template<
  typename InputValueT,
  typename StringT,
  typename ToolTipT,
  typename FilterT>
     requires requires(FilterT &f, std::vector<InputValueT> v) { f.update(v); }
class GenericComboWithMultiFilter
{
   public:
     GenericComboWithMultiFilter(
       std::string_view             name,
       std::span<const InputValueT> value_range,
       std::span<const StringT>     string_range,
       std::span<const ToolTipT>    tooltip_range,
       FilterT                     &filter,
       generic_combo_settings       settings = {})
       : name_(name)
       , values_(value_range)
       , strings_(string_range)
       , tool_tips_(tooltip_range)
       , filter_(filter)
       , settings_(settings)
       , current_idx_(
           std::ranges::size(value_range),
           false)
       , changed_(false)
       , spacing_(ImGui::GetStyle().ItemInnerSpacing.x)
     {
          updateCurrentIndex();
     }

     bool render() const
     {
          if (values_.empty() || strings_.empty())
          {
               return false;
          }

          updateCurrentIndex();

          const auto old_idx = current_idx_;
          renderCheckBox();
          renderComboBox();
          renderExploreButton();
          renderTitle();

          if (old_idx != current_idx_)
          {
               // Log changes between old and current index states
               for (auto [value, str, old_flag, new_flag] :
                    std::views::zip(values_, strings_, old_idx, current_idx_))
               {
                    if (old_flag != new_flag)
                    {
                         const char *state = new_flag ? "Enabled" : "Disabled";
                         spdlog::info(
                           "{}: \t{}\t{}\t{}\t{}",
                           gui_labels::set,
                           name_,
                           value,
                           str,
                           state);
                    }
               }

               filter_.get().update(
                 std::views::zip(values_, current_idx_)
                 | std::views::filter([](const auto &tuple)
                                      { return std::get<1>(tuple); })
                 | std::views::transform([](const auto &tuple)
                                         { return std::get<0>(tuple); })
                 | std::ranges::to<std::vector>());
          }

          return old_idx != current_idx_ || changed_;
     }

   private:
     std::string_view                name_;
     std::span<const InputValueT>    values_;
     std::span<const StringT>        strings_;
     std::span<const ToolTipT>       tool_tips_;
     std::reference_wrapper<FilterT> filter_;
     generic_combo_settings          settings_;
     mutable std::vector<bool>       current_idx_;
     mutable bool                    changed_;
     float                           spacing_;

     void                            updateCurrentIndex() const
     {
          const auto &filter_value = filter_.get().value();
          for (auto &&[value, enabled] : std::views::zip(values_, current_idx_))
          {
               if (auto it = std::ranges::find(filter_value, value);
                   it != filter_value.end())
               {
                    enabled = true;
               }
          }
     }

     // template<typename Range>
     // auto getNext(const Range &range, const auto &idx) const
     // {
     //      return std::ranges::next(std::ranges::begin(range), idx);
     // }
     void renderCheckBox() const
     {
          const auto _  = PushPopID();
          bool disabled = std::ranges::none_of(current_idx_, std::identity{});
          if (disabled && filter_.get().enabled())
          {
               filter_.get().disable();
               changed_ = true;
          }
          ImGui::BeginDisabled(disabled);

          if (bool checked = filter_.get().enabled();
              ImGui::Checkbox("", &checked))
          {
               checked ? filter_.get().enable() : filter_.get().disable();
               changed_ = true;
          }
          ImGui::SameLine(0, spacing_);
          ImGui::EndDisabled();
     }

     void renderComboBox() const
     {
          const auto  _            = PushPopID();
          const float button_size  = ImGui::GetFrameHeight();
          const float button_count = [&]()
          {
               int count = 1;
               if (settings_.show_explore_button)
               {
                    ++count;
               }
               return static_cast<float>(count);
          }();
          ImGui::PushItemWidth(
            ImGui::CalcItemWidth() - spacing_ * button_count
            - button_size * button_count);
          const auto pop_item_width
            = glengine::ScopeGuard(&ImGui::PopItemWidth);

          auto zip_view = std::views::zip(strings_, current_idx_, tool_tips_);

          auto current_view
            = std::views::zip(strings_, current_idx_)
              | std::views::filter([](const auto &tup)
                                   { return std::get<1>(tup); })
              | std::views::transform([](const auto &tup)
                                      { return std::get<0>(tup); });
          const auto current_item = [&]()
          {
               auto tmp = std::ranges::join_with_view(
                            current_view, std::string_view{ ", " })
                          | std::ranges::to<std::string>();
               if (std::ranges::empty(tmp))
               {
                    tmp = "Select a value...";
               }
               return tmp;
          }();


          if (ImGui::BeginCombo(
                "##Empty",
                std::ranges::data(current_item),
                ImGuiComboFlags_HeightLarge))
          // The second parameter is the label previewed
          // before opening the combo.
          {
               ImGui::Columns(settings_.num_columns, "##columns", false);

               for (auto &&[string, is_selected, tooltip] : zip_view)
               {
                    // You can store your selection however you
                    // want, outside or inside your objects
                    const char *c_str_value = std::ranges::data(string);
                    {
                         const auto pop_id = PushPopID();
                         const auto pop_column
                           = glengine::ScopeGuard{ &ImGui::NextColumn };
                         if (ImGui::Selectable(
                               c_str_value,
                               is_selected,
                               ImGuiSelectableFlags_DontClosePopups))
                         {
                              if (!is_selected)
                              {
                                   filter_.get().enable();
                              }
                              is_selected = !is_selected;
                              changed_    = true;
                         }

                         if (is_selected)
                         {
                              ImGui::SetItemDefaultFocus();
                              // You may set the initial focus when
                              // opening the combo (scrolling + for
                              // keyboard navigation support)
                         }
                         tool_tip(tooltip);
                    }
               }
               ImGui::Columns(1);
               ImGui::EndCombo();
          }

          tool_tip(current_item);
     }

     auto size_of_values() const
     {
          return static_cast<std::ranges::range_difference_t<
            std::remove_cvref_t<decltype(values_)>>>(
            std::ranges::size(values_));
     }

     void renderExploreButton() const
     {
          if constexpr (std::same_as<
                          std::filesystem::path,
                          std::ranges::range_value_t<
                            std::remove_cvref_t<decltype(values_)>>>)
          {
               if (!settings_.show_explore_button)
               {
                    return;
               }

               ImGui::SameLine(0, spacing_);
               const float button_size = ImGui::GetFrameHeight();
               const auto  _           = PushPopID();

               if (ImGui::Button(
                     ICON_FA_FOLDER_OPEN, ImVec2{ button_size, button_size }))
               {
                    ImGui::OpenPopup("ExploreDirectoryPopup");
               }

               tool_tip(gui_labels::explore_tooltip);

               if (ImGui::BeginPopup("ExploreDirectoryPopup"))
               {
                    bool any_active = false;
                    for (auto [val, active] :
                         std::views::zip(values_, current_idx_))
                    {
                         if (active)
                         {
                              any_active = true;
                              if (ImGui::MenuItem(val.c_str()))
                              {
                                   open_directory(val);
                                   ImGui::CloseCurrentPopup();
                              }
                         }
                    }

                    if (!any_active)
                    {
                         ImGui::TextDisabled("No active entries.");
                    }

                    ImGui::EndPopup();
               }
          }
     }

     void renderTitle() const
     {
          if (std::ranges::empty(name_))
          {
               return;
          }
          ImGui::SameLine(0, spacing_);
          format_imgui_text("{}", name_);
     }
};

template<
  std::ranges::contiguous_range ValueRangeT,
  std::ranges::contiguous_range StringRangeT,
  std::ranges::contiguous_range ToolTipRangeT,
  typename FilterT>
     requires requires(
       FilterT                                             &f,
       std::vector<std::ranges::range_value_t<ValueRangeT>> v) { f.update(v); }
GenericComboWithMultiFilter(
  std::string_view,
  ValueRangeT &&,
  StringRangeT &&,
  ToolTipRangeT &&,
  FilterT &,
  generic_combo_settings = {})
  -> GenericComboWithMultiFilter<
    std::ranges::range_value_t<ValueRangeT>,  // InputValueT
    std::ranges::range_value_t<StringRangeT>, // StringT
    std::ranges::range_value_t<ToolTipRangeT>,// ToolTipT
    FilterT                                   // FilterT
    >;


template<
  typename InputValueT,
  typename StringT,
  typename ToolTipT,
  typename FilterT>
     requires requires(
       std::remove_reference_t<FilterT> &f,
       std::remove_cvref_t<InputValueT>  v) { f.update(v); }
class GenericComboWithFilterAndFixedToggles
{
   public:
     GenericComboWithFilterAndFixedToggles(
       std::string_view             name,
       std::span<const InputValueT> value_range,
       const std::vector<bool>     &fixed_toggles_range,
       std::span<const StringT>     string_range,
       std::span<const ToolTipT>    tooltip_range,
       FilterT                     &filter,
       generic_combo_settings       settings = {})
       : name_(name)
       , values_(value_range)
       , fixed_toggles_(fixed_toggles_range)
       , strings_(string_range)
       , tool_tips_(tooltip_range)
       , filter_(filter)
       , settings_(settings)
       , current_idx_(0)
       , changed_(false)
       , spacing_(ImGui::GetStyle().ItemInnerSpacing.x)
     {
     }

     bool render() const
     {
          if (values_.empty() || strings_.empty())
          {
               return false;
          }// namespace fme

          updateCurrentIndex();

          const auto old_idx = current_idx_;
          renderCheckBox();
          {
               ImGui::BeginDisabled(
                 std::ranges::none_of(fixed_toggles_.get(), std::identity{}));
               const auto pop_disabled
                 = glengine::ScopeGuard{ &ImGui::EndDisabled };
               renderComboBox();
               renderLeftButton();
               renderRightButton();
               renderExploreButton();
          }
          renderTitle();


          if (old_idx != current_idx_)
          {
               static constexpr auto pattern = "{}: \t{}\t{}\t{}";
               spdlog::info(
                 pattern,
                 gui_labels::set,
                 name_,
                 *getNext(values_, current_idx_),
                 *getNext(strings_, current_idx_));
               filter_.get().update(*getNext(values_, current_idx_));
          }

          return old_idx != current_idx_ || changed_;
     }

   private:
     std::string_view                                name_;
     std::span<const InputValueT>                    values_;
     std::reference_wrapper<const std::vector<bool>> fixed_toggles_;
     std::span<const StringT>                        strings_;
     std::span<const ToolTipT>                       tool_tips_;
     std::reference_wrapper<FilterT>                 filter_;
     generic_combo_settings                          settings_;
     mutable std::ranges::range_difference_t<std::span<const InputValueT>>
                  current_idx_;
     mutable bool changed_;
     float        spacing_;

     void         updateCurrentIndex() const
     {
          const auto found = std::find(
            std::ranges::begin(values_),
            std::ranges::end(values_),
            filter_.get().value());
          if (found != std::ranges::end(values_))
          {
               current_idx_
                 = std::ranges::distance(std::ranges::begin(values_), found);
          }
          else
          {
               current_idx_ = 0;
               if (!values_.empty())
               {
                    filter_.get().update(values_.front());
               }
               changed_ = true;
          }
     }

     template<typename Range>
     auto getNext(
       const Range &range,
       const auto  &idx) const
     {
          assert(
            std::cmp_less(idx, std::ranges::size(range))
            && "Index out of range!");
          return std::ranges::next(std::ranges::begin(range), idx);
     }

     void renderCheckBox() const
     {
          if (std::cmp_less_equal(
                std::ranges::size(fixed_toggles_.get()), current_idx_))
          {
               return;
          }
          const auto  _ = PushPopID();
          const auto &current_fixed_toggle
            = *getNext(fixed_toggles_.get(), current_idx_);
          ImGui::BeginDisabled(
            !current_fixed_toggle && !filter_.get().enabled());
          const auto pop_disabled = glengine::ScopeGuard{ &ImGui::EndDisabled };
          if (bool checked = filter_.get().enabled();
              ImGui::Checkbox("", &checked))
          {
               checked ? filter_.get().enable() : filter_.get().disable();
               changed_ = true;
          }
          ImGui::SameLine(0, spacing_);
     }

     void renderComboBox() const
     {
          const auto  _            = PushPopID();// RAII for ImGui ID stack
          const float button_size  = ImGui::GetFrameHeight();
          const float button_count = [&]()
          {
               int count = 3;
               if (settings_.show_explore_button)
               {
                    ++count;
               }
               return static_cast<float>(count);
          }();

          // Set up combo box width
          ImGui::PushItemWidth(
            ImGui::CalcItemWidth() - spacing_ * button_count
            - button_size * button_count);
          const auto pop_item_width
            = glengine::ScopeGuard(&ImGui::PopItemWidth);

          const char *current_item_cstr = [&]() -> const char *
          {
               if (
                 std::ranges::empty(strings_)
                 || !std::cmp_less(current_idx_, std::ranges::size(strings_)))
               {
                    return "N/A";
               }
               return std::data(*getNext(strings_, current_idx_));
          }();

          // Begin combo box with preview of current item
          if (ImGui::BeginCombo(
                "##Empty", current_item_cstr, ImGuiComboFlags_HeightLarge))
          {
               ImGui::Columns(settings_.num_columns, "##columns", false);
               for (const auto &[index_raw, string] :
                    std::ranges::views::enumerate(strings_))
               {
                    const auto index
                      = static_cast<decltype(current_idx_)>(index_raw);
                    const bool is_selected = current_idx_ == index;
                    const auto pop_id      = PushPopID();
                    const auto pop_column
                      = glengine::ScopeGuard{ &ImGui::NextColumn };

                    if (std::cmp_less_equal(
                          std::ranges::size(fixed_toggles_.get()), index))
                    {
                         continue;
                    }
                    const auto &current_fixed_toggle
                      = *getNext(fixed_toggles_.get(), index);
                    if (!current_fixed_toggle)
                    {
                         continue;
                    }
                    ImGui::BeginDisabled(!current_fixed_toggle);
                    const auto pop_disabled
                      = glengine::ScopeGuard{ &ImGui::EndDisabled };

                    if (ImGui::Selectable(
                          std::ranges::data(string), is_selected))
                    {
                         current_idx_ = index;
                         changed_     = true;
                         filter_.get().enable();
                         break;
                    }
                    if (is_selected)
                    {
                         ImGui::SetItemDefaultFocus();
                         // You may set the initial focus when
                         // opening the combo (scrolling + for
                         // keyboard navigation support)
                    }
                    renderToolTip(index);
               }
               ImGui::Columns(1);
               ImGui::EndCombo();
          }
          renderToolTip(current_idx_);
     }

     void renderToolTip(const decltype(current_idx_) index) const
     {
          if (std::ranges::empty(tool_tips_))
          {
               return;
          }
          const auto &tooltip = *getNext(tool_tips_, index);
          tool_tip(tooltip);
     }

     auto size_of_values() const
     {
          return static_cast<std::ranges::range_difference_t<
            std::remove_cvref_t<decltype(values_)>>>(
            std::ranges::size(values_));
     }

     void renderLeftButton() const
     {
          const auto _           = PushPopID();

          const auto check_valid = [&]()
          {
               return (current_idx_ <= decltype(current_idx_){})
                      || (current_idx_ - 1 >= size_of_values());
          };
          // Check if there is any index < current_idx_ where fixed_toggles_ is
          // true
          const auto has_valid_left_index
            = [&]() -> std::optional<decltype(current_idx_)>
          {
               if (!check_valid())
               {
                    for (auto i = current_idx_ - 1; i >= 0; --i)
                    {
                         if (std::cmp_less_equal(
                               std::ranges::size(fixed_toggles_.get()), i))
                         {
                              continue;
                         }
                         if (*getNext(fixed_toggles_.get(), i))
                         {
                              return i;
                         }
                    }
               }
               return std::nullopt;
          }();
          ImGui::SameLine(0, spacing_);
          ImGui::BeginDisabled(!has_valid_left_index.has_value());
          if (ImGui::ArrowButton("##l", ImGuiDir_Left))
          {
               current_idx_ = has_valid_left_index.value();
               changed_     = true;
               filter_.get().enable();
          }
          ImGui::EndDisabled();
     }

     void renderRightButton() const
     {
          const auto _ = PushPopID();
          ImGui::SameLine(0, spacing_);
          const auto check_valid
            = [&]() { return (current_idx_ + 1 >= size_of_values()); };
          const auto has_valid_right_index
            = [&]() -> std::optional<decltype(current_idx_)>
          {
               if (!check_valid())
               {
                    for (auto i = current_idx_ + 1; i < size_of_values(); ++i)
                    {
                         if (std::cmp_less_equal(
                               std::ranges::size(fixed_toggles_.get()), i))
                         {
                              continue;
                         }
                         if (*getNext(fixed_toggles_.get(), i))
                         {
                              return i;
                         }
                    }
               }
               return std::nullopt;
          }();
          ImGui::BeginDisabled(!has_valid_right_index.has_value());
          if (ImGui::ArrowButton("##r", ImGuiDir_Right))
          {
               current_idx_ = has_valid_right_index.value();
               changed_     = true;
               filter_.get().enable();
          }
          ImGui::EndDisabled();
     }

     void renderExploreButton() const
     {
          if (!settings_.show_explore_button)
          {
               return;
          }
          ImGui::SameLine(0, spacing_);
          const float button_size = ImGui::GetFrameHeight();
          const auto  _           = PushPopID();
          if (ImGui::Button(
                ICON_FA_FOLDER_OPEN, ImVec2{ button_size, button_size }))
          {
               open_directory(*getNext(strings_, current_idx_));
          }
          tool_tip(gui_labels::explore_tooltip);
     }

     void renderTitle() const
     {
          if (std::ranges::empty(name_))
          {
               return;
          }
          ImGui::SameLine(0, spacing_);
          format_imgui_text("{}", name_);

          // Get size of the text for hover area
          ImVec2 textSize = ImGui::CalcTextSize(name_.data());

          // Move back to same position
          ImVec2 cursorBackup
            = ImGui::GetCursorScreenPos();// Save cursor position
          ImGui::SetItemAllowOverlap();   // Allow us to draw on top
          ImGui::SetCursorScreenPos(ImGui::GetItemRectMin());

          // Make invisible button the same size as the text
          const auto _ = PushPopID();
          ImGui::InvisibleButton("##hover_area", textSize);
          tool_tip(name_);

          // Restore original cursor position
          ImGui::SetCursorScreenPos(cursorBackup);
     }
};

template<
  std::ranges::contiguous_range ValueRangeT,
  std::ranges::contiguous_range StringRangeT,
  std::ranges::contiguous_range ToolTipRangeT,
  typename FilterT>
GenericComboWithFilterAndFixedToggles(
  std::string_view,
  ValueRangeT &&,
  const std::vector<bool> &,
  StringRangeT &&,
  ToolTipRangeT &&,
  FilterT &,
  generic_combo_settings = {})
  -> GenericComboWithFilterAndFixedToggles<
    std::ranges::range_value_t<ValueRangeT>,  // InputValueT
    std::ranges::range_value_t<StringRangeT>, // StringT
    std::ranges::range_value_t<ToolTipRangeT>,// ToolTipT
    FilterT                                   // FilterT
    >;


template<
  typename InputValueT,
  typename StringT,
  typename ValueT,
  typename ToolTipT = StringT>
     requires std::assignable_from<
       std::remove_reference_t<ValueT> &,
       std::remove_cvref_t<InputValueT>>
class GenericCombo
{
   public:
     GenericCombo(
       std::string_view             name,
       std::span<const InputValueT> value_range,
       std::span<const StringT>     string_range,
       ValueT                      &value,
       generic_combo_settings       settings = {})
       : name_(name)
       , values_(value_range)
       , strings_(string_range)
       , tool_tips_(string_range)
       , value_(value)
       , settings_(settings)
       , current_idx_(0)
       , changed_(false)
       , spacing_(ImGui::GetStyle().ItemInnerSpacing.x)
     {
     }

     GenericCombo(
       std::string_view             name,
       std::span<const InputValueT> value_range,
       std::span<const StringT>     string_range,
       std::span<const ToolTipT>    tooltip_range,
       ValueT                      &value,
       generic_combo_settings       settings = {})
       : name_(name)
       , values_(value_range)
       , strings_(string_range)
       , tool_tips_(tooltip_range)
       , value_(value)
       , settings_(settings)
       , current_idx_(0)
       , changed_(false)
       , spacing_(ImGui::GetStyle().ItemInnerSpacing.x)
     {
     }

     bool render() const
     {
          if (std::ranges::empty(values_) || std::ranges::empty(strings_))
          {
               return false;
          }

          updateCurrentIndex();

          const auto old_idx = current_idx_;
          renderComboBox();
          renderLeftButton();
          renderRightButton();
          renderExploreButton();
          renderTitle();

          value_.get() = *getNext(values_, current_idx_);

          if (old_idx != current_idx_)
          {
               spdlog::info(
                 "{}: \t{}\t{}\t{}",
                 gui_labels::set,
                 name_,
                 *getNext(values_, current_idx_),
                 *getNext(strings_, current_idx_));
          }
          return old_idx != current_idx_ || changed_;
     }

   private:
     std::string_view                                           name_;
     std::span<const InputValueT>                               values_;
     std::span<const StringT>                                   strings_;
     std::span<const ToolTipT>                                  tool_tips_;
     std::reference_wrapper<ValueT>                             value_;
     generic_combo_settings                                     settings_;
     mutable std::ranges::range_difference_t<decltype(values_)> current_idx_;
     mutable bool                                               changed_;
     const float                                                spacing_;

     void updateCurrentIndex() const
     {
          const auto found = std::find(
            std::ranges::begin(values_),
            std::ranges::end(values_),
            value_.get());
          if (found != std::ranges::end(values_))
          {
               current_idx_
                 = std::ranges::distance(std::ranges::begin(values_), found);
          }
          else
          {
               current_idx_ = 0;
               if (!values_.empty())
               {
                    value_.get() = values_.front();
               }
               changed_ = true;
          }
     }

     template<typename Range>
     auto getNext(
       const Range &range,
       const auto  &idx) const
     {
          return std::ranges::next(
            std::ranges::begin(range),
            static_cast<std::ranges::range_difference_t<Range>>(idx));
     }

     void renderComboBox() const
     {
          const auto  pop_id       = PushPopID();
          const float button_size  = ImGui::GetFrameHeight();
          const float button_count = [&]()
          {
               int count = 2;
               if (settings_.show_explore_button)
               {
                    ++count;
               }
               return static_cast<float>(count);
          }();
          ImGui::PushItemWidth(
            ImGui::CalcItemWidth() - spacing_ * button_count
            - button_size * button_count);
          const auto pop_item_width
            = glengine::ScopeGuard(&ImGui::PopItemWidth);

          const auto current_item_cstr = [&]() -> const char *
          {
               if (
                 std::ranges::empty(strings_)
                 || !std::cmp_less(current_idx_, std::ranges::size(strings_)))
               {
                    return "N/A";
               }
               return std::data(*getNext(strings_, current_idx_));
          }();

          auto max_it = std::ranges::max_element(
            strings_,
            {},
            [](const auto &s)
            { return ImGui::CalcTextSize(std::ranges::data(s)).x; });
          float width = 0.0f;
          if (max_it != strings_.end())
          {
               width = ImGui::CalcTextSize(std::data(*max_it)).x
                       * static_cast<float>(settings_.num_columns);
          }

          // Add some padding so text isn't right up against the edge
          width += ImGui::GetStyle().FramePadding.x * 4.0f
                   * static_cast<float>(settings_.num_columns);

          ImGui::SetNextWindowSize(ImVec2(width, 0));

          if (ImGui::BeginCombo(
                "##Empty", current_item_cstr, ImGuiComboFlags_HeightLarge))
          // The second parameter is the label previewed
          // before opening the combo.
          {
               ImGui::Columns(settings_.num_columns, "##columns", false);
               for (const auto &[index_raw, string] :
                    std::views::enumerate(strings_))
               {
                    const auto index
                      = static_cast<decltype(current_idx_)>(index_raw);
                    const bool is_selected = (index == current_idx_);
                    // You can store your selection however you
                    // want, outside or inside your objects
                    if (std::ranges::empty(string))
                    {
                         return;
                    }
                    const char *c_str_value = std::ranges::data(string);
                    {
                         const auto pop_id2 = PushPopID();
                         const auto pop_column
                           = glengine::ScopeGuard{ &ImGui::NextColumn };

                         if (ImGui::Selectable(c_str_value, is_selected))
                         {
                              for (current_idx_ = 0;
                                   const auto &temp : strings_)
                              {
                                   if (std::ranges::equal(temp, string))
                                   {
                                        changed_ = true;
                                        break;
                                   }
                                   ++current_idx_;
                              }
                         }
                    }
                    if (is_selected)
                    {
                         ImGui::SetItemDefaultFocus();
                         // You may set the initial focus when
                         // opening the combo (scrolling + for
                         // keyboard navigation support)
                    }
                    renderToolTip(index);
               }
               ImGui::Columns(1);
               ImGui::EndCombo();
          }
          renderToolTip(current_idx_);
     }
     void renderToolTip(const decltype(current_idx_) index) const
     {

          if (std::ranges::empty(tool_tips_))
          {
               return;
          }
          tool_tip(*getNext(tool_tips_, index));
     }

     void renderLeftButton() const
     {
          const auto _            = PushPopID();
          const auto pop_disabled = glengine::ScopeGuard{ &ImGui::EndDisabled };
          ImGui::SameLine(0, spacing_);
          const auto check_valid = [&]()
          {
               return (current_idx_ <= decltype(current_idx_){})
                      || (current_idx_ - 1 >= size_of_values());
          };
          const bool disabled = check_valid();
          ImGui::BeginDisabled(disabled);
          if (ImGui::ArrowButton("##l", ImGuiDir_Left))
          {
               --current_idx_;
               changed_ = true;

               while (std::ranges::empty(*getNext(strings_, current_idx_))
                      && !check_valid())
               {
                    --current_idx_;
               }
          }
     }

     auto size_of_values() const
     {
          return static_cast<std::ranges::range_difference_t<
            std::remove_cvref_t<decltype(values_)>>>(
            std::ranges::size(values_));
     }

     void renderRightButton() const
     {
          const auto pop_id_right = PushPopID();
          ImGui::SameLine(0, spacing_);
          const auto check_valid
            = [&]() { return (current_idx_ + 1 >= size_of_values()); };
          const bool disabled = check_valid();
          ImGui::BeginDisabled(disabled);
          if (ImGui::ArrowButton("##r", ImGuiDir_Right))
          {
               ++current_idx_;
               changed_ = true;
               while (std::ranges::empty(*getNext(strings_, current_idx_))
                      && !check_valid())
               {
                    ++current_idx_;
               }
          }
          ImGui::EndDisabled();
     }

     void renderExploreButton() const
     {
          if (!settings_.show_explore_button)
          {
               return;
          }
          ImGui::SameLine(0, spacing_);
          const float button_size = ImGui::GetFrameHeight();
          const auto  _           = PushPopID();
          if (ImGui::Button(
                ICON_FA_FOLDER_OPEN, ImVec2{ button_size, button_size }))
          {
               open_directory(*getNext(strings_, current_idx_));
          }
          tool_tip(gui_labels::explore_tooltip);
     }

     void renderTitle() const
     {
          if (std::ranges::empty(name_))
          {
               return;
          }
          ImGui::SameLine(0, spacing_);
          format_imgui_text("{}", name_);
     }
};

// Deduction guide
template<
  std::ranges::contiguous_range ValueRangeT,
  std::ranges::contiguous_range StringRangeT,
  std::ranges::contiguous_range ToolTipRangeT,
  typename ValueT>
GenericCombo(
  std::string_view,
  ValueRangeT &&,
  StringRangeT &&,
  ToolTipRangeT &&,
  ValueT &,
  generic_combo_settings = {})
  -> GenericCombo<
    std::ranges::range_value_t<ValueRangeT>, // InputValueT
    std::ranges::range_value_t<StringRangeT>,// StringT
    ValueT,                                  // ValueT
    std::ranges::range_value_t<ToolTipRangeT>// ToolTipT
    >;

template<
  std::ranges::contiguous_range ValueRangeT,
  std::ranges::contiguous_range StringRangeT,
  typename ValueT>
GenericCombo(
  std::string_view,
  ValueRangeT &&,
  StringRangeT &&,
  ValueT &,
  generic_combo_settings = {})
  -> GenericCombo<
    std::ranges::range_value_t<ValueRangeT>, // InputValueT
    std::ranges::range_value_t<StringRangeT>,// StringT
    ValueT,                                  // ValueT
    std::ranges::range_value_t<StringRangeT> // ToolTipT = StringT
    >;


// typename InputValueT, typename StringT, typename ValueT,
//   typename ToolTipT = StringT >

}// namespace fme
#endif// FIELD_MAP_EDITOR_GENERIC_COMBO_HPP
