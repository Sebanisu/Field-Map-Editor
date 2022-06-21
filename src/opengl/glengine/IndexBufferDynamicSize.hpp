//
// Created by pcvii on 12/6/2021.
//

#ifndef FIELD_MAP_EDITOR_INDEXBUFFERDYNAMICSIZE_HPP
#define FIELD_MAP_EDITOR_INDEXBUFFERDYNAMICSIZE_HPP
namespace glengine
{
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
  constexpr std::size_t size() const noexcept
  {
    return m_size;
  }
  constexpr void        bind() const noexcept {}

  static constexpr void unbind() noexcept {}
};
static_assert(
  SizedBindable<
    IndexBufferDynamicSize> && std::copyable<IndexBufferDynamicSize>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_INDEXBUFFERDYNAMICSIZE_HPP
