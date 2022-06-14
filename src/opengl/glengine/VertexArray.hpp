//
// Created by pcvii on 11/22/2021.
//

#ifndef FIELD_MAP_EDITOR_VERTEXARRAY_HPP
#define FIELD_MAP_EDITOR_VERTEXARRAY_HPP
#include "Renderer.hpp"
#include "unique_value.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
namespace glengine
{
class VertexArray
{
private:
  GLID m_renderer_id{};

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
  void        Bind() const;
  static void UnBind();
  template<Bindable bindableT, size_t ElementCount>
  void push_back(
    const bindableT                        &vertex_buffer,
    const VertexBufferLayout<ElementCount> &layout)
  {// todo tag vertex_buffers so we can exclude other types.
    Bind();
    vertex_buffer.Bind();
    std::ranges::for_each(
      layout,
      [i      = std::uint32_t{},
       offset = static_cast<const std::uint8_t *>(nullptr),
       &layout](const VertexBufferElement &element) mutable {
        GLCall{}(glEnableVertexAttribArray, i);

        GLCall{}(
          glVertexAttribPointer,
          i,
          element.count,
          element.type,
          element.normalized,
          layout.stride(),
          offset);

        offset += element.size();
        ++i;
      });
  }
};
static_assert(Bindable<VertexArray>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_VERTEXARRAY_HPP
