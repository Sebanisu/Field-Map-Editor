#include "Vertex.hpp"

namespace glengine
{

Quad CreateQuad(
  const glm::vec3 offset,
  const glm::vec4 color,
  const int       texture_id,
  const float     tiling_factor,
  const std::array<
    glm::vec2,
    4U>               uv,
  const glm::vec2     size,
  const int           id,
  const std::uint32_t pupu_id)
{
     const auto f_texture_id = static_cast<float>(texture_id);
     return {
          Vertex{ .location      = { offset },
                  .color         = { color },
                  .uv            = uv[0],
                  .texture_slot  = f_texture_id,
                  .tiling_factor = tiling_factor,
                  .tile_id       = id,
                  .pupu_id       = pupu_id },// 0
          Vertex{ .location      = { offset + glm::vec3{ size.x, 0.F, 0.F } },
                  .color         = { color },
                  .uv            = uv[1],
                  .texture_slot  = f_texture_id,
                  .tiling_factor = tiling_factor,
                  .tile_id       = id,
                  .pupu_id       = pupu_id },// 1
          Vertex{ .location      = { offset + glm::vec3{ size.x, size.y, 0.F } },
                  .color         = { color },
                  .uv            = uv[2],
                  .texture_slot  = f_texture_id,
                  .tiling_factor = tiling_factor,
                  .tile_id       = id,
                  .pupu_id       = pupu_id },// 2
          Vertex{ .location{ offset + glm::vec3{ 0.F, size.y, 0.F } },
                  .color         = { color },
                  .uv            = uv[3],
                  .texture_slot  = f_texture_id,
                  .tiling_factor = tiling_factor,
                  .tile_id       = id,
                  .pupu_id       = pupu_id },// 3
     };
}

std::vector<std::uint32_t> QuadIndices(std::size_t count)
{
     std::vector<std::uint32_t> indices{};
     indices.reserve(std::size(QuadIndicesInit) * count);
     static constexpr auto quad_size = std::size(Quad{});
     for (std::size_t i{}; i != count; ++i)
     {
          std::ranges::transform(QuadIndicesInit, std::back_inserter(indices), [&i](std::uint32_t index) {
               return static_cast<std::uint32_t>(index + i * quad_size);
          });
     }
     return indices;
}

std::vector<Vertex> &operator+=(
  std::vector<Vertex> &vertices,
  const Quad          &quad)
{
     std::ranges::copy(quad, std::back_inserter(vertices));
     return vertices;
}

std::vector<Vertex> &operator+=(
  std::vector<Vertex>       &vertices_left,
  const std::vector<Vertex> &vertices_right)
{
     std::ranges::copy(vertices_right, std::back_inserter(vertices_left));
     return vertices_left;
}

std::vector<Vertex> operator+(
  std::vector<Vertex> vertices,
  const Quad         &quad)
{
     vertices += quad;
     return vertices;
}

std::vector<Vertex> operator+(
  const Quad &quad_left,
  const Quad &quad_right)
{
     std::vector<Vertex> vertices{};
     vertices.reserve(std::size(quad_left) + std::size(quad_right));
     vertices += quad_left;
     vertices += quad_right;
     return vertices;
}

std::vector<Vertex> operator+(
  std::vector<Vertex>        vertices_left,
  const std::vector<Vertex> &vertices_right)
{
     vertices_left += vertices_right;
     return vertices_left;
}

}// namespace glengine