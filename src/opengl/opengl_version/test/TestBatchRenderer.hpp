//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_TESTBATCHRENDERER_HPP
#define FIELD_MAP_EDITOR_TESTBATCHRENDERER_HPP
#include "BatchRenderer.hpp"
#include "ImGuiViewPortWindow.hpp"
#include "ScopeGuard.hpp"


namespace test
{
class TestBatchRenderer
{
public:
  TestBatchRenderer();
  void on_update(float) const;
  void on_render() const;
  void on_im_gui_update() const;
  void on_event(const glengine::event::Item &event) const
  {
    m_imgui_viewport_window.on_event(event);
  }

private:
  void                                   GenerateQuads() const;
  void                                   set_uniforms() const;
  glengine::BatchRenderer                m_batch_renderer = { 10000 };
  mutable std::vector<glengine::Texture> m_textures       = {};
  mutable std::array<int, 2U>            m_count          = { 100, 100 };
  mutable glm::vec3                      view_offset      = { 0.F, 0.F, 0.F };
  glengine::ImGuiViewPortWindow          m_imgui_viewport_window = {
             "Test Batch Renderer"
  };
};
static_assert(glengine::Renderable<TestBatchRenderer>);
}// namespace test
#endif// FIELD_MAP_EDITOR_TESTBATCHRENDERER_HPP