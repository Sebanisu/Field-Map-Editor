//
// Created by pcvii on 11/21/2021.
//

#ifndef FIELD_MAP_EDITOR_INDEXBUFFER_HPP
#define FIELD_MAP_EDITOR_INDEXBUFFER_HPP
#include "IndexType.hpp"
#include "Renderer.hpp"
#include "UniqueValue.hpp"
namespace glengine
{
class IndexBuffer
{
   private:
     Glid        m_renderer_id = {};
     std::size_t m_size        = {};
     IndexType   m_type        = {};

   public:
     IndexBuffer() = default;
     template<std::ranges::contiguous_range R>
     // clang-format off
  requires (std::unsigned_integral<std::ranges::range_value_t<R>>
    && (sizeof(std::ranges::range_value_t<R>) <= sizeof(std::uint32_t)))
     // clang-format on
     IndexBuffer(R &&buffer)
       : m_renderer_id{ [&buffer]() -> std::uint32_t
                        {
                             std::uint32_t        tmp{};
                             const std::ptrdiff_t size_in_bytes
                               = static_cast<std::ptrdiff_t>(std::ranges::size(buffer) * sizeof(std::ranges::range_value_t<R>));
                             const void *data = std::ranges::data(buffer);
                             GlCall{}(glGenBuffers, 1, &tmp);
                             GlCall{}(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, tmp);
                             GlCall{}(glBufferData, GL_ELEMENT_ARRAY_BUFFER, size_in_bytes, data, GL_STATIC_DRAW);
                             return tmp;
                        }(),
                        [](const std::uint32_t id)
                        {
                             GlCall{}(glDeleteBuffers, 1, &id);
                             IndexBuffer::unbind();
                        } }
       , m_size(std::ranges::size(buffer))
       , m_type(GetIndexType<std::ranges::range_value_t<R>>())
     {
     }
     IndexType   type() const;
     void        bind() const;
     static void unbind();
     std::size_t size() const;
};
static_assert(Bindable<IndexBuffer> && has_Type_for_IndexType<IndexBuffer>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_INDEXBUFFER_HPP
