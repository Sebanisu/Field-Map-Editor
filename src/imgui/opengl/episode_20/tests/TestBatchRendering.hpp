//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TESTBATCHRENDERING_HPP
#define MYPROJECT_TESTBATCHRENDERING_HPP
#include "IndexBuffer.hpp"
#include "Test.h"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include <algorithm>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ranges>
namespace test
{
class TestBatchRendering
{
private:
  static VertexBufferLayout m_vector_buffer_layout public : struct Vertex
  {
    glm::vec2 location{};
    constexpr Vertex() = default;
    constexpr Vertex(float x, float y)
      : location(x, y)
    {
    }
  };
  TestBatchRendering()
    : m_vertex_buffer_layout{
      VertexBufferLayout::VertexBufferElementType<float>{ 2U }
    }
  {
  }

  void
    OnUpdate(float)
  {
    constexpr auto vertices_init = std::array{
      Vertex{ -0.5F, -0.5F },// 0
      Vertex{ 0.5F, -0.5F },// 1
      Vertex{ 0.5F, 0.5F },// 2
      Vertex{ -0.5F, 0.5F },// 3
    };
    [[maybe_unused]] constexpr auto indices_init = std::array{
      // clang-format off
        0U, 1U, 2U, // 0
        2U, 3U, 0U  // 1
      // clang-format on
    };
    auto vertices = std::vector(vertices_init.begin(), vertices_init.end());
    auto translate =
      glm::translate(glm::mat4{ 1.F }, glm::vec3{ 2.F, 0.F, 0.F });
    std::ranges::transform(
      vertices_init.cbegin(),
      vertices_init.cend(),
      std::back_inserter(vertices),
      [&translate](Vertex vertex)
      {
        vertex.location = translate * glm::vec4(vertex.location, 0.F, 0.F);
        return vertex;
      });

    auto       indices = std::vector(indices_init.begin(), indices_init.end());
    const auto quad_count = std::size(vertices) / std::size(vertices_init);
    for (std::size_t i = 1U; i != quad_count; ++i)
      std::ranges::transform(
        indices_init.cbegin(),
        indices_init.cend(),
        std::back_inserter(indices),
        [&vertices_init, &i](std::uint32_t index)
        { return index + (std::size(vertices_init) * i); });
    m_index_buffer = IndexBuffer{};
  }
  void
    OnRender()
  {
  }
  void
    OnImGuiRender()
  {
  }
  mutable VertexBuffer       m_vertex_buffer        = {};
  mutable VertexBufferLayout m_vertex_buffer_layout = {};
  mutable IndexBuffer        m_index_buffer         = {};
};
static_assert(Test<TestBatchRendering>);
}// namespace test
#endif// MYPROJECT_TESTBATCHRENDERING_HPP
