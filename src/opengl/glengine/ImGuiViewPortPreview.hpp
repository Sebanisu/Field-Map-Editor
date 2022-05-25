//
// Created by pcvii on 5/2/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIVIEWPORTPREVIEW_HPP
#define FIELD_MAP_EDITOR_IMGUIVIEWPORTPREVIEW_HPP
#include "ImGuiViewPortWindow.hpp"
#include "OrthographicCameraController.hpp"
namespace glengine
{
inline namespace impl
{
  class ImGuiViewPortPreview
  {
  public:
    ImGuiViewPortPreview()
    {
      m_preview_view_port.DisableDebugText();
    }
    [[nodiscard]] float ViewPortAspectRatio() const
    {
      return m_preview_view_port.ViewPortAspectRatio();
    }

    void OnUpdate(float) const
    {
      m_drawn = false;
      // m_camera_controller.SetZoom(m_preview_view_port.ViewPortDims().y);
    }
    // A way to hand a ImGuiViewPortWindow, and a OrthographicCamera or
    // OrthographicCameraController and draw a preview window.
    void OnRender(
      const bool            current_view_port_has_hover,
      std::invocable auto &&callable) const
    {
      if (!m_drawn && current_view_port_has_hover)
      {
        m_preview_view_port.SyncOpenGLViewPort();
        m_preview_view_port.OnRender(
          std::forward<decltype(callable)>(callable));
        m_drawn = true;
      }
    }
    constexpr void OnImGuiUpdate() const {}
    void           OnRender() const
    {
      if (!m_drawn)
      {
        m_preview_view_port.OnRender(
          []() {});// might need a function to draw the empty window.
      }
    }
    void OnEvent(const Event::Item &) const {}

  private:
    ImGuiViewPortWindow                  m_preview_view_port = { "Preview" };
    mutable bool                         m_drawn             = false;
  };
  static_assert(Renderable<ImGuiViewPortPreview>);
}// namespace impl
}// namespace glengine
#endif// FIELD_MAP_EDITOR_IMGUIVIEWPORTPREVIEW_HPP
