//
// Created by pcvii on 12/8/2021.
//

#ifndef FIELD_MAP_EDITOR_LAYERITEM_HPP
#define FIELD_MAP_EDITOR_LAYERITEM_HPP
#include "glengine/Event/EventItem.hpp"
#include "glengine/Renderable.hpp"
#include <memory>
#include <utility>
namespace glengine
{
namespace Layer
{
     class Item
     {
        private:
          class ItemConcept
          {
             protected:
               ItemConcept()                                   = default;
               ItemConcept(const ItemConcept &)                = default;
               ItemConcept(ItemConcept &&) noexcept            = default;
               ItemConcept &operator=(const ItemConcept &)     = default;
               ItemConcept &operator=(ItemConcept &&) noexcept = default;

             public:
               virtual ~ItemConcept() {};
               virtual void on_update(float) const              = 0;
               virtual void on_render() const                   = 0;
               virtual void on_im_gui_update() const            = 0;
               virtual void on_im_gui_file_menu() const         = 0;
               virtual void on_im_gui_edit_menu() const         = 0;
               virtual void on_im_gui_window_menu() const       = 0;
               virtual void on_im_gui_help_menu() const         = 0;
               virtual void on_event(const event::Item &) const = 0;
          };
          template<glengine::Renderable renderableT>
          class ItemModel final : public ItemConcept
          {
             public:
               template<typename... Args>
               ItemModel(Args &&...args)
                 : m_renderable(std::forward<Args>(args)...)
               {
               }
               void on_update(float ts) const final
               {
                    (void)OnUpdate(m_renderable, ts);
               }
               void on_render() const final
               {
                    (void)OnRender(m_renderable);
               }
               void on_im_gui_update() const final
               {
                    (void)OnImGuiUpdate(m_renderable);
               }
               void on_im_gui_file_menu() const final
               {
                    (void)OnImGuiFileMenu(m_renderable);
               }
               void on_im_gui_edit_menu() const final
               {
                    (void)OnImGuiEditMenu(m_renderable);
               }
               void on_im_gui_window_menu() const final
               {
                    (void)OnImGuiWindowMenu(m_renderable);
               }
               void on_im_gui_help_menu() const final
               {
                    (void)OnImGuiHelpMenu(m_renderable);
               }
               void on_event(const event::Item &e) const final
               {
                    (void)OnEvent(m_renderable, e);
               }
               ItemModel() = default;

               auto *get()
               {
                    return &m_renderable;
               }

               const auto *get() const
               {
                    return &m_renderable;
               }

             private:
               renderableT m_renderable;
          };

          mutable std::unique_ptr<const ItemConcept> m_impl{ nullptr };

        public:
          void on_update(float) const;
          void on_render() const;
          void on_im_gui_update() const;
          void on_im_gui_file_menu() const;
          void on_im_gui_edit_menu() const;
          void on_im_gui_window_menu() const;
          void on_im_gui_help_menu() const;
          void on_event(const event::Item &e) const;
          Item()
            : m_impl(nullptr)
          {
          }
          template<
            typename T,
            typename... argsT>
          Item(
            const std::in_place_type_t<T> &,
            argsT &&...args)
            : m_impl(
                std::make_unique<ItemModel<std::remove_cvref_t<T>>>(
                  std::forward<argsT>(args)...))
          {
               static_assert(glengine::Renderable<T>);
          }
          template<typename T>
          Item(T t)
            : Item(
                std::in_place_type_t<T>{},
                std::move(t))
          {
          }
          Item(const Item &other)                = delete;
          Item &operator=(const Item &other)     = delete;
          Item(Item &&other) noexcept            = default;
          Item &operator=(Item &&other) noexcept = default;
                operator bool() const;
          // template<typename T>
          // T *get()
          // {
          //      if (auto ptr = dynamic_cast<const ItemModel<T>
          //      *>(m_impl.get()))
          //           return ptr->get();
          //      return nullptr;
          // }
          template<typename T>
          const T *get() const
          {
               if (auto ptr = dynamic_cast<const ItemModel<T> *>(m_impl.get()))
                    return ptr->get();
               return nullptr;
          }
     };
}// namespace Layer
}// namespace glengine
#endif// FIELD_MAP_EDITOR_LAYERITEM_HPP
