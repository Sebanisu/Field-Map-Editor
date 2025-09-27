//
// Created by pcvii on 12/2/2021.
//

#ifndef FIELD_MAP_EDITOR_ORTHOGRAPHICCAMERACONTROLLER_HPP
#define FIELD_MAP_EDITOR_ORTHOGRAPHICCAMERACONTROLLER_HPP
#include "Event/EventItem.hpp"
#include "OrthographicCamera.hpp"
#include <Renderable.hpp>
#include <optional>
namespace glengine
{
class OrthographicCameraController
{
   public:
     constexpr OrthographicCameraController() = default;
     OrthographicCameraController(float aspect_ratio)
       : OrthographicCameraController(
           aspect_ratio,
           1.F)
     {
     }
     OrthographicCameraController(
       float aspect_ratio,
       float zoom)
       : m_viewport_aspect_ratio(aspect_ratio)
       , m_zoom_level(zoom)
       , m_camera{ -m_viewport_aspect_ratio * m_zoom_level,
                   m_viewport_aspect_ratio * m_zoom_level,
                   -m_zoom_level,
                   m_zoom_level }
     {
     }

     // should be after CheckInput and only have things that should trigger
     // always.
     void           on_update(float) const;
     // check for keyboard or mouse inputs must recommend to check for focus
     // first.
     void           check_input(float ts) const;
     bool           on_im_gui_update() const;
     constexpr void on_render() const {}
     void           on_event(const event::Item &) const;
     // check for keyboard or mouse events. Use Dispatcher::Filter.
     void           check_event(const event::Item &e) const;
     const glengine::OrthographicCamera &camera() const
     {
          return m_camera;
     }

     // todo add bounds for left,right,top,bottom.
     //  these will keep the not allow moving beyond those points.
     //


     struct ReturnValues
     {
          float left{};
          float right{};
          float bottom{};
          float top{};

                operator glengine::OrthographicCamera() const
          {
               return glengine::OrthographicCamera(left, right, bottom, top);
          }
          ReturnValues operator*(float multiplier) const noexcept
          {
               return { left * multiplier, right * multiplier,
                        bottom * multiplier, top * multiplier };
          }
     };
     ReturnValues                current_bounds() const;
     std::optional<ReturnValues> max_bounds() const;
     void                        set_image_bounds(ReturnValues bounds) const;
     void                        set_image_bounds(glm::vec2 dims) const;
     void                        disable_bounds() const;
     // void                         RefreshAspectRatio() const;
     void                refresh_aspect_ratio(float new_aspect_ratio) const;
     void                fit_both() const;
     void                fit_height() const;
     void                fit_width() const;
     [[nodiscard]] float zoom_level() const
     {
          return m_zoom_level;
     }
     [[maybe_unused]] void   set_zoom(float new_zoom) const;

     void                    set_position(glm::vec2) const;
     [[nodiscard]] glm::vec2 position() const
     {
          return m_position;
     }
     [[nodiscard]] glm::vec2 top_right_screen_space() const
     {
          auto clip_space_pos
            = m_camera.projection_matrix()
              * (m_camera.view_matrix() * glm::vec4{ m_bounds->right, m_bounds->top, 0.F, 1.F });
          glm::vec2 ndc_space_pos{ clip_space_pos.x, clip_space_pos.y };
          ndc_space_pos /= clip_space_pos.w;
          return ndc_space_pos;
          // vec2 windowSpacePos = ((ndcSpacePos.xy + 1.0) / 2.0) * viewSize +
          // viewOffset;
     }
     glm::vec2 bottom_left_screen_space() const
     {
          auto clip_space_pos
            = m_camera.projection_matrix()
              * (m_camera.view_matrix() * glm::vec4{ m_bounds->left, m_bounds->bottom, 0.F, 1.F });
          glm::vec2 ndc_space_pos{ clip_space_pos.x, clip_space_pos.y };
          ndc_space_pos /= clip_space_pos.w;
          return ndc_space_pos;
          // vec2 windowSpacePos = ((ndcSpacePos.xy + 1.0) / 2.0) * viewSize +
          // viewOffset;
     }


   private:
     void                                 set_projection() const;
     mutable float                        m_viewport_aspect_ratio = {};
     mutable float                        m_image_aspect_ratio    = {};
     mutable float                        m_zoom_level            = { 1.F };
     mutable float                        m_zoom_precision        = { 1.F };
     mutable glengine::OrthographicCamera m_camera                = {};
     mutable glm::vec3                    m_position              = {};
     mutable float                        m_rotation              = {};
     float                                m_translation_speed     = { -2.5F };
     float                                m_rotation_speed        = { 180.F };
     mutable std::optional<ReturnValues>  m_bounds                = {};
     void                                 zoom(const float offset = 0.F) const;
};
void MakeViewPortMatchBounds(
  const OrthographicCameraController::ReturnValues &bounds);
static_assert(Renderable<OrthographicCameraController>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_ORTHOGRAPHICCAMERACONTROLLER_HPP
