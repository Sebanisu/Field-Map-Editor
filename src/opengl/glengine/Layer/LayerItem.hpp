//
// Created by pcvii on 12/8/2021.
//

#ifndef FIELD_MAP_EDITOR_LAYERITEM_HPP
#define FIELD_MAP_EDITOR_LAYERITEM_HPP
#include "Event/EventItem.hpp"
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
      virtual ~ItemConcept(){};
      virtual void on_update(float) const              = 0;
      virtual void on_render() const                   = 0;
      virtual void on_im_gui_update() const            = 0;
      virtual void on_event(const event::Item &) const = 0;
    };
    template<glengine::Renderable renderableT>
    class ItemModel final : public ItemConcept
    {
    public:
      ItemModel(renderableT t)
        : m_renderable(std::move(t))
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
      ItemModel() = default;

    private:
      renderableT m_renderable;
    };

    mutable std::unique_ptr<const ItemConcept> m_impl{ nullptr };

  public:
    void on_update(float) const;
    void on_render() const;
    void on_im_gui_update() const;
    void on_event(const event::Item &e) const;
    Item()
      : m_impl(nullptr)
    {
    }
    template<typename T, typename... argsT>
    Item(std::in_place_type_t<T>, argsT &&...args)
      : m_impl(std::make_unique<ItemModel<std::remove_cvref_t<T>>>(
        std::forward<argsT>(args)...))
    {
      static_assert(glengine::Renderable<T>);
    }
    template<typename T>
    Item(T t)
      : Item(std::in_place_type_t<T>{}, std::move(t))
    {
    }
    Item(const Item &other)                = delete;
    Item &operator=(const Item &other)     = delete;
    Item(Item &&other) noexcept            = default;
    Item &operator=(Item &&other) noexcept = default;

          operator bool() const;
  };
}// namespace Layer
}// namespace glengine
#endif// FIELD_MAP_EDITOR_LAYERITEM_HPP
