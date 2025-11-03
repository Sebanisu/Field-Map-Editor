//
// Created by pcvii on 11/29/2021.
//

#ifndef FIELD_MAP_EDITOR_MENUITEM_HPP
#define FIELD_MAP_EDITOR_MENUITEM_HPP
#include "Event/EventItem.hpp"
#include "Renderable.hpp"
#include <memory>
#include <type_traits>
#include <utility>
namespace glengine
{
class MenuItem
{
   private:
     class MenuItemConcept
     {
        protected:
          MenuItemConcept()                                       = default;
          MenuItemConcept(const MenuItemConcept &)                = default;
          MenuItemConcept(MenuItemConcept &&) noexcept            = default;
          MenuItemConcept &operator=(const MenuItemConcept &)     = default;
          MenuItemConcept &operator=(MenuItemConcept &&) noexcept = default;

        public:
          virtual ~MenuItemConcept() {};
          virtual void on_update(float) const              = 0;
          virtual void on_render() const                   = 0;
          virtual void on_im_gui_update() const            = 0;
          virtual void on_event(const event::Item &) const = 0;
     };
     template<Renderable renderableT>
     class MenuItemModel final : public MenuItemConcept
     {
        public:
          //    MenuItemModel(renderableT t)
          //      : m_renderable(std::move(t))
          //    {
          //    }
          template<typename... Ts>
          MenuItemModel(Ts &&...ts)
            : m_renderable(std::forward<Ts>(ts)...)
          {
          }
          void on_update(float ts) const final
          {
               return m_renderable.on_update(ts);
          }
          void on_render() const final
          {
               return m_renderable.on_render();
          }
          void on_im_gui_update() const final
          {
               return m_renderable.on_im_gui_update();
          }
          void on_event(const event::Item &e) const final
          {
               return m_renderable.on_event(e);
          }

          MenuItemModel() = default;

        private:
          renderableT m_renderable;
     };

     mutable std::unique_ptr<const MenuItemConcept> m_impl{ nullptr };

   public:
     void on_update(float ts);
     void on_render();
     void on_im_gui_update();
     void on_event(const event::Item &);
     MenuItem()
       : m_impl(nullptr)
     {
     }
     template<
       typename T,
       typename... argsT>
     MenuItem(
       std::in_place_type_t<T>,
       argsT &&...args) noexcept
       : m_impl(
           std::make_unique<MenuItemModel<std::remove_cvref_t<T>>>(
             std::forward<argsT>(args)...))
     {
          static_assert(Renderable<T>);
     }
     template<typename T>
     [[maybe_unused]] MenuItem(T t)
       : MenuItem(
           std::in_place_type_t<T>{},
           std::move(t))
     {
     }
     MenuItem(const MenuItem &other)                = delete;
     MenuItem &operator=(const MenuItem &other)     = delete;
     MenuItem(MenuItem &&other) noexcept            = default;
     MenuItem &operator=(MenuItem &&other) noexcept = default;

               operator bool() const;
};
}// namespace glengine
#endif// FIELD_MAP_EDITOR_MENUITEM_HPP
