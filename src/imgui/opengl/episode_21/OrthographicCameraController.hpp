//
// Created by pcvii on 12/2/2021.
//

#ifndef MYPROJECT_ORTHOGRAPHICCAMERACONTROLLER_HPP
#define MYPROJECT_ORTHOGRAPHICCAMERACONTROLLER_HPP
#include "Application.hpp"
#include "EventItem.hpp"
#include "OrthographicCamera.hpp"
#include "tests/Test.hpp"
class OrthographicCameraController
{
public:
  OrthographicCameraController()
    : OrthographicCameraController(get_frame_buffer_aspect_ratio(), 1.F)
  {
  }
  OrthographicCameraController(float zoom)
    : OrthographicCameraController(get_frame_buffer_aspect_ratio(), zoom)
  {
  }
  OrthographicCameraController(float aspect_ratio, float zoom)
    : m_aspect_ratio(aspect_ratio)
    , m_zoom_level(zoom)
    , m_camera{ -m_aspect_ratio * m_zoom_level,
                m_aspect_ratio * m_zoom_level,
                -m_zoom_level,
                m_zoom_level }
  {
  }

  void                      OnUpdate(float ts) const;
  bool                      OnImGuiUpdate() const;
  void                      OnRender() const {}
  void                      OnEvent(const Event::Item &) const;
  const OrthographicCamera &Camera() const
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
  };
  return_values Bounds() const
  {
    return return_values{ .left = -m_zoom_level * m_aspect_ratio + m_position.x,
                          .right = m_zoom_level * m_aspect_ratio + m_position.x,
                          .bottom = -m_zoom_level + m_position.y,
                          .top    = m_zoom_level + m_position.y };
  }
  void SetMaxBounds(return_values bounds) const
  {
    if (
      bounds.left + bounds.right > std::numeric_limits<float>::epsilon()
      && bounds.bottom + bounds.top > std::numeric_limits<float>::epsilon())
      m_bounds = std::move(bounds);
    else
      DisableBounds();
  }
  void DisableBounds() const
  {
    m_bounds = std::nullopt;
  }
  void RefreshAspectRatio() const
  {
    RefreshAspectRatio(get_frame_buffer_aspect_ratio());
  }
  void RefreshAspectRatio(float new_aspect_ratio) const
  {
    m_aspect_ratio = new_aspect_ratio;
    set_projection();
  }
  void SetZoom() const
  {
    m_zoom_level = (m_bounds->top - m_bounds->bottom) / 2.F;
    if (m_bounds)
    {
      zoom(0.F);
    }
  }
  void SetZoom(float new_zoom) const
  {
    m_zoom_level = new_zoom;
    zoom(0.F);
  }

private:
  float get_frame_buffer_aspect_ratio() const
  {
    const auto &window_data = Application::CurrentWindow()->ViewWindowData();
    return static_cast<float>(window_data.frame_buffer_width)
           / static_cast<float>(window_data.frame_buffer_height);
  }
  void set_projection() const
  {
    m_camera.SetProjection(
      -m_aspect_ratio * m_zoom_level,
      m_aspect_ratio * m_zoom_level,
      -m_zoom_level,
      m_zoom_level);
  }
  mutable float                        m_aspect_ratio      = {};
  mutable float                        m_zoom_level        = { 1.F };
  mutable float                        m_zoom_precision    = { 1.F };
  mutable OrthographicCamera           m_camera            = {};
  mutable glm::vec3                    m_position          = {};
  mutable float                        m_rotation          = {};
  float                                m_translation_speed = { 16.F };
  float                                m_rotation_speed    = { 180.F };
  mutable std::optional<return_values> m_bounds            = {};
  void                                 zoom(const float offset) const;
};
static_assert(test::Test<OrthographicCameraController>);
#endif// MYPROJECT_ORTHOGRAPHICCAMERACONTROLLER_HPP
