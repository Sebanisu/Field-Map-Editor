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

static_assert(glengine::Renderable<test::TestMenu>);

test::TestMenu::TestMenu()
  : m_menu(
    glengine::Menu::MenuElementType<TestClearColor>{ "Test Clear Color" },
    glengine::Menu::MenuElementType<TestTexture2D>{ "Test Texture2D" },
    glengine::Menu::MenuElementType<TestBatchRendering>{
      "Test Batch Rendering" },
    glengine::Menu::MenuElementType<TestBatchRenderingTexture2D>{
      "Test Batch Rendering with Texture2D" },
    glengine::Menu::MenuElementType<TestBatchRenderingTexture2DDynamic>{
      "Test Batch Rendering with Texture2D Dynamic" },
    glengine::Menu::MenuElementType<TestBatchQuads>{
      "Test Batch Rendering with Quads" },
    glengine::Menu::MenuElementType<TestBatchRenderer>{
      "Test Batch Renderer Class" },
    glengine::Menu::MenuElementType<TestFf8Archive>{
      "Test FF8 Archive Loading" })
{
}
void test::TestMenu::on_render() const
{
  m_menu.on_render();
}
void test::TestMenu::on_im_gui_update() const
{
  m_menu.on_im_gui_update();
}
void test::TestMenu::on_update(float delta_time) const
{
  m_menu.on_update(delta_time);
}
void test::TestMenu::on_event(const glengine::event::Item &e) const
{
  m_menu.on_event(e);
}