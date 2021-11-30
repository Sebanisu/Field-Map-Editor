//
// Created by pcvii on 11/29/2021.
//

#ifndef MYPROJECT_MENUITEM_HPP
#define MYPROJECT_MENUITEM_HPP

#include "Test.hpp"
#include <memory>
namespace test
{
void OnUpdate();
void OnRender();
void OnImGuiRender();
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
    OnUpdate(const MenuItem &menu_item, float ts);
  friend void
    OnRender(const MenuItem &menu_item);
  friend void
    OnImGuiRender(const MenuItem &menu_item);

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
  MenuItem(MenuItem &&other) noexcept;
  MenuItem &
    operator=(MenuItem &&other) noexcept;

    operator bool() const;
  friend void
    swap(MenuItem &left, MenuItem &right) noexcept;
};
void
  OnUpdate(const MenuItem &menu_item, float ts);
void
  OnRender(const MenuItem &menu_item);
void
  OnImGuiRender(const MenuItem &menu_item);
void
  swap(MenuItem &left, MenuItem &right) noexcept;
}// namespace test
#endif// MYPROJECT_MENUITEM_HPP
