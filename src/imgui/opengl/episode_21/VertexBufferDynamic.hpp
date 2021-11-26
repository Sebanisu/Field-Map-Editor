//
// Created by pcvii on 11/21/2021.
//

#ifndef MYPROJECT_VertexBufferDynamic_HPP
#define MYPROJECT_VertexBufferDynamic_HPP
#include "Renderer.hpp"
#include <cstdint>
#include <ranges>
#include "Vertex.hpp"
class VertexBufferDynamic
{
private:
  std::uint32_t m_renderer_id = {};

public:
  VertexBufferDynamic()
  {
    GLCall{ glGenBuffers, 1, &m_renderer_id };
    GLCall{ glBindBuffer, GL_ARRAY_BUFFER, m_renderer_id };
    GLCall{
      glBufferData, GL_ARRAY_BUFFER, 1000*sizeof(Quad), nullptr, GL_DYNAMIC_DRAW
    };
  }
  ~VertexBufferDynamic();

  VertexBufferDynamic(const VertexBufferDynamic &) = delete;
  VertexBufferDynamic &
    operator=(const VertexBufferDynamic &) = delete;

  VertexBufferDynamic(VertexBufferDynamic &&other);
  VertexBufferDynamic &
    operator=(VertexBufferDynamic &&other);

  void
    Bind() const;
  void
    UnBind() const;

  friend void
    swap(VertexBufferDynamic &first, VertexBufferDynamic &second);
};


#endif// MYPROJECT_VertexBufferDynamic_HPP
