//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_TESTBATCHRENDERER_HPP
#define MYPROJECT_TESTBATCHRENDERER_HPP
#include "Test.hpp"
#include <BatchRenderer.hpp>
namespace test
{
class TestBatchRenderer
{
public:
  friend void
    OnUpdate(const TestBatchRenderer &, float);
  friend void
    OnRender(const TestBatchRenderer &);
  friend void
    OnImGuiRender(const TestBatchRenderer &);

private:
  BatchRenderer m_batch_renderer{};
};
inline void
  OnUpdate(const TestBatchRenderer &self, float ts)
{
  OnUpdate(self.m_batch_renderer, ts);
}
inline void
  OnRender(const TestBatchRenderer &self)
{
  OnRender(self.m_batch_renderer);
}
inline void
  OnImGuiRender(const TestBatchRenderer &self)
{
  OnImGuiRender(self.m_batch_renderer);
}
static_assert(Test<TestBatchRenderer>);
}// namespace test
#endif// MYPROJECT_TESTBATCHRENDERER_HPP
