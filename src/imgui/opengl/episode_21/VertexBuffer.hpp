//
// Created by pcvii on 11/21/2021.
//

#ifndef MYPROJECT_VERTEXBUFFER_HPP
#define MYPROJECT_VERTEXBUFFER_HPP
#include "Renderer.hpp"
#include <cstdint>
#include <ranges>
class VertexBuffer
{
private:
  std::uint32_t m_renderer_id = {};

public:
  VertexBuffer()              = default;
  VertexBuffer(std::ranges::contiguous_range auto &&buffer)
  {
    const std::ptrdiff_t size_in_bytes = static_cast<std::ptrdiff_t>(
      std::ranges::size(buffer)
      * sizeof(std::ranges::range_value_t<decltype(buffer)>));
    const void *data = std::ranges::data(buffer);
    GLCall{ glGenBuffers, 1, &m_renderer_id };
    GLCall{ glBindBuffer, GL_ARRAY_BUFFER, m_renderer_id };
    GLCall{
      glBufferData, GL_ARRAY_BUFFER, size_in_bytes, data, GL_STATIC_DRAW
    };
  }
  ~VertexBuffer();

  VertexBuffer(const VertexBuffer &) = delete;
  VertexBuffer &
    operator=(const VertexBuffer &) = delete;

  VertexBuffer(VertexBuffer &&other);
  VertexBuffer &
    operator=(VertexBuffer &&other);

  void
    Bind() const;
  void
    UnBind() const;

  friend void
    swap(VertexBuffer &first, VertexBuffer &second);
};
static_assert(Bindable<VertexBuffer>);


#endif// MYPROJECT_VERTEXBUFFER_HPP
