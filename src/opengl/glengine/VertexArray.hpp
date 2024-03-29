//
// Created by pcvii on 11/22/2021.
//

#ifndef FIELD_MAP_EDITOR_VERTEXARRAY_HPP
#define FIELD_MAP_EDITOR_VERTEXARRAY_HPP
#include "Renderer.hpp"
#include "UniqueValue.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
namespace glengine
{
class VertexArray
{
private:
  Glid m_renderer_id{};

public:
  VertexArray();
  template<Bindable bindableT, size_t ElementCount>
  VertexArray(
    const bindableT                        &vertex_buffer,
    const VertexBufferLayout<ElementCount> &layout)
    : VertexArray()
  {
    push_back(vertex_buffer, layout);
  }
  void        bind() const;
  static void unbind();
  template<Bindable bindableT, size_t ElementCount>
  void push_back(
    const bindableT                        &vertex_buffer,
    const VertexBufferLayout<ElementCount> &layout)
  {// todo tag vertex_buffers so we can exclude other types.
    bind();
    vertex_buffer.bind();
    std::ranges::for_each(
      layout,
      [i      = std::uint32_t{},
       offset = static_cast<const std::uint8_t *>(nullptr),
       &layout](const VertexBufferElement &element) mutable {
        GlCall{}(glEnableVertexAttribArray, i);
        if(element.m_type == GL_INT)
        {
          GlCall{}(
            glVertexAttribIPointer,
            i,
            element.m_count,
            element.m_type,
            layout.stride(),
            offset);
        }
        else
        {
          GlCall{}(
            glVertexAttribPointer,
            i,
            element.m_count,
            element.m_type,
            element.m_normalized,
            layout.stride(),
            offset);
        }

        offset += element.size();
        ++i;
      });
  }
};
static_assert(Bindable<VertexArray>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_VERTEXARRAY_HPP
