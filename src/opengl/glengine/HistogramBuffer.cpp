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
     glFinish();
     glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffer_id);
     // glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_count * sizeof(GLuint), data.data());
     void *ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_count * sizeof(GLuint), GL_MAP_READ_BIT);
     if (ptr)
     {
          std::memcpy(data.data(), ptr, m_count * sizeof(GLuint));
          glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
     }
     glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
     if (glGetError() != GL_NO_ERROR)
     {
          spdlog::error("Failed to read back HistogramBuffer data");
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
     glBindBuffer(GL_SHADER_STORAGE_BUFFER, temp_id);
     glBufferData(GL_SHADER_STORAGE_BUFFER, count * sizeof(GLuint), init_data.data(), GL_STATIC_COPY);
     glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
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