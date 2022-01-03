//
// Created by pcvii on 11/21/2021.
//

#ifndef MYPROJECT_VertexBufferDynamic_HPP
#define MYPROJECT_VertexBufferDynamic_HPP
#include "IndexBufferDynamicSize.hpp"
#include "Renderer.hpp"
#include "unique_value.hpp"
#include "Vertex.hpp"


class VertexBufferDynamic
{
private:
  GLID        m_renderer_id = {};
  std::size_t m_max_size{};

public:
  VertexBufferDynamic() = default;
  VertexBufferDynamic(size_t count);
  void        Bind() const;
  static void UnBind();
  template<std::ranges::contiguous_range T>
  [[nodiscard]] IndexBufferDynamicSize Update(const T &vertices) const
  {
    assert(std::ranges::size(vertices) <= m_max_size);
    Bind();
    GLCall{}(
      glBufferSubData,
      GL_ARRAY_BUFFER,
      0,
      static_cast<std::ptrdiff_t>(
        std::ranges::size(vertices) * sizeof(std::ranges::range_value_t<T>)),
      std::ranges::data(vertices));
    return IndexBufferDynamicSize(
      (std::ranges::size(vertices) / std::size(Quad{}) * 6U));
  }
};
static_assert(Bindable<VertexBufferDynamic>);

#endif// MYPROJECT_VertexBufferDynamic_HPP
