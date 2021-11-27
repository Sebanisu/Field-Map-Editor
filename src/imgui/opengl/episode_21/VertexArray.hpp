//
// Created by pcvii on 11/22/2021.
//

#ifndef MYPROJECT_VERTEXARRAY_HPP
#define MYPROJECT_VERTEXARRAY_HPP
#include "Renderer.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include <algorithm>
#include <ranges>
class VertexArray
{
private:
  std::uint32_t m_renderer_id{};

public:
  VertexArray();
  ~VertexArray();


  VertexArray(const VertexArray &) = delete;
  VertexArray &
    operator=(const VertexArray &) = delete;

  VertexArray(VertexArray &&other) noexcept;

  VertexArray &
    operator=(VertexArray &&other) noexcept;

  void
    Bind() const;
  void
    UnBind() const;
  void
    push_back(const Bindable auto &vb, const VertexBufferLayout &layout)
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
  friend void
    swap(VertexArray &first, VertexArray &second) noexcept;
};
static_assert(Bindable<VertexArray>);
#endif// MYPROJECT_VERTEXARRAY_HPP
