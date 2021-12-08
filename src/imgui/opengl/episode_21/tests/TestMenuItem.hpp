//
// Created by pcvii on 11/29/2021.
//

#ifndef MYPROJECT_TESTMENUITEM_HPP
#define MYPROJECT_TESTMENUITEM_HPP

#include "EventItem.hpp"
#include "Test.hpp"
#include <memory>
#include <utility>
namespace test
{
void
  OnUpdate();
void
  OnRender();
void
  OnImGuiUpdate();
void
  OnEvent();

template<typename T>
concept has_OnEvent = requires(const T &t, const Event::Item &e)
{
  test::OnEvent(t, e);
};
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
      OnImGuiUpdate() const = 0;
    virtual void
      OnEvent(const Event::Item &) const = 0;
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
      OnImGuiUpdate() const final
    {
      using test::OnImGuiUpdate;
      return OnImGuiUpdate(m_test);
    }

    void
      OnEvent(const Event::Item &e) const final
    {
      if constexpr (has_OnEvent<testT>)
      {
        using test::OnEvent;
        return OnEvent(m_test, e);
      }
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
    OnImGuiUpdate(const TestMenuItem &menu_item);
  friend void
    OnEvent(const TestMenuItem &, const Event::Item &);

  mutable std::unique_ptr<const TestMenuItemConcept> m_impl{ nullptr };

public:
  TestMenuItem()
    : m_impl(nullptr)
  {
  }
  template<typename T, typename... argsT>
  TestMenuItem(std::in_place_type_t<T>, argsT &&...args) noexcept
    : m_impl(std::make_unique<TestMenuItemModel<std::decay_t<T>>>(
      std::forward<argsT>(args)...))
  {
    static_assert(test::Test<T>);
  }
  template<typename T>
  TestMenuItem(T t)
    : TestMenuItem(std::in_place_type_t<T>{}, std::move(t))
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
  OnImGuiUpdate(const TestMenuItem &menu_item);
void
  OnEvent(const TestMenuItem &, const Event::Item &);
}// namespace test
#endif// MYPROJECT_TESTMENUITEM_HPP
