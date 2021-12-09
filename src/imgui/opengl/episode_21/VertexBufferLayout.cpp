//
// Created by pcvii on 11/22/2021.
//

#include "VertexBufferLayout.hpp"

std::size_t
  VertexBufferLayout::VertexBufferElement::size_of_type(std::uint32_t type)
{
  switch (type)
  {
    case GL_FLOAT:
      return sizeof(GLfloat);
    case GL_INT:
      return sizeof(GLint);
    case GL_UNSIGNED_INT:
      return sizeof(GLuint);
    case GL_BYTE:
      return sizeof(GLbyte);
  }
  assert(false);
  return 0;
}
std::size_t VertexBufferLayout::VertexBufferElement::size() const
{
  return count * size_of_type(type);
}

const std::vector<VertexBufferLayout::VertexBufferElement> &
  VertexBufferLayout::elements() const noexcept
{
  return m_elements;
}
std::size_t VertexBufferLayout::stride() const noexcept
{
  return m_stride;
}
