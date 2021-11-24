//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TESTMENU_HPP
#define MYPROJECT_TESTMENU_HPP
#include "TestBatchRendering.hpp"
#include "TestClearColor.hpp"
#include "TestTexture2D.hpp"
#include <functional>
#include <string>
#include <variant>
#include <vector>
namespace test
{
class TestMenu
{
public:
  using test_types = std::
    variant<std::monostate, TestClearColor, TestTexture2D, TestBatchRendering>;
  TestMenu()
    : TestMenu(std::monostate{})
  {
  }

  TestMenu(test_types current)
    : m_current(std::move(current))
  {
    push_back<TestClearColor>("Test Clear Color");
    push_back<TestTexture2D>("Test Texture2D");
  }
  void
    OnUpdate(float) const;
  void
    OnRender() const;
  void
    OnImGuiRender() const;
  template<Test T>
  void
    push_back(std::string name) const
  {
    push_back(std::move(name), []() -> test_types { return T{}; });
  }
  void
    push_back(std::string name, std::function<test_types()> funt) const
  {
    m_list.emplace_back(std::move(name), std::move(funt));
  }

private:
  mutable test_types m_current = {};
  mutable std::vector<std::pair<std::string, std::function<test_types()>>>
    m_list = {};
};
static_assert(Test<TestMenu>);
}// namespace test
#endif// MYPROJECT_TESTMENU_HPP
