//
// Created by pcvii on 11/29/2021.
//
#include "TestTexture2D.hpp"
#include "Renderer.hpp"
#include "scope_guard.hpp"
#include "Shader.hpp"
#include "Test.h"
#include "Texture.hpp"
#include <array>
#include <imgui.h>
static_assert(test::Test<test::TestTexture2D>);
namespace test
{
static const Renderer renderer{};
}
test::TestTexture2D::TestTexture2D()
  : m_vertex_buffer{ std::array{
    // clang-format off
  -50.0F,-50.0F, 0.F, 0.F, // 0
  50.0F,-50.0F, 1.F, 0.F, // 1
  50.0F, 50.0F, 1.F, 1.F, // 2
  -50.0F, 50.0F, 0.F, 1.F, // 3
    // clang-format on
  } }
  , m_index_buffer{ std::array{
      // clang-format off
    0U, 1U, 2U, // 0
    2U, 3U, 0U  // 1
      // clang-format on
    } }
  , m_vertex_buffer_layout{
    VertexBufferLayout::VertexBufferElementType<float>{2U},
    VertexBufferLayout::VertexBufferElementType<float>{2U},
  }
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
void
  test::OnImGuiRender(const TestTexture2D &self)
{
  int        id           = 0;
  const auto pop          = scope_guard(&ImGui::PopID);
  const auto pop2         = pop;
  const auto pop3         = pop;
  int        window_width = 1280;
  // glfwGetFramebufferSize(window, &window_width, &window_height);

  ImGui::PushID(++id);
  if (ImGui::SliderFloat3(
        "View Offset",
        &self.view_offset.x,
        0.F,
        static_cast<float>(window_width)))
  {
  }
  ImGui::PushID(++id);
  if (ImGui::SliderFloat3(
        "Model 1 Offset",
        &self.model_offset.x,
        0.F,
        static_cast<float>(window_width)))
  {
  }
  ImGui::PushID(++id);
  if (ImGui::SliderFloat3(
        "Model 2 Offset",
        &self.model2_offset.x,
        0.F,
        static_cast<float>(window_width)))
  {
  }
}
void
  test::OnRender(const TestTexture2D &self)
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
  const auto view = glm::translate(glm::mat4{ 1.F }, self.view_offset);
  self.m_shader.Bind();
  {
    const auto model = glm::translate(glm::mat4{ 1.F }, self.model_offset);
    const auto mvp   = proj * view * model;
    self.m_shader.SetUniform("u_MVP", mvp);
    renderer.Draw(self.m_vertex_array, self.m_index_buffer, self.m_texture);
  }
  {
    const auto model = glm::translate(glm::mat4{ 1.F }, self.model2_offset);
    const auto mvp   = proj * view * model;
    self.m_shader.SetUniform("u_MVP", mvp);
    renderer.Draw(self.m_vertex_array, self.m_index_buffer, self.m_texture);
  }
}

void
  test::OnUpdate(const TestTexture2D &, float)
{
}