//
// Created by pcvii on 1/25/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIINDENT_HPP
#define FIELD_MAP_EDITOR_IMGUIINDENT_HPP
#include "scope_guard.hpp"
namespace glengine
{
//[[nodiscard]] inline auto ImGuiIndent(float width) noexcept
//{// todo this one needs testing.
//  ImGui::Indent(width);
//  struct [[nodiscard]] [[maybe_unused]] UnIndent
//  {
//  private:
//    float m_width = {};
//    bool  enabled = { false };
//    UnIndent()    = default;
//
//  public:
//    UnIndent(float in_width)
//      : m_width(std::move(in_width))
//    {
//    }
//    UnIndent(const UnIndent &) = default;
//    UnIndent(UnIndent &&right) noexcept
//      : UnIndent()
//    {
//      swap(*this,right);
//    }
//    UnIndent &operator=(const UnIndent &) = default;
//    UnIndent &operator=(UnIndent &&right) noexcept
//    {
//      swap(*this,right);
//      return *this;
//    }
//    static void swap(UnIndent &left, UnIndent &right) noexcept
//    {
//      using std::swap;
//      swap(left.m_width, right.m_width);
//      swap(left.enabled, right.enabled);
//    }
//    ~UnIndent()
//    {
//      if (enabled)
//      {
//        ImGui::Unindent(m_width);
//      }
//    }
//  };
//  return UnIndent{ width };
//}

//[[nodiscard]] inline auto ImGuiIndent() noexcept
//
//{
//  ImGui::Indent();
//  return glengine::scope_guard{ []() { ImGui::Unindent(); } };
//}
[[nodiscard]] inline auto ImGuiIndent(float width = 0.F) noexcept

{
  ImGui::Indent(width);
  return glengine::scope_guard_captures{ [=]() { ImGui::Unindent(width); } };
}
}// namespace glengine
#endif// FIELD_MAP_EDITOR_IMGUIINDENT_HPP
