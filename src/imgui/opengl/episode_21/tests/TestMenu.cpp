//
// Created by pcvii on 11/29/2021.
//
#include "TestMenu.hpp"
#include "Test.h"
#include "TestBatchQuads.hpp"
#include "TestBatchRendering.hpp"
#include "TestBatchRenderingTexture2D.hpp"
#include "TestBatchRenderingTexture2DDynamic.hpp"
#include "TestClearColor.hpp"
#include "TestTexture2D.hpp"
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
      self.m_current = MenuItem{};
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
  test::swap(TestMenu &left, TestMenu &right) noexcept
{
  using std::swap;
  swap(left.m_current, right.m_current);
  swap(left.m_list, right.m_list);
}
void
  test::TestMenu::push_back(std::string name, std::function<test_types()> funt)
    const
{
  m_list.emplace_back(std::move(name), std::move(funt));
}
test::TestMenu &
  test::TestMenu::operator=(test::TestMenu &&other) noexcept
{
  swap(*this, other);
  return *this;
}
test::TestMenu::TestMenu(test::TestMenu &&other) noexcept
  : TestMenu()
{
  swap(*this, other);
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
}
test::TestMenu::TestMenu()
  : TestMenu(MenuItem{})
{
}
