//
// Created by pcvii on 4/26/2022.
//

#include "ImGuiViewPortWindow.hpp"
#include "Event/EventDispatcher.hpp"
namespace glengine
{
inline namespace impl
{
  static constinit float preview_aspect_ratio = 16.F / 9.F;
  void                   ImGuiViewPortWindow::OnImGuiDebugInfo() const
  {
    if (!m_debug_text)
    {
      return;
    }
    auto           &io = ImGui::GetIO();
    const glm::vec3 mouse_world_pos =
      m_main_camera.Camera().ScreenSpaceToWorldSpace(ViewPortMousePos());
    const glm::vec2 topright         = m_main_camera.TopRightScreenSpace();
    const glm::vec2 bottomleft       = m_main_camera.BottomLeftScreenSpace();
    const glm::vec3 mouse_world_pos2 = adjust_mouse_pos(topright, bottomleft);
    ImGui::Text(
      "%s",
      fmt::format(
        //"Window Frame Buffer - Width {}, Height: {}\n"
        "ViewPort - Hovered: {}, Focused: {}, Width: {}, Height: {}, "
        "Texture ID: {}\nButton - Hovered: {}, Focused: {}, Activated: "
        "{}\nWindow - Hovered: {}, Focused: {}\nParent Window - Hovered: "
        "{}, Focused: {}\nMouse - X: {} Y: {}\nContent Region - Min X: "
        "{}, "
        "Min Y: {}, Max X: {}, Max Y: {}\nClampMouse X: {}, Y: "
        "{}\nviewport_mouse_pos X: {}, Y: {}, Z:{}\n"
        "Mouse In WorldSpace - X: {}, Y: {}, Z: {}\nTR X {}, TR X {}, BL Y {}, "
        "BL Y {}\nMouse In WorldSpace 2 - X: {}, Y: {}, Z: {}\n",
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
        mouse_world_pos2.z)
        .c_str());
  }
  void ImGuiViewPortWindow::OnRender() const
  {
    m_main_camera.OnRender();
    m_mouse_camera.OnRender();
    // OnRender([]() {});
  }
  void ImGuiViewPortWindow::SetImageBounds(const glm::vec2 &dims) const
  {
    m_main_camera.SetImageBounds(dims);
    m_mouse_camera.SetImageBounds(dims);
  }
  void ImGuiViewPortWindow::FitBoth() const
  {
    m_main_camera.FitBoth();
  }
  void ImGuiViewPortWindow::FitHeight() const
  {
    m_main_camera.FitHeight();
  }
  void ImGuiViewPortWindow::FitWidth() const
  {
    m_main_camera.FitWidth();
  }
  void ImGuiViewPortWindow::OnEvent(const Event::Item &event) const
  {
    glengine::Event::Dispatcher::Filter(
      event, HasFocus(), HasHover(), [&event, this]() {
        m_main_camera.CheckEvent(event);
      });
    m_main_camera.OnEvent(event);
    m_mouse_camera.OnEvent(event);
  }

  void ImGuiViewPortWindow::OnUpdate(float ts) const
  {
    m_main_camera.RefreshAspectRatio(ViewPortAspectRatio());
    if (m_packed.focused)
    {
      m_main_camera.CheckInput(ts);
    }
    m_main_camera.OnUpdate(ts);
    m_mouse_camera.OnUpdate(ts);
  }
  void ImGuiViewPortWindow::OnImGuiUpdate() const
  {
    const auto pushid = ImGuiPushID();
    if (ImGui::Checkbox("Enable Debug Text", &m_debug_text))
    {
      // changed
    }
    ImGui::Separator();
    m_main_camera.OnImGuiUpdate();
    ImGui::Separator();
    m_mouse_camera.OnImGuiUpdate();
  }
  void ImGuiViewPortWindow::SyncOpenGLViewPort() const
  {
    GLCall{}(
      glViewport,
      GLint{},
      GLint{},
      static_cast<GLint>(m_viewport_size.x),
      static_cast<GLint>(m_viewport_size.y));
  }
  bool ImGuiViewPortWindow::HasFocus() const
  {
    return m_packed.focused;
  }
  bool ImGuiViewPortWindow::HasHover() const
  {
    return m_packed.hovered;
  }
  [[maybe_unused]] glm::vec2 ImGuiViewPortWindow::ViewPortDims() const
  {
    return m_viewport_size;
  }
  float ImGuiViewPortWindow::ViewPortAspectRatio() const
  {
    float ret = m_viewport_size.x / m_viewport_size.y;
    if (std::isnan(ret))
    {
      return 16.F / 9.F;
    }
    return ret;
  }
  glm::vec4 ImGuiViewPortWindow::ViewPortMousePos() const
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
                                 const float NewMax = 1.F) {
      return (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin))
             + NewMin;
    };
    return glm::vec4{
      convert_range(m_clamp_mouse_pos.x, bottomleft.x, topright.x),
      convert_range(m_clamp_mouse_pos.y, bottomleft.y, topright.y),
      0.F,
      1.F
    };
  }
  glm::vec2 ImGuiViewPortWindow::ConvertImVec2(ImVec2 in) const
  {
    return { in.x, in.y };
  }
  void ImGuiViewPortWindow::OnUpdateMouse() const
  {
    m_min = ConvertImVec2(ImGui::GetWindowContentRegionMin());
    m_max = ConvertImVec2(ImGui::GetWindowContentRegionMax());

    m_min.x += ImGui::GetWindowPos().x;
    m_min.y += ImGui::GetWindowPos().y;
    m_max.x += ImGui::GetWindowPos().x;
    m_max.y += ImGui::GetWindowPos().y;

    auto &io            = ImGui::GetIO();
    m_clamp_mouse_pos   = ConvertImVec2(io.MousePos);
    m_clamp_mouse_pos.x = std::clamp(m_clamp_mouse_pos.x, m_min.x, m_max.x);
    m_clamp_mouse_pos.y = std::clamp(m_clamp_mouse_pos.y, m_min.y, m_max.y);
    const auto convert_range = [](
                                 float       OldValue,
                                 const float OldMin,
                                 const float OldMax,
                                 const float NewMin = -1.F,
                                 const float NewMax = 1.F) {
      return (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin))
             + NewMin;
    };
    m_viewport_mouse_pos =
      glm::vec4{ convert_range(m_clamp_mouse_pos.x, m_min.x, m_max.x),
                 convert_range(m_clamp_mouse_pos.y, m_min.y, m_max.y),
                 0.F,
                 1.F };
  }
  void ImGuiViewPortWindow::OnUpdateFocusAndHover() const
  {

    m_packed.focused = m_packed.button_focused || m_packed.window_focused
                       || m_packed.parent_window_focused;
    m_packed.hovered = m_packed.button_hovered || m_packed.window_hovered
                       || m_packed.parent_window_hovered;
  }
  glm::mat4 ImGuiViewPortWindow::ViewProjectionMatrix() const
  {
    return m_main_camera.Camera().ViewProjectionMatrix();
  }
  glm::mat4 ImGuiViewPortWindow::PreviewViewProjectionMatrix() const
  {
    m_mouse_camera.RefreshAspectRatio(preview_aspect_ratio);
    const glm::vec2 position =
      m_main_camera.Camera().ScreenSpaceToWorldSpace(ViewPortMousePos());

    m_mouse_camera.SetZoom(m_main_camera.ZoomLevel() / 8.F);
    m_mouse_camera.SetPosition(position + m_main_camera.Position());
    return m_mouse_camera.Camera().ViewProjectionMatrix();
  }
  void ImGuiViewPortWindow::SetPreviewAspectRatio(float aspect_ratio) noexcept
  {
    preview_aspect_ratio = aspect_ratio;
  }
  void ImGuiViewPortWindow::Fit(const bool width, const bool height) const
  {
    if (height && width)
    {
      FitBoth();
    }
    else if (height)
    {
      FitHeight();
    }
    else if (width)
    {
      FitWidth();
    }
  }
  void ImGuiViewPortWindow::EnableDebugText()
  {
    m_debug_text = true;
  }
  void ImGuiViewPortWindow::DisableDebugText()
  {
    m_debug_text = false;
  }
}// namespace impl
}// namespace glengine