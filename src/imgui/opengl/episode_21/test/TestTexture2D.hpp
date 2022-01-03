//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TestTexture2D_HPP
#define MYPROJECT_TestTexture2D_HPP
#include "IndexBuffer.hpp"
#include "Shader.hpp"

#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

namespace test
{
class TestTexture2D
{
public:
  TestTexture2D();
  void OnUpdate(float) const {}
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const glengine::Event::Item &) const {}

private:
  glengine::VertexBuffer m_vertex_buffer        = {};
  glengine::IndexBuffer  m_index_buffer         = {};
  glengine::Texture      m_texture              = {};
  glengine::Shader       m_shader               = {};
  glengine::VertexArray  m_vertex_array         = {};
  mutable glm::vec3      view_offset            = { 0.F, 0.F, 0.F };
  mutable glm::vec3      model_offset           = { 200.F, 200.F, 0.F };
  mutable glm::vec3      model2_offset          = { 400.F, 200.F, 0.F };
  constexpr static auto  m_vertex_buffer_layout = glengine::VertexBufferLayout(
    glengine::VertexBufferElementType<float>{ 2U },
    glengine::VertexBufferElementType<float>{ 2U });
};
}// namespace test
#endif// MYPROJECT_TestTexture2D_HPP
