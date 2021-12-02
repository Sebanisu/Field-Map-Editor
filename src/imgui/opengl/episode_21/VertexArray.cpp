//
// Created by pcvii on 11/22/2021.
//

#include "VertexArray.hpp"
VertexArray::VertexArray()
{
  GLCall{ glGenVertexArrays, 1, &m_renderer_id };
}
VertexArray::~VertexArray()
{
  GLCall{ glDeleteVertexArrays, 1, &m_renderer_id };
  UnBind();
}
VertexArray::VertexArray(VertexArray &&other) noexcept
{
  m_renderer_id = other.m_renderer_id;
  other.m_renderer_id = 0;
}
VertexArray &
  VertexArray::operator=(VertexArray &&other) noexcept
{
  using std::swap;
  swap(m_renderer_id,other.m_renderer_id);
  return *this;
}
void
  VertexArray::Bind() const
{
  GLCall{ glBindVertexArray, m_renderer_id };
}
 void
  VertexArray::UnBind()
{

  GLCall{ glBindVertexArray, 0U };
}
