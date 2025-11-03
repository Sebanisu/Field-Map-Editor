#include "VertexBuffer.hpp"
void glengine::VertexBuffer::bind() const
{
     GlCall{}(glBindBuffer, GL_ARRAY_BUFFER, m_renderer_id);
}

void glengine::VertexBuffer::unbind()
{
     GlCall{}(glBindBuffer, GL_ARRAY_BUFFER, 0U);
}
