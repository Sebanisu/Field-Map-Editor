//
// Created by pcvii on 11/29/2021.
//

#include "TestBatchQuads.hpp"
#include "Application.hpp"
#include "ImGuiPushID.hpp"
#include "Renderer.hpp"
#include "Vertex.hpp"

static constinit bool fit_width  = true;
static constinit bool fit_height = true;
static constinit bool preview    = false;

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
static std::vector<Vertex> vertices = []() {
  std::vector<Vertex> r{};
  r.reserve(TestBatchQuads::VERT_COUNT);
  return r;
}();
static std::uint32_t draw_count = 0U;
}// namespace test
void test::TestBatchQuads::on_update(float ts) const
{
  draw_count = 0U;
  m_imgui_viewport_window.set_image_bounds({ m_count[0], m_count[1] });
  m_imgui_viewport_window.on_update(ts);
  m_imgui_viewport_window.fit(fit_width, fit_height);
}
void test::TestBatchQuads::GenerateQuads() const
{
  const auto flush = [this]() {
    index_buffer_size = m_vertex_buffer.update(vertices);
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
        { static_cast<float>(x) - static_cast<float>(m_count[0]) / 2.F,
          static_cast<float>(y) - static_cast<float>(m_count[1]) / 2.F,
          0.F },
        { r, g, 1.F, 1.F },
        0);
      if (std::ranges::size(vertices) == max_count)
      {
        flush();
      }
    }
  }

  index_buffer_size = m_vertex_buffer.update(vertices);
}
void test::TestBatchQuads::on_render() const
{
  set_uniforms();
  m_imgui_viewport_window.on_render([&]() { GenerateQuads(); });
  GetViewPortPreview().on_render(m_imgui_viewport_window, [this]() {
    preview = true;
    set_uniforms();
    GenerateQuads();
    preview = false;
  });
}
void test::TestBatchQuads::on_im_gui_update() const
{
  static constexpr float window_width = 100.F;
  const float            window_height =
    window_width / m_imgui_viewport_window.view_port_aspect_ratio();
  {
    const auto pop = glengine::ImGuiPushId();

    ImGui::Checkbox("fit Height", &fit_height);
    ImGui::Checkbox("fit Width", &fit_width);
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
    const auto pop3 = glengine::ImGuiPushId();
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
  ImGui::Separator();
  m_imgui_viewport_window.on_im_gui_update();
}
void test::TestBatchQuads::Draw() const
{
  if (std::ranges::empty(vertices))
  {
    return;
  }
  m_blank.bind(0);
  m_shader.set_uniform("u_Textures", std::array{ 0 });
  glengine::Renderer::Draw(index_buffer_size, m_vertex_array, m_index_buffer);
  ++draw_count;
}
void test::TestBatchQuads::on_event(const glengine::event::Item &) const {}
void test::TestBatchQuads::set_uniforms() const
{
  const glm::mat4 mvp = [&]() {
    if (preview)
    {
      return m_imgui_viewport_window.preview_view_projection_matrix();
    }
    return m_imgui_viewport_window.view_projection_matrix();
  }();
  m_shader.bind();
  m_shader.set_uniform("u_MVP", glm::translate(mvp, view_offset));
  m_shader.set_uniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}
