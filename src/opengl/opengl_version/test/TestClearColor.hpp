//
// Created by pcvii on 11/24/2021.
//

#ifndef FIELD_MAP_EDITOR_TESTCLEARCOLOR_HPP
#define FIELD_MAP_EDITOR_TESTCLEARCOLOR_HPP
#include <version>

#include "Application.hpp"
#include "ImGuiViewPortWindow.hpp"
#include "Renderer.hpp"
namespace test
{
#if __cpp_if_consteval
#define FME_NOT_CONSTEVAL !consteval// c++23
#else
#define FME_NOT_CONSTEVAL (!std::is_constant_evaluated())// c++20
#endif
class TestClearColor
{

public:
  constexpr TestClearColor() {}
  constexpr void on_update(float) const {}
  constexpr void on_render() const
  {
    if FME_NOT_CONSTEVAL
    {
      m_imgui_viewport_window.on_render([]() { local_clear(); });
      GetViewPortPreview().on_render(m_imgui_viewport_window.has_hover(), []() {
        const auto pop_preview =
          glengine::ScopeGuard([]() { Preview = false; });
        Preview = true;
        local_clear();
      });
    }
  }
  constexpr void on_im_gui_update() const
  {
    if FME_NOT_CONSTEVAL
    {
      if (ImGui::ColorEdit4("Clear Color", &m_clear_color.r))
      {
        local_clear.Color(m_clear_color);
      }
    }
  }
  constexpr void on_event(const glengine::event::Item &) const {}

private:
  static constexpr glm::vec4        default_color = { 0.2F, 0.3F, 0.8F, 1.F };
  static inline constinit glm::vec4 m_clear_color = { default_color };
  static inline constinit glengine::Clear_impl local_clear = { default_color };
  static inline constinit bool                 Preview     = false;
  glengine::ImGuiViewPortWindow                m_imgui_viewport_window = {
                   "Test Clear Color"
  };
};
#undef FME_NOT_CONSTEVAL
}// namespace test
static_assert(glengine::Renderable<test::TestClearColor>);
#endif// FIELD_MAP_EDITOR_TESTCLEARCOLOR_HPP
