#include "IndexBufferDynamic.hpp"
void
  IndexBufferDynamic::Bind() const
{
  GLCall{ glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_renderer_id };
}

void
  IndexBufferDynamic::UnBind()
{
  GLCall{ glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0U };
}
IndexType
  IndexBufferDynamic::Type() const
{
  return m_type;
}
