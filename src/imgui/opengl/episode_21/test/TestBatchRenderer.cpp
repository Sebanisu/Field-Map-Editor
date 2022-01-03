//
// Created by pcvii on 11/30/2021.
//
#include "TestBatchRenderer.hpp"

void test::TestBatchRenderer::GenerateQuads() const
{
  m_batch_renderer.Clear();
  auto x_rng = std::views::iota(uint32_t{}, static_cast<uint32_t>(m_count[0]));
  auto y_rng = std::views::iota(uint32_t{}, static_cast<uint32_t>(m_count[1]));
  auto text_index_rng =
    std::views::iota(uint32_t{}) | std::views::transform([this](const auto i) {
      return i % std::ranges::size(m_textures);
    });
  auto text_index = text_index_rng.begin();
  for (const auto x : x_rng)
  {
    for (const auto y : y_rng)
    {
      float r = static_cast<float>(x) / static_cast<float>(m_count[0]);
      float g = static_cast<float>(y) / static_cast<float>(m_count[1]);

      m_batch_renderer.DrawQuad(
        { static_cast<float>(x), static_cast<float>(y), 0.F },
        { r, g, 1.F, 1.F },
        m_textures.at(*text_index),
        static_cast<float>(1U << (*text_index)));
      ++text_index;
    }
  }
  m_batch_renderer.Draw();
}

test::TestBatchRenderer::TestBatchRenderer()
  : m_textures([]() {
    std::vector<Texture> r{};
    r.emplace_back(
      std::filesystem::current_path() / "res" / "textures" / "logo.png");
    r.emplace_back(
      std::filesystem::current_path() / "res" / "textures"
      / "mitchell-luo-q9ZiOzsMAhE-unsplash.png");
    r.emplace_back(
      std::filesystem::current_path() / "res" / "textures"
      / "math-yDq60_c-g2E-unsplash.png");
    return r;
  }())
{
}

void test::TestBatchRenderer::OnUpdate(const float ts) const
{
  glengine::OnUpdate(m_batch_renderer, ts);
}

void test::TestBatchRenderer::OnRender() const
{
  SetUniforms();
  // OnRender(m_batch_renderer);
  GenerateQuads();
}
void test::TestBatchRenderer::OnImGuiUpdate() const
{
  int window_width = 16;
  int id           = 0;

  {
    const auto pop = glengine::scope_guard(&ImGui::PopID);
    ImGui::PushID(++id);
    if (ImGui::SliderFloat2(
          "View Offset",
          &view_offset.x,
          -static_cast<float>(window_width),
          static_cast<float>(window_width)))
    {
    }
  }
  {
    const auto pop = glengine::scope_guard{ &ImGui::PopID };
    ImGui::PushID(++id);
    if (ImGui::SliderFloat("Zoom", &m_zoom, 4.F, .001F))
    {
    }
  }
  {
    const auto pop = glengine::scope_guard{ &ImGui::PopID };
    ImGui::PushID(++id);
    if (ImGui::SliderInt2("Quad Axis Count (X, Y)", std::data(m_count), 0, 256))
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
  glengine::OnImGuiUpdate(m_batch_renderer);
}
void test::TestBatchRenderer::SetUniforms() const
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
  m_batch_renderer.Shader().Bind();
  m_batch_renderer.Shader().SetUniform("u_MVP", mvp);
  m_batch_renderer.Shader().SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}