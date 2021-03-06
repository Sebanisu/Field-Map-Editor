//
// Created by pcvii on 11/22/2021.
//

#include "VertexArray.hpp"
namespace glengine
{
VertexArray::VertexArray()
  : m_renderer_id{ []() -> std::uint32_t {
                    std::uint32_t tmp{};
                    GlCall{}(glGenVertexArrays, 1, &tmp);
                    return tmp;
                  }(),
                   [](const std::uint32_t id) {
                     GlCall{}(glDeleteVertexArrays, 1, &id);
                     VertexArray::unbind();
                   } }
{
}
void VertexArray::bind() const
{
  GlCall{}(glBindVertexArray, m_renderer_id);
}
void VertexArray::unbind()
{

  GlCall{}(glBindVertexArray, 0U);
}
}// namespace glengine