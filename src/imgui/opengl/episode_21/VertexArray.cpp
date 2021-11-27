//
// Created by pcvii on 11/22/2021.
//

#include "VertexArray.hpp"
VertexArray::VertexArray()
{
  GLCall{ glGenVertexArrays, 1, &m_renderer_id };
}
VertexArray::~VertexArray()
{
  GLCall{ glDeleteVertexArrays, 1, &m_renderer_id };
}
VertexArray::VertexArray(VertexArray &&other) noexcept
  : VertexArray()
{
  swap(*this, other);
}
VertexArray &
  VertexArray::operator=(VertexArray &&other) noexcept
{
  swap(*this, other);
  return *this;
}
void
  VertexArray::Bind() const
{
  GLCall{ glBindVertexArray, m_renderer_id };
}
void
  VertexArray::UnBind() const
{

  GLCall{ glBindVertexArray, 0U };
}


void
  swap(VertexArray &first, VertexArray &second) noexcept// nothrow
{
  // enable ADL (not necessary in our case, but good practice)
  using std::swap;

  // by swapping the members of two objects,
  // the two objects are effectively swapped
  swap(first.m_renderer_id, second.m_renderer_id);
}
