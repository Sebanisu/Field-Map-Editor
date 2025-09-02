#include "HistogramBuffer.hpp"

namespace glengine
{
HistogramBuffer::HistogramBuffer(size_t count)
  : m_count(count)
  , m_buffer_id(Glid{ create(m_count), destroy })
{
}

void HistogramBuffer::bind(GLuint binding_point)
{
     if (!m_buffer_id)
     {
          spdlog::error("HistogramBuffer not initialized, cannot bind");
          return;
     }
     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, m_buffer_id);
     if (glGetError() != GL_NO_ERROR)
     {
          spdlog::error("Failed to bind HistogramBuffer");
     }
}

void HistogramBuffer::read_back(std::vector<GLuint> &data)
{
     if (!m_buffer_id)
     {
          spdlog::error("HistogramBuffer not initialized, cannot read back");
          return;
     }
     data.resize(m_count);
     // glFinish();
     GlCall{}(glBindBuffer, GL_SHADER_STORAGE_BUFFER, m_buffer_id);
     // glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_count * sizeof(GLuint), data.data());
     void *ptr =
       GlCall{}(glMapBufferRange, GL_SHADER_STORAGE_BUFFER, 0, static_cast<GLsizeiptr>(m_count * sizeof(GLuint)), GL_MAP_READ_BIT);
     if (ptr)
     {
          std::memcpy(data.data(), ptr, m_count * sizeof(GLuint));
          bool result = GlCall{}(glUnmapBuffer, GL_SHADER_STORAGE_BUFFER);
          if (!result)
          {
               spdlog::error("glUnmapBuffer failed: buffer data may be corrupted (id={}, count={})", m_buffer_id, m_count);
          }
     }
     GlCall{}(glBindBuffer, GL_SHADER_STORAGE_BUFFER, 0);
     if (glGetError() != GL_NO_ERROR)
     {
          spdlog::error("Failed to read back HistogramBuffer data");
     }
}


void HistogramBuffer::reset() const
{
     // todo we could write a shader to clear the buffer on the gpu. donno if it would be faster.
     if (!m_buffer_id)
     {
          spdlog::error("HistogramBuffer not initialized, cannot reset");
          return;
     }
     std::vector<GLuint> zero_data(m_count, 0);
     GlCall{}(glBindBuffer, GL_SHADER_STORAGE_BUFFER, m_buffer_id);
     GlCall{}(glBufferSubData, GL_SHADER_STORAGE_BUFFER, 0, static_cast<GLsizeiptr>(m_count * sizeof(GLuint)), zero_data.data());
     GlCall{}(glBindBuffer, GL_SHADER_STORAGE_BUFFER, 0);
     if (glGetError() != GL_NO_ERROR)
     {
          spdlog::error("Failed to reset HistogramBuffer data");
     }
}

GlidCopy HistogramBuffer::id() const
{
     return m_buffer_id;
}

size_t HistogramBuffer::count() const
{
     return m_count;
}


GLuint HistogramBuffer::create(size_t count)
{
     GLuint temp_id = {};
     GlCall{}(glGenBuffers, 1, &temp_id);
     if (!temp_id)
     {
          spdlog::error("Failed to generate HistogramBuffer");
          return {};
     }
     std::vector<GLuint> init_data(count, 0);
     GlCall{}(glBindBuffer, GL_SHADER_STORAGE_BUFFER, temp_id);
     GlCall{}(glBufferData, GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(count * sizeof(GLuint)), init_data.data(), GL_DYNAMIC_COPY);
     GlCall{}(glBindBuffer, GL_SHADER_STORAGE_BUFFER, 0);
     if (glGetError() != GL_NO_ERROR)
     {
          spdlog::error("Failed to initialize HistogramBuffer data");
     }
     return temp_id;
}

void HistogramBuffer::destroy(const GLuint id)
{
     GlCall{}(glDeleteBuffers, 1, &id);
}
}// namespace glengine