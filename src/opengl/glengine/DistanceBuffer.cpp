#include "DistanceBuffer.hpp"
namespace glengine
{
DistanceBuffer::DistanceBuffer(size_t count)
  : m_count(count)
  , m_buffer_id(Glid{ create(m_count), destroy })
{
}

void DistanceBuffer::bind(GLuint binding_point)
{
     if (!m_buffer_id)
     {
          spdlog::error("DistanceBuffer not initialized, cannot bind");
          return;
     }
     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, m_buffer_id);
     if (glGetError() != GL_NO_ERROR)
     {
          spdlog::error("Failed to bind DistanceBuffer");
     }
}

void DistanceBuffer::read_back(std::vector<float> &data)
{
     if (!m_buffer_id)
     {
          spdlog::error("DistanceBuffer not initialized, cannot read back");
          return;
     }
     data.resize(m_count);
     // glFinish();
     GlCall{}(glBindBuffer, GL_SHADER_STORAGE_BUFFER, m_buffer_id);
     // glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_count * sizeof(float), data.data());
     void *ptr = GlCall{}(glMapBufferRange, GL_SHADER_STORAGE_BUFFER, 0, static_cast<GLsizeiptr>(m_count * sizeof(float)), GL_MAP_READ_BIT);
     if (ptr)
     {
          std::memcpy(data.data(), ptr, m_count * sizeof(float));
          glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
     }
     GlCall{}(glBindBuffer, GL_SHADER_STORAGE_BUFFER, 0);
     if (glGetError() != GL_NO_ERROR)
     {
          spdlog::error("Failed to read back DistanceBuffer data");
     }
}


void DistanceBuffer::reset() const
{
     // todo we could write a shader to clear the buffer on the gpu. donno if it would be faster.
     if (!m_buffer_id)
     {
          spdlog::error("DistanceBuffer not initialized, cannot reset");
          return;
     }
     std::vector<float> zero_data(m_count, 0.F);
     GlCall{}(glBindBuffer, GL_SHADER_STORAGE_BUFFER, m_buffer_id);
     GlCall{}(glBufferSubData, GL_SHADER_STORAGE_BUFFER, 0, static_cast<GLsizeiptr>(m_count * sizeof(float)), zero_data.data());
     GlCall{}(glBindBuffer, GL_SHADER_STORAGE_BUFFER, 0);
     if (glGetError() != GL_NO_ERROR)
     {
          spdlog::error("Failed to reset DistanceBuffer data");
     }
}

GlidCopy DistanceBuffer::id() const
{
     return m_buffer_id;
}

size_t DistanceBuffer::count() const
{
     return m_count;
}


GLuint DistanceBuffer::create(size_t count)
{
     GLuint temp_id = {};
     GlCall{}(glGenBuffers, 1, &temp_id);
     if (!temp_id)
     {
          spdlog::error("Failed to generate DistanceBuffer");
          return {};
     }
     std::vector<float> init_data(count, 0);
     GlCall{}(glBindBuffer, GL_SHADER_STORAGE_BUFFER, temp_id);
     GlCall{}(glBufferData, GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(count * sizeof(float)), init_data.data(), GL_DYNAMIC_COPY);
     GlCall{}(glBindBuffer, GL_SHADER_STORAGE_BUFFER, 0);
     if (glGetError() != GL_NO_ERROR)
     {
          spdlog::error("Failed to initialize DistanceBuffer data");
     }
     return temp_id;
}

void DistanceBuffer::destroy(const GLuint id)
{
     GlCall{}(glDeleteBuffers, 1, &id);
}
}// namespace glengine