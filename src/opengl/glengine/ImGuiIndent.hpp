//
// Created by pcvii on 1/25/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIINDENT_HPP
#define FIELD_MAP_EDITOR_IMGUIINDENT_HPP
#include "scope_guard.hpp"
namespace glengine
{
auto ImGuiIndent(float width = 0.F) noexcept

{
  ImGui::Indent(width);
  struct UnIndent
  {
    float width = { width };
    ~UnIndent()
    {
      ImGui::Unindent(width);
    }
  } r{};
  return r;
}
}// namespace glengine
#endif// FIELD_MAP_EDITOR_IMGUIINDENT_HPP
