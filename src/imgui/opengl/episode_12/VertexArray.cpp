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
}
VertexArray::VertexArray(VertexArray &&other)
  : VertexArray()
{
  swap(*this, other);
}
VertexArray &
  VertexArray::operator=(VertexArray &&other)
{
  swap(*this, other);
  return *this;
}
void
  VertexArray::Bind() const
{
  GLCall{ glBindVertexArray, m_renderer_id };
}
void
  VertexArray::UnBind() const
{

  GLCall{ glBindVertexArray, 0U };
}
void
  VertexArray::push_back(
    const VertexBuffer       &vb,
    const VertexBufferLayout &layout)
{
  Bind();
  vb.Bind();
  std::ranges::for_each(
    layout.elements(),
    [i      = std::uint32_t{},
     offset = static_cast<const std::uint8_t *>(nullptr),
     &layout](const VertexBufferLayout::VertexBufferElement &element) mutable
    {
      GLCall{ glEnableVertexAttribArray, i };
      GLCall{ glVertexAttribPointer,
              i,
              element.count,
              element.type,
              element.normalized,
              static_cast<int32_t>(layout.stride()),
              offset };
      offset += element.size();
      ++i;
    });
}
void
  swap(VertexArray &first, VertexArray &second)// nothrow
{
  // enable ADL (not necessary in our case, but good practice)
  using std::swap;

  // by swapping the members of two objects,
  // the two objects are effectively swapped
  swap(first.m_renderer_id, second.m_renderer_id);
}

