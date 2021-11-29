#include "IndexBuffer.hpp"

IndexBuffer::~IndexBuffer()
{
  GLCall{ glDeleteBuffers, 1, &m_renderer_id };
  UnBind();
}

IndexBuffer::IndexBuffer(IndexBuffer &&other) noexcept
  : IndexBuffer()
{
  swap(*this, other);
}

IndexBuffer &
  IndexBuffer::operator=(IndexBuffer &&other) noexcept
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
  IndexBuffer::UnBind()
{
  GLCall{ glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0U };
}

std::size_t
  IndexBuffer::size() const
{
  return m_size;
}

void
  swap(IndexBuffer &first, IndexBuffer &second) noexcept// nothrow
{
  // enable ADL (not necessary in our case, but good practice)
  using std::swap;

  // by swapping the members of two objects,
  // the two objects are effectively swapped
  swap(first.m_renderer_id, second.m_renderer_id);
  swap(first.m_size, second.m_size);
}