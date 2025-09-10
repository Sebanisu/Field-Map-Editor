//
// Created by pcvii on 11/29/2021.
//

#ifndef FIELD_MAP_EDITOR_EVENTITEM_HPP
#define FIELD_MAP_EDITOR_EVENTITEM_HPP
#include "Event.hpp"
#include "MakeVisitor.hpp"
namespace glengine
{
namespace event
{
     class Item
     {
        public:
          mutable Types m_impl = { std::monostate{} };

        public:
          Item() = default;
          template<
            event::is T,
            typename... argsT>
          Item(
            std::in_place_type_t<T> type,
            argsT &&...args) noexcept
            : m_impl(
                std::move(type),
                std::forward<argsT>(args)...)
          {
          }
          template<event::is T>
          Item(T t)
            : Item(
                std::in_place_type_t<T>{},
                std::move(t))
          {
          }

          using ValueType = event::Item;
          std::string_view name() const
          {
               return std::visit(
                 glengine::MakeVisitor(
                   [](const event::is auto &value) { return value.name(); },
                   [](std::monostate) { return std::string_view(""); }),
                 m_impl);
          }
          event::Category category() const
          {
               return std::visit(
                 glengine::MakeVisitor(
                   [](const event::is auto &value) { return value.category(); },
                   [](std::monostate) { return event::Category::None; }),
                 m_impl);
          }
          std::string_view category_name() const
          {
               return std::visit(
                 glengine::MakeVisitor(
                   [](const event::is auto &value)
                   { return value.category_name(); },
                   [](std::monostate) { return std::string_view(""); }),
                 m_impl);
          }
          bool handled() const
          {
               return std::visit(
                 glengine::MakeVisitor(
                   [](const event::is auto &value) { return value.handled(); },
                   [](std::monostate) { return false; }),
                 m_impl);
          }

          std::string data() const
          {
               return std::visit(
                 glengine::MakeVisitor(
                   [](const event::is auto &value) { return value.data(); },
                   [](std::monostate) { return std::string{}; }),
                 m_impl);
          }
          template<event::is eventT>
          bool contains() const
          {
               return std::get_if<eventT>(&m_impl) == nullptr ? false : true;
          }

          template<event::is eventT>
          const eventT *get() const
          {
               return std::get_if<eventT>(&m_impl);
          }

          operator bool() const
          {
               return m_impl.index() != 0U;
          }
     };
     static_assert(event::is<Item>);
}// namespace event
}// namespace glengine
#endif// FIELD_MAP_EDITOR_EVENTITEM_HPP
