//
// Created by pcvii on 11/29/2021.
//

#ifndef MYPROJECT_FF8MENUITEM_HPP
#define MYPROJECT_FF8MENUITEM_HPP

#include "tests/Test.hpp"
#include <memory>
namespace ff8
{
void
  OnUpdate();
void
  OnRender();
void
  OnImGuiRender();
class FF8MenuItem
{
private:
  class FF8MenuItemConcept
  {
  protected:
    FF8MenuItemConcept()                               = default;
    FF8MenuItemConcept(const FF8MenuItemConcept &)     = default;
    FF8MenuItemConcept(FF8MenuItemConcept &&) noexcept = default;
    FF8MenuItemConcept &
      operator=(const FF8MenuItemConcept &) = default;
    FF8MenuItemConcept &
      operator=(FF8MenuItemConcept &&) noexcept = default;

  public:
    virtual ~FF8MenuItemConcept(){};
    virtual void
      OnUpdate(float) const = 0;
    virtual void
      OnRender() const = 0;
    virtual void
      OnImGuiRender() const = 0;
  };
  template<test::Test testT>
  class FF8MenuItemModel final : public FF8MenuItemConcept
  {
  public:
    FF8MenuItemModel(testT t)
      : m_test(std::move(t))
    {
    }
    void
      OnUpdate(float ts) const final
    {
      using ff8::OnUpdate;
      return OnUpdate(m_test, ts);
    }
    void
      OnRender() const final
    {
      using ff8::OnRender;
      return OnRender(m_test);
    }
    void
      OnImGuiRender() const final
    {
      using ff8::OnImGuiRender;
      return OnImGuiRender(m_test);
    }

    FF8MenuItemModel() = default;

  private:
    testT m_test;
  };

  friend void
    OnUpdate(const FF8MenuItem &menu_item, float ts);
  friend void
    OnRender(const FF8MenuItem &menu_item);
  friend void
    OnImGuiRender(const FF8MenuItem &menu_item);

  mutable std::unique_ptr<const FF8MenuItemConcept> m_impl{ nullptr };

public:
  FF8MenuItem()
    : m_impl(nullptr)
  {
  }
  template<typename T, typename... argsT>
  FF8MenuItem(std::in_place_type_t<T>, argsT &&...args)
    : m_impl(std::make_unique<FF8MenuItemModel<std::decay_t<T>>>(
      std::forward<argsT>(args)...))
  {
    static_assert(test::Test<T>);
  }
  template<typename T>
  FF8MenuItem(T t)
    : FF8MenuItem(std::in_place_type_t<T>{}, std::move(t))
  {
  }
  FF8MenuItem(const FF8MenuItem &other) = delete;
  FF8MenuItem &
    operator=(const FF8MenuItem &other)     = delete;
  FF8MenuItem(FF8MenuItem &&other) noexcept = default;
  FF8MenuItem &
    operator=(FF8MenuItem &&other) noexcept = default;

    operator bool() const;
};
void
  OnUpdate(const FF8MenuItem &menu_item, float ts);
void
  OnRender(const FF8MenuItem &menu_item);
void
  OnImGuiRender(const FF8MenuItem &menu_item);
}// namespace ff8
#endif// MYPROJECT_FF8MENUITEM_HPP
