//
// Created by pcvii on 11/25/2021.
//

#ifndef MYPROJECT_VERTEX_HPP
#define MYPROJECT_VERTEX_HPP
#include "VertexBufferLayout.hpp"
#include <algorithm>
#include <array>
#include <glm/glm.hpp>
#include <iterator>
#include <ranges>
#include <vector>
struct Vertex
{
  glm::vec2 location{};
  glm::vec4 color{};
  glm::vec2 uv{};
  float     texture_slot{};
  constexpr Vertex() = default;
  constexpr Vertex(glm::vec2 in_location, glm::vec4 in_color)
    : location(std::move(in_location))
    , color(std::move(in_color))
  {
  }
  constexpr Vertex(
    glm::vec2 in_location,
    glm::vec4 in_color,
    glm::vec2 in_uv,
    int       in_texture_slot = {})
    : location(std::move(in_location))
    , color(std::move(in_color))
    , uv(in_uv)
    , texture_slot(static_cast<float>(in_texture_slot))
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
    int   in_texture_slot = {})
    : location(x, y)
    , color(r, g, b, a)
    , uv(u, v)
    , texture_slot(static_cast<float>(in_texture_slot))
  {
  }
  static VertexBufferLayout
    Layout()
  {
    return { VertexBufferLayout::VertexBufferElementType<float>{ 2U },
             VertexBufferLayout::VertexBufferElementType<float>{ 4U },
             VertexBufferLayout::VertexBufferElementType<float>{ 2U },
             VertexBufferLayout::VertexBufferElementType<float>{ 1U } };
  }
};
static_assert(
  std::movable<
    Vertex> && std::copyable<Vertex> && std::default_initializable<Vertex>);
using Quad = std::array<Vertex, 4U>;
constexpr inline Quad
  CreateQuad(
    glm::vec2 offset,
    glm::vec4 color,
    int       texture_id = {},
    float     size       = 1.F)
{
  return {
    Vertex{ offset, color, { 0.F, 0.F }, texture_id },// 0
    Vertex{
      offset + glm::vec2{ size, 0.F }, color, { 1.F, 0.F }, texture_id },// 1
    Vertex{
      offset + glm::vec2{ size, size }, color, { 1.F, 1.F }, texture_id },// 2
    Vertex{
      offset + glm::vec2{ 0.F, size }, color, { 0.F, 1.F }, texture_id },// 3
  };
}
inline std::vector<std::uint32_t>
  QuadIndices(std::size_t count)
{
  static constexpr auto      init = std::array{ 0, 1, 2, 2, 3, 0 };
  std::vector<std::uint32_t> indices{};
  indices.reserve(std::size(init) * count);
  constexpr auto quad_size = std::size(Quad{});
  for (std::size_t i{}; i != count; ++i)
  {
    std::ranges::transform(
      init,
      std::back_inserter(indices),
      [&i, &quad_size](std::uint32_t index)
      { return static_cast<std::uint32_t>(index + i * quad_size); });
  }
  return indices;
}
inline std::vector<Vertex> &
  operator+=(std::vector<Vertex> &vertices, const Quad &quad)
{
  //  vertices.insert(std::end(vertices), std::begin(quad), std::end(quad));
  std::ranges::copy(quad, std::back_inserter(vertices));
  return vertices;
}
inline std::vector<Vertex> &
  operator+=(
    std::vector<Vertex>       &vertices_left,
    const std::vector<Vertex> &vertices_right)
{
  std::ranges::copy(vertices_right, std::back_inserter(vertices_left));
  //  vertices_left.insert(
  //    std::end(vertices_left),
  //    std::begin(vertices_right),
  //    std::end(vertices_right));
  return vertices_left;
}
[[nodiscard]] inline std::vector<Vertex>
  operator+(std::vector<Vertex> vertices, const Quad &quad)
{
  vertices += quad;
  return vertices;
}
[[nodiscard]] inline std::vector<Vertex>
  operator+(const Quad &quad_left, const Quad &quad_right)
{
  std::vector<Vertex> vertices{};
  vertices.reserve(std::size(quad_left) + std::size(quad_right));
  vertices += quad_left;
  vertices += quad_right;
  return vertices;
}
[[nodiscard]] inline std::vector<Vertex>
  operator+(
    std::vector<Vertex>        vertices_left,
    const std::vector<Vertex> &vertices_right)
{
  vertices_left += vertices_right;
  return vertices_left;
}
#endif// MYPROJECT_VERTEX_HPP
