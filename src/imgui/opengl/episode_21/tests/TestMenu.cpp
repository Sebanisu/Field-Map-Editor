//
// Created by pcvii on 11/24/2021.
//

#include "TestMenu.hpp"
#include <imgui.h>
namespace test
{

void
  TestMenu::OnRender() const
{
  std::visit(
    []<typename T>(T &test)
    {
      if constexpr (Test<T>)
      {
        test.OnRender();
      }
    },
    m_current);
}
void
  TestMenu::OnImGuiRender() const
{
  std::visit(
    [&]<typename T>(T &test)
    {
      if constexpr (Test<T>)
      {
        test.OnImGuiRender();
        if (ImGui::Button("Back"))
        {
          m_current = std::monostate{};
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
    },
    m_current);
}

void
  TestMenu::OnUpdate(float delta_time) const
{
  std::visit(
    [&delta_time]<typename T>(T &test)
    {
      if constexpr (Test<T>)
      {
        test.OnUpdate(delta_time);
      }
    },
    m_current);
}
}// namespace test
