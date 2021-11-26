//
// Created by pcvii on 11/25/2021.
//

#ifndef MYPROJECT_VERTEX_HPP
#define MYPROJECT_VERTEX_HPP
#include "VertexBufferLayout.hpp"
#include <array>
#include <glm/glm.hpp>
#include <vector>
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
  static VertexBufferLayout
    Layout()
  {
    return { VertexBufferLayout::VertexBufferElementType<float>{ 2U },
             VertexBufferLayout::VertexBufferElementType<float>{ 4U },
             VertexBufferLayout::VertexBufferElementType<float>{ 2U },
             VertexBufferLayout::VertexBufferElementType<float>{ 1U } };
  }
};
using Quad = std::array<Vertex, 4U>;
constexpr inline Quad
  CreateQuad(glm::vec2 offset, glm::vec4 color, int texture_id = {})
{
  return {
    Vertex{ glm::vec2{ -0.5F, -0.5F } + offset,
            color,
            { 0.F, 0.F },
            static_cast<float>(texture_id) },// 0
    Vertex{ glm::vec2{ 0.5F, -0.5F } + offset,
            color,
            { 1.F, 0.F },
            static_cast<float>(texture_id) },// 1
    Vertex{ glm::vec2{ 0.5F, 0.5F } + offset,
            color,
            { 1.F, 1.F },
            static_cast<float>(texture_id) },// 2
    Vertex{ glm::vec2{ -0.5F, 0.5F } + offset,
            color,
            { 0.F, 1.F },
            static_cast<float>(texture_id) },// 3
  };
}
inline std::vector<Vertex> &
  operator+=(std::vector<Vertex> &vertices, const Quad &quad)
{
  vertices.insert(std::end(vertices), std::begin(quad), std::end(quad));
  return vertices;
}
inline std::vector<Vertex> &
  operator+=(
    std::vector<Vertex>       &vertices_left,
    const std::vector<Vertex> &vertices_right)
{
  vertices_left.insert(
    std::end(vertices_left),
    std::begin(vertices_right),
    std::end(vertices_right));
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
