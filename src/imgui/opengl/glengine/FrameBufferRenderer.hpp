//
// Created by pcvii on 12/20/2021.
//

#ifndef MYPROJECT_FRAMEBUFFERRENDERER_HPP
#define MYPROJECT_FRAMEBUFFERRENDERER_HPP
#include "FrameBuffer.hpp"
#include "IndexBuffer.hpp"
#include "OrthographicCamera.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
namespace glengine
{
class FrameBufferRenderer
{
  struct simple_vertex
  {
    glm::vec2 location{};
    glm::vec2 uv{};
  };
  Shader m_shader = Shader(
    std::filesystem::path("res") / "shader" / "frame_buffer_shader.glsl");
  constexpr static glm::vec2                 m_location = { -0.5F, -0.5F };
  constexpr static glm::vec2                 m_size     = { 1.F, 1.F };
  constexpr static std::array<glm::vec2, 4U> m_uv = { glm::vec2{ 0.F, 0.F },
                                                      glm::vec2{ 1.F, 0.F },
                                                      glm::vec2{ 1.F, 1.F },
                                                      glm::vec2{ 0.F, 1.F } };
  VertexBuffer                               m_vertex_buffer = { std::array{
    simple_vertex{ .location = { m_location }, .uv = m_uv[0] },// 0
    simple_vertex{ .location = { m_location + glm::vec2{ m_size.x, 0.F } },
                                                 .uv = m_uv[1] },// 1
    simple_vertex{ .location = { m_location + glm::vec2{ m_size.x, m_size.y } },
                                                 .uv = m_uv[2] },// 2
    simple_vertex{ .location{ m_location + glm::vec2{ 0.F, m_size.y } },
                                                 .uv = m_uv[3] },// 3
  } };
  OrthographicCamera m_camera = OrthographicCamera(m_location, m_size);
  static constexpr VertexBufferLayout m_layout = VertexBufferLayout(
    VertexBufferElementType<float>{ 2U },
    VertexBufferElementType<float>{ 2U });
  VertexArray m_vertex_array = { m_vertex_buffer, m_layout };
  IndexBuffer m_index_buffer{ QuadIndicesInit };
  void        SetUniform() const;

public:
  void Draw(const glengine::FrameBuffer &frame_buffer) const;
};
}// namespace glengine
#endif// MYPROJECT_FRAMEBUFFERRENDERER_HPP
