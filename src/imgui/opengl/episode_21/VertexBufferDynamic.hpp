//
// Created by pcvii on 11/21/2021.
//

#ifndef MYPROJECT_VertexBufferDynamic_HPP
#define MYPROJECT_VertexBufferDynamic_HPP
#include "Renderer.hpp"
#include "Vertex.hpp"
#include <cstdint>
#include <ranges>
class IndexBufferDynamicSize
{
private:
  std::size_t m_size{};

public:
  constexpr IndexBufferDynamicSize() noexcept = default;
  constexpr IndexBufferDynamicSize(std::size_t in_size) noexcept
    : m_size(in_size)
  {
  }
  constexpr std::size_t
    size() const noexcept
  {
    return m_size;
  }
  constexpr void
    Bind() const noexcept
  {
  }

  constexpr void
    UnBind() const noexcept
  {
  }
};
static_assert(
  SizedBindable<
    IndexBufferDynamicSize> && std::copyable<IndexBufferDynamicSize>);
class VertexBufferDynamic
{
private:
  std::uint32_t m_renderer_id = {};
  std::size_t   m_max_size{};

public:
  VertexBufferDynamic() = default;
  VertexBufferDynamic(size_t count)
    : m_max_size(count * 4U)
  {
    GLCall{ glGenBuffers, 1, &m_renderer_id };
    GLCall{ glBindBuffer, GL_ARRAY_BUFFER, m_renderer_id };
    GLCall{ glBufferData,
            GL_ARRAY_BUFFER,
            static_cast<std::ptrdiff_t>(count * sizeof(Quad)),
            nullptr,
            GL_DYNAMIC_DRAW };
  }
  ~VertexBufferDynamic();

  VertexBufferDynamic(const VertexBufferDynamic &) = delete;
  VertexBufferDynamic &
    operator=(const VertexBufferDynamic &) = delete;

  VertexBufferDynamic(VertexBufferDynamic &&other) noexcept;
  VertexBufferDynamic &
    operator=(VertexBufferDynamic &&other) noexcept;

  void
    Bind() const;
  void
    UnBind() const;
  template<std::ranges::contiguous_range T>
  [[nodiscard]] IndexBufferDynamicSize
    Update(const T &vertices) const
  {
    assert(std::ranges::size(vertices) <= m_max_size);
    Bind();
    GLCall{ glBufferSubData,
            GL_ARRAY_BUFFER,
            0,
            static_cast<std::ptrdiff_t>(
              std::ranges::size(vertices)
              * sizeof(std::ranges::range_value_t<decltype(vertices)>)),
            std::ranges::data(vertices) };
    return IndexBufferDynamicSize(
      (std::ranges::size(vertices) / std::size(Quad{}) * 6U));
  }


  friend void
    swap(VertexBufferDynamic &first, VertexBufferDynamic &second) noexcept;
};
static_assert(Bindable<VertexBufferDynamic>);

#endif// MYPROJECT_VertexBufferDynamic_HPP
