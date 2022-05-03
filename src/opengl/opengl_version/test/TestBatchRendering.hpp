//
// Created by pcvii on 11/24/2021.
//

#ifndef FIELD_MAP_EDITOR_TESTBATCHRENDERING_HPP
#define FIELD_MAP_EDITOR_TESTBATCHRENDERING_HPP
#include "ImGuiViewPortWindow.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"

namespace test
{
class TestBatchRendering
{
public:
  TestBatchRendering();
  void OnUpdate(float) const {}
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const glengine::Event::Item &) const {}

private:
  glengine::VertexBuffer        m_vertex_buffer         = {};
  glengine::IndexBuffer         m_index_buffer          = {};
  glengine::Shader              m_shader                = {};
  glengine::VertexArray         m_vertex_array          = {};
  mutable glm::vec3             view_offset             = { 0.F, 0.F, 0.F };
  mutable glm::vec3             model_offset            = { 0.F, 0.F, 0.F };
  glengine::ImGuiViewPortWindow m_imgui_viewport_window = {
    "Test Batch Rendering"
  };
};
}// namespace test

#endif// FIELD_MAP_EDITOR_TESTBATCHRENDERING_HPP
