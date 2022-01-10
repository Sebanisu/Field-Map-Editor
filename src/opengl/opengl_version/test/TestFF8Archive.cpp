//
// Created by pcvii on 11/30/2021.
//
#include "TestFF8Archive.hpp"

void test::TestFF8Archive::OnUpdate(float ts) const
{
  ff8_menu.OnUpdate(ts);
}
void test::TestFF8Archive::OnRender() const
{
  ff8_menu.OnRender();
}
void test::TestFF8Archive::OnImGuiUpdate() const
{
  ff8_menu.OnImGuiUpdate();
}
void test::TestFF8Archive::OnEvent(const glengine::Event::Item &e) const
{
  ff8_menu.OnEvent(e);
}