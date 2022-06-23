//
// Created by pcvii on 11/29/2021.
//

#include "TestBatchQuads.hpp"
#include "Application.hpp"
#include "ImGuiPushID.hpp"
#include "Renderer.hpp"
#include "Vertex.hpp"

static constinit bool FitWidth  = true;
static constinit bool FitHeight = true;
static constinit bool Preview   = false;

static_assert(glengine::Renderable<test::TestBatchQuads>);
test::TestBatchQuads::TestBatchQuads()
  : m_shader(
    std::filesystem::current_path() / "res" / "shader" / "basic3.shader")
{
  m_vertex_array.bind();
  m_vertex_array.push_back(m_vertex_buffer, Vertex::layout());
  m_shader.bind();
  m_shader.set_uniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}
namespace test
{
static std::vector<Vertex> Vertices = []() {
  std::vector<Vertex> r{};
  r.reserve(TestBatchQuads::vert_count);
  return r;
}();
static std::uint32_t DrawCount = 0U;
}// namespace test
void test::TestBatchQuads::on_update(float ts) const
{
  DrawCount = 0U;
  m_imgui_viewport_window.set_image_bounds({ m_count[0], m_count[1] });
  m_imgui_viewport_window.on_update(ts);
  m_imgui_viewport_window.fit(FitWidth, FitHeight);
}
void test::TestBatchQuads::generate_quads() const
{
  const auto flush = [this]() {
    index_buffer_size = m_vertex_buffer.update(Vertices);
    draw();
    Vertices.clear();
  };
  const auto max_count = TestBatchQuads::vert_count;
  Vertices.clear();
  auto x_rng = std::views::iota(uint32_t{}, static_cast<uint32_t>(m_count[0]));
  auto y_rng = std::views::iota(uint32_t{}, static_cast<uint32_t>(m_count[1]));
  for (const auto x : x_rng)
  {
    for (const auto y : y_rng)
    {
      float r = static_cast<float>(x) / static_cast<float>(m_count[0]);
      float g = static_cast<float>(y) / static_cast<float>(m_count[1]);
      Vertices += CreateQuad(
        { static_cast<float>(x) - static_cast<float>(m_count[0]) / 2.F,
          static_cast<float>(y) - static_cast<float>(m_count[1]) / 2.F,
          0.F },
        { r, g, 1.F, 1.F },
        0);
      if (std::ranges::size(Vertices) == max_count)
      {
        flush();
      }
    }
  }

  index_buffer_size = m_vertex_buffer.update(Vertices);
}
void test::TestBatchQuads::on_render() const
{
  set_uniforms();
  m_imgui_viewport_window.on_render([&]() { generate_quads(); });
  GetViewPortPreview().on_render(m_imgui_viewport_window, [this]() {
    Preview                = true;
    const auto pop_preview = glengine::ScopeGuard([]() { Preview = false; });
    set_uniforms();
    generate_quads();
  });
}
void test::TestBatchQuads::on_im_gui_update() const
{
  static constexpr float window_width = 100.F;
  const float            window_height =
    window_width / m_imgui_viewport_window.view_port_aspect_ratio();
  {
    const auto pop = glengine::ImGuiPushId();

    ImGui::Checkbox("fit Height", &FitHeight);
    ImGui::Checkbox("fit Width", &FitWidth);
  }
  {
    const auto pop = glengine::ImGuiPushId();
    if (ImGui::SliderFloat2(
          "View Offset", &view_offset.x, -window_width, window_width))
    {
      view_offset.y = std::clamp(view_offset.y, -window_height, window_height);
    }
  }
  {
    const auto pop_3 = glengine::ImGuiPushId();
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
  ImGui::Text("%s", fmt::format("Total Draws: {}", test::DrawCount).c_str());
  ImGui::Separator();
  m_imgui_viewport_window.on_im_gui_update();
}
void test::TestBatchQuads::draw() const
{
  if (std::ranges::empty(Vertices))
  {
    return;
  }
  m_blank.bind(0);
  m_shader.set_uniform("u_Textures", std::array{ 0 });
  glengine::Renderer::Draw(index_buffer_size, m_vertex_array, m_index_buffer);
  ++DrawCount;
}
void test::TestBatchQuads::on_event(const glengine::event::Item &) const {}
void test::TestBatchQuads::set_uniforms() const
{
  const glm::mat4 mvp = [&]() {
    if (Preview)
    {
      return m_imgui_viewport_window.preview_view_projection_matrix();
    }
    return m_imgui_viewport_window.view_projection_matrix();
  }();
  m_shader.bind();
  m_shader.set_uniform("u_MVP", glm::translate(mvp, view_offset));
  m_shader.set_uniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}
