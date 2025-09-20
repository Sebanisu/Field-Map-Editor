//
// Created by pcvii on 11/21/2021.
//

#ifndef FIELD_MAP_EDITOR_VERTEXBUFFER_HPP
#define FIELD_MAP_EDITOR_VERTEXBUFFER_HPP
#include "Renderer.hpp"
#include "UniqueValue.hpp"
namespace glengine
{
class VertexBuffer
{
   private:
     Glid m_renderer_id = {};

   public:
     VertexBuffer() = default;
     VertexBuffer(std::ranges::contiguous_range auto &&buffer)
       : m_renderer_id(
           [&]() -> std::uint32_t
           {
                std::uint32_t        tmp = {};
                const std::ptrdiff_t size_in_bytes
                  = static_cast<std::ptrdiff_t>(
                    std::ranges::size(buffer)
                    * sizeof(std::ranges::range_value_t<decltype(buffer)>));
                const void *data = std::ranges::data(buffer);
                GlCall{}(glGenBuffers, 1, &tmp);
                GlCall{}(glBindBuffer, GL_ARRAY_BUFFER, tmp);
                GlCall{}(
                  glBufferData,
                  GL_ARRAY_BUFFER,
                  size_in_bytes,
                  data,
                  GL_STATIC_DRAW);
                return tmp;
           }(),
           [](const std::uint32_t id)
           {
                GlCall{}(glDeleteBuffers, 1, &id);
                VertexBuffer::unbind();
           })
     {
     }

     void        bind() const;
     static void unbind();
};
static_assert(Bindable<VertexBuffer>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_VERTEXBUFFER_HPP
