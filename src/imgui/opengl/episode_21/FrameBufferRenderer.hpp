//
// Created by pcvii on 12/20/2021.
//

#ifndef MYPROJECT_FRAMEBUFFERRENDERER_HPP
#define MYPROJECT_FRAMEBUFFERRENDERER_HPP
#include "FrameBuffer.hpp"
#include "OrthographicCamera.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "VertexBufferLayout.hpp"
class FrameBufferRenderer
{
  struct simple_vertex
  {
    glm::vec2 location{};
    glm::vec2 uv{};
  };
  Shader m_shader = Shader(
    std::filesystem::path("res") / "shader" / "frame_buffer_shader.glsl");
  constexpr static glm::vec2                    offset = { -0.5F, -0.5F };
  constexpr static glm::vec2                    size   = { 1.F, 1.F };
  constexpr static std::array<glm::vec2, 4U>    uv = { glm::vec2{ 0.F, 0.F },
                                                    glm::vec2{ 1.F, 0.F },
                                                    glm::vec2{ 1.F, 1.F },
                                                    glm::vec2{ 0.F, 1.F } };
  constexpr static std::array<simple_vertex, 4> m_vertices{
    simple_vertex{ .location = { offset }, .uv = uv[0] },// 0
    simple_vertex{ .location = { offset + glm::vec2{ size.x, 0.F } },
                   .uv       = uv[1] },// 1
    simple_vertex{ .location = { offset + glm::vec2{ size.x, size.y } },
                   .uv       = uv[2] },// 2
    simple_vertex{ .location{ offset + glm::vec2{ 0.F, size.y } },
                   .uv = uv[3] },// 3
  };
  OrthographicCamera    m_camera = OrthographicCamera(offset, size);
  static constexpr VertexBufferLayout m_layout = VertexBufferLayout(VertexBufferElementType<float>{ 2U },
                                     VertexBufferElementType<float>{ 2U });
  void                  SetUniform() const
  {
    m_shader.SetUniform("u_MVP", m_camera.ViewProjectionMatrix());
  }
};
#endif// MYPROJECT_FRAMEBUFFERRENDERER_HPP
