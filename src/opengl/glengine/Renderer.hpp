//
// Created by pcvii on 11/21/2021.
//

#ifndef FIELD_MAP_EDITOR_RENDERER_HPP
#define FIELD_MAP_EDITOR_RENDERER_HPP
#include "GLCheck.hpp"
#include "IndexType.hpp"
#include <glm/glm.hpp>
namespace glengine
{
namespace Renderer
{
  class Clear_impl
  {
  public:
    constexpr Clear_impl() = default;
    constexpr Clear_impl(glm::vec4 color)
      : m_color(std::move(color))
    {
    }
    constexpr void Color(glm::vec4 color)
    {
      m_color = std::move(color);
    }
    constexpr void operator()() const
    {
      if (!std::is_constant_evaluated())
      {
        GLCall{}(glClearColor, m_color.r, m_color.g, m_color.b, m_color.a);
        GLCall{}(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      }
    }

  private:
    glm::vec4 m_color{};
  };
  static inline constinit Clear_impl Clear = {};
  template<Bindable... Ts>
  inline void Draw(const Ts &...ts)
  {
    glengine::Bind<Ts...>(ts...);
    size_t size = glengine::size<Ts...>(ts...);
    auto   type = glengine::Type<Ts...>(ts...);

    assert(
      type == glengine::IndexType::UNSIGNED_INT
      || type == glengine::IndexType::UNSIGNED_SHORT
      || type == glengine::IndexType::UNSIGNED_BYTE);
    assert(size != 0);
    GLCall{}(
      glDrawElements,
      GL_TRIANGLES,
      static_cast<std::int32_t>(size),
      +type,
      nullptr);
  }
}// namespace Renderer
}// namespace glengine
#endif// FIELD_MAP_EDITOR_RENDERER_HPP
