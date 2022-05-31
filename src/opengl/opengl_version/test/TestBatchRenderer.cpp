//
// Created by pcvii on 11/30/2021.
//
#include "TestBatchRenderer.hpp"
#include "Application.hpp"
#include "ImGuiPushID.hpp"
static constinit bool fit_width = true;
static constinit bool fit_height = true;
static constinit bool preview = false;
void test::TestBatchRenderer::GenerateQuads() const
{
  m_batch_renderer.Clear();
  auto x_rng = std::views::iota(int32_t{}, m_count[0]);
  auto y_rng = std::views::iota(int32_t{}, m_count[1]);
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
        { static_cast<float>(x) - static_cast<float>(m_count[0]) / 2.F,
          static_cast<float>(y) - static_cast<float>(m_count[1]) / 2.F,
          0.F },
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
    std::vector<glengine::Texture> r{};
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
  m_batch_renderer.OnUpdate(ts);
  m_imgui_viewport_window.SetImageBounds({ m_count[0], m_count[1] });
  m_imgui_viewport_window.OnUpdate(ts);
  m_imgui_viewport_window.Fit(fit_width, fit_height);
}

void test::TestBatchRenderer::OnRender() const
{
  SetUniforms();
  m_imgui_viewport_window.OnRender([this]() { GenerateQuads(); });
  GetViewPortPreview().OnRender(m_imgui_viewport_window.HasHover(), [this]() {
    preview = true;
    SetUniforms();
    GenerateQuads();
    preview = false;
  });
}
void test::TestBatchRenderer::OnImGuiUpdate() const
{
  const float window_width = 100.F;
  float       window_height =
    window_width / m_imgui_viewport_window.ViewPortAspectRatio();
  {
    const auto pop = glengine::ImGuiPushID();

    ImGui::Checkbox("Fit Height", &fit_height);
    ImGui::Checkbox("Fit Width", &fit_width);
  }
  {
    const auto pop = glengine::ImGuiPushID();
    if (ImGui::SliderFloat2(
          "View Offset", &view_offset.x, -window_width, window_width))
    {
      view_offset.y = std::clamp(view_offset.y, -window_height, window_height);
    }
  }

  {
    const auto pop = glengine::ImGuiPushID();
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
  ImGui::Separator();
  m_imgui_viewport_window.OnImGuiUpdate();
}

void test::TestBatchRenderer::SetUniforms() const
{
  const glm::mat4 mvp = [&]() {
    if (preview)
    {
      return m_imgui_viewport_window.PreviewViewProjectionMatrix();
    }
    return m_imgui_viewport_window.ViewProjectionMatrix();
  }();

  m_batch_renderer.Shader().Bind();
  m_batch_renderer.Shader().SetUniform(
    "u_MVP", glm::translate(mvp, view_offset));
  m_batch_renderer.Shader().SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}