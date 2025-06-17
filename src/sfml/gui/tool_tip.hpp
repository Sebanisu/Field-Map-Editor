#ifndef B4F3B564_D2A3_4C2C_992A_95E9DAEA83EF
#define B4F3B564_D2A3_4C2C_992A_95E9DAEA83EF
#include "format_imgui_text.hpp"
#include "push_pop_id.hpp"
#include <concepts>
#include <functional>
#include <string_view>
namespace fme
{
/**
 * @brief Displays a tooltip with the specified text.
 *
 * This function creates a tooltip when the user hovers over an item. If the `override`
 * parameter is set to true, the tooltip is displayed regardless of whether the item is hovered.
 *
 * @param str The text to display in the tooltip.
 * @param override Optional. If true, forces the tooltip to display even if the item is not hovered. Default is false.
 */
template<typename T>
     requires(!std::invocable<T>)
void tool_tip(T &&str, bool override = false)
{
     if (!ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !override)
     {
          return;
     }

     // const auto pop_id       = PushPopID();
     if (ImGui::BeginTooltip())
     {
          const auto pop_tool_tip    = glengine::ScopeGuard{ &ImGui::EndTooltip };
          const auto pop_textwrappos = glengine::ScopeGuard{ &ImGui::PopTextWrapPos };
          ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);// Adjust wrap width as needed
          format_imgui_text("{}", std::forward<T>(str));
     }
}

/**
 * @brief Displays a tooltip using a lambda function to customize its content.
 *
 * This template function allows advanced tooltips by passing a lambda function to define
 * custom content. The tooltip is shown when the item is hovered unless `override` is true.
 *
 * @tparam lambdaT The type of the lambda function that generates the tooltip content.
 * @param lambda The lambda function to invoke for rendering the tooltip content.
 * @param override If true, forces the tooltip to display even if the item is not hovered.  Default is false.
 */
template<std::invocable lambdaT>
static inline void tool_tip(lambdaT &&lambda, bool override = false)
{
     if (!ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !override)
     {
          return;
     }

     // const auto pop_id       = PushPopID();
     if (ImGui::BeginTooltip())
     {
          const auto pop_tool_tip = glengine::ScopeGuard{ &ImGui::EndTooltip };

          std::invoke(std::forward<lambdaT>(lambda));
     }
}

}// namespace fme

#endif /* B4F3B564_D2A3_4C2C_992A_95E9DAEA83EF */
