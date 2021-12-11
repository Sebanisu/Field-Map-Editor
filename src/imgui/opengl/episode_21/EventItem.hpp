//
// Created by pcvii on 11/29/2021.
//

#ifndef MYPROJECT_EVENTITEM_HPP
#define MYPROJECT_EVENTITEM_HPP
#include "Event.hpp"
#include "MakeVisitor.hpp"


namespace Event
{
class Item
{
public:
  mutable Types m_impl = { std::monostate{} };

public:
  Item() = default;
  template<Event::is T, typename... argsT>
  Item(std::in_place_type_t<T> type, argsT &&...args) noexcept
    : m_impl(std::move(type), std::forward<argsT>(args)...)
  {
  }
  template<Event::is T>
  Item(T t)
    : Item(std::in_place_type_t<T>{}, std::move(t))
  {
  }

  using value_type = Event::Item;
  std::string_view Name() const
  {
    return std::visit(
      make_visitor(
        [](const Event::is auto &value) { return value.Name(); },
        [](std::monostate) { return std::string_view(""); }),
      m_impl);
  }
  Event::Category category() const
  {
    return std::visit(
      make_visitor(
        [](const Event::is auto &value) { return value.category(); },
        [](std::monostate) { return Event::Category::None; }),
      m_impl);
  }
  std::string_view CategoryName() const
  {
    return std::visit(
      make_visitor(
        [](const Event::is auto &value) { return value.CategoryName(); },
        [](std::monostate) { return std::string_view(""); }),
      m_impl);
  }
  bool Handled() const
  {
    return std::visit(
      make_visitor(
        [](const Event::is auto &value) { return value.Handled(); },
        [](std::monostate) { return false; }),
      m_impl);
  }

  std::string Data() const
  {
    return std::visit(
      make_visitor(
        [](const Event::is auto &value) { return value.Data(); },
        [](std::monostate) { return std::string{}; }),
      m_impl);
  }
  template<Event::is eventT>
  bool contains() const
  {
    return std::get_if<eventT>(&m_impl) == nullptr ? false : true;
  }

  template<Event::is eventT>
  const eventT *Get() const
  {
    return std::get_if<eventT>(&m_impl);
  }

  operator bool() const
  {
    return m_impl.index() != 0U;
  }
};
static_assert(Event::is<Item>);
}// namespace Event
#endif// MYPROJECT_EVENTITEM_HPP
