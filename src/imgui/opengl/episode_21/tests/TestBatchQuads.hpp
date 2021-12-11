//
// Created by pcvii on 11/29/2021.
//

#ifndef MYPROJECT_TESTBATCHQUADS_HPP
#define MYPROJECT_TESTBATCHQUADS_HPP
#include "IndexBufferDynamic.hpp"
#include "Shader.hpp"
#include "Test.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBufferDynamic.hpp"


namespace test
{
class TestBatchQuads
{
public:
  TestBatchQuads();


  void                         OnUpdate(float) const;
  void                         OnRender() const;
  void                         OnImGuiUpdate() const;
  void                         OnEvent(const Event::Item &) const {}
  void                         gen_verts() const;

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
  Texture m_blank = { (std::numeric_limits<std::uint32_t>::max)() };
};
}// namespace test
#endif// MYPROJECT_TESTBATCHQUADS_HPP
