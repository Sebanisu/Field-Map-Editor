#include "VertexBuffer.hpp"
VertexBuffer::~VertexBuffer()
{
  GLCall{ glDeleteBuffers, 1, &m_renderer_id };
}

VertexBuffer::VertexBuffer(VertexBuffer &&other) noexcept
  : VertexBuffer()
{
  swap(*this, other);
}

VertexBuffer &
  VertexBuffer::operator=(VertexBuffer &&other) noexcept
{
  swap(*this, other);
  return *this;
}

void
  VertexBuffer::Bind() const
{
  GLCall{ glBindBuffer, GL_ARRAY_BUFFER, m_renderer_id };
}

void
  VertexBuffer::UnBind() const
{
  GLCall{ glBindBuffer, GL_ARRAY_BUFFER, 0U };
}

void
  swap(VertexBuffer &first, VertexBuffer &second) noexcept// nothrow
{
  // enable ADL (not necessary in our case, but good practice)
  using std::swap;

  // by swapping the members of two objects,
  // the two objects are effectively swapped
  swap(first.m_renderer_id, second.m_renderer_id);
}
