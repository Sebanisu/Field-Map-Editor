//
// Created by pcvii on 12/20/2021.
//

#include "FrameBufferRenderer.hpp"
void glengine::FrameBufferRenderer::Draw(
  const glengine::FrameBuffer &frame_buffer) const
{
  SetUniform();
  Renderer::Draw(
    frame_buffer.GetColorAttachment(), m_vertex_array, m_index_buffer);
}
void glengine::FrameBufferRenderer::SetUniform() const
{
  m_shader.Bind();
  m_shader.SetUniform("u_MVP", m_camera.ViewProjectionMatrix());
}
