//
// Created by pcvii on 11/24/2021.
//

#include "TestTexture2D.hpp"
#include "Renderer.hpp"
#include "scope_guard.hpp"
#include <imgui.h>
namespace test
{
void
  TestTexture2D::OnRender() const
{
  int  window_width  = 1280;
  int  window_height = 720;
  auto proj          = glm::ortho(
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
    renderer.Draw(m_vertex_array, m_index_buffer, m_shader, m_texture);
  }
  {
    const auto model = glm::translate(glm::mat4{ 1.F }, model2_offset);
    const auto mvp   = proj * view * model;
    m_shader.SetUniform("u_MVP", mvp);
    renderer.Draw(m_vertex_array, m_index_buffer, m_shader, m_texture);
  }
}
void
  TestTexture2D::OnImGuiRender() const
{
  int        id            = 0;
  const auto pop           = scope_guard(&ImGui::PopID);
  const auto pop2          = pop;
  const auto pop3          = pop;
  int        window_width  = 1280;
  // glfwGetFramebufferSize(window, &window_width, &window_height);

  ImGui::PushID(++id);
  if (ImGui::SliderFloat3(
        "View Offset", &view_offset.x, 0.F, static_cast<float>(window_width)))
  {
  }
  ImGui::PushID(++id);
  if (ImGui::SliderFloat3(
        "Model 1 Offset",
        &model_offset.x,
        0.F,
        static_cast<float>(window_width)))
  {
  }
  ImGui::PushID(++id);
  if (ImGui::SliderFloat3(
        "Model 2 Offset",
        &model2_offset.x,
        0.F,
        static_cast<float>(window_width)))
  {
  }
}

}// namespace test