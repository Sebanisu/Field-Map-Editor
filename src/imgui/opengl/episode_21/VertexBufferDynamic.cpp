#include "VertexBufferDynamic.hpp"
VertexBufferDynamic::~VertexBufferDynamic()
{
  GLCall{ glDeleteBuffers, 1, &m_renderer_id };
}

VertexBufferDynamic::VertexBufferDynamic(VertexBufferDynamic &&other)
  : VertexBufferDynamic()
{
  swap(*this, other);
}

VertexBufferDynamic &
  VertexBufferDynamic::operator=(VertexBufferDynamic &&other)
{
  swap(*this, other);
  return *this;
}

void
  VertexBufferDynamic::Bind() const
{
  GLCall{ glBindBuffer, GL_ARRAY_BUFFER, m_renderer_id };
}

void
  VertexBufferDynamic::UnBind() const
{
  GLCall{ glBindBuffer, GL_ARRAY_BUFFER, 0U };
}

void
  swap(VertexBufferDynamic &first, VertexBufferDynamic &second)// nothrow
{
  // enable ADL (not necessary in our case, but good practice)
  using std::swap;

  // by swapping the members of two objects,
  // the two objects are effectively swapped
  swap(first.m_renderer_id, second.m_renderer_id);
}
