#include "IndexBuffer.hpp"
namespace glengine
{
void IndexBuffer::bind() const
{
     GlCall{}(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_renderer_id);
}

void IndexBuffer::unbind()
{
     GlCall{}(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0U);
}

std::size_t IndexBuffer::size() const
{
     return m_size;
}
IndexType IndexBuffer::type() const
{
     return m_type;
}
}// namespace glengine