//
// Created by pcvii on 12/20/2021.
//

#include "FrameBufferRenderer.hpp"
void glengine::FrameBufferRenderer::draw(
  const glengine::FrameBuffer &frame_buffer) const
{
  set_uniform();
  Renderer::Draw(
    frame_buffer.get_color_attachment(), m_vertex_array, m_index_buffer);
}
void glengine::FrameBufferRenderer::set_uniform() const
{
  m_shader.bind();
  m_shader.set_uniform("u_MVP", m_camera.view_projection_matrix());
}
