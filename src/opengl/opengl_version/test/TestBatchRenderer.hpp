//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_TESTBATCHRENDERER_HPP
#define FIELD_MAP_EDITOR_TESTBATCHRENDERER_HPP
#include "BatchRenderer.hpp"
#include "ImGuiViewPortWindow.hpp"
#include "scope_guard.hpp"


namespace test
{
class TestBatchRenderer
{
public:
  TestBatchRenderer();
  void OnUpdate(float) const;
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const glengine::Event::Item &) const {}

private:
  void                                   GenerateQuads() const;
  void                                   SetUniforms() const;
  glengine::BatchRenderer                m_batch_renderer = { 10000 };
  mutable std::vector<glengine::Texture> m_textures       = {};
  mutable std::array<int, 2U>            m_count          = { 100, 100 };
  mutable glm::vec3                      view_offset      = { 0.F, 0.F, 0.F };
  mutable float                          m_zoom           = { 1.F };
  glengine::ImGuiViewPortWindow          m_imgui_viewport_window = {
             "Test Batch Renderer"
  };
};
static_assert(glengine::Renderable<TestBatchRenderer>);
}// namespace test
#endif// FIELD_MAP_EDITOR_TESTBATCHRENDERER_HPP