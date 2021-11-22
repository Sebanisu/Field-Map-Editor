//
// Created by pcvii on 11/22/2021.
//

#ifndef MYPROJECT_VERTEXBUFFERLAYOUT_HPP
#define MYPROJECT_VERTEXBUFFERLAYOUT_HPP
#include <cassert>
#include <GL/glew.h>
#include <vector>
class VertexBufferLayout
{
public:
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

#endif// MYPROJECT_VERTEXBUFFERLAYOUT_HPP
