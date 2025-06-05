//
// Created by pcvii on 11/15/2021.
//

#ifndef FIELD_MAP_EDITOR_GENERIC_COMBO_HPP
#define FIELD_MAP_EDITOR_GENERIC_COMBO_HPP
#include "format_imgui_text.hpp"
#include "gui/fa_icons.hpp"
#include "gui/gui_labels.hpp"
#include "gui/push_pop_id.hpp"
#include "gui/tool_tip.hpp"
#include "open_file_explorer.hpp"
#include "scope_guard.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <functional>
#include <imgui.h>
#include <ranges>
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
concept returns_range_concept = requires(std::remove_cvref_t<T> callable) {
     { callable() } -> std::ranges::range;
};
template<typename T>
concept filter_concept = requires(std::remove_cvref_t<T> filter) {
     { filter.enabled() } -> std::convertible_to<bool>;
     { filter.update(filter.value()) } -> std::convertible_to<T>;
     { filter.enable() } -> std::convertible_to<T>;
     { filter.disable() } -> std::convertible_to<T>;
};
template<typename T>
concept returns_filter_concept = requires(std::remove_cvref_t<T> callable) {
     { callable() } -> filter_concept;
};

template<
  returns_range_concept  ValueLambdaT,
  returns_range_concept  StringLambdaT,
  returns_range_concept  tool_tip_lambda_t,
  returns_filter_concept filter_lambdaT>
class GenericComboClassWithFilter
{
   public:
     GenericComboClassWithFilter(
       std::string_view       name,
       ValueLambdaT         &&value_lambda,
       StringLambdaT        &&string_lambda,
       tool_tip_lambda_t    &&tool_tip_lambda,
       filter_lambdaT       &&filter_lambda,
       generic_combo_settings settings = {})
       : name_(name)
       , values_(std::invoke(std::forward<ValueLambdaT>(value_lambda)))
       , strings_(std::invoke(std::forward<StringLambdaT>(string_lambda)))
       , tool_tips_(std::invoke(std::forward<tool_tip_lambda_t>(tool_tip_lambda)))
       , filter_(std::invoke(std::forward<filter_lambdaT>(filter_lambda)))
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
               spdlog::info(pattern, gui_labels::set, name_, *getNext(values_, current_idx_), *getNext(strings_, current_idx_));
               filter_.get().update(*getNext(values_, current_idx_));
          }

          return old_idx != current_idx_ || changed_;
     }

   private:
     std::string_view                                                                  name_;
     std::invoke_result_t<ValueLambdaT>                                                values_;
     std::invoke_result_t<StringLambdaT>                                               strings_;
     std::invoke_result_t<tool_tip_lambda_t>                                           tool_tips_;
     std::reference_wrapper<std::remove_cvref_t<std::invoke_result_t<filter_lambdaT>>> filter_;
     generic_combo_settings                                                            settings_;
     mutable std::ranges::range_difference_t<decltype(values_)>                        current_idx_;
     mutable bool                                                                      changed_;
     const float                                                                       spacing_;

     void                                                                              updateCurrentIndex() const
     {
          const auto found = std::find(std::ranges::begin(values_), std::ranges::end(values_), filter_.get().value());
          if (found != std::ranges::end(values_))
          {
               current_idx_ = std::ranges::distance(std::ranges::begin(values_), found);
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
     auto getNext(const Range &range, const auto &idx) const
     {
          return std::ranges::next(std::ranges::begin(range), idx);
     }
     void renderCheckBox() const
     {
          const auto _ = PushPopID();
          if (bool checked = filter_.get().enabled(); ImGui::Checkbox("", &checked))
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
          const float button_count = [&]() {
               int count = 3;
               if (settings_.show_explore_button)
               {
                    ++count;
               }
               return static_cast<float>(count);
          }();
          ImGui::PushItemWidth(ImGui::CalcItemWidth() - spacing_ * button_count - button_size * button_count);
          const auto  pop_item_width = scope_guard(&ImGui::PopItemWidth);

          const auto &current_item   = *getNext(strings_, current_idx_);

          if (ImGui::BeginCombo("##Empty", std::ranges::data(current_item), ImGuiComboFlags_HeightLarge))
          // The second parameter is the label previewed
          // before opening the combo.
          {
               ImGui::Columns(settings_.num_columns, "##columns", false);

               std::ranges::for_each(strings_, [&, index = decltype(current_idx_){}](const auto &string) mutable {
                    const bool  is_selected = (current_item == string);
                    // You can store your selection however you
                    // want, outside or inside your objects
                    const char *c_str_value = std::ranges::data(string);
                    {
                         const auto pop_id     = PushPopID();
                         const auto pop_column = scope_guard{ &ImGui::NextColumn };
                         if (ImGui::Selectable(c_str_value, is_selected))
                         {
                              for (current_idx_ = 0; const auto &temp : strings_)
                              {
                                   if (std::ranges::equal(temp, string))
                                   {
                                        changed_ = true;
                                        filter_.get().enable();
                                        break;
                                   }
                                   ++current_idx_;
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
                    renderToolTip(index);
                    ++index;
               });
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
          return static_cast<std::ranges::range_difference_t<std::remove_cvref_t<decltype(values_)>>>(std::ranges::size(values_));
     }

     void renderLeftButton() const
     {
          const auto _ = PushPopID();
          ImGui::SameLine(0, spacing_);
          const auto check_valid = [&]() { return (current_idx_ <= decltype(current_idx_){}) || (current_idx_ - 1 >= size_of_values()); };
          const bool disabled    = check_valid();
          ImGui::BeginDisabled(disabled);
          if (ImGui::ArrowButton("##l", ImGuiDir_Left))
          {
               --current_idx_;
               changed_ = true;
               filter_.get().enable();
               while (std::ranges::empty(*getNext(strings_, current_idx_)) && !check_valid())
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
          const auto check_valid = [&]() { return (current_idx_ + 1 >= size_of_values()); };
          const bool disabled    = check_valid();
          ImGui::BeginDisabled(disabled);
          if (ImGui::ArrowButton("##r", ImGuiDir_Right))
          {
               ++current_idx_;
               changed_ = true;
               filter_.get().enable();
               while (std::ranges::empty(*getNext(strings_, current_idx_)) && !check_valid())
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
          if (ImGui::Button(ICON_FA_FOLDER_OPEN, ImVec2{ button_size, button_size }))
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
  returns_range_concept  ValueLambdaT,
  returns_range_concept  FixedTogglesLambdaT,
  returns_range_concept  StringLambdaT,
  returns_range_concept  ToolTipLambdaT,
  returns_filter_concept FilterLambdaT>
class GenericComboClassWithFilterAndFixedToggles
{
   public:
     GenericComboClassWithFilterAndFixedToggles(
       std::string_view       name,
       ValueLambdaT         &&value_lambda,
       FixedTogglesLambdaT  &&fixed_toggles_lambda,
       StringLambdaT        &&string_lambda,
       ToolTipLambdaT       &&tool_tip_lambda,
       FilterLambdaT        &&filter_lambda,
       generic_combo_settings settings = {})
       : name_(name)
       , values_(std::invoke(std::forward<ValueLambdaT>(value_lambda)))
       , fixed_toggles_(std::invoke(std::forward<FixedTogglesLambdaT>(fixed_toggles_lambda)))
       , strings_(std::invoke(std::forward<StringLambdaT>(string_lambda)))
       , tool_tips_(std::invoke(std::forward<ToolTipLambdaT>(tool_tip_lambda)))
       , filter_(std::invoke(std::forward<FilterLambdaT>(filter_lambda)))
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
          {
               ImGui::BeginDisabled(std::ranges::none_of(fixed_toggles_, std::identity{}));
               const auto pop_disabled = scope_guard{ &ImGui::EndDisabled };
               renderComboBox();
               renderLeftButton();
               renderRightButton();
               renderExploreButton();
          }
          renderTitle();


          if (old_idx != current_idx_)
          {
               static constexpr auto pattern = "{}: \t{}\t{}\t{}";
               spdlog::info(pattern, gui_labels::set, name_, *getNext(values_, current_idx_), *getNext(strings_, current_idx_));
               filter_.get().update(*getNext(values_, current_idx_));
          }

          return old_idx != current_idx_ || changed_;
     }

   private:
     std::string_view                                                                 name_;
     std::invoke_result_t<ValueLambdaT>                                               values_;
     std::invoke_result_t<FixedTogglesLambdaT>                                        fixed_toggles_;
     std::invoke_result_t<StringLambdaT>                                              strings_;
     std::invoke_result_t<ToolTipLambdaT>                                             tool_tips_;
     std::reference_wrapper<std::remove_cvref_t<std::invoke_result_t<FilterLambdaT>>> filter_;
     generic_combo_settings                                                           settings_;
     mutable std::ranges::range_difference_t<decltype(values_)>                       current_idx_;
     mutable bool                                                                     changed_;
     const float                                                                      spacing_;

     void                                                                             updateCurrentIndex() const
     {
          const auto found = std::find(std::ranges::begin(values_), std::ranges::end(values_), filter_.get().value());
          if (found != std::ranges::end(values_))
          {
               current_idx_ = std::ranges::distance(std::ranges::begin(values_), found);
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
     auto getNext(const Range &range, const auto &idx) const
     {
          return std::ranges::next(std::ranges::begin(range), idx);
     }
     void renderCheckBox() const
     {
          const auto  _                    = PushPopID();
          const auto &current_fixed_toggle = *getNext(fixed_toggles_, current_idx_);
          ImGui::BeginDisabled(!current_fixed_toggle && !filter_.get().enabled());
          const auto pop_disabled = scope_guard{ &ImGui::EndDisabled };
          if (bool checked = filter_.get().enabled(); ImGui::Checkbox("", &checked))
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
          const float button_count = [&]() {
               int count = 3;
               if (settings_.show_explore_button)
               {
                    ++count;
               }
               return static_cast<float>(count);
          }();

          // Set up combo box width
          ImGui::PushItemWidth(ImGui::CalcItemWidth() - spacing_ * button_count - button_size * button_count);
          const auto  pop_item_width = scope_guard(&ImGui::PopItemWidth);

          // Get the current item
          const auto &current_item   = *getNext(strings_, current_idx_);

          // Begin combo box with preview of current item
          if (ImGui::BeginCombo("##Empty", std::ranges::data(current_item), ImGuiComboFlags_HeightLarge))
          {
               ImGui::Columns(settings_.num_columns, "##columns", false);
               for (const auto &[index_raw, string] : std::ranges::views::enumerate(strings_))
               {
                    const auto  index                = static_cast<decltype(current_idx_)>(index_raw);
                    const bool  is_selected          = std::ranges::equal(current_item, string);
                    const auto  pop_id               = PushPopID();
                    const auto  pop_column           = scope_guard{ &ImGui::NextColumn };

                    const auto &current_fixed_toggle = *getNext(fixed_toggles_, index);
                    ImGui::BeginDisabled(!current_fixed_toggle);
                    const auto pop_disabled = scope_guard{ &ImGui::EndDisabled };

                    if (ImGui::Selectable(std::ranges::data(string), is_selected))
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
          return static_cast<std::ranges::range_difference_t<std::remove_cvref_t<decltype(values_)>>>(std::ranges::size(values_));
     }

     void renderLeftButton() const
     {
          const auto _           = PushPopID();

          const auto check_valid = [&]() { return (current_idx_ <= decltype(current_idx_){}) || (current_idx_ - 1 >= size_of_values()); };
          // Check if there is any index < current_idx_ where fixed_toggles_ is true
          const auto has_valid_left_index = [&]() -> std::optional<decltype(current_idx_)> {
               if (!check_valid())
               {
                    for (auto i = current_idx_ - 1; i >= 0; --i)
                    {
                         if (*getNext(fixed_toggles_, i))
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
          const auto check_valid           = [&]() { return (current_idx_ + 1 >= size_of_values()); };
          const auto has_valid_right_index = [&]() -> std::optional<decltype(current_idx_)> {
               if (!check_valid())
               {
                    for (auto i = current_idx_ + 1; i < size_of_values(); ++i)
                    {
                         if (*getNext(fixed_toggles_, i))
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
          if (ImGui::Button(ICON_FA_FOLDER_OPEN, ImVec2{ button_size, button_size }))
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
          ImVec2 cursorBackup = ImGui::GetCursorScreenPos();// Save cursor position
          ImGui::SetItemAllowOverlap();// Allow us to draw on top
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
  returns_range_concept ValueLambdaT,
  returns_range_concept StringLambdaT,
  typename ValueT,
  returns_range_concept tool_tip_lambda_t = StringLambdaT>
class GenericComboClass
{
   public:
     GenericComboClass(
       std::string_view       name,
       ValueLambdaT         &&value_lambda,
       StringLambdaT        &&string_lambda,
       ValueT                &value,
       generic_combo_settings settings = {})
       : name_(name)
       , values_(std::invoke(std::forward<ValueLambdaT>(value_lambda)))
       , strings_(std::invoke(std::forward<StringLambdaT>(string_lambda)))
       , tool_tips_(std::invoke(std::forward<StringLambdaT>(string_lambda)))
       , value_(value)
       , settings_(settings)
       , current_idx_(0)
       , changed_(false)
       , spacing_(ImGui::GetStyle().ItemInnerSpacing.x)
     {
     }

     GenericComboClass(
       std::string_view       name,
       ValueLambdaT         &&value_lambda,
       StringLambdaT        &&string_lambda,
       tool_tip_lambda_t    &&tooltip_lamda,
       ValueT                &value,
       generic_combo_settings settings = {})
       : name_(name)
       , values_(std::invoke(std::forward<ValueLambdaT>(value_lambda)))
       , strings_(std::invoke(std::forward<StringLambdaT>(string_lambda)))
       , tool_tips_(std::invoke(std::forward<tool_tip_lambda_t>(tooltip_lamda)))
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
               spdlog::info("{}: \t{}\t{}\t{}", gui_labels::set, name_, *getNext(values_, current_idx_), *getNext(strings_, current_idx_));
          }
          return old_idx != current_idx_ || changed_;
     }

   private:
     std::string_view                                           name_;
     std::invoke_result_t<ValueLambdaT>                         values_;
     std::invoke_result_t<StringLambdaT>                        strings_;
     std::invoke_result_t<tool_tip_lambda_t>                    tool_tips_;
     std::reference_wrapper<ValueT>                             value_;
     generic_combo_settings                                     settings_;
     mutable std::ranges::range_difference_t<decltype(values_)> current_idx_;
     mutable bool                                               changed_;
     const float                                                spacing_;

     void                                                       updateCurrentIndex() const
     {
          const auto found = std::find(std::ranges::begin(values_), std::ranges::end(values_), value_.get());
          if (found != std::ranges::end(values_))
          {
               current_idx_ = std::ranges::distance(std::ranges::begin(values_), found);
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
     auto getNext(const Range &range, const auto &idx) const
     {
          return std::ranges::next(std::ranges::begin(range), static_cast<std::ranges::range_difference_t<Range>>(idx));
     }

     void renderComboBox() const
     {
          const auto  pop_id       = PushPopID();
          const float button_size  = ImGui::GetFrameHeight();
          const float button_count = [&]() {
               int count = 2;
               if (settings_.show_explore_button)
               {
                    ++count;
               }
               return static_cast<float>(count);
          }();
          ImGui::PushItemWidth(ImGui::CalcItemWidth() - spacing_ * button_count - button_size * button_count);
          const auto  pop_item_width = scope_guard(&ImGui::PopItemWidth);

          const auto &current_item   = *getNext(strings_, current_idx_);

          if (ImGui::BeginCombo("##Empty", std::ranges::data(current_item), ImGuiComboFlags_HeightLarge))
          // The second parameter is the label previewed
          // before opening the combo.
          {
               ImGui::Columns(settings_.num_columns, "##columns", false);
               std::ranges::for_each(strings_, [&, index = decltype(current_idx_){}](const auto &string) mutable {
                    const bool is_selected = (current_item == string);
                    // You can store your selection however you
                    // want, outside or inside your objects
                    if (std::ranges::empty(string))
                    {
                         return;
                    }
                    const char *c_str_value = std::ranges::data(string);
                    {
                         const auto pop_id     = PushPopID();
                         const auto pop_column = scope_guard{ &ImGui::NextColumn };

                         if (ImGui::Selectable(c_str_value, is_selected))
                         {
                              for (current_idx_ = 0; const auto &temp : strings_)
                              {
                                   if (std::ranges::equal(temp, string))
                                   {
                                        changed_ = true;
                                        break;
                                   }
                                   ++current_idx_;
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
                    renderToolTip(index++);
               });
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
          if (!ImGui::IsItemHovered())
          {
               return;
          }
     }

     void renderLeftButton() const
     {
          const auto _            = PushPopID();
          const auto pop_disabled = scope_guard{ &ImGui::EndDisabled };
          ImGui::SameLine(0, spacing_);
          const auto check_valid = [&]() { return (current_idx_ <= decltype(current_idx_){}) || (current_idx_ - 1 >= size_of_values()); };
          const bool disabled    = check_valid();
          ImGui::BeginDisabled(disabled);
          if (ImGui::ArrowButton("##l", ImGuiDir_Left))
          {
               --current_idx_;
               changed_ = true;

               while (std::ranges::empty(*getNext(strings_, current_idx_)) && !check_valid())
               {
                    --current_idx_;
               }
          }
     }

     auto size_of_values() const
     {
          return static_cast<std::ranges::range_difference_t<std::remove_cvref_t<decltype(values_)>>>(std::ranges::size(values_));
     }

     void renderRightButton() const
     {
          const auto pop_id_right = PushPopID();
          ImGui::SameLine(0, spacing_);
          const auto check_valid = [&]() { return (current_idx_ + 1 >= size_of_values()); };
          const bool disabled    = check_valid();
          ImGui::BeginDisabled(disabled);
          if (ImGui::ArrowButton("##r", ImGuiDir_Right))
          {
               ++current_idx_;
               changed_ = true;
               while (std::ranges::empty(*getNext(strings_, current_idx_)) && !check_valid())
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
          if (ImGui::Button(ICON_FA_FOLDER_OPEN, ImVec2{ button_size, button_size }))
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

}// namespace fme
#endif// FIELD_MAP_EDITOR_GENERIC_COMBO_HPP
