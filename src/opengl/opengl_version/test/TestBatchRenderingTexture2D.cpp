//
// Created by pcvii on 11/29/2021.
//
#include "TestBatchRenderingTexture2D.hpp"
#include "ImGuiPushID.hpp"
#include "Vertex.hpp"

static_assert(glengine::Renderable<test::TestBatchRenderingTexture2D>);
test::TestBatchRenderingTexture2D::TestBatchRenderingTexture2D()
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
  constexpr auto      colors = std::array{ glm::vec4{ 1.F, 0.F, 0.F, 1.F },
                                      glm::vec4{ 0.F, 1.F, 0.F, 1.F },
                                      glm::vec4{ 0.F, 0.F, 1.F, 1.F } };


  std::vector<Vertex> vertices{};
  vertices.reserve(12U);
  vertices += CreateQuad({ 0.F, 0.F, 0.F }, colors[0], 1)
              + CreateQuad({ 2.F, 0.F, 0.F }, colors[1], 2)
              + CreateQuad({ 4.F, 0.F, 0.F }, colors[2], 3);

  m_vertex_buffer           = glengine::VertexBuffer{ vertices };
  constexpr auto quad_size  = std::size(Quad{});
  const auto     quad_count = std::size(vertices) / quad_size;
  m_index_buffer            = glengine::IndexBuffer{ QuadIndices(quad_count) };

  m_vertex_array.Bind();
  m_vertex_array.push_back(m_vertex_buffer, Vertex::Layout());
  m_shader.Bind();
  m_shader.SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}
void test::TestBatchRenderingTexture2D::OnRender() const
{
  m_imgui_viewport_window.SyncOpenGLViewPort();
  m_imgui_viewport_window.OnRender([this]() {
    static constexpr float window_width = 16.F;
    const float            window_height =
      window_width / m_imgui_viewport_window.ViewPortAspectRatio();
    auto proj = glm::ortho(
      0.F, window_width, 0.F, static_cast<float>(window_height), -1.F, 1.F);
    const auto view = glm::translate(glm::mat4{ 1.F }, view_offset);
    {
      const auto model = glm::translate(glm::mat4{ 1.F }, model_offset);
      const auto mvp   = proj * view * model;
      m_shader.Bind();
      m_shader.SetUniform("u_MVP", mvp);
      m_shader.SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
      std::vector<std::int32_t> slots{ 0 };
      slots.reserve(std::size(m_textures) + 1U);
      for (std::int32_t i{}; auto &texture : m_textures)
      {
        texture.Bind(slots.emplace_back(1 + i));
        ++i;
      }
      m_shader.SetUniform("u_Textures", slots);
      glengine::Renderer::Draw(m_vertex_array, m_index_buffer);
    }
  });
}
void test::TestBatchRenderingTexture2D::OnImGuiUpdate() const
{
  static constexpr float window_width = 15.F;
  const float            window_height =
    window_width / m_imgui_viewport_window.ViewPortAspectRatio();
  {
    const auto pop = glengine::ImGuiPushID();
    if (ImGui::SliderFloat3("View Offset", &view_offset.x, 0.F, window_width))
    {
      view_offset.y = std::clamp(view_offset.y, 0.F, window_height);
    }
  }
  {
    const auto pop = glengine::ImGuiPushID();
    if (ImGui::SliderFloat3("Model Offset", &model_offset.x, 0.F, window_width))
    {
      model_offset.y = std::clamp(model_offset.y, 0.F, window_height);
    }
  }
}