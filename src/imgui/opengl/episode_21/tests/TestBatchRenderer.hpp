//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_TESTBATCHRENDERER_HPP
#define MYPROJECT_TESTBATCHRENDERER_HPP
#include "Test.hpp"
#include <BatchRenderer.hpp>
#include <imgui.h>
#include <scope_guard.hpp>

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
  void
                              GenerateQuads() const;
  BatchRenderer               m_batch_renderer{};
  mutable std::array<int, 2U> m_count = { 100, 100 };
};
inline void
  OnUpdate(const TestBatchRenderer &self, float ts)
{
  OnUpdate(self.m_batch_renderer, ts);
}
inline void
  OnRender(const TestBatchRenderer &self)
{
  // OnRender(self.m_batch_renderer);
  self.GenerateQuads();
}
inline void
  OnImGuiRender(const TestBatchRenderer &self)
{
  const auto pop = scope_guard{ &ImGui::PopID };
  int        id{};
  ImGui::PushID(++id);
  if (ImGui::SliderInt2("Quad (X, Y)", std::data(self.m_count), 0, 100))
  {
  }
  ImGui::Text(
    "%s",
    fmt::format("Total Quads Rendered: {}", self.m_count[0] * self.m_count[1])
      .c_str());
  ImGui::Text(
    "%s",
    fmt::format(
      "Total Verts Rendered: {}", self.m_count[0] * self.m_count[1] * 4U)
      .c_str());
  OnImGuiRender(self.m_batch_renderer);
}
static_assert(Test<TestBatchRenderer>);
}// namespace test
#endif// MYPROJECT_TESTBATCHRENDERER_HPP
