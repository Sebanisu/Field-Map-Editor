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
class Item
{
private:
  class ItemConcept
  {
  protected:
    ItemConcept()                        = default;
    ItemConcept(const ItemConcept &)     = default;
    ItemConcept(ItemConcept &&) noexcept = default;
    ItemConcept &operator=(const ItemConcept &) = default;
    ItemConcept &operator=(ItemConcept &&) noexcept = default;

  public:
    virtual ~ItemConcept(){};
    virtual void OnUpdate(float) const              = 0;
    virtual void OnRender() const                   = 0;
    virtual void OnImGuiUpdate() const              = 0;
    virtual void OnEvent(const Event::Item &) const = 0;
  };
  template<test::Test testT>
  class ItemModel final : public ItemConcept
  {
  public:
    ItemModel(testT t)
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
    ItemModel() = default;

  private:
    testT m_test;
  };

  mutable std::unique_ptr<const ItemConcept> m_impl{ nullptr };

public:
  void OnUpdate(float) const;
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const Event::Item &e) const;
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
  Item &operator=(const Item &other) = delete;
  Item(Item &&other) noexcept        = default;
  Item &operator=(Item &&other) noexcept = default;

        operator bool() const;
};
}// namespace Layer
#endif// MYPROJECT_LAYERITEM_HPP
