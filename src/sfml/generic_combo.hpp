//
// Created by pcvii on 11/15/2021.
//

#ifndef FIELD_MAP_EDITOR_GENERIC_COMBO_HPP
#define FIELD_MAP_EDITOR_GENERIC_COMBO_HPP
#include "format_imgui_text.hpp"
#include "gui/gui_labels.hpp"
#include "gui/push_pop_id.hpp"
#include "gui/tool_tip.hpp"
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
       std::string_view    name,
       ValueLambdaT      &&value_lambda,
       StringLambdaT     &&string_lambda,
       tool_tip_lambda_t &&tool_tip_lambda,
       filter_lambdaT    &&filter_lambda,
       int                 num_columns = 2)
       : name_(name)
       , values_(std::invoke(std::forward<ValueLambdaT>(value_lambda)))
       , strings_(std::invoke(std::forward<StringLambdaT>(string_lambda)))
       , tool_tips_(std::invoke(std::forward<tool_tip_lambda_t>(tool_tip_lambda)))
       , filter_(std::invoke(std::forward<filter_lambdaT>(filter_lambda)))
       , num_columns_(num_columns)
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
     int                                                                               num_columns_;
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
          const float button_count = 3.0f;
          ImGui::PushItemWidth(ImGui::CalcItemWidth() - spacing_ * button_count - button_size * button_count);
          const auto  pop_item_width = scope_guard(&ImGui::PopItemWidth);

          const auto &current_item   = *getNext(strings_, current_idx_);

          if (ImGui::BeginCombo("##Empty", std::ranges::data(current_item), ImGuiComboFlags_HeightLarge))
          // The second parameter is the label previewed
          // before opening the combo.
          {
               ImGui::Columns(num_columns_, "##columns", false);

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

     void renderLeftButton() const
     {
          const auto _ = PushPopID();
          ImGui::SameLine(0, spacing_);
          const bool disabled =
            std::cmp_less_equal(current_idx_, 0) || std::cmp_greater_equal(current_idx_ - 1, std::ranges::size(values_));
          ImGui::BeginDisabled(disabled);
          if (ImGui::ArrowButton("##l", ImGuiDir_Left))
          {
               --current_idx_;
               changed_ = true;
               filter_.get().enable();
          }
          ImGui::EndDisabled();
     }

     void renderRightButton() const
     {
          const auto _ = PushPopID();
          ImGui::SameLine(0, spacing_);
          const bool disabled = std::cmp_greater_equal(current_idx_ + 1, std::ranges::size(values_));
          ImGui::BeginDisabled(disabled);
          if (ImGui::ArrowButton("##r", ImGuiDir_Right))
          {
               ++current_idx_;
               changed_ = true;
               filter_.get().enable();
          }
          ImGui::EndDisabled();
     }
     void renderTitle() const
     {
          ImGui::SameLine(0, spacing_);
          format_imgui_text("{}", name_);
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
       std::string_view name,
       ValueLambdaT   &&value_lambda,
       StringLambdaT  &&string_lambda,
       ValueT          &value,
       int              num_columns = 2)
       : name_(name)
       , values_(std::invoke(std::forward<ValueLambdaT>(value_lambda)))
       , strings_(std::invoke(std::forward<StringLambdaT>(string_lambda)))
       , tool_tips_(std::invoke(std::forward<StringLambdaT>(string_lambda)))
       , value_(value)
       , num_columns_(num_columns)
       , current_idx_(0)
       , changed_(false)
       , spacing_(ImGui::GetStyle().ItemInnerSpacing.x)
     {
     }

     GenericComboClass(
       std::string_view    name,
       ValueLambdaT      &&value_lambda,
       StringLambdaT     &&string_lambda,
       tool_tip_lambda_t &&tooltip_lamda,
       ValueT             &value,
       int                 num_columns = 2)
       : name_(name)
       , values_(std::invoke(std::forward<ValueLambdaT>(value_lambda)))
       , strings_(std::invoke(std::forward<StringLambdaT>(string_lambda)))
       , tool_tips_(std::invoke(std::forward<tool_tip_lambda_t>(tooltip_lamda)))
       , value_(value)
       , num_columns_(num_columns)
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
          renderComboBox();
          renderLeftButton();
          renderRightButton();
          renderTitle();

          value_.get() = *getNext(values_, current_idx_);

          if (old_idx != current_idx_)
          {
               static constexpr auto pattern = "{}: \t{}\t{}\t{}";
               spdlog::info(pattern, gui_labels::set, name_, *getNext(values_, current_idx_), *getNext(strings_, current_idx_));
          }
          return old_idx != current_idx_ || changed_;
     }

   private:
     std::string_view                                           name_;
     std::invoke_result_t<ValueLambdaT>                         values_;
     std::invoke_result_t<StringLambdaT>                        strings_;
     std::invoke_result_t<tool_tip_lambda_t>                    tool_tips_;
     std::reference_wrapper<ValueT>                             value_;
     int                                                        num_columns_;
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
          return std::ranges::next(std::ranges::begin(range), idx);
     }

     void renderComboBox() const
     {
          const auto  pop_id       = PushPopID();
          const float button_size  = ImGui::GetFrameHeight();
          const float button_count = 2.0f;
          ImGui::PushItemWidth(ImGui::CalcItemWidth() - spacing_ * button_count - button_size * button_count);
          const auto  pop_item_width = scope_guard(&ImGui::PopItemWidth);

          const auto &current_item   = *getNext(strings_, current_idx_);

          if (ImGui::BeginCombo("##Empty", std::ranges::data(current_item), ImGuiComboFlags_HeightLarge))
          // The second parameter is the label previewed
          // before opening the combo.
          {
               ImGui::Columns(num_columns_, "##columns", false);
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
          tool_tip(*getNext(tool_tips_, index)); if (!ImGui::IsItemHovered())
          {
               return;
          }
     }

     void renderLeftButton() const
     {
          const auto _            = PushPopID();
          const auto pop_disabled = scope_guard{ &ImGui::EndDisabled };
          ImGui::SameLine(0, spacing_);
          const bool disabled =
            std::cmp_less_equal(current_idx_, 0) || std::cmp_greater_equal(current_idx_ - 1, std::ranges::size(values_));
          ImGui::BeginDisabled(disabled);
          if (ImGui::ArrowButton("##l", ImGuiDir_Left))
          {
               --current_idx_;
               changed_ = true;
          }
     }

     void renderRightButton() const
     {
          const auto pop_id_right = PushPopID();
          ImGui::SameLine(0, spacing_);
          const bool disabled = std::cmp_greater_equal(current_idx_ + 1, std::ranges::size(values_));
          ImGui::BeginDisabled(disabled);
          if (ImGui::ArrowButton("##r", ImGuiDir_Right))
          {
               ++current_idx_;
               changed_ = true;
          }
          ImGui::EndDisabled();
     }
     void renderTitle() const
     {
          ImGui::SameLine(0, spacing_);
          format_imgui_text("{}", name_);
     }
};

}// namespace fme
#endif// FIELD_MAP_EDITOR_GENERIC_COMBO_HPP
