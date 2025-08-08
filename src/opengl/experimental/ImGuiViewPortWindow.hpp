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
#include "ScopeGuard.hpp"
#include <imgui.h>
namespace glengine
{
inline namespace impl
{
     class ImGuiViewPortPreview;
     class ImGuiViewPortWindow
     {
        public:
          constexpr ImGuiViewPortWindow() = default;
          constexpr ImGuiViewPortWindow(const char *title)
            : m_title(std::move(title))
          {
          }

          void on_render(const std::invocable auto &&callable) const
          {
               const auto pop_style = ImGuiPushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.F, 0.F));
               {
                    const auto pop_id_0 = ImGuiPushId();
                    const auto pop_end  = ScopeGuard([]() { ImGui::End(); });
                    if (!ImGui::Begin(m_title))
                    {
                         return;
                    }
                    sync_open_gl_view_port();
                    // do any rendering here.
                    {
                         m_packed.parent_window_hovered = ImGui::IsWindowHovered();
                         m_packed.parent_window_focused = ImGui::IsWindowFocused();
                         // Using a Child allow to fill all the space of the window.
                         // It also allows customization
                         const auto pop_id_1            = ImGuiPushId();
                         ImGui::BeginChild(m_title);
                         const auto pop_child    = ScopeGuard([]() { ImGui::EndChild(); });
                         m_packed.window_hovered = ImGui::IsWindowHovered();
                         m_packed.window_focused = ImGui::IsWindowFocused();
                         // Get the size of the child (i.e. the whole draw size of the
                         // windows).
                         m_viewport_size         = convert_im_vec_2(ImGui::GetContentRegionAvail());// ImGui::GetWindowSize();
                         if (
                           !m_fb || (m_fb.specification().height != static_cast<int>(m_viewport_size.y) && m_viewport_size.y > 5.0F)
                           || m_fb.specification().width != static_cast<int>(m_viewport_size.x))
                         {
                              m_fb = glengine::FrameBuffer(
                                m_fb.specification().resize(static_cast<int>(m_viewport_size.x), static_cast<int>(m_viewport_size.y)));
                         }
                         else if (m_fb.specification().height != static_cast<int>(m_viewport_size.y))
                         {
                              m_viewport_size.y = static_cast<float>(m_fb.specification().height);
                              // Sometimes imgui would detect height as 4 px. I donno why. Seemed
                              // to be related to drawing more than once. I removed that extra
                              // draw but left this check here.
                         }
                         {
                              const auto ffb = FrameBufferBackup();
                              m_fb.bind();
                              m_clear_impl();
                              m_fb.clear_non_standard_color_attachments();
                              callable();
                              if (m_debug_text)
                              {
                                   m_tile_id = m_fb.read_pixel(
                                     1, static_cast<int>(m_viewport_int_mouse_pos.x), static_cast<int>(m_viewport_int_mouse_pos.y));
                              }
                              m_fb.unbind();
                         }
                         // Because I use the texture from OpenGL, I need to invert the V from
                         // the UV.
                         m_imgui_texture_id_ref = ConvertGliDtoImTextureId<std::uint64_t>(m_fb.color_attachment_id(0));
                         const auto c_pos       = ImGui::GetCursorPos();
                         ImGui::SetItemAllowOverlap();
                         const auto color = ImVec4(0.F, 0.F, 0.F, 0.F);
                         ImGui::PushStyleColor(ImGuiCol_Button, color);
                         ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
                         ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
                         m_packed.button_clicked = ImGui::ImageButton(
                           "##view_port_button",
                           m_imgui_texture_id_ref,
                           ImVec2(static_cast<float>(m_fb.specification().width), static_cast<float>(m_fb.specification().height)),
                           ImVec2(0, 1),
                           ImVec2(1, 0));

                         ImGui::PopStyleColor(3);
                         ImGui::SetCursorPos(c_pos);
                         on_update_mouse();
                         m_packed.button_hovered   = ImGui::IsItemHovered();
                         m_packed.button_focused   = ImGui::IsItemFocused();
                         m_packed.button_activated = ImGui::IsItemActivated();
                         on_update_focus_and_hover();
                         on_im_gui_debug_info();
                    }
               }
          }
          void                  on_event(const event::Item &) const;
          void                  on_update(float) const;
          void                  on_render() const;
          glm::mat4             view_projection_matrix() const;
          glm::mat4             preview_view_projection_matrix() const;
          void                  set_image_bounds(const glm::vec2 &) const;
          void                  on_im_gui_update() const;
          bool                  has_focus() const;
          bool                  has_hover() const;
          float                 view_port_aspect_ratio() const;
          void                  disable_debug_text();
          [[maybe_unused]] void enable_debug_text();
          void                  fit(const bool width, const bool height) const;
          glm::vec2             offset_mouse_pos() const;

          friend ImGuiViewPortPreview;

        private:
          static void                set_preview_aspect_ratio(float) noexcept;
          glm::vec4                  adjust_mouse_pos(glm::vec2 topright, glm::vec2 bottomleft) const;
          [[maybe_unused]] glm::vec2 view_port_dims() const;
          glm::vec4                  view_port_mouse_pos() const;
          void                       fit_both() const;
          void                       fit_height() const;
          void                       fit_width() const;
          void                       sync_open_gl_view_port() const;
          glm::vec2                  convert_im_vec_2(ImVec2 in) const;
          void                       on_update_mouse() const;
          void                       on_update_focus_and_hover() const;
          void                       on_im_gui_debug_info() const;
          glm::vec4                  get_background_color() const noexcept;

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
               bool fit_width : 1             = { true };
               bool fit_height : 1            = { true };
          };
          const char                                    *m_title                  = {};
          mutable glengine::FrameBuffer                  m_fb                     = {};
          mutable PackedSettings                         m_packed                 = {};
          mutable bool                                   m_debug_text             = { false };
          mutable glm::vec2                              m_min                    = {};
          mutable glm::vec2                              m_max                    = {};
          mutable glm::vec2                              m_viewport_size          = {};
          mutable glm::vec2                              m_clamp_mouse_pos        = {};
          mutable ImTextureID                            m_imgui_texture_id_ref   = {};
          mutable glm::vec4                              m_viewport_mouse_pos     = {};
          mutable glengine::OrthographicCameraController m_main_camera            = {};
          mutable glengine::OrthographicCameraController m_mouse_camera           = {};
          mutable glm::vec4                              m_background_color       = { 0.F, 0.F, 0.F, 255.F };
          mutable glengine::Clear_impl                   m_clear_impl             = { m_background_color };
          mutable glm::vec4                              m_viewport_int_mouse_pos = {};
          mutable int                                    m_tile_id                = {};
     };
     static_assert(Renderable<ImGuiViewPortWindow>);
}// namespace impl
}// namespace glengine
#endif// FIELD_MAP_EDITOR_IMGUIVIEWPORTWINDOW_HPP
