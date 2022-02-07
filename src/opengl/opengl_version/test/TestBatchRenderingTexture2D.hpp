//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TestBatchRenderingTexture2D_HPP
#define MYPROJECT_TestBatchRenderingTexture2D_HPP
#include "IndexBuffer.hpp"
#include "Shader.hpp"

#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"


namespace test
{
class TestBatchRenderingTexture2D
{
public:
  TestBatchRenderingTexture2D();
  void OnUpdate(float) const {}
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const glengine::Event::Item &) const {}

private:
  glengine::VertexBuffer         m_vertex_buffer = {};
  glengine::IndexBuffer          m_index_buffer  = {};
  glengine::Shader               m_shader        = {};
  glengine::VertexArray          m_vertex_array  = {};
  std::vector<glengine::Texture> m_textures      = {};
  mutable glm::vec3              view_offset     = { 0.F, 0.F, 0.F };
  mutable glm::vec3              model_offset    = { 0.F, 0.F, 0.F };
};
}// namespace test
#endif// MYPROJECT_TestBatchRenderingTexture2D_HPP