//
// Created by pcvii on 12/2/2021.
//

#ifndef FIELD_MAP_EDITOR_ORTHOGRAPHICCAMERA_HPP
#define FIELD_MAP_EDITOR_ORTHOGRAPHICCAMERA_HPP
//#include "Application.hpp"
#include <fmt/ranges.h>
namespace glengine
{
static const float half = 2.F;
class OrthographicCamera
{
public:
  OrthographicCamera() = default;
  OrthographicCamera(float zoom, float aspect)
    : OrthographicCamera(
      -(zoom * aspect) / half,
      (zoom * aspect) / half,
      -zoom / half,
      zoom / half)
  {
  }
  OrthographicCamera(glm::vec2 size)
    : OrthographicCamera(
      -size.x / half,
      size.x / half,
      -size.y / half,
      size.y / half)
  {
  }
  //  OrthographicCamera(glm::vec3 position, glm::vec2 size)
  //    : OrthographicCamera(
  //      -size.x / half,
  //      size.x / half,
  //      -size.y / half,
  //      size.y / half)
  //  {
  //    SetPosition(position);
  //  }
  OrthographicCamera(float left, float right, float bottom, float top)
    : m_projection_matrix(glm::ortho(left, right, bottom, top, near, far))
    , m_bounds(left, right, bottom, top)
  {
    recalculate_mvp();
  }

  void SetProjection(float left, float right, float bottom, float top)
  {
    m_projection_matrix = glm::ortho(left, right, bottom, top, near, far);
    m_bounds            = glm::vec4(left, right, bottom, top);
    recalculate_mvp();
  }

  void SetProjection(float zoom, float aspect)
  {
    SetProjection(
      -(zoom * aspect) / half,
      (zoom * aspect) / half,
      -zoom / half,
      zoom / half);
  }
  void SetProjection(glm::vec2 size)
  {
    SetProjection(-size.x / half, size.x / half, -size.y / half, size.y / half);
  }
  void SetPosition(glm::vec3 position)
  {
    m_position    = position;
    m_view_matrix = glm::translate(identity_matrix, m_position);
    recalculate_mvp();
  }
  void SetRotation(float rotation)
  {
    m_rotation     = rotation;
    m_model_matrix = glm::rotate(
      identity_matrix, glm::radians(m_rotation), glm::vec3(0.F, 0.F, 1.F));
    recalculate_mvp();
  }

  static constexpr glm::mat4 identity_matrix = glm::mat4{ 1.F };
  const glm::mat4           &ViewProjectionMatrix() const
  {
    return m_view_projection_matrix;
  }
  const glm::mat4 InverseViewProjectionMatrix() const
  {
    return glm::inverse(glm::translate(m_view_projection_matrix, -m_position));
  }
  const auto &ViewMatrix() const
  {
    return m_view_matrix;
  }
  const auto &ProjectionMatrix() const
  {
    return m_projection_matrix;
  }
  const auto ScreenSpaceToWorldSpace(glm::vec4 in) const
  {
    auto out = in * InverseViewProjectionMatrix();
    out.w    = 1.F / out.w;
    out.x *= -out.w;
    out.y *= out.w;
    out.z *= out.w;
    return out;
  }
  const glm::vec4 &Bounds() const noexcept
  {
    return m_bounds;
  }
  void OnImGuiUpdate() const
  {
    const auto fmt_mat4 = [](glm::mat4 in) -> std::string {
      return fmt::format(
        "[{},{},{},{}], [{},{},{},{}], [{},{},{},{}], [{},{},{},{}]",
        in[0][0],
        in[0][1],
        in[0][2],
        in[0][3],
        in[1][0],
        in[1][1],
        in[1][2],
        in[1][3],
        in[2][0],
        in[2][1],
        in[2][2],
        in[2][3],
        in[3][0],
        in[3][1],
        in[3][2],
        in[3][3]);
    };

    ImGui::Text(
      "%s",
      fmt::format(
        "Projection: {}\nView: {}\nModel: {}\nMVP: {}\nPosition - X: {}, Y: "
        "{}, Z: {}\nRotation: {}\nPre-transform Bounds - left: {}, right: {}, "
        "bottom: {}, top: {}\n",
        fmt_mat4(m_projection_matrix),
        fmt_mat4(m_view_matrix),
        fmt_mat4(m_model_matrix),
        fmt_mat4(m_view_projection_matrix),
        m_position.x,
        m_position.y,
        m_position.z,
        m_rotation,
        m_bounds.x,
        m_bounds.y,
        m_bounds.z,
        m_bounds.w)
        .c_str());
  }

private:
  void recalculate_mvp()
  {
    m_view_projection_matrix =
      m_projection_matrix * m_view_matrix * m_model_matrix;
  }
  static constexpr float near                = -1.0F;
  static constexpr float far                 = 1.0F;
  glm::mat4              m_projection_matrix = identity_matrix;
  glm::mat4              m_view_matrix       = identity_matrix;
  glm::mat4              m_model_matrix      = identity_matrix;
  glm::mat4 m_view_projection_matrix = m_projection_matrix * m_view_matrix;
  glm::vec3 m_position               = {};
  float     m_rotation               = {};
  glm::vec4 m_bounds                 = {};
};
}// namespace glengine
#endif// FIELD_MAP_EDITOR_ORTHOGRAPHICCAMERA_HPP
