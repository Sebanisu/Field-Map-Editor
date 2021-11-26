//
// Created by pcvii on 11/24/2021.
//
#include "TestBatchRendering.hpp"
void
  test::TestBatchRendering::OnImGuiRender() const
{
    int        id           = 0;
    const auto pop          = scope_guard(&ImGui::PopID);
    const auto pop2         = pop;
    int        window_width = 16;
    // glfwGetFramebufferSize(window, &window_width, &window_height);

    ImGui::PushID(++id);
    if (ImGui::SliderFloat3(
            "View Offset", &view_offset.x, 0.F, static_cast<float>(window_width)))
    {
    }
    ImGui::PushID(++id);
    if (ImGui::SliderFloat3(
            "Model Offset",
            &model_offset.x,
            0.F,
            static_cast<float>(window_width)))
    {
    }
}
void
  test::TestBatchRendering::OnUpdate(float) const
{
}
void
  test::TestBatchRendering::OnRender() const
{
  const int window_width  = 16;
  const int window_height = 9;
  auto      proj          = glm::ortho(
    0.F,
    static_cast<float>(window_width),
    0.F,
    static_cast<float>(window_height),
    -1.F,
    1.F);
  const auto view = glm::translate(glm::mat4{ 1.F }, view_offset);
  Renderer   renderer{};
  {
    const auto model = glm::translate(glm::mat4{ 1.F }, model_offset);
    const auto mvp   = proj * view * model;
    m_shader.SetUniform("u_MVP", mvp);
    m_shader.SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
    // m_shader.SetUniform("u_Texture", 0);
    renderer.Draw(m_vertex_array, m_index_buffer, m_shader);
  }
  //    {
  //      const auto model = glm::translate(glm::mat4{ 1.F }, model2_offset);
  //      const auto mvp   = proj * view * model;
  //      m_shader.SetUniform("u_MVP", mvp);
  //      renderer.Draw(m_vertex_array, m_index_buffer, m_shader, m_texture);
  //    }
}
