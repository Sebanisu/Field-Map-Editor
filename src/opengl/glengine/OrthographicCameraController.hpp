//
// Created by pcvii on 12/2/2021.
//

#ifndef FIELD_MAP_EDITOR_ORTHOGRAPHICCAMERACONTROLLER_HPP
#define FIELD_MAP_EDITOR_ORTHOGRAPHICCAMERACONTROLLER_HPP
#include "Event/EventItem.hpp"
#include "OrthographicCamera.hpp"
namespace glengine
{
class OrthographicCameraController
{
public:
  OrthographicCameraController() = default;
  OrthographicCameraController(float aspect_ratio)
    : OrthographicCameraController(aspect_ratio, 1.F)
  {
  }
  OrthographicCameraController(float aspect_ratio, float zoom)
    : m_viewport_aspect_ratio(aspect_ratio)
    , m_zoom_level(zoom)
    , m_camera{ -m_viewport_aspect_ratio * m_zoom_level,
                m_viewport_aspect_ratio * m_zoom_level,
                -m_zoom_level,
                m_zoom_level }
  {
  }

  // should be after CheckInput and only have things that should trigger always.
  void                                OnUpdate(float) const;
  // check for keyboard or mouse inputs must recommend to check for focus first.
  void                                CheckInput(float ts) const;
  bool                                OnImGuiUpdate() const;
  constexpr void                      OnRender() const {}
  void                                OnEvent(const Event::Item &) const;
  // check for keyboard or mouse events. Use Dispatcher::Filter.
  void                                CheckEvent(const Event::Item &) const;
  const glengine::OrthographicCamera &Camera() const
  {
    return m_camera;
  }

  // todo add bounds for left,right,top,bottom.
  //  these will keep the not allow moving beyond those points.
  //


  struct return_values
  {
    float left{};
    float right{};
    float bottom{};
    float top{};

          operator glengine::OrthographicCamera() const
    {
      return glengine::OrthographicCamera(left, right, bottom, top);
    }
    return_values operator*(float multiplier) const noexcept
    {
      return { left * multiplier,
               right * multiplier,
               bottom * multiplier,
               top * multiplier };
    }
  };
  return_values                CurrentBounds() const;
  std::optional<return_values> MaxBounds() const;
  void                         SetImageBounds(return_values bounds) const;
  void                         SetImageBounds(glm::vec2 dims) const;
  void                         DisableBounds() const;
  // void                         RefreshAspectRatio() const;
  void                         RefreshAspectRatio(float new_aspect_ratio) const;
  void                         FitBoth() const;
  void                         FitHeight() const;
  void                         FitWidth() const;
  [[maybe_unused]] void        SetZoom(float new_zoom) const;

  void                         SetPosition(glm::vec2) const;

private:
  void                                 SetProjection() const;
  mutable float                        m_viewport_aspect_ratio = {};
  mutable float                        m_image_aspect_ratio    = {};
  mutable float                        m_zoom_level            = { 1.F };
  mutable float                        m_zoom_precision        = { 1.F };
  mutable glengine::OrthographicCamera m_camera                = {};
  mutable glm::vec3                    m_position              = {};
  mutable float                        m_rotation              = {};
  float                                m_translation_speed     = { -2.5F };
  float                                m_rotation_speed        = { 180.F };
  mutable std::optional<return_values> m_bounds                = {};
  void                                 zoom(const float offset) const;
};
void MakeViewPortMatchBounds(
  const OrthographicCameraController::return_values &bounds);
static_assert(Renderable<OrthographicCameraController>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_ORTHOGRAPHICCAMERACONTROLLER_HPP
