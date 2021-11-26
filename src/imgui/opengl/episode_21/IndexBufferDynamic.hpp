//
// Created by pcvii on 11/21/2021.
//

#ifndef MYPROJECT_IndexBufferDynamic_HPP
#define MYPROJECT_IndexBufferDynamic_HPP
#include "Renderer.hpp"
#include "Vertex.hpp"
#include <cstdint>
class IndexBufferDynamic
{
private:
  std::uint32_t m_renderer_id = {};

public:
  IndexBufferDynamic() = default;
  IndexBufferDynamic(std::size_t count)
  :IndexBufferDynamic(QuadIndices(count))
  {
  }
  template<std::ranges::contiguous_range R>
  requires std::unsigned_integral<std::ranges::range_value_t<R>>
    IndexBufferDynamic(const R &buffer)
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
  ~IndexBufferDynamic();


  IndexBufferDynamic(const IndexBufferDynamic &) = delete;
  IndexBufferDynamic &
    operator=(const IndexBufferDynamic &) = delete;

  IndexBufferDynamic(IndexBufferDynamic &&other);
  IndexBufferDynamic &
    operator=(IndexBufferDynamic &&other);

  void
    Bind() const;
  void
    UnBind() const;

  friend void
    swap(IndexBufferDynamic &first, IndexBufferDynamic &second);
};
static_assert(Bindable<IndexBufferDynamic>);
#endif// MYPROJECT_IndexBufferDynamic_HPP
