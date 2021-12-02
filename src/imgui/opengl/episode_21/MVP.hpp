//
// Created by pcvii on 12/2/2021.
//

#ifndef MYPROJECT_MVP_HPP
#define MYPROJECT_MVP_HPP
#include "Shader.hpp"
#include "tests/Test.hpp"
class MVP
{
public:
  friend void
    OnUpdate(const MVP &, float);
  friend void
    OnRender(const MVP &);
  friend bool
    OnImGuiRender(const MVP &);
  friend void
    SetUniforms(const MVP &, const Shader &, const float, const glm::vec4);

private:
  mutable glm::vec2 m_view_offset{};
  mutable float m_zoom{1.F};
};
static_assert(test::Test<MVP>);

void
  OnUpdate(const MVP &, float);
void
  OnRender(const MVP &);
bool
  OnImGuiRender(const MVP &);
void
  SetUniforms(
    const MVP &,
    const Shader &,
    const float     height = 720.F,
    const glm::vec4 color  = glm::vec4{ 1.F });
#endif// MYPROJECT_MVP_HPP
