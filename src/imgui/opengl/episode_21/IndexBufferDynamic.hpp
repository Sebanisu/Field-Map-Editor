//
// Created by pcvii on 11/21/2021.
//

#ifndef MYPROJECT_IndexBufferDynamic_HPP
#define MYPROJECT_IndexBufferDynamic_HPP
#include "Renderer.hpp"
#include <cstdint>
class IndexBufferDynamic
{
private:
  std::uint32_t m_renderer_id = {};
  std::size_t   m_size        = {};

public:
  IndexBufferDynamic()
  {
    GLCall{ glGenBuffers, 1, &m_renderer_id };
    GLCall{ glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_renderer_id };
    GLCall{
      glBufferData, GL_ELEMENT_ARRAY_BUFFER, 2*3*1000*sizeof(uint32_t),
            nullptr, GL_DYNAMIC_DRAW
    };
  }
  ~IndexBufferDynamic();


  IndexBufferDynamic(const IndexBufferDynamic &) = delete;
  IndexBufferDynamic &
    operator=(const IndexBufferDynamic &) = delete;

  IndexBufferDynamic(IndexBufferDynamic &&other);
  IndexBufferDynamic &
    operator=(IndexBufferDynamic &&other);

  void
    Bind() const;
  void
    UnBind() const;
  std::size_t
    size() const;

  friend void
    swap(IndexBufferDynamic &first, IndexBufferDynamic &second);
};

#endif// MYPROJECT_IndexBufferDynamic_HPP
