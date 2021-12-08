//
// Created by pcvii on 12/8/2021.
//

#ifndef MYPROJECT_LAYERITEM_HPP
#define MYPROJECT_LAYERITEM_HPP
#include "tests/Test.hpp"
#include <EventItem.hpp>
#include <memory>
namespace Layer
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
  Layer::OnEvent(t, e);
};
class Item
{
private:
  class ItemConcept
  {
  protected:
    ItemConcept()                        = default;
    ItemConcept(const ItemConcept &)     = default;
    ItemConcept(ItemConcept &&) noexcept = default;
    ItemConcept &
      operator=(const ItemConcept &) = default;
    ItemConcept &
      operator=(ItemConcept &&) noexcept = default;

  public:
    virtual ~ItemConcept(){};
    virtual void
      OnUpdate(float) const = 0;
    virtual void
      OnRender() const = 0;
    virtual void
      OnImGuiUpdate() const = 0;
    virtual void
      OnEvent(const Event::Item &) const = 0;
  };
  template<test::Test testT>
  class ItemModel final : public ItemConcept
  {
  public:
    ItemModel(testT t)
      : m_test(std::move(t))
    {
    }
    void
      OnUpdate(float ts) const final
    {
      using Layer::OnUpdate;
      return OnUpdate(m_test, ts);
    }
    void
      OnRender() const final
    {
      using Layer::OnRender;
      return OnRender(m_test);
    }
    void
      OnImGuiUpdate() const final
    {
      using Layer::OnImGuiUpdate;
      return OnImGuiUpdate(m_test);
    }
    void
      OnEvent(const Event::Item &e) const final
    {
      if constexpr (has_OnEvent<testT>)
      {
        using Layer::OnEvent;
        return OnEvent(m_test, e);
      }
    }

    ItemModel() = default;

  private:
    testT m_test;
  };

  friend void
    OnUpdate(const Item &menu_item, float ts);
  friend void
    OnRender(const Item &menu_item);
  friend void
    OnImGuiUpdate(const Item &menu_item);
  friend void
    OnEvent(const Item &menu_item, const Event::Item &);

  mutable std::unique_ptr<const ItemConcept> m_impl{ nullptr };

public:
  Item()
    : m_impl(nullptr)
  {
  }
  template<typename T, typename... argsT>
  Item(std::in_place_type_t<T>, argsT &&...args)
    : m_impl(std::make_unique<ItemModel<std::decay_t<T>>>(
      std::forward<argsT>(args)...))
  {
    static_assert(test::Test<T>);
  }
  template<typename T>
  Item(T t)
    : Item(std::in_place_type_t<T>{}, std::move(t))
  {
  }
  Item(const Item &other) = delete;
  Item &
    operator=(const Item &other) = delete;
  Item(Item &&other) noexcept    = default;
  Item &
    operator=(Item &&other) noexcept = default;

    operator bool() const;
};
void
  OnUpdate(const Item &menu_item, float ts);
void
  OnRender(const Item &menu_item);
void
  OnImGuiUpdate(const Item &menu_item);
void
  OnEvent(const Item &menu_item, const Event::Item &);
}// namespace Layer
#endif// MYPROJECT_LAYERITEM_HPP
