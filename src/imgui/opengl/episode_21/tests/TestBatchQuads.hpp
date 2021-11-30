//
// Created by pcvii on 11/29/2021.
//

#ifndef MYPROJECT_TESTBATCHQUADS_HPP
#define MYPROJECT_TESTBATCHQUADS_HPP
#include "IndexBufferDynamic.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBufferDynamic.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace test
{
class TestBatchQuads
{
public:
  TestBatchQuads();

  friend void
    OnUpdate(const TestBatchQuads &, float);
  friend void
    OnRender(const TestBatchQuads &);
  friend void
    OnImGuiRender(const TestBatchQuads &);

  friend void
                               gen_verts(const TestBatchQuads &);

  static constexpr std::size_t QUAD_COUNT = { 100U };
  static constexpr std::size_t VERT_COUNT = { QUAD_COUNT * 4U };

private:
  VertexBufferDynamic            m_vertex_buffer   = { QUAD_COUNT };
  IndexBufferDynamic             m_index_buffer    = { QUAD_COUNT };
  mutable IndexBufferDynamicSize index_buffer_size = {};
  Shader                         m_shader          = {};
  VertexArray                    m_vertex_array    = {};
  mutable glm::vec3              view_offset       = { -2.F, -1.F, 0.F };
  mutable std::array<int, 2U>    m_count           = { 100, 100 };
  mutable float                  m_zoom            = { 0.078F };
  Texture m_blank = {(std::numeric_limits<std::uint32_t>::max)()};
};
void
  OnUpdate(const TestBatchQuads &, float);
void
  OnRender(const TestBatchQuads &);
void
  OnImGuiRender(const TestBatchQuads &);
void
  gen_verts(const TestBatchQuads &);

}// namespace test
#endif// MYPROJECT_TESTBATCHQUADS_HPP
