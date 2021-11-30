//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TESTMENU_HPP
#define MYPROJECT_TESTMENU_HPP
#include "Test.h"
#include "TestBatchRendering.hpp"
#include "TestBatchRenderingTexture2D.hpp"
#include "TestBatchRenderingTexture2DDynamic.hpp"
#include "TestClearColor.hpp"
#include "TestTexture2D.hpp"
#include <functional>
#include <imgui.h>
#include <string>
#include <variant>
#include <vector>
namespace test
{
class MenuItem
{
private:
  class MenuItemConcept
  {
  protected:
    MenuItemConcept()                            = default;
    MenuItemConcept(const MenuItemConcept &)     = default;
    MenuItemConcept(MenuItemConcept &&) noexcept = default;
    MenuItemConcept &
      operator=(const MenuItemConcept &) = default;
    MenuItemConcept &
      operator=(MenuItemConcept &&) noexcept = default;

  public:
    virtual ~MenuItemConcept(){};
    virtual void
      OnUpdate(float) const = 0;
    virtual void
      OnRender() const = 0;
    virtual void
      OnImGuiRender() const = 0;
  };
  template<Test testT>
  class MenuItemModel final : public MenuItemConcept
  {
  public:
    MenuItemModel(testT t)
      : m_test(std::move(t))
    {
    }
    void
      OnUpdate(float ts) const final
    {
      using test::OnUpdate;
      return OnUpdate(m_test, ts);
    }
    void
      OnRender() const final
    {
      using test::OnRender;
      return OnRender(m_test);
    }
    void
      OnImGuiRender() const final
    {
      using test::OnImGuiRender;
      return OnImGuiRender(m_test);
    }

  private:
    MenuItemModel() = default;
    testT m_test;
  };

  friend void
    OnUpdate(const MenuItem &menu_item, float ts)
  {
    if (menu_item)
    {
      return menu_item.m_impl->OnUpdate(ts);
    }
  }
  friend void
    OnRender(const MenuItem &menu_item)
  {
    if (menu_item)
    {
      return menu_item.m_impl->OnRender();
    }
  }
  friend void
    OnImGuiRender(const MenuItem &menu_item)
  {
    if (menu_item)
    {
      return menu_item.m_impl->OnImGuiRender();
    }
  }

  mutable std::unique_ptr<const MenuItemConcept> m_impl{ nullptr };

public:
  MenuItem()
    : m_impl(nullptr)
  {
  }
  template<typename T>
  MenuItem(T &&test) noexcept
    : MenuItem()
  {
    if constexpr (decay_same_as<T, MenuItem>)
    {
      swap(*this, test);
    }
    else
    {
      using std::swap;
      static_assert(Test<T>);
      m_impl =
        std::make_unique<MenuItemModel<std::decay_t<T>>>(std::move(test));
    }
  }
  MenuItem(const MenuItem &other) = delete;
  MenuItem &
    operator=(const MenuItem &other) = delete;
  MenuItem(MenuItem &&other) noexcept
    : MenuItem()
  {
    swap(*this, other);
  }
  MenuItem &
    operator=(MenuItem &&other) noexcept
  {
    swap(*this, other);
    return *this;
  }

  operator bool() const
  {
    return bool{ m_impl };
  }
  friend void
    swap(MenuItem &left, MenuItem &right) noexcept
  {
    using std::swap;
    swap(left.m_impl, right.m_impl);
  }
};
class TestMenu
{
public:
  using test_types = MenuItem;
  TestMenu()
    : TestMenu(MenuItem{})
  {
  }
  TestMenu(test_types current)
    : m_current(std::move(current))
  {
    push_back<TestClearColor>("Test Clear Color");
    push_back<TestTexture2D>("Test Texture2D");
    push_back<TestBatchRendering>("Test Batch Rendering");
    push_back<TestBatchRenderingTexture2D>(
      "Test Batch Rendering with Texture2D");
    push_back<TestBatchRenderingTexture2DDynamic>(
      "Test Batch Rendering with Texture2D Dynamic");
  }
  TestMenu(TestMenu &&other) noexcept
    : TestMenu()
  {
    swap(*this, other);
  }
  TestMenu &
    operator=(TestMenu &&other) noexcept
  {
    swap(*this, other);
    return *this;
  }
  friend void
    OnUpdate(const TestMenu &, float);
  friend void
    OnRender(const TestMenu &);
  friend void
    OnImGuiRender(const TestMenu &);
  template<Test T>
  void
    push_back(std::string name) const
  {
    push_back(std::move(name), []() -> test_types { return MenuItem(T{}); });
  }
  void
    push_back(std::string name, std::function<test_types()> funt) const
  {
    m_list.emplace_back(std::move(name), std::move(funt));
  }

  friend void
    swap(TestMenu &left, TestMenu &right) noexcept;

private:
  mutable test_types m_current = {};
  mutable std::vector<std::pair<std::string, std::function<test_types()>>>
    m_list = {};
};
void
  OnRender(const TestMenu &self);
void
  OnImGuiRender(const TestMenu &self);
void
  OnUpdate(const TestMenu &self, float delta_time);
void
  swap(TestMenu &left, TestMenu &right) noexcept;
}// namespace test
#endif// MYPROJECT_TESTMENU_HPP
