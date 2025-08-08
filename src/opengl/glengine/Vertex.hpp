//
// Created by pcvii on 11/25/2021.
//

#ifndef FIELD_MAP_EDITOR_VERTEX_HPP
#define FIELD_MAP_EDITOR_VERTEX_HPP
#include "VertexBufferLayout.hpp"
#include <algorithm>
#include <glm/glm.hpp>
#include <ranges>
#include <vector>
namespace glengine
{
struct Vertex
{
     glm::vec3     location      = {};
     glm::vec4     color         = { 1.F, 1.F, 1.F, 1.F };
     glm::vec2     uv            = {};
     float         texture_slot  = {};
     float         tiling_factor = { 1.F };
     int           tile_id       = { -1 };
     std::uint32_t pupu_id       = { 0 };

     static auto consteval layout()
     {
          return VertexBufferLayout{ glengine::VertexBufferElementType<float>{ 3U },       glengine::VertexBufferElementType<float>{ 4U },
                                     glengine::VertexBufferElementType<float>{ 2U },       glengine::VertexBufferElementType<float>{ 1U },
                                     glengine::VertexBufferElementType<float>{ 1U },       glengine::VertexBufferElementType<int>{ 1U },
                                     glengine::VertexBufferElementType<unsigned int>{ 1U } };
     }
};
static_assert(std::movable<Vertex> && std::copyable<Vertex> && std::default_initializable<Vertex>);

using Quad                            = std::array<Vertex, 4U>;
static constexpr auto QuadIndicesInit = std::array<std::uint32_t, 6U>{ 0, 1, 2, 2, 3, 0 };


template<std::size_t count>
constexpr inline std::array<std::uint32_t, count * std::size(QuadIndicesInit)> QuadIndices()
{
     using std::ranges::size;
     std::array<std::uint32_t, count * size(QuadIndicesInit)> indices{};
     constexpr auto                                           quad_size = size(Quad{});
     for (std::size_t i{}; i != count; ++i)
     {
          using std::ranges::range_difference_t;
          using std::ranges::advance;
          using std::ranges::transform;
          using std::ranges::begin;
          auto f = begin(indices);
          advance(f, static_cast<range_difference_t<decltype(indices)>>(i * size(QuadIndicesInit)));
          transform(QuadIndicesInit, f, [&](std::uint32_t index) { return static_cast<std::uint32_t>(index + i * quad_size); });
     }
     return indices;
}

Quad CreateQuad(
  const glm::vec3                 offset,
  const glm::vec4                 color,
  const int                       texture_id    = {},
  const float                     tiling_factor = 1.F,
  const std::array<glm::vec2, 4U> uv      = { glm::vec2{ 0.F, 0.F }, glm::vec2{ 1.F, 0.F }, glm::vec2{ 1.F, 1.F }, glm::vec2{ 0.F, 1.F } },
  const glm::vec2                 size    = { 1.F, 1.F },
  const int                       id      = -1,
  const std::uint32_t             pupu_id = 0);

[[nodiscard]] std::vector<std::uint32_t> QuadIndices(std::size_t count);
std::vector<Vertex>                     &operator+=(std::vector<Vertex> &vertices, const Quad &quad);
std::vector<Vertex>                     &operator+=(std::vector<Vertex> &vertices_left, const std::vector<Vertex> &vertices_right);
[[nodiscard]] std::vector<Vertex>        operator+(std::vector<Vertex> vertices, const Quad &quad);
[[nodiscard]] std::vector<Vertex>        operator+(const Quad &quad_left, const Quad &quad_right);
[[nodiscard]] std::vector<Vertex>        operator+(std::vector<Vertex> vertices_left, const std::vector<Vertex> &vertices_right);

}// namespace glengine
#endif// FIELD_MAP_EDITOR_VERTEX_HPP
