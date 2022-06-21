//
// Created by pcvii on 11/29/2021.
//

#ifndef FIELD_MAP_EDITOR_TESTBATCHQUADS_HPP
#define FIELD_MAP_EDITOR_TESTBATCHQUADS_HPP
#include "IndexBufferDynamic.hpp"
#include "Shader.hpp"

#include "ImGuiViewPortWindow.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBufferDynamic.hpp"


namespace test
{
class TestBatchQuads
{
public:
  TestBatchQuads();


  void                         on_update(float) const;
  void                         on_render() const;
  void                         on_im_gui_update() const;
  void                         on_event(const glengine::event::Item &) const;
  void                         GenerateQuads() const;

  static constexpr std::size_t QUAD_COUNT = { 100U };
  static constexpr std::size_t VERT_COUNT = { QUAD_COUNT * 4U };

private:
  void                                     Draw() const;
  glengine::VertexBufferDynamic            m_vertex_buffer   = { QUAD_COUNT };
  glengine::IndexBufferDynamic             m_index_buffer    = { QUAD_COUNT };
  mutable glengine::IndexBufferDynamicSize index_buffer_size = {};
  glengine::Shader                         m_shader          = {};
  glengine::VertexArray                    m_vertex_array    = {};
  mutable glm::vec3                        view_offset = { 0.F, 0.F, 0.F };
  mutable std::array<int, 2U>              m_count     = { 100, 100 };
  glengine::Texture m_blank = { (std::numeric_limits<std::uint32_t>::max)() };
  glengine::ImGuiViewPortWindow m_imgui_viewport_window = {
    "Test Batch Quads"
  };
  void set_uniforms() const;
};
}// namespace test
#endif// FIELD_MAP_EDITOR_TESTBATCHQUADS_HPP
