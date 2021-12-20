//
// Created by pcvii on 11/21/2021.
//

#include "Renderer.hpp"

void Renderer::Clear_impl::Color(glm::vec4 color)
{
  m_color = std::move(color);
}
void Renderer::Clear_impl::Color(float r, float g, float b, float a)
{
  Color({ std::move(r), std::move(g), std::move(b), std::move(a) });
}
void Renderer::Clear_impl::operator()() const
{
  GLCall{}(
    glClearColor, m_color.r, m_color.g, m_color.b, m_color.a);
  GLCall{}(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
