#include "VertexBuffer.hpp"

void
  VertexBuffer::Bind() const
{
  GLCall{ glBindBuffer, GL_ARRAY_BUFFER, m_renderer_id };
}

 void
  VertexBuffer::UnBind()
{
  GLCall{ glBindBuffer, GL_ARRAY_BUFFER, 0U };
}


