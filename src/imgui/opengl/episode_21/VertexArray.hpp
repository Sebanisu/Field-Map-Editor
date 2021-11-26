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
#endif// MYPROJECT_VERTEXARRAY_HPP
