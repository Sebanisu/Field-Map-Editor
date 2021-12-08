//
// Created by pcvii on 11/29/2021.
//

#include "TestBatchQuads.hpp"
#include "Renderer.hpp"
#include "scope_guard.hpp"
#include "Test.hpp"
#include "TestBatchQuads.hpp"
#include "Vertex.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <ranges>
static_assert(test::Test<test::TestBatchQuads>);
namespace test
{
static const Renderer renderer{};
}// namespace test
test::TestBatchQuads::TestBatchQuads()
  : m_shader(
    std::filesystem::current_path() / "res" / "shader" / "basic3.shader")
{


  m_vertex_array.Bind();
  m_vertex_array.push_back(m_vertex_buffer, Vertex::Layout());
  m_shader.Bind();
  m_shader.SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}
namespace test
{
static std::vector<Vertex> vertices = []()
{
  std::vector<Vertex> r{};
  r.reserve(TestBatchQuads::VERT_COUNT);
  return r;
}();
static std::uint32_t draw_count = 0U;
}// namespace test
void
  test::OnUpdate(const TestBatchQuads &self, float)
{
  draw_count = 0U;
  gen_verts(self);
}
void
  test::gen_verts(const test::TestBatchQuads &self)
{
  const auto flush = [&self]()
  {
    self.index_buffer_size = self.m_vertex_buffer.Update(vertices);
    OnRender(self);
    vertices.clear();
  };
  const auto max_count = TestBatchQuads::VERT_COUNT;
  vertices.clear();
  auto x_rng =
    std::views::iota(uint32_t{}, static_cast<uint32_t>(self.m_count[0]));
  auto y_rng =
    std::views::iota(uint32_t{}, static_cast<uint32_t>(self.m_count[1]));
  for (const auto x : x_rng)
  {
    for (const auto y : y_rng)
    {
      float r = static_cast<float>(x) / static_cast<float>(self.m_count[0]);
      float g = static_cast<float>(y) / static_cast<float>(self.m_count[1]);
      vertices += CreateQuad(
        { static_cast<float>(x), static_cast<float>(y) },
        { r, g, 1.F, 1.F },
        0);
      if (std::ranges::size(vertices) == max_count)
      {
        flush();
      }
    }
  }

  self.index_buffer_size = self.m_vertex_buffer.Update(vertices);
}
void
  test::OnRender(const TestBatchQuads &self)
{
  if (std::ranges::empty(vertices))
  {
    return;
  }
  const float window_width  = 16.F;
  const float window_height = 9.F;
  const auto  proj          = glm::ortho(
    self.view_offset.x / self.m_zoom,
    (self.view_offset.x + window_width) / self.m_zoom,
    self.view_offset.y / self.m_zoom,
    (self.view_offset.y + window_height) / self.m_zoom,
    -1.F,
    1.F);
  {
    const auto mvp = proj;
    self.m_shader.Bind();
    self.m_shader.SetUniform("u_MVP", mvp);
    self.m_shader.SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
    self.m_blank.Bind(0);
    self.m_shader.SetUniform("u_Textures", std::array{ 0 });
    renderer.Draw(
      self.index_buffer_size, self.m_vertex_array, self.m_index_buffer);
    ++draw_count;
  }
}
void
  test::OnImGuiUpdate(const TestBatchQuads &self)
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

  const auto pop3 = pop;
  ImGui::PushID(++id);
  if (ImGui::SliderInt2("Quad (X, Y)", std::data(self.m_count), 0, 256))
  {
  }
  ImGui::Text(
    "%s",
    fmt::format("Total Quads Rendered: {}", self.m_count[0] * self.m_count[1])
      .c_str());
  ImGui::Text(
    "%s",
    fmt::format(
      "Total Vertices Rendered: {}", self.m_count[0] * self.m_count[1] * 4)
      .c_str());
  ImGui::Text(
    "%s",
    fmt::format(
      "Total Indices Rendered: {}", self.m_count[0] * self.m_count[1] * 6)
      .c_str());
  ImGui::Text("%s", fmt::format("Total Draws: {}", test::draw_count).c_str());
}