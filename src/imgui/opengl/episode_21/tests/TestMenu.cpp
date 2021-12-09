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
#include "TestFF8Archive.hpp"
#include "TestTexture2D.hpp"
#include <imgui.h>
static_assert(test::Test<test::TestMenu>);
void test::TestMenu::OnRender() const
{
  m_current.OnRender();
}
void test::TestMenu::OnImGuiUpdate() const
{
  if (m_current)
  {
    m_current.OnImGuiUpdate();
    if (ImGui::Button("Back"))
    {
      m_current = TestMenuItem{};
    }
  }
  else
  {
    for (const auto &[name, funt] : m_list)
    {
      if (ImGui::Button(name.c_str()))
      {
        m_current = funt();
        break;
      }
    }
  }
}

void test::TestMenu::OnUpdate(float delta_time) const
{
  m_current.OnUpdate(delta_time);
}

void test::TestMenu::push_back(
  std::string                 name,
  std::function<test_types()> funt) const
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

void test::TestMenu::OnEvent(const Event::Item &e) const
{
  m_current.OnEvent(e);
}
