//
// Created by pcvii on 11/29/2021.
//
#include "TestMenu.hpp"
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