#include "VertexBufferDynamic.hpp"
VertexBufferDynamic::~VertexBufferDynamic()
{
  GLCall{ glDeleteBuffers, 1, &m_renderer_id };
  UnBind();
}

VertexBufferDynamic::VertexBufferDynamic(VertexBufferDynamic &&other) noexcept
  : VertexBufferDynamic()
{
  swap(*this, other);
}

VertexBufferDynamic &
  VertexBufferDynamic::operator=(VertexBufferDynamic &&other) noexcept
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
  VertexBufferDynamic::UnBind()
{
  GLCall{ glBindBuffer, GL_ARRAY_BUFFER, 0U };
}
VertexBufferDynamic::VertexBufferDynamic(size_t count)
  : m_max_size(count * 4U)
{
  GLCall{ glGenBuffers, 1, &m_renderer_id };
  GLCall{ glBindBuffer, GL_ARRAY_BUFFER, m_renderer_id };
  GLCall{ glBufferData,
          GL_ARRAY_BUFFER,
          static_cast<std::ptrdiff_t>(count * sizeof(Quad)),
          nullptr,
          GL_DYNAMIC_DRAW };
}

void
  swap(
    VertexBufferDynamic &first,
    VertexBufferDynamic &second) noexcept// nothrow
{
  // enable ADL (not necessary in our case, but good practice)
  using std::swap;

  // by swapping the members of two objects,
  // the two objects are effectively swapped
  swap(first.m_renderer_id, second.m_renderer_id);
  swap(first.m_max_size, second.m_max_size);
}
