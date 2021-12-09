//
// Created by pcvii on 12/8/2021.
//

#ifndef MYPROJECT_LAYERSTACK_HPP
#define MYPROJECT_LAYERSTACK_HPP
#include "LayerItem.hpp"
#include <algorithm>
#include <functional>
#include <span>
#include <string>
#include <vector>
namespace Layer
{
class Stack
{
public:
  Stack() = default;
  Stack(std::span<Item> layers, std::span<Item> overlays = {})
  {
    m_layers.reserve(std::ranges::size(layers) + std::ranges::size(overlays));
    std::ranges::move(layers, std::back_inserter(m_layers));
    end_of_layers = std::ranges::end(m_layers);
    std::ranges::move(overlays, std::back_inserter(m_layers));
  }
  Stack(Stack &&other) noexcept = default;
  Stack &operator=(Stack &&other) noexcept = default;
  void   OnUpdate(float) const;
  void   OnRender() const;
  void   OnImGuiUpdate() const;
  void   OnEvent(const Event::Item &) const;
  template<typename... T>
  void emplace_layers(T &&...args) const
  {
    end_of_layers = m_layers.emplace(end_of_layers, std::forward<T>(args)...);
  }
  template<typename... T>
  void emplace_overlays(T &&...args) const
  {
    m_layers.emplace_back(std::forward<T>(args)...);
  }

private:
  mutable std::vector<Item>                           m_layers = {};
  mutable std::ranges::iterator_t<decltype(m_layers)> end_of_layers{
    std::ranges::begin(m_layers)
  };
  // start of overlay
};
}// namespace Layer
#endif// MYPROJECT_LAYERSTACK_HPP
