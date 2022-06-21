//
// Created by pcvii on 11/24/2021.
//

#ifndef FIELD_MAP_EDITOR_MENU_HPP
#define FIELD_MAP_EDITOR_MENU_HPP
#include "Event/EventItem.hpp"
#include "MenuItem.hpp"
namespace glengine
{
template<typename T>
concept is_MenuElementType =
  Renderable<typename std::remove_cvref_t<
    T>::ValueType> && requires(const T &t) {
                        typename std::remove_cvref_t<T>::ValueType;
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
    ((void)push_back<typename std::remove_cvref_t<T>::ValueType>(
       std::move(t.name)),
     ...);
  }
  template<Renderable T>
  struct MenuElementType
  {
    using ValueType = T;
    std::string name;
  };

  Menu(const Menu &other) noexcept            = delete;
  Menu &operator=(const Menu &other) noexcept = delete;
  Menu(Menu &&other) noexcept                 = default;
  Menu &operator=(Menu &&other) noexcept      = default;
  void  on_update(float) const;
  void  on_render() const;
  void  on_im_gui_update() const;
  void  on_event(const event::Item &) const;
  template<Renderable T>
  void push_back(std::string name) const
  {
    push_back(
      std::move(name), []() -> MenuItem { return std::in_place_type_t<T>{}; });
  }
  void push_back(std::string name, std::function<MenuItem()> funt) const;
  void reload() const;

private:
  mutable std::vector<MenuItem> m_current = {};
  //  mutable std::string_view m_current_string = {};
  //  mutable std::size_t      m_current_index  = {};
  mutable std::vector<std::pair<std::string, std::function<MenuItem()>>>
    m_list = {};
};
}// namespace glengine
#endif// FIELD_MAP_EDITOR_MENU_HPP
