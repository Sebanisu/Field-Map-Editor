//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_BATCHRENDERER_HPP
#define MYPROJECT_BATCHRENDERER_HPP
#include "IndexBufferDynamic.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBufferDynamic.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace test
{
class BatchRenderer
{
public:
  BatchRenderer();
  BatchRenderer(std::size_t quad_count);

  friend void
    OnUpdate(const BatchRenderer &, float);
  friend void
    OnRender(const BatchRenderer &);
  friend void
    OnImGuiRender(const BatchRenderer &);


  [[nodiscard]] std::size_t
    QUAD_COUNT() const noexcept
  {
    return m_quad_count;
  };
  [[nodiscard]] std::size_t
    VERT_COUNT() const noexcept
  {
    return m_quad_count * 4U;
  };
  [[nodiscard]] [[maybe_unused]] std::size_t
    INDEX_COUNT() const noexcept
  {
    return m_quad_count * 6U;
  };
  void
    Clear() const;
  void
    Draw(Quad quad) const;
  void
    Draw() const;

  const auto &
    Shader() const
  {
    return m_shader;
  }

private:
  void
    FlushVertices() const;
  void
                              DrawVertices() const;
  std::size_t                 m_quad_count    = { 100U };
  VertexBufferDynamic         m_vertex_buffer = { QUAD_COUNT() };
  IndexBufferDynamic          m_index_buffer  = { QUAD_COUNT() };
  mutable std::vector<Vertex> m_vertices      = { [this]()
                                             {
                                               std::vector<Vertex>r{};
                                               r.reserve(VERT_COUNT());
                                               return r;
                                             }() };
  mutable IndexBufferDynamicSize index_buffer_size = {};
  ::Shader                       m_shader          = {};
  VertexArray                    m_vertex_array    = {};
};
void
  OnUpdate(const BatchRenderer &, float);
void
  OnRender(const BatchRenderer &);
void
  OnImGuiRender(const BatchRenderer &);

}// namespace test
#endif// MYPROJECT_BATCHRENDERER_HPP
