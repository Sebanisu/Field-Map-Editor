//
// Created by pcvii on 11/29/2021.
//

#ifndef MYPROJECT_TESTMENUITEM_HPP
#define MYPROJECT_TESTMENUITEM_HPP

#include "Test.hpp"
#include "test_dummy.hpp"
#include <memory>
namespace test
{
void
  OnUpdate();
void
  OnRender();
void
  OnImGuiRender();
class TestMenuItem
{
private:
  class TestMenuItemConcept
  {
  protected:
    TestMenuItemConcept()                                = default;
    TestMenuItemConcept(const TestMenuItemConcept &)     = default;
    TestMenuItemConcept(TestMenuItemConcept &&) noexcept = default;
    TestMenuItemConcept &
      operator=(const TestMenuItemConcept &) = default;
    TestMenuItemConcept &
      operator=(TestMenuItemConcept &&) noexcept = default;

  public:
    virtual ~TestMenuItemConcept(){};
    virtual void
      OnUpdate(float) const = 0;
    virtual void
      OnRender() const = 0;
    virtual void
      OnImGuiRender() const = 0;
  };
  template<Test testT>
  class TestMenuItemModel final : public TestMenuItemConcept
  {
  public:
    TestMenuItemModel(testT t)
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

    TestMenuItemModel() = default;

  private:
    testT m_test;
  };

  friend void
    OnUpdate(const TestMenuItem &menu_item, float ts);
  friend void
    OnRender(const TestMenuItem &menu_item);
  friend void
    OnImGuiRender(const TestMenuItem &menu_item);

  mutable std::unique_ptr<const TestMenuItemConcept> m_impl{ nullptr };

public:
  TestMenuItem()
    : m_impl(nullptr)
  {
  }
  template<typename T, typename... argsT>
  TestMenuItem(test_dummy<T>, argsT &&...args) noexcept
    : m_impl(std::make_unique<TestMenuItemModel<std::decay_t<T>>>(
      std::forward<argsT>(args)...))
  {
  }
  template<typename T>
  TestMenuItem(T t)
    : TestMenuItem(test::test_dummy<T>{}, std::move(t))
  {
  }
  TestMenuItem(const TestMenuItem &other) = delete;
  TestMenuItem &
    operator=(const TestMenuItem &other)      = delete;
  TestMenuItem(TestMenuItem &&other) noexcept = default;
  TestMenuItem &
    operator=(TestMenuItem &&other) noexcept = default;

    operator bool() const;
};
void
  OnUpdate(const TestMenuItem &menu_item, float ts);
void
  OnRender(const TestMenuItem &menu_item);
void
  OnImGuiRender(const TestMenuItem &menu_item);
void
  swap(TestMenuItem &left, TestMenuItem &right) noexcept;
}// namespace test
#endif// MYPROJECT_TESTMENUITEM_HPP
