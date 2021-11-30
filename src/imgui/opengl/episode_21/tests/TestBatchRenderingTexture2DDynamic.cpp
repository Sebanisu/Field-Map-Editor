//
// Created by pcvii on 11/29/2021.
//
//#include "IndexBufferDynamic.hpp"
#include "scope_guard.hpp"
//#include "Shader.hpp"
#include "Test.h"
//#include "Texture.hpp"
#include "Vertex.hpp"
//#include "VertexArray.hpp"
//#include "VertexBufferDynamic.hpp"
//#include "VertexBufferLayout.hpp"
//#include <algorithm>
//#include <array>
//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
//#include <ranges>
#include "TestBatchRenderingTexture2DDynamic.hpp"
#include "Renderer.hpp"
static_assert(test::Test<test::TestBatchRenderingTexture2DDynamic>);
namespace test
{
static const Renderer renderer{};
}
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
void
  test::OnUpdate(const TestBatchRenderingTexture2DDynamic &self, float)
{
  constexpr auto      colors = std::array{ glm::vec4{ 1.F, 0.F, 0.F, 1.F },
                                      glm::vec4{ 0.F, 1.F, 0.F, 1.F },
                                      glm::vec4{ 0.F, 0.F, 1.F, 1.F } };


  std::vector<Vertex> vertices{};
  vertices.reserve(12U);
  vertices += CreateQuad(self.model_offset1, colors[0], 1)
              + CreateQuad(self.model_offset2, colors[1], 2)
              + CreateQuad(self.model_offset3, colors[2], 3);

  self.index_buffer_size = self.m_vertex_buffer.Update(vertices);
}
void
  test::OnRender(const TestBatchRenderingTexture2DDynamic &self)
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
  const auto view = glm::translate(glm::mat4{ 1.F }, self.view_offset);
  {
    const auto mvp = proj * view;
    self.m_shader.Bind();
    self.m_shader.SetUniform("u_MVP", mvp);
    self.m_shader.SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
    std::vector<std::int32_t> slots{ 0 };
    slots.reserve(std::size(self.m_textures) + 1U);
    for (std::int32_t i{}; auto &texture : self.m_textures)
    {
      texture.Bind(slots.emplace_back(1 + i));
      ++i;
    }
    self.m_shader.SetUniform("u_Textures", slots);
    renderer.Draw(
      self.index_buffer_size, self.m_vertex_array, self.m_index_buffer);
  }
}
void
  test::OnImGuiRender(const TestBatchRenderingTexture2DDynamic &self)
{
  int        id           = 0;

  int        window_width = 16;
  const auto pop          = scope_guard(&ImGui::PopID);
  ImGui::PushID(++id);
  if (ImGui::SliderFloat3(
        "View Offset",
        &self.view_offset.x,
        0.F,
        static_cast<float>(window_width)))
  {
  }

  const auto pop2 = pop;
  ImGui::PushID(++id);
  if (ImGui::SliderFloat2(
        "Model Offset",
        &self.model_offset1.x,
        0.F,
        static_cast<float>(window_width)))
  {
  }

  const auto pop3 = pop;
  ImGui::PushID(++id);
  if (ImGui::SliderFloat2(
        "Model Offset",
        &self.model_offset2.x,
        0.F,
        static_cast<float>(window_width)))
  {
  }

  const auto pop4 = pop;
  ImGui::PushID(++id);
  if (ImGui::SliderFloat2(
        "Model Offset",
        &self.model_offset3.x,
        0.F,
        static_cast<float>(window_width)))
  {
  }
}