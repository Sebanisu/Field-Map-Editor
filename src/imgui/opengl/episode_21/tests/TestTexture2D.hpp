//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TestTexture2D_HPP
#define MYPROJECT_TestTexture2D_HPP
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "Test.h"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
namespace test
{
class TestTexture2D
{
public:
  TestTexture2D();
  void
    OnUpdate(float) const
  {
  }
  void
    OnRender() const;
  void
    OnImGuiRender() const;

private:
  mutable VertexArray        m_vertex_array         = {};
  mutable VertexBuffer       m_vertex_buffer        = {};
  mutable IndexBuffer        m_index_buffer         = {};
  mutable VertexBufferLayout m_vertex_buffer_layout = {};
  mutable Texture            m_texture              = {};
  mutable Shader             m_shader               = {};
  mutable
      glm::vec3       view_offset = { 0.F, 0.F, 0.F };
  mutable glm::vec3       model_offset = { 200.F, 200.F, 0.F };
  mutable glm::vec3       model2_offset = { 400.F, 200.F, 0.F };
};
static_assert(Test<TestTexture2D>);
}// namespace test
#endif// MYPROJECT_TestTexture2D_HPP