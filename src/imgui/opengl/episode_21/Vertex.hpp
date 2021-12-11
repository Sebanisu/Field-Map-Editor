//
// Created by pcvii on 11/25/2021.
//

#ifndef MYPROJECT_VERTEX_HPP
#define MYPROJECT_VERTEX_HPP
#include "VertexBufferLayout.hpp"


struct Vertex
{
  glm::vec2                 location{};
  glm::vec4                 color{ 1.F, 1.F, 1.F, 1.F };
  glm::vec2                 uv{};
  float                     texture_slot{};
  float                     tiling_factor{ 1.F };
  static VertexBufferLayout Layout()
  {
    return { VertexBufferLayout::VertexBufferElementType<float>{ 2U },
             VertexBufferLayout::VertexBufferElementType<float>{ 4U },
             VertexBufferLayout::VertexBufferElementType<float>{ 2U },
             VertexBufferLayout::VertexBufferElementType<float>{ 1U },
             VertexBufferLayout::VertexBufferElementType<float>{ 1U } };
  }
};
static_assert(
  std::movable<
    Vertex> && std::copyable<Vertex> && std::default_initializable<Vertex>);
using Quad = std::array<Vertex, 4U>;
constexpr inline Quad CreateQuad(
  const glm::vec2                 offset,
  const glm::vec4                 color,
  const int                       texture_id    = {},
  const float                     tiling_factor = 1.F,
  const std::array<glm::vec2, 4U> uv            = { glm::vec2{ 0.F, 0.F },
                                         glm::vec2{ 1.F, 0.F },
                                         glm::vec2{ 1.F, 1.F },
                                         glm::vec2{ 0.F, 1.F } },
  const glm::vec2                 size          = { 1.F, 1.F })
{
  const auto f_texture_id = static_cast<float>(texture_id);
  return {
    Vertex{ .location      = { offset },
            .color         = { color },
            .uv            = uv[0],
            .texture_slot  = f_texture_id,
            .tiling_factor = tiling_factor },// 0
    Vertex{ .location      = { offset + glm::vec2{ size.x, 0.F } },
            .color         = { color },
            .uv            = uv[1],
            .texture_slot  = f_texture_id,
            .tiling_factor = tiling_factor },// 1
    Vertex{ .location      = { offset + glm::vec2{ size.x, size.y } },
            .color         = { color },
            .uv            = uv[2],
            .texture_slot  = f_texture_id,
            .tiling_factor = tiling_factor },// 2
    Vertex{ .location{ offset + glm::vec2{ 0.F, size.y } },
            .color         = { color },
            .uv            = uv[3],
            .texture_slot  = f_texture_id,
            .tiling_factor = tiling_factor },// 3
  };
}
inline std::vector<std::uint32_t> QuadIndices(std::size_t count)
{
  static constexpr auto      init = std::array{ 0, 1, 2, 2, 3, 0 };
  std::vector<std::uint32_t> indices{};
  indices.reserve(std::size(init) * count);
  constexpr auto quad_size = std::size(Quad{});
  for (std::size_t i{}; i != count; ++i)
  {
    std::ranges::transform(
      init, std::back_inserter(indices), [&i, &quad_size](std::uint32_t index) {
        return static_cast<std::uint32_t>(index + i * quad_size);
      });
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
inline std::vector<Vertex> &operator+=(
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
[[nodiscard]] inline std::vector<Vertex> operator+(
  std::vector<Vertex>        vertices_left,
  const std::vector<Vertex> &vertices_right)
{
  vertices_left += vertices_right;
  return vertices_left;
}
#endif// MYPROJECT_VERTEX_HPP
