//
// Created by pcvii on 12/8/2021.
//

#ifndef FIELD_MAP_EDITOR_LAYERSTACK_HPP
#define FIELD_MAP_EDITOR_LAYERSTACK_HPP
#include "LayerItem.hpp"
#include <algorithm>
#include <ranges>
#include <vector>
namespace glengine
{
namespace Layer
{
     class Stack
     {
        public:
          Stack() = default;
          Stack(
            std::span<Item> layers,
            std::span<Item> overlays = {})
          {
               m_layers.reserve(
                 std::ranges::size(layers) + std::ranges::size(overlays));
               std::ranges::move(layers, std::back_inserter(m_layers));
               end_of_layers = std::ranges::end(m_layers);
               std::ranges::move(overlays, std::back_inserter(m_layers));
          }
          Stack(Stack &&other) noexcept            = default;
          Stack &operator=(Stack &&other) noexcept = default;
          void   on_update(float) const;
          void   on_render() const;
          void   on_im_gui_update() const;
          void   on_event(const event::Item &) const;
          template<typename... T>
          void emplace_layers(T &&...args) const
          {
               end_of_layers
                 = m_layers.emplace(end_of_layers, std::forward<T>(args)...);
          }
          template<typename... T>
          void emplace_overlays(T &&...args) const
          {
               m_layers.emplace_back(std::forward<T>(args)...);
               end_of_layers = std::ranges::begin(m_layers);
          }
          template<typename T>
          const T *get() const
          {
               for (auto &item : m_layers)
                    if (auto ptr = item.get<T>())
                         return ptr;
               return nullptr;
          }

        private:
          mutable std::vector<Item>                           m_layers = {};
          mutable std::ranges::iterator_t<decltype(m_layers)> end_of_layers{
               std::ranges::begin(m_layers)
          };
          // start of overlay
     };
}// namespace Layer
}// namespace glengine
static_assert(glengine::Renderable<glengine::Layer::Stack>);
#endif// FIELD_MAP_EDITOR_LAYERSTACK_HPP