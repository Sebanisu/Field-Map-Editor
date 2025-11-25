//
// Created by pcvii on 4/26/2022.
//

#include "ImGuiViewPortWindow.hpp"
#include <glengine/Event/EventDispatcher.hpp>
#include <imgui.h>
namespace glengine
{
inline namespace impl
{
     static constinit float preview_aspect_ratio = 16.F / 9.F;
     void                   ImGuiViewPortWindow::on_im_gui_debug_info() const
     {
          if (!m_debug_text)
          {
               return;
          }
          auto           &io = ImGui::GetIO();
          const glm::vec3 mouse_world_pos
            = m_main_camera.camera().screen_space_to_world_space(
              view_port_mouse_pos());
          const glm::vec2 topright   = m_main_camera.top_right_screen_space();
          const glm::vec2 bottomleft = m_main_camera.bottom_left_screen_space();
          const glm::vec3 mouse_world_pos2
            = adjust_mouse_pos(topright, bottomleft);
          ImGui::Text(
            "%s",
            fmt::format(
              //"Window Frame Buffer - Width {}, Height: {}\n"
              "ViewPort - Hovered: {}, Focused: {}, Width: {}, Height: {}, "
              "Texture ID: {}\n"
              "Button - Hovered: {}, Focused: {}, Activated: {}\n"
              "Window - Hovered: {}, Focused: {}\n"
              "Parent Window - Hovered: {}, Focused: {}\n"
              "Mouse - X: {} Y: {}\n"
              "Content Region - Min X: {}, Min Y: {}, Max X: {}, Max Y: {}\n"
              "ClampMouse X: {}, Y: {}\n"
              "Mouse limited X: {}, Y: {}\n"
              "viewport_mouse_pos X: {}, Y: {}, Z:{}\n"
              "Mouse In WorldSpace - X: {}, Y: {}, Z: {}\n"
              "TR X {}, TR X {}, BL Y {}, BL Y {}\n"
              "Mouse In WorldSpace 2 - X: {}, Y: {}, Z: {}\n"
              "tile id: {}",
              // window->ViewWindowData().frame_buffer_width,
              // window->ViewWindowData().frame_buffer_height,
              +m_packed.hovered,
              +m_packed.focused,
              m_viewport_size.x,
              m_viewport_size.y,
              m_imgui_texture_id_ref,
              +m_packed.button_hovered,
              +m_packed.button_focused,
              +m_packed.button_activated,
              +m_packed.window_hovered,
              +m_packed.window_focused,
              +m_packed.parent_window_hovered,
              +m_packed.parent_window_focused,
              io.MousePos.x,
              io.MousePos.y,
              m_min.x,
              m_min.y,
              m_max.x,
              m_max.y,
              m_clamp_mouse_pos.x,
              m_clamp_mouse_pos.y,
              m_viewport_int_mouse_pos.x,
              m_viewport_int_mouse_pos.y,
              m_viewport_mouse_pos.x,
              m_viewport_mouse_pos.y,
              m_viewport_mouse_pos.z,
              mouse_world_pos.x,
              mouse_world_pos.y,
              mouse_world_pos.z,
              topright.x,
              topright.y,
              bottomleft.x,
              bottomleft.y,
              mouse_world_pos2.x,
              mouse_world_pos2.y,
              mouse_world_pos2.z,
              m_tile_id)
              .c_str());
     }
     void ImGuiViewPortWindow::on_render() const
     {
          m_main_camera.on_render();
          m_mouse_camera.on_render();
          // on_render([]() {});
     }
     void ImGuiViewPortWindow::set_image_bounds(const glm::vec2 &dims) const
     {
          m_main_camera.set_image_bounds(dims);
          m_mouse_camera.set_image_bounds(dims);
     }
     void ImGuiViewPortWindow::fit_both() const
     {
          m_main_camera.fit_both();
     }
     void ImGuiViewPortWindow::fit_height() const
     {
          m_main_camera.fit_height();
     }
     void ImGuiViewPortWindow::fit_width() const
     {
          m_main_camera.fit_width();
     }
     void ImGuiViewPortWindow::on_event(const event::Item &event) const
     {
          glengine::event::Dispatcher::Filter(
            event,
            has_focus(),
            has_hover(),
            [&event, this]() { m_main_camera.check_event(event); });
          m_main_camera.on_event(event);
          m_mouse_camera.on_event(event);
     }

     void ImGuiViewPortWindow::on_update(float ts) const
     {
          m_main_camera.refresh_aspect_ratio(view_port_aspect_ratio());
          if (m_packed.focused)
          {
               m_main_camera.check_input(ts);
          }
          m_main_camera.on_update(ts);
          m_mouse_camera.on_update(ts);
          if (m_packed.fit_width && m_packed.fit_height)
          {
               fit_both();
          }
          else if (m_packed.fit_height)
          {
               fit_height();
          }
          else if (m_packed.fit_width)
          {
               fit_width();
          }
     }
     void ImGuiViewPortWindow::on_im_gui_update() const
     {
          {
               const auto push_id = imgui_utils::ImGuiPushId();
               if (ImGui::Checkbox("Enable Debug Text", &m_debug_text))
               {
                    // changed
               }
          }
          {
               const auto pop_id = imgui_utils::ImGuiPushId();
               if (ImGui::ColorEdit3("Clear Color", &m_background_color.r))
               {
                    m_clear_impl.Color(m_background_color);
               }
          }
          ImGui::Separator();
          m_main_camera.on_im_gui_update();
          ImGui::Separator();
          m_mouse_camera.on_im_gui_update();
     }
     void ImGuiViewPortWindow::sync_open_gl_view_port() const
     {
          GlCall{}(
            glViewport,
            GLint{},
            GLint{},
            static_cast<GLint>(m_viewport_size.x),
            static_cast<GLint>(m_viewport_size.y));
     }
     bool ImGuiViewPortWindow::has_focus() const
     {
          return m_packed.focused;
     }
     bool ImGuiViewPortWindow::has_hover() const
     {
          return m_packed.hovered;
     }
     [[maybe_unused]] glm::vec2 ImGuiViewPortWindow::view_port_dims() const
     {
          return m_viewport_size;
     }
     float ImGuiViewPortWindow::view_port_aspect_ratio() const
     {
          float ret = m_viewport_size.x / m_viewport_size.y;
          if (std::isnan(ret))
          {
               return 16.F / 9.F;
          }
          return ret;
     }
     glm::vec4 ImGuiViewPortWindow::view_port_mouse_pos() const
     {
          return m_viewport_mouse_pos;
     }
     glm::vec4 ImGuiViewPortWindow::adjust_mouse_pos(
       glm::vec2 topright,
       glm::vec2 bottomleft) const
     {
          const auto convert_range = [](
                                       float       OldValue,
                                       const float OldMin,
                                       const float OldMax,
                                       const float NewMin = -1.F,
                                       const float NewMax = 1.F)
          {
               return (((OldValue - OldMin) * (NewMax - NewMin))
                       / (OldMax - OldMin))
                      + NewMin;
          };
          return glm::vec4{
               convert_range(m_clamp_mouse_pos.x, bottomleft.x, topright.x),
               convert_range(m_clamp_mouse_pos.y, bottomleft.y, topright.y),
               0.F, 1.F
          };
     }
     glm::vec2 ImGuiViewPortWindow::convert_im_vec_2(ImVec2 in) const
     {
          return { in.x, in.y };
     }
     void ImGuiViewPortWindow::on_update_mouse() const
     {
          m_min = convert_im_vec_2(ImGui::GetWindowContentRegionMin());
          m_max = convert_im_vec_2(ImGui::GetWindowContentRegionMax());

          m_min.x += ImGui::GetWindowPos().x;
          m_min.y += ImGui::GetWindowPos().y;
          m_max.x += ImGui::GetWindowPos().x;
          m_max.y += ImGui::GetWindowPos().y;

          auto &io          = ImGui::GetIO();
          m_clamp_mouse_pos = convert_im_vec_2(io.MousePos);
          m_clamp_mouse_pos.x
            = std::clamp(m_clamp_mouse_pos.x, m_min.x, m_max.x);
          m_clamp_mouse_pos.y
            = std::clamp(m_clamp_mouse_pos.y, m_min.y, m_max.y);
          const auto convert_range = [](
                                       float       OldValue,
                                       const float OldMin,
                                       const float OldMax,
                                       const float NewMin = -1.F,
                                       const float NewMax = 1.F)
          {
               return (((OldValue - OldMin) * (NewMax - NewMin))
                       / (OldMax - OldMin))
                      + NewMin;
          };
          m_viewport_mouse_pos
            = glm::vec4{ convert_range(m_clamp_mouse_pos.x, m_min.x, m_max.x),
                         convert_range(m_clamp_mouse_pos.y, m_min.y, m_max.y),
                         0.F, 1.F };
          m_viewport_int_mouse_pos = glm::vec4{
               convert_range(
                 m_clamp_mouse_pos.x, m_min.x, m_max.x, 0.F, m_viewport_size.x),
               m_viewport_size.y
                 - convert_range(
                   m_clamp_mouse_pos.y,
                   m_min.y,
                   m_max.y,
                   0.F,
                   m_viewport_size.y),
               0.F, 1.F
          };
     }
     void ImGuiViewPortWindow::on_update_focus_and_hover() const
     {

          m_packed.focused = m_packed.button_focused || m_packed.window_focused
                             || m_packed.parent_window_focused;
          m_packed.hovered = m_packed.button_hovered || m_packed.window_hovered
                             || m_packed.parent_window_hovered;
     }
     glm::mat4 ImGuiViewPortWindow::view_projection_matrix() const
     {
          return m_main_camera.camera().view_projection_matrix();
     }
     glm::vec2 ImGuiViewPortWindow::offset_mouse_pos() const
     {

          return static_cast<glm::vec2>(
                   m_main_camera.camera().screen_space_to_world_space(
                     view_port_mouse_pos()))
                 + m_main_camera.position();
     }
     glm::mat4 ImGuiViewPortWindow::preview_view_projection_matrix() const
     {
          m_mouse_camera.refresh_aspect_ratio(preview_aspect_ratio);
          m_mouse_camera.set_zoom(m_main_camera.zoom_level() / 8.F);
          m_mouse_camera.set_position(offset_mouse_pos());
          return m_mouse_camera.camera().view_projection_matrix();
     }
     void ImGuiViewPortWindow::set_preview_aspect_ratio(
       float aspect_ratio) noexcept
     {
          preview_aspect_ratio = aspect_ratio;
     }
     glm::vec4 ImGuiViewPortWindow::get_background_color() const noexcept
     {
          return m_background_color;
     }
     void ImGuiViewPortWindow::fit(
       const bool width,
       const bool height) const
     {
          m_packed.fit_width  = width;
          m_packed.fit_height = height;
     }
     void ImGuiViewPortWindow::enable_debug_text()
     {
          m_debug_text = true;
     }
     void ImGuiViewPortWindow::disable_debug_text()
     {
          m_debug_text = false;
     }
}// namespace impl
}// namespace glengine