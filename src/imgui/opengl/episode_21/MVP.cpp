//
// Created by pcvii on 12/2/2021.
//

#include "MVP.hpp"
#include "Renderer.hpp"
#include <open_viii/graphics/Rectangle.hpp>
void
  OnUpdate(const MVP &, float)
{
}
void
  OnRender(const MVP &)
{
}
bool
  OnImGuiRender(const MVP &)
{
  return false;
}
static std::array<open_viii::graphics::Point<int>, 2>
  GetViewport()
{
  std::array<open_viii::graphics::Point<int>, 2> viewport{};
  GLCall{ glGetIntegerv,
          GL_VIEWPORT,
          reinterpret_cast<int *>(std::ranges::data(viewport)) };
  return viewport;
}
void
  SetUniforms(
    const MVP      &self,
    const Shader   &shader,
    const float     height,
    const glm::vec4 color)
{
  //todo cache the matrix to only update when it changes. more work?
  const auto viewport      = GetViewport();
  float      window_height = height;
  float      window_width  = window_height * (viewport[1].x() - viewport[0].x())
                       / (viewport[1].y() - viewport[0].y());

  const auto proj = glm::ortho(
    self.m_view_offset.x / self.m_zoom,
    (self.m_view_offset.x + window_width) / self.m_zoom,
    self.m_view_offset.y / self.m_zoom,
    (self.m_view_offset.y + window_height) / self.m_zoom,
    -1.F,
    1.F);
  shader.Bind();
  shader.SetUniform("u_MVP", proj);
  shader.SetUniform("u_Color", color.r, color.g, color.b, color.a);
}