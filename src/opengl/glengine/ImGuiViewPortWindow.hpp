//
// Created by pcvii on 4/26/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIVIEWPORTWINDOW_HPP
#define FIELD_MAP_EDITOR_IMGUIVIEWPORTWINDOW_HPP
#include "FrameBuffer.hpp"
#include "FrameBufferBackup.hpp"
#include "ImGuiPushID.hpp"
#include "scope_guard.hpp"
namespace glengine
{
inline namespace impl
{
  class ImGuiViewPortWindow
  {
  public:
    ImGuiViewPortWindow() = default;
    ImGuiViewPortWindow(std::string title)
      : m_title(std::move(title))
    {
    }
    void OnEvent(const Event::Item &) const {}
    void OnUpdate(float) const {}

    void OnRender(const std::invocable auto &&callable) const
    {
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.F, 0.F));
      {

        const auto pop_id0   = ImGuiPushID();
        const auto pop_style = scope_guard([]() { ImGui::PopStyleVar(); });
        const auto pop_end   = scope_guard([]() { ImGui::End(); });
        if (!ImGui::Begin(m_title.c_str()))
        {
          return;
        }
        // do any rendering here.
        {
          m_parent_window_hovered = ImGui::IsWindowHovered();
          m_parent_window_focused = ImGui::IsWindowFocused();
          // Using a Child allow to fill all the space of the window.
          // It also allows customization
          const auto pop_id1      = ImGuiPushID();
          ImGui::BeginChild(m_title.c_str());
          const auto pop_child = scope_guard([]() { ImGui::EndChild(); });
          m_window_hovered     = ImGui::IsWindowHovered();
          m_window_focused     = ImGui::IsWindowFocused();
          // Get the size of the child (i.e. the whole draw size of the
          // windows).
          m_viewport_size =
            ImGui::GetContentRegionAvail();// ImGui::GetWindowSize();
          if (
            !m_fb
            || (m_fb.Specification().height
                 != static_cast<int>(m_viewport_size.y) && m_viewport_size.y > 5.0F)
            || m_fb.Specification().width
                 != static_cast<int>(m_viewport_size.x))
          {
            m_fb = glengine::FrameBuffer(glengine::FrameBufferSpecification{
              .width  = static_cast<int>(m_viewport_size.x),
              .height = static_cast<int>(m_viewport_size.y) });
          }
          else if (
            m_fb.Specification().height != static_cast<int>(m_viewport_size.y))
          {
            m_viewport_size.y = static_cast<float>(m_fb.Specification().height);
            // Sometimes imgui would detect height as 4 px. I donno why. Seemed
            // to be related to drawing more than once. I removed that extra
            // draw but left this check here.
          }
          {
            const auto ffb = FrameBufferBackup();
            m_fb.Bind();
            glengine::Renderer::Clear();
            callable();
            m_fb.UnBind();
          }
          // Because I use the texture from OpenGL, I need to invert the V from
          // the UV.
          const auto convert = [](uint32_t r_id) -> ImTextureID {
            return reinterpret_cast<ImTextureID>(static_cast<intptr_t>(r_id));
          };
          m_imgui_texture_id_ref = convert(m_fb.GetColorAttachment().ID());
          const auto cPos        = ImGui::GetCursorPos();
          ImGui::SetItemAllowOverlap();
          const auto color = ImVec4(0.F, 0.F, 0.F, 0.F);
          ImGui::PushStyleColor(ImGuiCol_Button, color);
          ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
          m_button_clicked = ImGui::ImageButton(
            m_imgui_texture_id_ref,
            ImVec2(
              static_cast<float>(m_fb.Specification().width),
              static_cast<float>(m_fb.Specification().height)),
            ImVec2(0, 1),
            ImVec2(1, 0),
            0);

          ImGui::PopStyleColor(3);
          ImGui::SetCursorPos(cPos);
          OnUpdateMouse();
          m_button_hovered   = ImGui::IsItemHovered();
          m_button_focused   = ImGui::IsItemFocused();
          m_button_activated = ImGui::IsItemActivated();
          OnUpdateFocusAndHover();
          OnImGuiDebugInfo();
        }
      }
    }
    void OnImGuiUpdate() const {}

    void SyncOpenGLViewPort() const
    {
      GLCall{}(
        glViewport,
        GLint{},
        GLint{},
        static_cast<GLint>(m_viewport_size.x),
        static_cast<GLint>(m_viewport_size.y));
    }

    glm::vec2 ViewPortDims() const
    {
      return { m_viewport_size.x, m_viewport_size.y };
    }
    float ViewPortAspectRatio() const
    {
      float ret = m_viewport_size.x / m_viewport_size.y;
      if (std::isnan(ret))
      {
        return 16.F / 9.F;
      }
      return ret;
    }
    glm::vec4 ViewPortMousePos() const
    {
      return m_viewport_mouse_pos;
    }

  private:
    void OnUpdateMouse() const
    {

      m_min = ImGui::GetWindowContentRegionMin();
      m_max = ImGui::GetWindowContentRegionMax();

      m_min.x += ImGui::GetWindowPos().x;
      m_min.y += ImGui::GetWindowPos().y;
      m_max.x += ImGui::GetWindowPos().x;
      m_max.y += ImGui::GetWindowPos().y;

      auto &io            = ImGui::GetIO();
      m_clamp_mouse_pos   = io.MousePos;
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
    void OnUpdateFocusAndHover() const
    {
      m_focused =
        m_button_focused || m_window_focused || m_parent_window_focused;
      m_hovered =
        m_button_hovered || m_window_hovered || m_parent_window_hovered;
    }
    void OnImGuiDebugInfo() const
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
          m_hovered,
          m_focused,
          m_viewport_size.x,
          m_viewport_size.y,
          m_imgui_texture_id_ref,
          m_button_hovered,
          m_button_focused,
          m_button_activated,
          m_window_hovered,
          m_window_focused,
          m_parent_window_hovered,
          m_parent_window_focused,
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
    std::string                   m_title                 = {};
    mutable glengine::FrameBuffer m_fb                    = {};
    mutable bool                  m_focused               = { false };
    mutable bool                  m_hovered               = { false };
    mutable bool                  m_button_clicked        = { false };
    mutable bool                  m_button_focused        = { false };
    mutable bool                  m_window_focused        = { false };
    mutable bool                  m_parent_window_focused = { false };
    mutable bool                  m_button_hovered        = { false };
    mutable bool                  m_window_hovered        = { false };
    mutable bool                  m_parent_window_hovered = { false };
    mutable bool                  m_button_activated      = { false };
    mutable ImVec2                m_min                   = {};
    mutable ImVec2                m_max                   = {};
    mutable ImVec2                m_viewport_size         = {};
    mutable ImVec2                m_clamp_mouse_pos       = {};
    mutable ImTextureID           m_imgui_texture_id_ref  = {};
    mutable glm::vec4             m_viewport_mouse_pos    = {};
  };
}// namespace impl
}// namespace glengine
#endif// FIELD_MAP_EDITOR_IMGUIVIEWPORTWINDOW_HPP
