//
// Created by pcvii on 4/26/2022.
//

#include "ImGuiViewPortWindow.hpp"
namespace glengine
{
inline namespace impl
{
  void ImGuiViewPortWindow::OnImGuiDebugInfo() const
  {
    auto &io = ImGui::GetIO();
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
        "{}\nviewport_mouse_pos X: {}, Y: {}, Z:{}, W:{}",
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
        m_viewport_mouse_pos.w)
        .c_str());
  }
  void ImGuiViewPortWindow::OnRender() const
  {
    OnRender([]() {});
  }
  void ImGuiViewPortWindow::OnEvent(const Event::Item &) const {}
  void ImGuiViewPortWindow::OnUpdate(float) const {}
  void ImGuiViewPortWindow::OnImGuiUpdate() const {}
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
}// namespace impl
}// namespace glengine