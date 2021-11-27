//
// Created by pcvii on 11/21/2021.
//

#ifndef MYPROJECT_INDEXBUFFER_HPP
#define MYPROJECT_INDEXBUFFER_HPP
#include "Renderer.hpp"
#include <cstdint>
#include <span>
class IndexBuffer
{
private:
  std::uint32_t m_renderer_id = {};
  std::size_t   m_size        = {};

public:
  IndexBuffer() = default;
  template<std::ranges::contiguous_range R>
  requires std::unsigned_integral<std::ranges::range_value_t<R>>
    IndexBuffer(R &&buffer)
    : m_size(std::ranges::size(buffer))
  {
    const std::ptrdiff_t size_in_bytes = static_cast<std::ptrdiff_t>(
      std::ranges::size(buffer) * sizeof(std::ranges::range_value_t<R>));
    const void *data = std::ranges::data(buffer);
    GLCall{ glGenBuffers, 1, &m_renderer_id };
    GLCall{ glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_renderer_id };
    GLCall{
      glBufferData, GL_ELEMENT_ARRAY_BUFFER, size_in_bytes, data, GL_STATIC_DRAW
    };
  }
  ~IndexBuffer();


  IndexBuffer(const IndexBuffer &) = delete;
  IndexBuffer &
    operator=(const IndexBuffer &) = delete;

  IndexBuffer(IndexBuffer &&other) noexcept;
  IndexBuffer &
    operator=(IndexBuffer &&other) noexcept;

  void
    Bind() const;
  void
    UnBind() const;
  std::size_t
    size() const;

  friend void
    swap(IndexBuffer &first, IndexBuffer &second) noexcept;
};
static_assert(Bindable<IndexBuffer>);

#endif// MYPROJECT_INDEXBUFFER_HPP
