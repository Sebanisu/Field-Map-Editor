#include "VertexBufferDynamic.hpp"
namespace glengine
{
void VertexBufferDynamic::bind() const
{
     GlCall{}(glBindBuffer, GL_ARRAY_BUFFER, m_renderer_id);
}

void VertexBufferDynamic::unbind()
{
     GlCall{}(glBindBuffer, GL_ARRAY_BUFFER, 0U);
}

VertexBufferDynamic::VertexBufferDynamic(size_t count)
  : m_renderer_id{ [&count]() -> std::uint32_t
                   {
                        const auto    pop_backup = backup();
                        std::uint32_t tmp;
                        GlCall{}(glGenBuffers, 1, &tmp);
                        GlCall{}(glBindBuffer, GL_ARRAY_BUFFER, tmp);
                        GlCall{}(
                          glBufferData, GL_ARRAY_BUFFER,
                          static_cast<std::ptrdiff_t>(count * sizeof(Quad)),
                          nullptr, GL_DYNAMIC_DRAW);
                        return tmp;
                   }(),
                   [](const std::uint32_t id)
                   {
                        GlCall{}(glDeleteBuffers, 1, &id);
                        VertexBufferDynamic::unbind();
                   } }
  , m_max_size(count * 4U)
{
}
}// namespace glengine