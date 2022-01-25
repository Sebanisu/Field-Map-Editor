//
// Created by pcvii on 1/25/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIINDENT_HPP
#define FIELD_MAP_EDITOR_IMGUIINDENT_HPP
#include "scope_guard.hpp"
namespace glengine
{
class ImGuiIndent_impl
{
public:
  [[nodiscard]] scope_guard_expensive
    operator()(float width = 0.F) const noexcept
  {
    ImGui::Indent(width);
    return { [=]() { ImGui::Unindent(width); } };
  }
};
inline static constexpr ImGuiIndent_impl ImGuiIndent = {};
}// namespace glengine
#endif// FIELD_MAP_EDITOR_IMGUIINDENT_HPP
