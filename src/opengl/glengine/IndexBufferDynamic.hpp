//
// Created by pcvii on 11/21/2021.
//

#ifndef FIELD_MAP_EDITOR_INDEXBUFFERDYNAMIC_HPP
#define FIELD_MAP_EDITOR_INDEXBUFFERDYNAMIC_HPP
#include "IndexType.hpp"
#include "Renderer.hpp"
#include "ScopeGuard.hpp"
#include "UniqueValue.hpp"
#include "Vertex.hpp"
namespace glengine
{
class IndexBufferDynamic
{
   private:
     Glid      m_renderer_id = {};
     IndexType m_type        = {};

   public:
     IndexBufferDynamic() = default;
     IndexBufferDynamic(std::size_t count)
       : IndexBufferDynamic(QuadIndices(count))
     {
     }
     template<std::ranges::contiguous_range R>
     // clang-format off
  requires (std::unsigned_integral<std::ranges::range_value_t<R>>
    && (sizeof(std::ranges::range_value_t<R>) <= sizeof(std::uint32_t)))
     // clang-format on
     IndexBufferDynamic(const R &buffer)
       : m_renderer_id(
           [&buffer]() -> std::uint32_t {
                const auto           pop_backup = backup();
                std::uint32_t        tmp{};
                const std::ptrdiff_t size_in_bytes =
                  static_cast<std::ptrdiff_t>(std::ranges::size(buffer) * sizeof(std::ranges::range_value_t<R>));
                const void *data = std::ranges::data(buffer);
                GlCall{}(glGenBuffers, 1, &tmp);
                GlCall{}(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, tmp);
                GlCall{}(glBufferData, GL_ELEMENT_ARRAY_BUFFER, size_in_bytes, data, GL_STATIC_DRAW);
                return tmp;
           }(),
           [](const std::uint32_t id) {
                GlCall{}(glDeleteBuffers, 1, &id);
                IndexBufferDynamic::unbind();
           })
       , m_type(GetIndexType<std::ranges::range_value_t<R>>())
     {
     }

     void        bind() const;
     static void unbind();
     static auto backup()
     {
          GLint ibo_binding{ 0 };// save original
          GlCall{}(glGetIntegerv, GL_ELEMENT_ARRAY_BUFFER_BINDING, &ibo_binding);
          return ScopeGuardCaptures{ [=]() { GlCall{}(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, ibo_binding); } };
     }
     IndexType type() const;
};
static_assert(Bindable<IndexBufferDynamic> && has_Type_for_IndexType<IndexBufferDynamic>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_INDEXBUFFERDYNAMIC_HPP
