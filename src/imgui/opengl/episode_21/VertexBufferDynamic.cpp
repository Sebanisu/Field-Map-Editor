#include "VertexBufferDynamic.hpp"
void
  VertexBufferDynamic::Bind() const
{
  GLCall{ glBindBuffer, GL_ARRAY_BUFFER, m_renderer_id };
}

void
  VertexBufferDynamic::UnBind()
{
  GLCall{ glBindBuffer, GL_ARRAY_BUFFER, 0U };
}
VertexBufferDynamic::VertexBufferDynamic(size_t count)
  : m_max_size(count * 4U)
  , m_renderer_id{ [&count]() -> std::uint32_t
                   {
                     std::uint32_t tmp;
                     GLCall{ glGenBuffers, 1, &tmp };
                     GLCall{ glBindBuffer, GL_ARRAY_BUFFER, tmp };
                     GLCall{ glBufferData,
                             GL_ARRAY_BUFFER,
                             static_cast<std::ptrdiff_t>(count * sizeof(Quad)),
                             nullptr,
                             GL_DYNAMIC_DRAW };
                     return tmp;
                   }(),
                   [](std::uint32_t id)
                   {
                     GLCall{ glDeleteBuffers, 1, &id };
                     VertexBufferDynamic::UnBind();
                   } }
{
}
