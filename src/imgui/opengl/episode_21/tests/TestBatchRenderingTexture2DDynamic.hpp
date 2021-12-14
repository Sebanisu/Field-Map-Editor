//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TestBatchRenderingTexture2DDynamic_HPP
#define MYPROJECT_TestBatchRenderingTexture2DDynamic_HPP
#include "IndexBufferDynamic.hpp"
#include "Shader.hpp"

#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBufferDynamic.hpp"


namespace test
{
class TestBatchRenderingTexture2DDynamic
{
public:
  TestBatchRenderingTexture2DDynamic();
  void OnUpdate(float) const;
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const Event::Item &) const {}

private:
  VertexBufferDynamic            m_vertex_buffer   = { 1000 };
  IndexBufferDynamic             m_index_buffer    = { 1000 };
  mutable IndexBufferDynamicSize index_buffer_size = {};
  Shader                         m_shader          = {};
  VertexArray                    m_vertex_array    = {};
  std::vector<Texture>           m_textures        = {};
  mutable glm::vec3              view_offset       = { 0.F, 0.F, 0.F };
  mutable glm::vec2              model_offset1     = { 2.F, 0.F };
  mutable glm::vec2              model_offset2     = { 4.F, 0.F };
  mutable glm::vec2              model_offset3     = { 6.F, 0.F };
};
void OnUpdate(const TestBatchRenderingTexture2DDynamic &, float);
void OnRender(const TestBatchRenderingTexture2DDynamic &);
void OnImGuiUpdate(const TestBatchRenderingTexture2DDynamic &);

}// namespace test
#endif// MYPROJECT_TestBatchRenderingTexture2DDynamic_HPP
