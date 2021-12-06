//
// Created by pcvii on 12/2/2021.
//

#ifndef MYPROJECT_ORTHOGRAPHICCAMERA_HPP
#define MYPROJECT_ORTHOGRAPHICCAMERA_HPP
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class OrthographicCamera
{
public:
  OrthographicCamera() = default;
  OrthographicCamera(float left, float right, float bottom, float top)
    : m_projection_matrix(glm::ortho(left, right, bottom, top, near, far))
  {
  }
  void
    SetProjection(float left, float right, float bottom, float top)
  {
    m_projection_matrix      = glm::ortho(left, right, bottom, top, near, far);
    m_view_projection_matrix = m_projection_matrix * m_view_matrix;
  }
  void
    SetPosition(glm::vec3 position)
  {
    m_position = position;
    recalculate_view_matrix();
  }
  void
    SetRotation(float rotation)
  {
    m_rotation = rotation;
    recalculate_view_matrix();
  }

  static constexpr glm::mat4 identity_matrix = glm::mat4{ 1.F };
  const glm::mat4 &
    ViewProjectionMatrix() const
  {
    return m_view_projection_matrix;
  }

private:
  void
    recalculate_view_matrix()
  {
    const auto view_transform =
      glm::translate(identity_matrix, m_position)
      * glm::rotate(
        identity_matrix, glm::radians(m_rotation), glm::vec3(0.F, 0.F, 1.F));
    m_view_matrix            = glm::inverse(view_transform);
    m_view_projection_matrix = m_projection_matrix * m_view_matrix;
  }
  static constexpr float near                = -1.0F;
  static constexpr float far                 = 1.0F;
  glm::mat4              m_projection_matrix = identity_matrix;
  glm::mat4              m_view_matrix       = identity_matrix;
  glm::mat4 m_view_projection_matrix = m_projection_matrix * m_view_matrix;
  glm::vec3 m_position               = {};
  float     m_rotation               = {};
};
#endif// MYPROJECT_ORTHOGRAPHICCAMERA_HPP
