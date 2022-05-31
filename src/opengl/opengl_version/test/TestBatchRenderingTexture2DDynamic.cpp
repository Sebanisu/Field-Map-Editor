//
// Created by pcvii on 11/29/2021.
//
#include "TestBatchRenderingTexture2DDynamic.hpp"
#include "Application.hpp"
#include "ImGuiPushID.hpp"
#include "Renderer.hpp"
#include "Vertex.hpp"
static constinit bool fit_width  = true;
static constinit bool fit_height = true;
static constinit bool preview    = false;
static_assert(glengine::Renderable<test::TestBatchRenderingTexture2DDynamic>);
test::TestBatchRenderingTexture2DDynamic::TestBatchRenderingTexture2DDynamic()
  : m_shader(
    std::filesystem::current_path() / "res" / "shader" / "basic3.shader")
{
  m_textures.emplace_back(
    std::filesystem::current_path() / "res" / "textures"
    / "mitchell-luo-q9ZiOzsMAhE-unsplash.png");
  m_textures.emplace_back(
    std::filesystem::current_path() / "res" / "textures" / "logo.png");
  m_textures.emplace_back(
    std::filesystem::current_path() / "res" / "textures"
    / "math-yDq60_c-g2E-unsplash.png");

  m_vertex_array.Bind();
  m_vertex_array.push_back(m_vertex_buffer, Vertex::Layout());
  m_shader.Bind();
  m_shader.SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}
void test::TestBatchRenderingTexture2DDynamic::OnUpdate(float ts) const
{
  constexpr auto      colors = std::array{ glm::vec4{ 1.F, 0.F, 0.F, 1.F },
                                      glm::vec4{ 0.F, 1.F, 0.F, 1.F },
                                      glm::vec4{ 0.F, 0.F, 1.F, 1.F } };
  std::vector<Vertex> vertices{};
  vertices.reserve(12U);
  const auto offset = glm::vec3{ -0.5F, -0.5F, 0.F };
  vertices += CreateQuad(model_offset1 + offset, colors[0], 1)
              + CreateQuad(model_offset2 + offset, colors[1], 2)
              + CreateQuad(model_offset3 + offset, colors[2], 3);
  m_imgui_viewport_window.OnUpdate(ts);
  m_imgui_viewport_window.Fit(fit_width, fit_height);
  index_buffer_size = m_vertex_buffer.Update(vertices);
}
void test::TestBatchRenderingTexture2DDynamic::OnRender() const
{
  SetUniforms();
  m_imgui_viewport_window.OnRender([this]() { RenderFrameBuffer(); });
  GetViewPortPreview().OnRender(m_imgui_viewport_window.HasHover(), [this]() {
    preview = true;
    SetUniforms();
    RenderFrameBuffer();
    preview = false;
  });
}
void test::TestBatchRenderingTexture2DDynamic::OnImGuiUpdate() const
{

  const float window_width = 16.F;
  const float window_height =
    (window_width / m_imgui_viewport_window.ViewPortAspectRatio());
  m_imgui_viewport_window.SetImageBounds({ window_width, window_height });
  const float clamp_width  = window_width / 2.F - 1.F;
  const float clamp_height = window_height / 2.F - 1.F;
  {
    const auto pop = glengine::ImGuiPushID();

    ImGui::Checkbox("Fit Height", &fit_height);
    ImGui::Checkbox("Fit Width", &fit_width);
  }
  {
    const auto pop = glengine::ImGuiPushID();
    if (ImGui::SliderFloat2(
          "View Offset", &view_offset.x, -clamp_width, clamp_width))
    {
      view_offset.y = std::clamp(view_offset.y, -clamp_height, clamp_height);
    }
  }
  {
    const auto pop2 = glengine::ImGuiPushID();
    if (ImGui::SliderFloat2(
          "Model Offset 1", &model_offset1.x, -clamp_width, clamp_width))
    {
      model_offset1.y =
        std::clamp(model_offset1.y, -clamp_height, clamp_height);
    }
  }
  {
    const auto pop3 = glengine::ImGuiPushID();
    if (ImGui::SliderFloat2(
          "Model Offset 2", &model_offset2.x, -clamp_width, clamp_width))
    {
      model_offset2.y =
        std::clamp(model_offset2.y, -clamp_height, clamp_height);
    }
  }
  {
    const auto pop4 = glengine::ImGuiPushID();
    if (ImGui::SliderFloat2(
          "Model Offset 3", &model_offset3.x, -clamp_width, clamp_width))
    {
      model_offset3.y =
        std::clamp(model_offset3.y, -clamp_height, clamp_height);
    }
  }
  ImGui::Separator();
  m_imgui_viewport_window.OnImGuiUpdate();
}
void test::TestBatchRenderingTexture2DDynamic::RenderFrameBuffer() const
{

  //const auto view = glm::translate(glm::mat4{ 1.F }, view_offset);
  {
    std::vector<std::int32_t> slots{ 0 };
    slots.reserve(std::size(m_textures) + 1U);
    for (std::int32_t i{}; auto &texture : m_textures)
    {
      texture.Bind(slots.emplace_back(1 + i));
      ++i;
    }
    m_shader.SetUniform("u_Textures", slots);
    glengine::Renderer::Draw(index_buffer_size, m_vertex_array, m_index_buffer);
  }
}
void test::TestBatchRenderingTexture2DDynamic::SetUniforms() const
{
  const glm::mat4 mvp = [&]() {
    if (preview)
    {
      return m_imgui_viewport_window.PreviewViewProjectionMatrix();
    }
    return m_imgui_viewport_window.ViewProjectionMatrix();
  }();

  m_shader.Bind();
  m_shader.SetUniform("u_MVP", glm::translate(mvp, view_offset));
  m_shader.SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}
