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
  struct Vertex
  {
    glm::vec2 location{};
    glm::vec4 color{};
    glm::vec2 uv{};
    float     texture_slot;
    constexpr Vertex() = default;
    constexpr Vertex(
      glm::vec2 in_location,
      glm::vec4 in_color,
      glm::vec2 in_uv,
      float     in_texture_slot = {})
      : location(std::move(in_location))
      , color(std::move(in_color))
      , uv(in_uv)
      , texture_slot(in_texture_slot)
    {
    }
    constexpr Vertex(
      float x,
      float y,
      float r,
      float g,
      float b,
      float a,
      float u,
      float v,
      float in_texture_slot = {})
      : location(x, y)
      , color(r, g, b, a)
      , uv(u, v)
      , texture_slot(in_texture_slot)
    {
    }
  };
  TestBatchRenderingTexture2D()
    : m_vertex_buffer_layout(
      VertexBufferLayout::VertexBufferElementType<float>{ 2U },
      VertexBufferLayout::VertexBufferElementType<float>{ 4U },
      VertexBufferLayout::VertexBufferElementType<float>{ 2U },
      VertexBufferLayout::VertexBufferElementType<float>{ 1U })
    , m_shader(
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
    constexpr auto colors        = std::array{ glm::vec4{ 1.F, 0.F, 0.F, 1.F },
                                        glm::vec4{ 0.F, 1.F, 0.F, 1.F },
                                        glm::vec4{ 0.F, 0.F, 1.F, 1.F } };
    constexpr auto vertices_init = std::array{
      Vertex{ { -0.5F, -0.5F }, colors[0], { 0.F, 0.F }, 1.F },// 0
      Vertex{ { 0.5F, -0.5F }, colors[0], { 1.F, 0.F }, 1.F },// 1
      Vertex{ { 0.5F, 0.5F }, colors[0], { 1.F, 1.F }, 1.F },// 2
      Vertex{ { -0.5F, 0.5F }, colors[0], { 0.F, 1.F }, 1.F },// 3
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
        [&colors, &i](Vertex vertex)
        {
          vertex.location.x += 2.F * static_cast<float>(i);
          vertex.color = colors[i];
          vertex.texture_slot += 1.F * static_cast<float>(i);
          return vertex;
        });
    m_vertex_buffer    = VertexBuffer{ vertices };
    auto       indices = std::vector(indices_init.begin(), indices_init.end());
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
    m_vertex_array.push_back(m_vertex_buffer, m_vertex_buffer_layout);
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
      std::vector<std::int32_t> slots{0};
      slots.reserve(std::size(m_textures)+1U);
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
  VertexBuffer         m_vertex_buffer        = {};
  VertexBufferLayout   m_vertex_buffer_layout = {};
  IndexBuffer          m_index_buffer         = {};
  Shader               m_shader               = {};
  VertexArray          m_vertex_array         = {};
  std::vector<Texture> m_textures             = {};
  mutable glm::vec3    view_offset            = { 0.F, 0.F, 0.F };
  mutable glm::vec3    model_offset           = { 0.F, 0.F, 0.F };
  // mutable glm::vec3  model2_offset          = { 400.F, 200.F, 0.F };
};
static_assert(Test<TestBatchRenderingTexture2D>);
}// namespace test
#endif// MYPROJECT_TestBatchRenderingTexture2D_HPP
