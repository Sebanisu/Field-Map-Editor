//
// Created by pcvii on 11/29/2021.
//
#include "TestMenu.hpp"
#include "TestBatchQuads.hpp"
#include "TestBatchRenderer.hpp"
#include "TestBatchRendering.hpp"
#include "TestBatchRenderingTexture2D.hpp"
#include "TestBatchRenderingTexture2DDynamic.hpp"
#include "TestClearColor.hpp"
#include "TestFF8Archive.hpp"
#include "TestTexture2D.hpp"

static_assert(test::Test<test::TestMenu>);

test::TestMenu::TestMenu()
  : m_menu(
    Menu::MenuElementType<TestClearColor>{ "Test Clear Color" },
    Menu::MenuElementType<TestTexture2D>{ "Test Texture2D" },
    Menu::MenuElementType<TestBatchRendering>{ "Test Batch Rendering" },
    Menu::MenuElementType<TestBatchRenderingTexture2D>{
      "Test Batch Rendering with Texture2D" },
    Menu::MenuElementType<TestBatchRenderingTexture2DDynamic>{
      "Test Batch Rendering with Texture2D Dynamic" },
    Menu::MenuElementType<TestBatchQuads>{ "Test Batch Rendering with Quads" },
    Menu::MenuElementType<TestBatchRenderer>{ "Test Batch Renderer Class" },
    Menu::MenuElementType<TestFF8Archive>{ "Test FF8 Archive Loading" })
{
}
void test::TestMenu::OnRender() const
{
  m_menu.OnRender();
}
void test::TestMenu::OnImGuiUpdate() const
{
  m_menu.OnImGuiUpdate();
}
void test::TestMenu::OnUpdate(float delta_time) const
{
  m_menu.OnUpdate(delta_time);
}
void test::TestMenu::OnEvent(const Event::Item &e) const
{
  m_menu.OnEvent(e);
}