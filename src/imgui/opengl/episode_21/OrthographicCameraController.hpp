//
// Created by pcvii on 12/2/2021.
//

#ifndef MYPROJECT_ORTHOGRAPHICCAMERACONTROLLER_HPP
#define MYPROJECT_ORTHOGRAPHICCAMERACONTROLLER_HPP
#include "EventItem.hpp"
#include "OrthographicCamera.hpp"
#include <fmt/format.h>
class OrthographicCameraController
{
public:
  OrthographicCameraController() = default;
  OrthographicCameraController(float aspect_ratio);

  friend void
    OnUpdate(const OrthographicCameraController &, float ts);
  friend void
    OnImguiRender(const OrthographicCameraController &);
  friend void
    OnRender(const OrthographicCameraController &)
  {
  }
  friend void OnEvent(const OrthographicCameraController &, const Event::Item &);
  const OrthographicCamera &
    Camera() const
  {
    return m_camera;
  }

  // todo add bounds for left,right,top,bottom.
  //  these will keep the not allow moving beyond those points.
  //


  struct return_values
  {
    bool left;
    bool right;
    bool bottom;
    bool top;
  };
  return_values
    CheckBounds() const
  {
    //todo test code.
    const glm::mat4 &vpm  = m_camera.ViewProjectionMatrix();
    const float      near = vpm[3][2] / (vpm[2][2] - 1.0f);
    const float      far  = vpm[3][2] / (vpm[2][2] + 1.0f);
    fmt::print("near: {}, far: {}\n", near, far);
    const float nearBottom = near * (vpm[2][1] - 1) / vpm[1][1];
    const float nearTop    = near * (vpm[2][1] + 1) / vpm[1][1];
    const float nearLeft   = near * (vpm[2][0] - 1) / vpm[0][0];
    const float nearRight  = near * (vpm[2][0] + 1) / vpm[0][0];
    fmt::print(
      "nearLeft {}, nearRight: {}, nearBottom: {},nearTop: {}\n",
      nearLeft,
      nearRight,
      nearBottom,
      nearTop);
    const float farBottom = far * (vpm[2][1] - 1) / vpm[1][1];
    const float farTop    = far * (vpm[2][1] + 1) / vpm[1][1];
    const float farLeft   = far * (vpm[2][0] - 1) / vpm[0][0];
    const float farRight  = far * (vpm[2][0] + 1) / vpm[0][0];
    fmt::print(
      "farLeft {}, farRight: {}, farBottom: {},farTop: {}\n",
      farLeft,
      farRight,
      farBottom,
      farTop);
    return return_values{
      .left   = m_bounds.x >= nearLeft && m_bounds.x >= farLeft,
      .right  = m_bounds.y <= nearRight && m_bounds.y <= farRight,
      .bottom = m_bounds.z >= nearBottom && m_bounds.z >= farBottom,
      .top    = m_bounds.w <= nearTop && m_bounds.w <= farTop
    };
  }

private:
  mutable float              m_aspect_ratio      = { 16.F / 9.F };
  mutable float              m_zoom_level        = { 1.F };
  mutable OrthographicCamera m_camera            = {};
  bool                       m_rotation_enabled  = { true };
  mutable glm::vec3          m_position          = {};
  mutable float              m_rotation          = {};
  float                      m_translation_speed = { 16.F };
  float                      m_rotation_speed    = { 180.F };
  mutable glm::vec4          m_bounds            = {};
};
void
  OnUpdate(const OrthographicCameraController &, float ts);
void
  OnImguiRender(const OrthographicCameraController &);
void OnEvent(const OrthographicCameraController &, const Event::Item &);
#endif// MYPROJECT_ORTHOGRAPHICCAMERACONTROLLER_HPP
