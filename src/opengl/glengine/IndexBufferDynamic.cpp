#include "IndexBufferDynamic.hpp"
namespace glengine
{
void IndexBufferDynamic::bind() const
{
  GlCall{}(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_renderer_id);
}

void IndexBufferDynamic::unbind()
{
  GlCall{}(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0U);
}
IndexType IndexBufferDynamic::type() const
{
  return m_type;
}
}// namespace glengine