//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TESTCLEARCOLOR_HPP
#define MYPROJECT_TESTCLEARCOLOR_HPP


#include "ImGuiViewPortWindow.hpp"
#include "Renderer.hpp"
namespace test
{
class TestClearColor
{
public:
  constexpr TestClearColor() {}
  constexpr void OnUpdate(float) const {}
  constexpr void OnRender() const
  {
    if (!std::is_constant_evaluated())
    {
      m_imgui_viewport_window.SyncOpenGLViewPort();
      m_imgui_viewport_window.OnRender([]() { LocalClear(); });
    }
  }
  constexpr void OnImGuiUpdate() const
  {
    if (!std::is_constant_evaluated())
    {
      if (ImGui::ColorEdit4("Clear Color", &m_clear_color.r))
      {
        LocalClear.Color(m_clear_color);
      }
    }
  }
  constexpr void OnEvent(const glengine::Event::Item &) const {}

private:
  static constexpr glm::vec4        DEFAULT_COLOR = { 0.2F, 0.3F, 0.8F, 1.F };
  static inline constinit glm::vec4 m_clear_color = { DEFAULT_COLOR };
  static inline constinit glengine::Renderer::Clear_impl LocalClear = {
    DEFAULT_COLOR
  };
  glengine::ImGuiViewPortWindow m_imgui_viewport_window = {
    "Test Clear Color"
  };
};
}// namespace test
static_assert(glengine::Renderable<test::TestClearColor>);
#endif// MYPROJECT_TESTCLEARCOLOR_HPP
