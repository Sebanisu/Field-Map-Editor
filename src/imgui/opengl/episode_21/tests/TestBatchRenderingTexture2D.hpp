//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TestBatchRenderingTexture2D_HPP
#define MYPROJECT_TestBatchRenderingTexture2D_HPP
#include "IndexBuffer.hpp"
#include "scope_guard.hpp"
#include "Shader.hpp"
#include "Test.h"
#include "Texture.hpp"
#include "Vertex.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include <algorithm>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <ranges>

namespace test
{
class TestBatchRenderingTexture2D
{
public:
  TestBatchRenderingTexture2D();

  void
    OnUpdate(float) const;
  void
    OnRender() const;
  void
    OnImGuiRender() const;

private:
  VertexBuffer         m_vertex_buffer = {};
  IndexBuffer          m_index_buffer  = {};
  Shader               m_shader        = {};
  VertexArray          m_vertex_array  = {};
  std::vector<Texture> m_textures      = {};
  mutable glm::vec3    view_offset     = { 0.F, 0.F, 0.F };
  mutable glm::vec3    model_offset    = { 0.F, 0.F, 0.F };
  // mutable glm::vec3  model2_offset          = { 400.F, 200.F, 0.F };
};
static_assert(Test<TestBatchRenderingTexture2D>);
}// namespace test
#endif// MYPROJECT_TestBatchRenderingTexture2D_HPP
