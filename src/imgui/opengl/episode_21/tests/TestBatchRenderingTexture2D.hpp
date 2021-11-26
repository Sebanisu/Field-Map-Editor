//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TestBatchRenderingTexture2D_HPP
#define MYPROJECT_TestBatchRenderingTexture2D_HPP
#include "IndexBuffer.hpp"
#include "scope_guard.hpp"
#include "Shader.hpp"
#include "Test.h"
#include "Texture.hpp"
#include "Vertex.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include <algorithm>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <ranges>

namespace test
{
class TestBatchRenderingTexture2D
{
public:
  TestBatchRenderingTexture2D()
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
    constexpr auto colors = std::array{ glm::vec4{ 1.F, 0.F, 0.F, 1.F },
                                        glm::vec4{ 0.F, 1.F, 0.F, 1.F },
                                        glm::vec4{ 0.F, 0.F, 1.F, 1.F } };


    std::vector<Vertex> vertices{};
    vertices.reserve(12U);
    vertices += CreateQuad({ 2.F, 0.F }, colors[0], 1)
                + CreateQuad({ 4.F, 0.F }, colors[1], 2)
                + CreateQuad({ 6.F, 0.F }, colors[2], 3);

    m_vertex_buffer    = VertexBuffer{ vertices };
    constexpr auto quad_size         = std::size(Quad{});
    const auto quad_count = std::size(vertices) / quad_size;
    m_index_buffer = IndexBuffer{ QuadIndices(quad_count) };

    m_vertex_array.Bind();
    m_vertex_array.push_back(m_vertex_buffer, Vertex::Layout());
    m_shader.Bind();
    m_shader.SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
  }

  void
    OnUpdate(float)
  {
  }
  void
    OnRender()
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
      renderer.Draw(m_vertex_array, m_index_buffer);
    }
    //    {
    //      const auto model = glm::translate(glm::mat4{ 1.F }, model2_offset);
    //      const auto mvp   = proj * view * model;
    //      m_shader.SetUniform("u_MVP", mvp);
    //      renderer.Draw(m_vertex_array, m_index_buffer, m_shader, m_texture);
    //    }
  }
  void
    OnImGuiRender()
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

private:
  VertexBuffer         m_vertex_buffer = {};
  IndexBuffer          m_index_buffer  = {};
  Shader               m_shader        = {};
  VertexArray          m_vertex_array  = {};
  std::vector<Texture> m_textures      = {};
  mutable glm::vec3    view_offset     = { 0.F, 0.F, 0.F };
  mutable glm::vec3    model_offset    = { 0.F, 0.F, 0.F };
  // mutable glm::vec3  model2_offset          = { 400.F, 200.F, 0.F };
};
static_assert(Test<TestBatchRenderingTexture2D>);
}// namespace test
#endif// MYPROJECT_TestBatchRenderingTexture2D_HPP
