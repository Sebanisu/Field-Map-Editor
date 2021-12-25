//
// Created by pcvii on 11/21/2021.
//

#ifndef MYPROJECT_RENDERER_HPP
#define MYPROJECT_RENDERER_HPP

#include "GLCheck.hpp"
#include "IndexType.hpp"

namespace Renderer
{
class Clear_impl
{
public:
  void Color(glm::vec4 color);
  void Color(const float r, const float g, const float b, const float a);
  void operator()() const;

private:
  glm::vec4 m_color{};
};
static inline constinit Clear_impl Clear = {};
template<Bindable... Ts>
inline void Draw(const Ts &...ts)
{
  ::Bind<Ts...>(ts...);
  size_t size = ::size<Ts...>(ts...);
  auto   type = ::Type<Ts...>(ts...);

  assert(
    type == IndexType::UNSIGNED_INT || type == IndexType::UNSIGNED_SHORT
    || type == IndexType::UNSIGNED_BYTE);
  assert(size != 0);
  GLCall{}(
    glDrawElements,
    GL_TRIANGLES,
    static_cast<std::int32_t>(size),
    +type,
    nullptr);
}
};// namespace Renderer
#endif// MYPROJECT_RENDERER_HPP
