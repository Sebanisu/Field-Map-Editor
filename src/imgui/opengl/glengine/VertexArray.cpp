//
// Created by pcvii on 11/22/2021.
//

#include "VertexArray.hpp"
namespace glengine
{
VertexArray::VertexArray()
  : m_renderer_id{ []() -> std::uint32_t {
                    std::uint32_t tmp{};
                    GLCall{}(glGenVertexArrays, 1, &tmp);
                    return tmp;
                  }(),
                   [](const std::uint32_t id) {
                     GLCall{}(glDeleteVertexArrays, 1, &id);
                     VertexArray::UnBind();
                   } }
{
}
void VertexArray::Bind() const
{
  GLCall{}(glBindVertexArray, m_renderer_id);
}
void VertexArray::UnBind()
{

  GLCall{}(glBindVertexArray, 0U);
}
}// namespace glengine