//
// Created by pcvii on 11/29/2021.
//

#ifndef MYPROJECT_FF8MENUITEM_HPP
#define MYPROJECT_FF8MENUITEM_HPP

#include "tests/Test.hpp"
#include <EventItem.hpp>
#include <memory>
namespace ff8
{
class FF8MenuItem
{
private:
  class FF8MenuItemConcept
  {
  protected:
    FF8MenuItemConcept()                               = default;
    FF8MenuItemConcept(const FF8MenuItemConcept &)     = default;
    FF8MenuItemConcept(FF8MenuItemConcept &&) noexcept = default;
    FF8MenuItemConcept &operator=(const FF8MenuItemConcept &) = default;
    FF8MenuItemConcept &operator=(FF8MenuItemConcept &&) noexcept = default;

  public:
    virtual ~FF8MenuItemConcept(){};
    virtual void OnUpdate(float) const              = 0;
    virtual void OnRender() const                   = 0;
    virtual void OnImGuiUpdate() const              = 0;
    virtual void OnEvent(const Event::Item &) const = 0;
  };
  template<test::Test testT>
  class FF8MenuItemModel final : public FF8MenuItemConcept
  {
  public:
    FF8MenuItemModel(testT t)
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

    FF8MenuItemModel() = default;

  private:
    testT m_test;
  };
  mutable std::unique_ptr<const FF8MenuItemConcept> m_impl{ nullptr };

public:
  void OnUpdate(float) const;
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const Event::Item &) const;
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
  FF8MenuItem &operator=(const FF8MenuItem &other) = delete;
  FF8MenuItem(FF8MenuItem &&other) noexcept        = default;
  FF8MenuItem &operator=(FF8MenuItem &&other) noexcept = default;

               operator bool() const;
};
}// namespace ff8
#endif// MYPROJECT_FF8MENUITEM_HPP
