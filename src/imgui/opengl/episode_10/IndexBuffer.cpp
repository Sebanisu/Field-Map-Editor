#include "IndexBuffer.hpp"

IndexBuffer::~IndexBuffer()
{
  if (m_renderer_id != 0U)
  {
    GLCall{ glDeleteBuffers, 1, &m_renderer_id };
  }
}

IndexBuffer::IndexBuffer(IndexBuffer &&other)
  : IndexBuffer()
{
  swap(*this, other);
}

IndexBuffer &
  IndexBuffer::operator=(IndexBuffer &&other)
{
  swap(*this, other);
  return *this;
};

void
  IndexBuffer::Bind() const
{
  GLCall{ glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_renderer_id };
}

void
  IndexBuffer::UnBind() const
{
  GLCall{ glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0U };
}

std::size_t
  IndexBuffer::Count() const
{
  return m_count;
}

void
  swap(IndexBuffer &first, IndexBuffer &second)// nothrow
{
  // enable ADL (not necessary in our case, but good practice)
  using std::swap;

  // by swapping the members of two objects,
  // the two objects are effectively swapped
  swap(first.m_renderer_id, second.m_renderer_id);
  swap(first.m_count, second.m_count);
}