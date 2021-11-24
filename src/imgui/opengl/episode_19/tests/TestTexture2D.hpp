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
  TestTexture2D()
    : m_vertex_buffer{ std::array{
      // clang-format off
       -50.0F,-50.0F, 0.F, 0.F, // 0
        50.0F,-50.0F, 1.F, 0.F, // 1
        50.0F, 50.0F, 1.F, 1.F, // 2
       -50.0F, 50.0F, 0.F, 1.F, // 3
      // clang-format on
    } }
    , m_index_buffer{ std::array{
        // clang-format off
        0U, 1U, 2U, // 0
        2U, 3U, 0U  // 1
        // clang-format on
      } }
    , m_vertex_buffer_layout{
      VertexBufferLayout::VertexBufferElementType<float>{2U},
      VertexBufferLayout::VertexBufferElementType<float>{2U},
    }
    , m_texture{ std::filesystem::current_path() / "res" / "textures"
                 / "logo.png" }
    , m_shader{ std::filesystem::current_path() / "res" / "shader"
                / "basic.shader" }
  {
    m_vertex_array.Bind();
    m_vertex_array.push_back(m_vertex_buffer, m_vertex_buffer_layout);
    m_shader.Bind();
    m_shader.SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
    m_shader.SetUniform("u_Texture", 0);
  }
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
