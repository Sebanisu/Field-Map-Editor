//
// Created by pcvii on 11/30/2021.
//

#include "BatchRenderer.hpp"
#include "scope_guard.hpp"
#include "tests/Test.hpp"
#include <imgui.h>
#include <ranges>
static_assert(test::Test<BatchRenderer>);
static const Renderer renderer{};
static std::uint32_t  draw_count = 0U;
BatchRenderer::BatchRenderer()
  : BatchRenderer(100U)
{
}
BatchRenderer::BatchRenderer(std::size_t quad_count)
  : m_quad_count(quad_count)
  , m_shader(
      std::filesystem::current_path() / "res" / "shader" / "basic3.shader")
{
  m_vertex_array.Bind();
  m_vertex_array.push_back(m_vertex_buffer, Vertex::Layout());
}

void
  OnUpdate(const BatchRenderer &, float)
{
  draw_count = 0U;
}
void
  BatchRenderer::Draw() const
{
  index_buffer_size = m_vertex_buffer.Update(m_vertices);
  DrawVertices();
}
void
  BatchRenderer::Draw(Quad quad) const
{
  if (std::ranges::size(m_vertices) == VERT_COUNT())
  {
    FlushVertices();
  }
  m_vertices += std::move(quad);
}
void
  OnRender(const BatchRenderer &)
{
}
void
  OnImGuiRender(const BatchRenderer &)
{
  ImGui::Text("%s", fmt::format("Total Draws: {}", draw_count).c_str());
}
void
  BatchRenderer::FlushVertices() const
{
  index_buffer_size = m_vertex_buffer.Update(m_vertices);
  DrawVertices();
  m_vertices.clear();
}
void
  BatchRenderer::DrawVertices() const
{
  if (std::ranges::empty(m_vertices))
  {
    return;
  }
  renderer.Draw(index_buffer_size, m_vertex_array, m_index_buffer, m_shader);
  ++draw_count;
}
void
  BatchRenderer::Clear() const
{
  m_vertices.clear();
}
