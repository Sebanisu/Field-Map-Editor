//
// Created by pcvii on 11/22/2021.
//

#ifndef MYPROJECT_VERTEXBUFFERLAYOUT_HPP
#define MYPROJECT_VERTEXBUFFERLAYOUT_HPP
#include <cassert>
#include <GL/glew.h>
#include <vector>
template<typename T, typename U>
concept decay_same_as = std::is_same_v<std::decay_t<T>, std::decay_t<U>>;
template<typename T>
concept is_VertexBufferElementType = requires(T t)
{
  typename std::decay_t<T>::value_type;
  {
    t.count
    } -> decay_same_as<std::uint32_t>;
};
class VertexBufferLayout
{
public:
  VertexBufferLayout() = default;
  template<typename T>
  struct VertexBufferElementType
  {
    using value_type = T;
    std::uint32_t count;
  };
  template<is_VertexBufferElementType... Ts>
  VertexBufferLayout(Ts... ts)
  {
    ((void)push_back<typename Ts::value_type>(ts.count), ...);
  }
  struct VertexBufferElement
  {
    std::uint32_t type       = {};
    std::uint32_t count      = {};
    std::uint8_t  normalized = {};
    static std::size_t
      size_of_type(std::uint32_t type);
    std::size_t
      size() const;
  };
  template<typename T>
  void push_back(std::uint32_t) = delete;
  const std::vector<VertexBufferElement> &
    elements() const noexcept;
  std::size_t
    stride() const noexcept;

private:
  std::size_t                      m_stride   = {};
  std::vector<VertexBufferElement> m_elements = {};
};

template<>
inline void
  VertexBufferLayout::push_back<float>(std::uint32_t count)
{
  m_stride +=
    m_elements.emplace_back(GL_FLOAT, count, std::uint8_t{ GL_FALSE }).size();
}
template<>
inline void
  VertexBufferLayout::push_back<std::uint32_t>(std::uint32_t count)
{
  m_stride +=
    m_elements.emplace_back(GL_UNSIGNED_INT, count, std::uint8_t{ GL_FALSE })
      .size();
}
template<>
inline void
  VertexBufferLayout::push_back<std::uint8_t>(std::uint32_t count)
{
  m_stride +=
    m_elements.emplace_back(GL_UNSIGNED_BYTE, count, std::uint8_t{ GL_TRUE })
      .size();
}
#endif// MYPROJECT_VERTEXBUFFERLAYOUT_HPP
