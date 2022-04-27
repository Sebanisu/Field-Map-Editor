//
// Created by pcvii on 11/29/2021.
//
#include "TestTexture2D.hpp"
#include "ImGuiPushID.hpp"
#include "Renderer.hpp"
#include "scope_guard.hpp"
static_assert(glengine::Renderable<test::TestTexture2D>);
test::TestTexture2D::TestTexture2D()
  : m_vertex_buffer{ std::array{
    // clang-format off
  -0.5F,-0.5F, 0.F, 0.F, // 0
  0.5F,-0.5F, 1.F, 0.F, // 1
  0.5F, 0.5F, 1.F, 1.F, // 2
  -0.5F, 0.5F, 0.F, 1.F, // 3
    // clang-format on
  } }
  , m_index_buffer{ std::array{
      // clang-format off
    0U, 1U, 2U, // 0
    2U, 3U, 0U  // 1
      // clang-format on
    } }
  , m_texture{ std::filesystem::current_path() / "res" / "textures"
               / "logo.png" }
  , m_shader{ std::filesystem::current_path() / "res" / "shader"
              / "basic.shader" }
{
  m_vertex_array.Bind();
  m_vertex_array.push_back(m_vertex_buffer, m_vertex_buffer_layout);
  m_shader.Bind();
  m_shader.SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
  m_shader.SetUniform("u_Texture", 0);
}
void test::TestTexture2D::OnImGuiUpdate() const
{
  float window_width = 16.F;
  float window_height =
    window_width / m_imgui_viewport_window.ViewPortAspectRatio();
  {
    const auto pop = glengine::ImGuiPushID();
    // glfwGetFramebufferSize(window, &window_width, &window_height);

    if (ImGui::SliderFloat2("View Offset", &view_offset.x, 0.F, window_width))
    {
      view_offset.y = std::clamp(view_offset.y, 0.F, window_height);
    }
  }
  {
    const auto pop2 = glengine::ImGuiPushID();
    if (ImGui::SliderFloat2(
          "Model Offset 1", &model_offset.x, 0.F, window_width))
    {
      model_offset.y = std::clamp(model_offset.y, 0.F, window_height);
    }
  }
  {
    const auto pop3 = glengine::ImGuiPushID();
    if (ImGui::SliderFloat2(
          "Model Offset 1", &model2_offset.x, 0.F, window_width))
    {
      model2_offset.y = std::clamp(model2_offset.y, 0.F, window_height);
    }
  }
}
void test::TestTexture2D::OnRender() const
{
  m_imgui_viewport_window.SyncOpenGLViewPort();
  m_imgui_viewport_window.OnRender([this]() {
    auto proj = glm::ortho(
      0.F,
      16.F,
      0.F,
      16.F / m_imgui_viewport_window.ViewPortAspectRatio(),
      -1.F,
      1.F);
    const auto view = glm::translate(glm::mat4{ 1.F }, view_offset);
    m_shader.Bind();
    {
      const auto model = glm::translate(glm::mat4{ 1.F }, model_offset);
      const auto mvp   = proj * view * model;
      m_shader.SetUniform("u_MVP", mvp);
      glengine::Renderer::Draw(m_vertex_array, m_index_buffer, m_texture);
    }
    {
      const auto model = glm::translate(glm::mat4{ 1.F }, model2_offset);
      const auto mvp   = proj * view * model;
      m_shader.SetUniform("u_MVP", mvp);
      glengine::Renderer::Draw(m_vertex_array, m_index_buffer, m_texture);
    }
  });
}