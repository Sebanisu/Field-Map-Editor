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
#include <glm/glm.hpp>
#include <vector>

namespace test
{
class TestBatchRenderingTexture2D
{
public:
  TestBatchRenderingTexture2D();

  friend void
    OnUpdate(const TestBatchRenderingTexture2D &, float);
  friend void
    OnRender(const TestBatchRenderingTexture2D &);
  friend void
    OnImGuiRender(const TestBatchRenderingTexture2D &);

private:
  VertexBuffer         m_vertex_buffer = {};
  IndexBuffer          m_index_buffer  = {};
  Shader               m_shader        = {};
  VertexArray          m_vertex_array  = {};
  std::vector<Texture> m_textures      = {};
  mutable glm::vec3    view_offset     = { 0.F, 0.F, 0.F };
  mutable glm::vec3    model_offset    = { 0.F, 0.F, 0.F };
};
void
  OnUpdate(const TestBatchRenderingTexture2D &, float);
void
  OnRender(const TestBatchRenderingTexture2D &);
void
  OnImGuiRender(const TestBatchRenderingTexture2D &);
}// namespace test
#endif// MYPROJECT_TestBatchRenderingTexture2D_HPP
