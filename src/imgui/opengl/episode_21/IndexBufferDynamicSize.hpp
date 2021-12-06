//
// Created by pcvii on 12/6/2021.
//

#ifndef MYPROJECT_INDEXBUFFERDYNAMICSIZE_HPP
#define MYPROJECT_INDEXBUFFERDYNAMICSIZE_HPP
#include <cstdint>
#include "concepts.hpp"
/**
 * This is a bindable holder for the size of the used index buffer. To be passed
 * into renderer. The Renderer will call size() and get the size out of it.
 */
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

  static constexpr void
    UnBind() noexcept
  {
  }
};
static_assert(
  SizedBindable<
    IndexBufferDynamicSize> && std::copyable<IndexBufferDynamicSize>);
#endif// MYPROJECT_INDEXBUFFERDYNAMICSIZE_HPP
