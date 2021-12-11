//
// Created by pcvii on 11/29/2021.
//

#ifndef MYPROJECT_TestMenuItem_HPP
#define MYPROJECT_TestMenuItem_HPP
#include "EventItem.hpp"
#include "Test.hpp"


namespace test
{
class TestMenuItem
{
private:
  class TestMenuItemConcept
  {
  protected:
    TestMenuItemConcept()                                = default;
    TestMenuItemConcept(const TestMenuItemConcept &)     = default;
    TestMenuItemConcept(TestMenuItemConcept &&) noexcept = default;
    TestMenuItemConcept &operator=(const TestMenuItemConcept &) = default;
    TestMenuItemConcept &operator=(TestMenuItemConcept &&) noexcept = default;

  public:
    virtual ~TestMenuItemConcept(){};
    virtual void OnUpdate(float) const              = 0;
    virtual void OnRender() const                   = 0;
    virtual void OnImGuiUpdate() const              = 0;
    virtual void OnEvent(const Event::Item &) const = 0;
  };
  template<Test testT>
  class TestMenuItemModel final : public TestMenuItemConcept
  {
  public:
    TestMenuItemModel(testT t)
      : m_test(std::move(t))
    {
    }
    void OnUpdate(float ts) const final
    {
      return m_test.OnUpdate(ts);
    }
    void OnRender() const final
    {
      return m_test.OnRender();
    }
    void OnImGuiUpdate() const final
    {
      return m_test.OnImGuiUpdate();
    }
    void OnEvent(const Event::Item &e) const final
    {
      return m_test.OnEvent(e);
    }

    TestMenuItemModel() = default;

  private:
    testT m_test;
  };

  mutable std::unique_ptr<const TestMenuItemConcept> m_impl{ nullptr };

public:
  void OnUpdate(float ts);
  void OnRender();
  void OnImGuiUpdate();
  void OnEvent(const Event::Item &);
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
  [[maybe_unused]] TestMenuItem(T t)
    : TestMenuItem(std::in_place_type_t<T>{}, std::move(t))
  {
  }
  TestMenuItem(const TestMenuItem &other) = delete;
  TestMenuItem &operator=(const TestMenuItem &other) = delete;
  TestMenuItem(TestMenuItem &&other) noexcept        = default;
  TestMenuItem &operator=(TestMenuItem &&other) noexcept = default;

                operator bool() const;
};
}// namespace test
#endif// MYPROJECT_TestMenuItem_HPP
