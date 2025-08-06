//
// Created by pcvii on 11/21/2021.
//

#ifndef FIELD_MAP_EDITOR_VERTEXBUFFERDYNAMIC_HPP
#define FIELD_MAP_EDITOR_VERTEXBUFFERDYNAMIC_HPP
#include "IndexBufferDynamicSize.hpp"
#include "Renderer.hpp"
#include "ScopeGuard.hpp"
#include "UniqueValue.hpp"
#include "Vertex.hpp"
namespace glengine
{
class VertexBufferDynamic
{
   private:
     Glid        m_renderer_id = {};
     std::size_t m_max_size{};

   public:
     // A quad is made of 2 triangles, each needing 3 indices → 2 × 3 = 6
     static constexpr std::size_t IndicesPerQuad = 2 * 3;

     VertexBufferDynamic()                       = default;
     VertexBufferDynamic(size_t count);
     void                      bind() const;
     static void               unbind();
     [[nodiscard]] static auto backup()
     {
          GLint vbo_binding{ 0 };
          GlCall{}(glGetIntegerv, GL_ARRAY_BUFFER_BINDING, &vbo_binding);

          return ScopeGuard{ [=]() { glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(vbo_binding)); } };
     }
     template<std::ranges::contiguous_range T>
     [[nodiscard]] glengine::IndexBufferDynamicSize update(const T &vertices) const
     {
          assert(std::ranges::size(vertices) <= m_max_size);
          bind();
          GlCall{}(
            glBufferSubData,
            GL_ARRAY_BUFFER,
            0,
            static_cast<std::ptrdiff_t>(std::ranges::size(vertices) * sizeof(std::ranges::range_value_t<T>)),
            std::ranges::data(vertices));
          return glengine::IndexBufferDynamicSize((std::ranges::size(vertices) / std::size(Quad{}) * IndicesPerQuad));
     }
};
static_assert(Bindable<VertexBufferDynamic>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_VERTEXBUFFERDYNAMIC_HPP
