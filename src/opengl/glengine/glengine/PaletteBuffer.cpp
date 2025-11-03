#include "PaletteBuffer.hpp"
namespace glengine
{
void PaletteBuffer::initialize(const std::vector<glm::vec4> &data)
{
     if (!m_buffer_id)
     {
          spdlog::error("PaletteBuffer not initialized, cannot set data");
          return;
     }
     GlCall{}(glBindBuffer, GL_SHADER_STORAGE_BUFFER, m_buffer_id);
     GlCall{}(
       glBufferData, GL_SHADER_STORAGE_BUFFER,
       static_cast<GLsizeiptr>(data.size() * sizeof(glm::vec4)), data.data(),
       GL_STATIC_DRAW);
     GlCall{}(glBindBuffer, GL_SHADER_STORAGE_BUFFER, 0);
     m_size = data.size() / 4;// Assuming vec4 data
     if (glGetError() != GL_NO_ERROR)
     {
          spdlog::error("Failed to initialize PaletteBuffer data");
     }
}

void PaletteBuffer::bind(GLuint binding_point)
{
     if (m_buffer_id == 0)
     {
          spdlog::error("PaletteBuffer not initialized, cannot bind");
          return;
     }
     GlCall{}(
       glBindBufferBase, GL_SHADER_STORAGE_BUFFER, binding_point, m_buffer_id);
     if (glGetError() != GL_NO_ERROR)
     {
          spdlog::error("Failed to bind PaletteBuffer");
     }
}

GlidCopy PaletteBuffer::id() const
{
     return m_buffer_id;
}
size_t PaletteBuffer::size() const
{
     return m_size;
}
}// namespace glengine