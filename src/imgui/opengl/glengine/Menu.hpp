//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_MENU_HPP
#define MYPROJECT_MENU_HPP
#include "Event/EventItem.hpp"
#include "MenuItem.hpp"
namespace glengine
{
template<typename T>
concept is_MenuElementType =
  Renderable<typename std::decay_t<T>::value_type> && requires(const T &t)
{
  typename std::decay_t<T>::value_type;
  {
    t.name
    } -> decay_same_as<std::string>;
};

class Menu
{
public:
  Menu();
  template<is_MenuElementType... T>
  Menu(T &&...t)
    : Menu()
  {
    ((void)push_back<typename std::decay_t<T>::value_type>(std::move(t.name)),
     ...);
  }
  template<Renderable T>
  struct MenuElementType
  {
    using value_type = T;
    std::string name;
  };

  Menu(const Menu &other) noexcept = delete;
  Menu &operator=(const Menu &other) noexcept = delete;
  Menu(Menu &&other) noexcept                 = default;
  Menu &operator=(Menu &&other) noexcept = default;
  void  OnUpdate(float) const;
  void  OnRender() const;
  void  OnImGuiUpdate() const;
  void  OnEvent(const Event::Item &) const;
  template<Renderable T>
  void push_back(std::string name) const
  {
    push_back(
      std::move(name), []() -> MenuItem { return std::in_place_type_t<T>{}; });
  }
  void push_back(std::string name, std::function<MenuItem()> funt) const;
  void reload() const
  {
    if (m_current)
    {
      m_current = m_list[m_current_index].second();
    }
  }
  bool selected() const
  {
    return m_current;
  }

private:
  mutable MenuItem    m_current       = {};
  mutable std::size_t m_current_index = {};
  mutable std::vector<std::pair<std::string, std::function<MenuItem()>>>
    m_list = {};
};
}// namespace glengine
#endif// MYPROJECT_MENU_HPP
