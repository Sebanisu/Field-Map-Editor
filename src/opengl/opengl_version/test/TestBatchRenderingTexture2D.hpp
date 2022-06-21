//
// Created by pcvii on 11/24/2021.
//

#ifndef FIELD_MAP_EDITOR_TestBatchRenderingTexture2D_HPP
#define FIELD_MAP_EDITOR_TestBatchRenderingTexture2D_HPP
#include "IndexBuffer.hpp"
#include "Shader.hpp"

#include "ImGuiViewPortWindow.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"


namespace test
{
class TestBatchRenderingTexture2D
{
public:
  TestBatchRenderingTexture2D();
  void on_update(float) const;
  void on_render() const;
  void on_im_gui_update() const;
  void on_event(const glengine::event::Item &event) const;


private:
  void                           set_uniforms() const;
  glengine::VertexBuffer         m_vertex_buffer         = {};
  glengine::IndexBuffer          m_index_buffer          = {};
  glengine::Shader               m_shader                = {};
  glengine::VertexArray          m_vertex_array          = {};
  std::vector<glengine::Texture> m_textures              = {};
  mutable glm::vec3              view_offset             = { 0.F, 0.F, 0.F };
  glengine::ImGuiViewPortWindow  m_imgui_viewport_window = {
     "Test Batch Rendering W/ Texture2D"
  };
  void render_frame_buffer() const;
};
}// namespace test
#endif// FIELD_MAP_EDITOR_TestBatchRenderingTexture2D_HPP
