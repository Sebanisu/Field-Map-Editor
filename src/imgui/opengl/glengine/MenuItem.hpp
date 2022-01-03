//
// Created by pcvii on 11/29/2021.
//

#ifndef MYPROJECT_MenuItem_HPP
#define MYPROJECT_MenuItem_HPP
#include "Event/EventItem.hpp"
namespace glengine
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
    MenuItemConcept &operator=(const MenuItemConcept &) = default;
    MenuItemConcept &operator=(MenuItemConcept &&) noexcept = default;

  public:
    virtual ~MenuItemConcept(){};
    virtual void OnUpdate(float) const              = 0;
    virtual void OnRender() const                   = 0;
    virtual void OnImGuiUpdate() const              = 0;
    virtual void OnEvent(const Event::Item &) const = 0;
  };
  template<Renderable renderableT>
  class MenuItemModel final : public MenuItemConcept
  {
  public:
    MenuItemModel(renderableT t)
      : m_renderable(std::move(t))
    {
    }
    void OnUpdate(float ts) const final
    {
      return m_renderable.OnUpdate(ts);
    }
    void OnRender() const final
    {
      return m_renderable.OnRender();
    }
    void OnImGuiUpdate() const final
    {
      return m_renderable.OnImGuiUpdate();
    }
    void OnEvent(const Event::Item &e) const final
    {
      return m_renderable.OnEvent(e);
    }

    MenuItemModel() = default;

  private:
    renderableT m_renderable;
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
    static_assert(Renderable<T>);
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
};// namespace glengine
#endif// MYPROJECT_MenuItem_HPP
