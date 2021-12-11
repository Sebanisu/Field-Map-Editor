//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_TESTBATCHRENDERER_HPP
#define MYPROJECT_TESTBATCHRENDERER_HPP
#include "BatchRenderer.hpp"
#include "scope_guard.hpp"
#include "Test.hpp"


namespace test
{
class TestBatchRenderer
{
public:
  TestBatchRenderer();
  void OnUpdate(float) const;
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const Event::Item &) const {}

private:
  void                         GenerateQuads() const;
  void                         SetUniforms() const;
  BatchRenderer                m_batch_renderer = { 10000 };
  mutable std::vector<Texture> m_textures       = {};
  mutable std::array<int, 2U>  m_count          = { 100, 100 };
  mutable glm::vec3            view_offset      = { -2.F, -1.F, 0.F };
  mutable float                m_zoom           = { 0.078F };
};
static_assert(Test<TestBatchRenderer>);
}// namespace test
#endif// MYPROJECT_TESTBATCHRENDERER_HPP