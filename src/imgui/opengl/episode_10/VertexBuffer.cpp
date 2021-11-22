#include "VertexBuffer.hpp"

VertexBuffer::~VertexBuffer()
{
  if (m_renderer_id != 0U)
  {
    GLCall{ glDeleteBuffers, 1, &m_renderer_id };
  }
}

VertexBuffer::VertexBuffer(VertexBuffer &&other)
  : VertexBuffer()
{
  swap(*this, other);
}

VertexBuffer &
  VertexBuffer::operator=(VertexBuffer &&other)
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
  swap(VertexBuffer &first, VertexBuffer &second)// nothrow
{
  // enable ADL (not necessary in our case, but good practice)
  using std::swap;

  // by swapping the members of two objects,
  // the two objects are effectively swapped
  swap(first.m_renderer_id, second.m_renderer_id);
}