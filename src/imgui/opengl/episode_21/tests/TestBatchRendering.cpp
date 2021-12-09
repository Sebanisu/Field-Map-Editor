//
// Created by pcvii on 11/29/2021.
//
//#include "IndexBuffer.hpp"
#include "scope_guard.hpp"
//#include "Shader.hpp"
#include "Test.hpp"
#include "Vertex.hpp"
//#include "VertexArray.hpp"
//#include "VertexBuffer.hpp"
//#include "VertexBufferLayout.hpp"
#include <algorithm>
#include <array>
#include <vector>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
#include "TestBatchRendering.hpp"
#include <imgui.h>
#include <ranges>
static_assert(test::Test<test::TestBatchRendering>);
void test::TestBatchRendering::OnImGuiUpdate() const
{
  int        id           = 0;
  const auto pop          = scope_guard::array<2U>(&ImGui::PopID);
  int        window_width = 16;
  // glfwGetFramebufferSize(window, &window_width, &window_height);

  ImGui::PushID(++id);
  if (ImGui::SliderFloat3(
        "View Offset", &view_offset.x, 0.F, static_cast<float>(window_width)))
  {
  }
  ImGui::PushID(++id);
  if (ImGui::SliderFloat3(
        "Model Offset", &model_offset.x, 0.F, static_cast<float>(window_width)))
  {
  }
}
void test::TestBatchRendering::OnRender() const
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
  m_shader.Bind();
  {
    const auto model = glm::translate(glm::mat4{ 1.F }, model_offset);
    const auto mvp   = proj * view * model;
    m_shader.SetUniform("u_MVP", mvp);
    m_shader.SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
    // m_shader.SetUniform("u_Texture", 0);
    renderer.Draw(m_vertex_array, m_index_buffer);
  }
}
test::TestBatchRendering::TestBatchRendering()
  : m_shader(
    std::filesystem::current_path() / "res" / "shader" / "basic2.shader")
{
  constexpr auto colors        = std::array{ glm::vec4{ 1.F, 0.F, 0.F, 1.F },
                                      glm::vec4{ 0.F, 1.F, 0.F, 1.F },
                                      glm::vec4{ 0.F, 0.F, 1.F, 1.F } };
  constexpr auto vertices_init = std::array{
    Vertex{ { -0.5F, -0.5F }, colors[0] },// 0
    Vertex{ { 0.5F, -0.5F }, colors[0] },// 1
    Vertex{ { 0.5F, 0.5F }, colors[0] },// 2
    Vertex{ { -0.5F, 0.5F }, colors[0] },// 3
  };
  [[maybe_unused]] constexpr auto indices_init = std::array{
    // clang-format off
    0U, 1U, 2U, // 0
    2U, 3U, 0U  // 1
    // clang-format on
  };
  auto vertices = std::vector(vertices_init.begin(), vertices_init.end());
  //    auto translate =
  //      glm::translate(glm::mat4{ 1.F }, glm::vec3{ 2.F, 0.F, 0.F });
  for (std::size_t i = 1U; i != 3U; ++i)
    std::ranges::transform(
      vertices_init.cbegin(),
      vertices_init.cend(),
      std::back_inserter(vertices),
      [&colors, &i](Vertex vertex) {
        vertex.location.x += 2.F * static_cast<float>(i);
        vertex.color = colors[i];
        return vertex;
      });
  m_vertex_buffer       = VertexBuffer{ vertices };
  auto       indices    = std::vector(indices_init.begin(), indices_init.end());
  const auto quad_count = std::size(vertices) / std::size(vertices_init);
  for (std::size_t i = 1U; i != quad_count; ++i)
    std::ranges::transform(
      indices_init.cbegin(),
      indices_init.cend(),
      std::back_inserter(indices),
      [&vertices_init, &i](std::uint32_t index) {
        return static_cast<std::uint32_t>(
          index + (std::size(vertices_init) * i));
      });
  m_index_buffer = IndexBuffer{ indices };


  m_vertex_array.Bind();
  m_vertex_array.push_back(m_vertex_buffer, Vertex::Layout());
  m_shader.Bind();
  m_shader.SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
  // m_shader.SetUniform("u_Texture", 0);
}
//#include "TestBatchRendering.hpp"