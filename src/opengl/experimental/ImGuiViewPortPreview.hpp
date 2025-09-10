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
               m_preview_view_port.disable_debug_text();
          }
          [[nodiscard]] float view_port_aspect_ratio() const
          {
               return m_preview_view_port.view_port_aspect_ratio();
          }

          void on_update(float) const
          {
               m_drawn = false;
               ImGuiViewPortWindow::set_preview_aspect_ratio(
                 view_port_aspect_ratio());
               // m_camera_controller.SetZoom(m_preview_view_port.ViewPortDims().y);
          }
          // A way to hand a ImGuiViewPortWindow, and a OrthographicCamera or
          // OrthographicCameraController and draw a preview window.
          void on_render(
            const ImGuiViewPortWindow &window,
            std::invocable auto      &&callable) const
          {
               if (!m_drawn && window.has_hover())
               {
                    m_preview_view_port.sync_open_gl_view_port();
                    m_preview_view_port.m_clear_impl.Color(
                      window.get_background_color());
                    m_preview_view_port.m_background_color
                      = window.get_background_color();
                    m_preview_view_port.on_render(
                      std::forward<decltype(callable)>(callable));
                    m_drawn = true;
               }
          }
          constexpr void on_im_gui_update() const {}
          void           on_render() const
          {
               if (!m_drawn)
               {
                    m_preview_view_port.m_background_color = {};
                    m_preview_view_port.m_clear_impl.Color({});
                    m_preview_view_port.on_render(
                      []() {});// might need a function to draw the empty
                               // window.
               }
          }
          void on_event(const event::Item &) const {}

        private:
          ImGuiViewPortWindow m_preview_view_port = { "Preview" };
          mutable bool        m_drawn             = false;
     };
     static_assert(Renderable<ImGuiViewPortPreview>);
}// namespace impl
}// namespace glengine
#endif// FIELD_MAP_EDITOR_IMGUIVIEWPORTPREVIEW_HPP
