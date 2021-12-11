//
// Created by pcvii on 11/29/2021.
//

#ifndef MYPROJECT_MenuItem_HPP
#define MYPROJECT_MenuItem_HPP
#include "EventItem.hpp"
#include "tests/Test.hpp"


class MenuItem
{
private:
  class MenuItemConcept
  {
  protected:
    MenuItemConcept()                            = default;
    MenuItemConcept(const MenuItemConcept &)     = default;
    MenuItemConcept(MenuItemConcept &&) noexcept = default;
    MenuItemConcept &operator=(const MenuItemConcept &) = default;
    MenuItemConcept &operator=(MenuItemConcept &&) noexcept = default;

  public:
    virtual ~MenuItemConcept(){};
    virtual void OnUpdate(float) const              = 0;
    virtual void OnRender() const                   = 0;
    virtual void OnImGuiUpdate() const              = 0;
    virtual void OnEvent(const Event::Item &) const = 0;
  };
  template<test::Test testT>
  class MenuItemModel final : public MenuItemConcept
  {
  public:
    MenuItemModel(testT t)
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

    MenuItemModel() = default;

  private:
    testT m_test;
  };

  mutable std::unique_ptr<const MenuItemConcept> m_impl{ nullptr };

public:
  void OnUpdate(float ts);
  void OnRender();
  void OnImGuiUpdate();
  void OnEvent(const Event::Item &);
  MenuItem()
    : m_impl(nullptr)
  {
  }
  template<typename T, typename... argsT>
  MenuItem(std::in_place_type_t<T>, argsT &&...args) noexcept
    : m_impl(std::make_unique<MenuItemModel<std::decay_t<T>>>(
      std::forward<argsT>(args)...))
  {
    static_assert(test::Test<T>);
  }
  template<typename T>
  [[maybe_unused]] MenuItem(T t)
    : MenuItem(std::in_place_type_t<T>{}, std::move(t))
  {
  }
  MenuItem(const MenuItem &other) = delete;
  MenuItem &operator=(const MenuItem &other) = delete;
  MenuItem(MenuItem &&other) noexcept        = default;
  MenuItem &operator=(MenuItem &&other) noexcept = default;

            operator bool() const;
};
#endif// MYPROJECT_MenuItem_HPP
