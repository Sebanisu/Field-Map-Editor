//
// Created by pcvii on 11/29/2021.
//

#include "TestBatchQuads.hpp"
#include "ImGuiPushID.hpp"
#include "Renderer.hpp"
#include "scope_guard.hpp"
#include "Vertex.hpp"


static_assert(glengine::Renderable<test::TestBatchQuads>);
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
static std::vector<Vertex> vertices = []() {
  std::vector<Vertex> r{};
  r.reserve(TestBatchQuads::VERT_COUNT);
  return r;
}();
static std::uint32_t draw_count = 0U;
}// namespace test
void test::TestBatchQuads::OnUpdate(float) const
{
  draw_count = 0U;
}
void test::TestBatchQuads::gen_verts() const
{
  const auto flush = [this]() {
    index_buffer_size = m_vertex_buffer.Update(vertices);
    Draw();
    vertices.clear();
  };
  const auto max_count = TestBatchQuads::VERT_COUNT;
  vertices.clear();
  auto x_rng = std::views::iota(uint32_t{}, static_cast<uint32_t>(m_count[0]));
  auto y_rng = std::views::iota(uint32_t{}, static_cast<uint32_t>(m_count[1]));
  for (const auto x : x_rng)
  {
    for (const auto y : y_rng)
    {
      float r = static_cast<float>(x) / static_cast<float>(m_count[0]);
      float g = static_cast<float>(y) / static_cast<float>(m_count[1]);
      vertices += CreateQuad(
        { static_cast<float>(x), static_cast<float>(y), 0.F },
        { r, g, 1.F, 1.F },
        0);
      if (std::ranges::size(vertices) == max_count)
      {
        flush();
      }
    }
  }

  index_buffer_size = m_vertex_buffer.Update(vertices);
}
void test::TestBatchQuads::OnRender() const
{
  m_imgui_viewport_window.SyncOpenGLViewPort();

  m_imgui_viewport_window.OnRender([&]() { gen_verts(); });
}
void test::TestBatchQuads::OnImGuiUpdate() const
{
  static constexpr float window_width = 100.F;
  const float            window_height =
    window_width / m_imgui_viewport_window.ViewPortAspectRatio();
  {
    const auto pop = glengine::ImGuiPushID();
    if (ImGui::SliderFloat2(
          "View Offset", &view_offset.x, -window_width, window_width))
    {
      view_offset.y = std::clamp(view_offset.y, -window_height, window_height);
    }
  }
  {
    const auto pop2 = glengine::ImGuiPushID();
    if (ImGui::SliderFloat("Zoom", &m_zoom, 2.F, 100.F/256.F))
    {
    }
  }
  {
    const auto pop3 = glengine::ImGuiPushID();
    if (ImGui::SliderInt2("Quad (X, Y)", std::data(m_count), 0, 256))
    {
    }
  }
  ImGui::Text(
    "%s",
    fmt::format("Total Quads Rendered: {}", m_count[0] * m_count[1]).c_str());
  ImGui::Text(
    "%s",
    fmt::format("Total Vertices Rendered: {}", m_count[0] * m_count[1] * 4)
      .c_str());
  ImGui::Text(
    "%s",
    fmt::format("Total Indices Rendered: {}", m_count[0] * m_count[1] * 6)
      .c_str());
  ImGui::Text("%s", fmt::format("Total Draws: {}", test::draw_count).c_str());
}
void test::TestBatchQuads::Draw() const
{
  if (std::ranges::empty(vertices))
  {
    return;
  }

  static constexpr float window_width = 100.F;
  const float            window_height =
    window_width / m_imgui_viewport_window.ViewPortAspectRatio();
  const auto proj = glm::ortho(
    view_offset.x / m_zoom,
    (view_offset.x + window_width) / m_zoom,
    view_offset.y / m_zoom,
    (view_offset.y + window_height) / m_zoom,
    -1.F,
    1.F);
  {
    const auto mvp = proj;
    m_shader.Bind();
    m_shader.SetUniform("u_MVP", mvp);
    m_shader.SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
    m_blank.Bind(0);
    m_shader.SetUniform("u_Textures", std::array{ 0 });
    glengine::Renderer::Draw(index_buffer_size, m_vertex_array, m_index_buffer);
    ++draw_count;
  }
}
