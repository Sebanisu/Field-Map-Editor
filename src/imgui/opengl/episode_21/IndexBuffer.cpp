#include "IndexBuffer.hpp"

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
IndexType IndexBuffer::Type() const {
  return m_type;
};