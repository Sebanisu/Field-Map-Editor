//
// Created by pcvii on 11/22/2021.
//

#ifndef MYPROJECT_VERTEXARRAY_HPP
#define MYPROJECT_VERTEXARRAY_HPP
#include "Renderer.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include <algorithm>
#include <ranges>
class VertexArray
{
private:
  std::uint32_t m_renderer_id{};

public:
  VertexArray();
  ~VertexArray();


  VertexArray(const VertexArray &) = delete;
  VertexArray &
    operator=(const VertexArray &) = delete;

  VertexArray(VertexArray &&other);

  VertexArray &
    operator=(VertexArray &&other);

  void
    Bind() const;
  void
    UnBind() const;
  void
    push_back(const VertexBuffer &vb, const VertexBufferLayout &layout);
  friend void
    swap(VertexArray &first, VertexArray &second);
};


template<>
inline void
  VertexBufferLayout::push_back<float>(std::uint32_t count)
{
  m_stride +=
    m_elements.emplace_back(GL_FLOAT, count, std::uint8_t{ GL_FALSE }).size();
}
template<>
inline void
  VertexBufferLayout::push_back<std::uint32_t>(std::uint32_t count)
{
  m_stride +=
    m_elements.emplace_back(GL_UNSIGNED_INT, count, std::uint8_t{ GL_FALSE })
      .size();
}
template<>
inline void
  VertexBufferLayout::push_back<std::uint8_t>(std::uint32_t count)
{
  m_stride +=
    m_elements.emplace_back(GL_UNSIGNED_BYTE, count, std::uint8_t{ GL_TRUE })
      .size();
}
#endif// MYPROJECT_VERTEXARRAY_HPP
