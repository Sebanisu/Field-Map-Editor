//
// Created by pcvii on 11/29/2021.
//
#include "TestTexture2D.hpp"
#include "Application.hpp"
#include "ImGuiPushID.hpp"
#include "Renderer.hpp"
static_assert(glengine::Renderable<test::TestTexture2D>);
static constinit bool Preview = { false };
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
  m_vertex_array.bind();
  m_vertex_array.push_back(m_vertex_buffer, m_vertex_buffer_layout);
  m_shader.bind();
  m_shader.set_uniform("u_Color", 1.F, 1.F, 1.F, 1.F);
  m_shader.set_uniform("u_Texture", 0);
}
void test::TestTexture2D::on_im_gui_update() const
{
  constexpr float window_width = 16.F;
  float           window_height =
    window_width / m_imgui_viewport_window.view_port_aspect_ratio();
  m_imgui_viewport_window.set_image_bounds({ window_width, window_height });
  constexpr float clamp_width  = window_width / 2.F - 1.F;
  const float     clamp_height = window_height / 2.F - 1.F;
  {
    const auto pop = glengine::ImGuiPushId();
    // glfwGetFramebufferSize(window, &window_width, &window_height);

    if (ImGui::SliderFloat2(
          "View Offset", &view_offset.x, -clamp_width, clamp_width))
    {
      view_offset.y = std::clamp(view_offset.y, -clamp_height, clamp_height);
    }
  }
  {
    const auto pop_2 = glengine::ImGuiPushId();
    if (ImGui::SliderFloat2(
          "Model Offset 1", &model_offset.x, -clamp_width, clamp_width))
    {
      model_offset.y = std::clamp(model_offset.y, -clamp_height, clamp_height);
    }
  }
  {
    const auto pop_3 = glengine::ImGuiPushId();
    if (ImGui::SliderFloat2(
          "Model Offset 1", &model_2_offset.x, -clamp_width, clamp_width))
    {
      model_2_offset.y =
        std::clamp(model_2_offset.y, -clamp_height, clamp_height);
    }
  }
}

void test::TestTexture2D::render_frame_buffer() const
{
  const glm::mat4 proj = [&]() {
    if (Preview)
    {
      return m_imgui_viewport_window.preview_view_projection_matrix();
    }
    return m_imgui_viewport_window.view_projection_matrix();
  }();
  const auto view = glm::translate(glm::mat4{ 1.F }, view_offset);
  m_shader.bind();
  {
    const auto model = glm::translate(glm::mat4{ 1.F }, model_offset);
    const auto mvp   = proj * view * model;
    m_shader.set_uniform("u_MVP", mvp);
    glengine::Renderer::Draw(m_vertex_array, m_index_buffer, m_texture);
  }
  {
    const auto model = glm::translate(glm::mat4{ 1.F }, model_2_offset);
    const auto mvp   = proj * view * model;
    m_shader.set_uniform("u_MVP", mvp);
    glengine::Renderer::Draw(m_vertex_array, m_index_buffer, m_texture);
  }
}
void test::TestTexture2D::on_render() const
{
  m_imgui_viewport_window.on_render([this]() { render_frame_buffer(); });
  GetViewPortPreview().on_render(m_imgui_viewport_window, [this]() {
    const auto pop_preview = glengine::ScopeGuard([]() { Preview = false; });
    Preview                = true;
    render_frame_buffer();
  });
}