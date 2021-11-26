#include "IndexBufferDynamic.hpp"

IndexBufferDynamic::~IndexBufferDynamic()
{
  GLCall{ glDeleteBuffers, 1, &m_renderer_id };
}

IndexBufferDynamic::IndexBufferDynamic(IndexBufferDynamic &&other)
  : IndexBufferDynamic()
{
  swap(*this, other);
}

IndexBufferDynamic &
  IndexBufferDynamic::operator=(IndexBufferDynamic &&other)
{
  swap(*this, other);
  return *this;
};

void
  IndexBufferDynamic::Bind() const
{
  GLCall{ glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_renderer_id };
}

void
  IndexBufferDynamic::UnBind() const
{
  GLCall{ glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0U };
}

void
  swap(IndexBufferDynamic &first, IndexBufferDynamic &second)// nothrow
{
  // enable ADL (not necessary in our case, but good practice)
  using std::swap;

  // by swapping the members of two objects,
  // the two objects are effectively swapped
  swap(first.m_renderer_id, second.m_renderer_id);
}