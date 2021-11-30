//
// Created by pcvii on 11/30/2021.
//

#include "BatchRenderer.hpp"
#include "Renderer.hpp"
#include "scope_guard.hpp"
#include "tests/Test.hpp"
#include "Vertex.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <ranges>
static_assert(test::Test<test::BatchRenderer>);
namespace test
{
static const Renderer renderer{};
static std::uint32_t  draw_count = 0U;
}// namespace test
test::BatchRenderer::BatchRenderer()
  : BatchRenderer(100U)
{
}
test::BatchRenderer::BatchRenderer(std::size_t quad_count)
  : m_quad_count(quad_count)
  , m_shader(
      std::filesystem::current_path() / "res" / "shader" / "basic3.shader")
{
  m_vertex_array.Bind();
  m_vertex_array.push_back(m_vertex_buffer, Vertex::Layout());
}

void
  test::OnUpdate(const BatchRenderer &, float)
{
  draw_count = 0U;
}
void
  test::BatchRenderer::Draw() const
{
  index_buffer_size = m_vertex_buffer.Update(m_vertices);
  DrawVertices();
}
void
  test::BatchRenderer::Draw(Quad quad) const
{
  if (std::ranges::size(m_vertices) == VERT_COUNT())
  {
    FlushVertices();
  }
  m_vertices += std::move(quad);
}
void
  test::OnRender(const BatchRenderer &)
{
}
void
  test::OnImGuiRender(const BatchRenderer &self)
{
  int        id           = 0;

  int        window_width = 16;
  const auto pop          = scope_guard(&ImGui::PopID);
  ImGui::PushID(++id);
  if (ImGui::SliderFloat2(
        "View Offset",
        &self.view_offset.x,
        -static_cast<float>(window_width),
        static_cast<float>(window_width)))
  {
  }
  const auto pop2 = pop;
  ImGui::PushID(++id);
  if (ImGui::SliderFloat("Zoom", &self.m_zoom, 1.F, .001F))
  {
  }


  ImGui::Text("%s", fmt::format("Total Draws: {}", test::draw_count).c_str());
}
void
  test::BatchRenderer::FlushVertices() const
{
  index_buffer_size = m_vertex_buffer.Update(m_vertices);
  DrawVertices();
  m_vertices.clear();
}
void
  test::BatchRenderer::DrawVertices() const
{
  if (std::ranges::empty(m_vertices))
  {
    return;
  }
  SetUniforms();
  renderer.Draw(index_buffer_size, m_vertex_array, m_index_buffer, m_shader);
  ++draw_count;
}
void
  test::BatchRenderer::SetUniforms() const
{
  const float window_width  = 16.F;
  const float window_height = 9.F;
  const auto  proj          = glm::ortho(
              view_offset.x / m_zoom,
              (view_offset.x + window_width) / m_zoom,
              view_offset.y / m_zoom,
              (view_offset.y + window_height) / m_zoom,
              -1.F,
              1.F);

  const auto mvp = proj;
  m_shader.Bind();
  m_shader.SetUniform("u_MVP", mvp);
  m_shader.SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}
void
  test::BatchRenderer::Clear() const
{
  m_vertices.clear();
}
