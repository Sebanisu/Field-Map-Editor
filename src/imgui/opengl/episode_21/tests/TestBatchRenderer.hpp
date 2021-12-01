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
  TestBatchRenderer()
    : m_textures(
      []()
      {
        std::vector<Texture> r{};
        r.emplace_back(
          std::filesystem::current_path() / "res" / "textures" / "logo.png");
        r.emplace_back(
          std::filesystem::current_path() / "res" / "textures"
          / "mitchell-luo-q9ZiOzsMAhE-unsplash.png");
        r.emplace_back(
          std::filesystem::current_path() / "res" / "textures"
          / "math-yDq60_c-g2E-unsplash.png");
        return r;
      }())
  {
  }
  friend void
    OnUpdate(const TestBatchRenderer &, float);
  friend void
    OnRender(const TestBatchRenderer &);
  friend void
    OnImGuiRender(const TestBatchRenderer &);

private:
  void
    GenerateQuads() const;
  void
                                     SetUniforms() const;
  BatchRenderer                      m_batch_renderer= {10000};
  mutable std::vector<Texture>       m_textures      = {};
  mutable std::array<int, 2U>        m_count         = { 100, 100 };
  mutable glm::vec3                  view_offset     = { -2.F, -1.F, 0.F };
  mutable float                      m_zoom          = { 0.078F };
};
inline void
  OnUpdate(const TestBatchRenderer &self, float ts)
{
  OnUpdate(self.m_batch_renderer, ts);
}
inline void
  OnRender(const TestBatchRenderer &self)
{
  self.SetUniforms();
  // OnRender(self.m_batch_renderer);
  self.GenerateQuads();
}
inline void
  OnImGuiRender(const TestBatchRenderer &self)
{
  int window_width = 16;
  int id           = 0;

  {
    const auto pop = scope_guard(&ImGui::PopID);
    ImGui::PushID(++id);
    if (ImGui::SliderFloat2(
          "View Offset",
          &self.view_offset.x,
          -static_cast<float>(window_width),
          static_cast<float>(window_width)))
    {
    }
  }
  {
    const auto pop = scope_guard{ &ImGui::PopID };
    ImGui::PushID(++id);
    if (ImGui::SliderFloat("Zoom", &self.m_zoom, 4.F, .001F))
    {
    }
  }
  {
    const auto pop = scope_guard{ &ImGui::PopID };
    ImGui::PushID(++id);
    if (ImGui::SliderInt2(
          "Quad Axis Count (X, Y)", std::data(self.m_count), 0, 256))
    {
    }
  }
  ImGui::Text(
    "%s",
    fmt::format("Total Quads Rendered: {}", self.m_count[0] * self.m_count[1])
      .c_str());
  ImGui::Text(
    "%s",
    fmt::format(
      "Total Vertices Rendered: {}", self.m_count[0] * self.m_count[1] * 4)
      .c_str());
  ImGui::Text(
    "%s",
    fmt::format(
      "Total Indices Rendered: {}", self.m_count[0] * self.m_count[1] * 6)
      .c_str());
  OnImGuiRender(self.m_batch_renderer);
}
inline void
  test::TestBatchRenderer::SetUniforms() const
{
  const float window_width  = 16.F;
  const float window_height = 9.F;
  const auto  proj          = glm::ortho(
              view_offset.x / m_zoom,
              (view_offset.x + window_width) / m_zoom,
              view_offset.y / m_zoom,
              (view_offset.y + window_height) / m_zoom,
              -1.F,
              1.F);

  const auto mvp = proj;
  m_batch_renderer.Shader().Bind();
  m_batch_renderer.Shader().SetUniform("u_MVP", mvp);
  m_batch_renderer.Shader().SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}
static_assert(Test<TestBatchRenderer>);
}// namespace test
#endif// MYPROJECT_TESTBATCHRENDERER_HPP
