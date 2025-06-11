//
// Created by pcvii on 11/24/2021.
//

#ifndef FIELD_MAP_EDITOR_MENU_HPP
#define FIELD_MAP_EDITOR_MENU_HPP
#include "Event/EventItem.hpp"
#include "MenuItem.hpp"
#include <functional>
namespace glengine
{
template<typename T>
concept is_MenuElementType = Renderable<typename std::remove_cvref_t<T>::ValueType> && requires(const T &t) {
     typename std::remove_cvref_t<T>::ValueType;
     { t.name } -> decay_same_as<std::string>;
};

class Menu
{
   public:
     Menu() = default;
     Menu(const char *const title)
       : m_title(title)
     {
     }
     template<is_MenuElementType... T>
     Menu(const char *const title, T &&...t)
       : Menu(title)
     {
          ((void)push_back<typename std::remove_cvref_t<T>::ValueType>(std::move(t.name)), ...);
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
     bool  on_im_gui_menu() const;
     void  on_event(const event::Item &) const;
     template<Renderable T>
     void push_back(std::string name) const
     {
          push_back(std::move(name), []() -> MenuItem { return std::in_place_type_t<T>{}; });
     }
     void        push_back(std::string name, std::function<MenuItem()> function) const;
     void        reload() const;
     std::size_t size() const noexcept
     {
          return std::size(m_current);
     }
     const char *title() const noexcept
     {
          return m_title;
     }
     void toggle_items(std::ranges::range auto &&toggles) const
     {
          if (std::ranges::size(toggles) != size())
          {
               return;
          }
          for (std::size_t i = {}; const auto &[name, function] : m_list)
          {
               auto b = std::ranges::cbegin(toggles);
               std::ranges::advance(b, static_cast<std::ranges::range_difference_t<decltype(toggles)>>(i));
               if (*b && !m_current[i])
               {
                    m_current[i] = function();
               }
               else
               {
                    m_current[i] = MenuItem{};
               }
               ++i;
          }
     }
     std::vector<bool> get_toggles() const
     {
          std::vector<bool> ret{};
          ret.reserve(size());
          std::ranges::transform(m_current, std::back_inserter(ret), [](auto &&val) -> bool { return val; });
          return ret;
     };

   private:
     const char                                                            *m_title   = {};
     mutable std::vector<MenuItem>                                          m_current = {};
     //  mutable std::string_view m_current_string = {};
     //  mutable std::size_t      m_current_index  = {};
     mutable std::vector<std::pair<std::string, std::function<MenuItem()>>> m_list    = {};
};
}// namespace glengine
#endif// FIELD_MAP_EDITOR_MENU_HPP
