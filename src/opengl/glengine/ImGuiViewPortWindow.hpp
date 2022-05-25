//
// Created by pcvii on 4/26/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIVIEWPORTWINDOW_HPP
#define FIELD_MAP_EDITOR_IMGUIVIEWPORTWINDOW_HPP
#include "FrameBuffer.hpp"
#include "FrameBufferBackup.hpp"
#include "ImGuiPushID.hpp"
#include "ImGuiPushStyleVar.hpp"
#include "OrthographicCameraController.hpp"
#include "scope_guard.hpp"
namespace glengine
{
inline namespace impl
{
  class ImGuiViewPortWindow
  {
  public:
    constexpr ImGuiViewPortWindow() = default;
    constexpr ImGuiViewPortWindow(const char *title)
      : m_title(std::move(title))
    {
    }
    void OnEvent(const Event::Item &) const;
    void OnUpdate(float) const;
    void OnRender() const;
    void OnRender(const std::invocable auto &&callable) const
    {
      const auto pop_style =
        ImGuiPushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.F, 0.F));
      {
        const auto pop_id0 = ImGuiPushID();
        const auto pop_end = scope_guard([]() { ImGui::End(); });
        if (!ImGui::Begin(m_title))
        {
          return;
        }
        // do any rendering here.
        {
          m_packed.parent_window_hovered = ImGui::IsWindowHovered();
          m_packed.parent_window_focused = ImGui::IsWindowFocused();
          // Using a Child allow to fill all the space of the window.
          // It also allows customization
          const auto pop_id1             = ImGuiPushID();
          ImGui::BeginChild(m_title);
          const auto pop_child    = scope_guard([]() { ImGui::EndChild(); });
          m_packed.window_hovered = ImGui::IsWindowHovered();
          m_packed.window_focused = ImGui::IsWindowFocused();
          // Get the size of the child (i.e. the whole draw size of the
          // windows).
          m_viewport_size         = ConvertImVec2(
            ImGui::GetContentRegionAvail());// ImGui::GetWindowSize();
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
          m_packed.button_clicked = ImGui::ImageButton(
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
          m_packed.button_hovered   = ImGui::IsItemHovered();
          m_packed.button_focused   = ImGui::IsItemFocused();
          m_packed.button_activated = ImGui::IsItemActivated();
          OnUpdateFocusAndHover();
          OnImGuiDebugInfo();
        }
      }
    }
    glm::mat4                  ViewProjectionMatrix() const;
    glm::mat4                  PreviewViewProjectionMatrix(float) const;
    void                       SetImageBounds(const glm::vec2 &) const;
    void                       FitBoth() const;
    void                       FitHeight() const;
    void                       FitWidth() const;
    void                       OnImGuiUpdate() const;
    void                       SyncOpenGLViewPort() const;
    bool                       HasFocus() const;
    bool                       HasHover() const;
    [[maybe_unused]] glm::vec2 ViewPortDims() const;
    float                      ViewPortAspectRatio() const;
    glm::vec4                  ViewPortMousePos() const;
    glm::vec4 adjust_mouse_pos(glm::vec2 topright, glm::vec2 bottomleft) const;
    void      DisableDebugText()
    {
      m_debug_text = false;
    }
    void EnableDebugText()
    {
      m_debug_text = true;
    }
    void Fit(const bool width, const bool height) const
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

  private:
    glm::vec2 ConvertImVec2(ImVec2 in) const;
    void      OnUpdateMouse() const;
    void      OnUpdateFocusAndHover() const;
    void      OnImGuiDebugInfo() const;

    struct PackedSettings
    {
      bool focused : 1               = { false };
      bool hovered : 1               = { false };
      bool button_clicked : 1        = { false };
      bool button_focused : 1        = { false };
      bool window_focused : 1        = { false };
      bool parent_window_focused : 1 = { false };
      bool button_hovered : 1        = { false };
      bool window_hovered : 1        = { false };
      bool parent_window_hovered : 1 = { false };
      bool button_activated : 1      = { false };
    };
    const char                                    *m_title           = {};
    mutable glengine::FrameBuffer                  m_fb              = {};
    mutable PackedSettings                         m_packed          = {};
    mutable bool                                   m_debug_text      = { true };
    mutable glm::vec2                              m_min             = {};
    mutable glm::vec2                              m_max             = {};
    mutable glm::vec2                              m_viewport_size   = {};
    mutable glm::vec2                              m_clamp_mouse_pos = {};
    mutable ImTextureID                            m_imgui_texture_id_ref = {};
    mutable glm::vec4                              m_viewport_mouse_pos   = {};
    mutable glengine::OrthographicCameraController m_main_camera          = {};
    mutable glengine::OrthographicCameraController m_mouse_camera         = {};
  };
  static_assert(Renderable<ImGuiViewPortWindow>);
}// namespace impl
}// namespace glengine
#endif// FIELD_MAP_EDITOR_IMGUIVIEWPORTWINDOW_HPP
