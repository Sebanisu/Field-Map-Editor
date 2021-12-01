//
// Created by pcvii on 11/29/2021.
//
#include "TestMenu.hpp"
#include "Test.hpp"
#include "TestBatchQuads.hpp"
#include "TestBatchRenderer.hpp"
#include "TestBatchRendering.hpp"
#include "TestBatchRenderingTexture2D.hpp"
#include "TestBatchRenderingTexture2DDynamic.hpp"
#include "TestClearColor.hpp"
#include "TestTexture2D.hpp"
#include "TestFF8Archive.hpp"
#include <imgui.h>
static_assert(test::Test<test::TestMenu>);
void
  test::OnRender(const TestMenu &self)
{
  OnRender(self.m_current);
}
void
  test::OnImGuiRender(const TestMenu &self)
{
  if (self.m_current)
  {
    OnImGuiRender(self.m_current);
    if (ImGui::Button("Back"))
    {
      self.m_current = TestMenuItem{};
    }
  }
  else
  {
    for (const auto &[name, funt] : self.m_list)
    {
      if (ImGui::Button(name.c_str()))
      {
        self.m_current = funt();
        break;
      }
    }
  }
}

void
  test::OnUpdate(const TestMenu &self, float delta_time)
{
  OnUpdate(self.m_current, delta_time);
}

void
  test::TestMenu::push_back(std::string name, std::function<test_types()> funt)
    const
{
  m_list.emplace_back(std::move(name), std::move(funt));
}

test::TestMenu::TestMenu(test::TestMenu::test_types current)
  : m_current(std::move(current))
{
  push_back<TestClearColor>("Test Clear Color");
  push_back<TestTexture2D>("Test Texture2D");
  push_back<TestBatchRendering>("Test Batch Rendering");
  push_back<TestBatchRenderingTexture2D>("Test Batch Rendering with Texture2D");
  push_back<TestBatchRenderingTexture2DDynamic>(
    "Test Batch Rendering with Texture2D Dynamic");
  push_back<TestBatchQuads>("Test Batch Rendering with Quads");
  push_back<TestBatchRenderer>("Test Batch Renderer Class");
  push_back<TestFF8Archive>("Test FF8 Archive Loading");
}
test::TestMenu::TestMenu()
  : TestMenu(TestMenuItem{})
{
}
