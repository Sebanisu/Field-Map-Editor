//
// Created by pcvii on 12/7/2021.
//

#ifndef FIELD_MAP_EDITOR_EVENTCONCEPTS_HPP
#define FIELD_MAP_EDITOR_EVENTCONCEPTS_HPP
#include "glengine/Event/EventEnums.hpp"
#include "glengine/KeyCodes.hpp"
#include "glengine/MouseButtonCodes.hpp"
#include <string>
namespace glengine
{
namespace event
{
     template<typename T>
     concept is = glengine::decay_same_as<typename T::ValueType, T>
                  && requires(const T &t) {
                          {
                               t.name()
                          } -> glengine::decay_same_as<std::string_view>;
                          { t.category() } -> glengine::decay_same_as<Category>;
                          {
                               t.category_name()
                          } -> glengine::decay_same_as<std::string_view>;
                          { t.handled() } -> glengine::decay_same_as<bool>;
                          { t.data() } -> glengine::decay_same_as<std::string>;
                     };
     template<typename T>
     concept is_Key = is<T> && requires(const T &t) {
          T(glengine::Key::A, glengine::Mods::None);
          { t.key() } -> glengine::decay_same_as<glengine::Key>;
          { t.mods() } -> glengine::decay_same_as<glengine::Mods>;
     };
     template<typename T>
     concept is_MouseMove = is<T> && requires(const T &t) {
          T(0.F, 0.F);
          { t.position() } -> glengine::decay_same_as<std::array<float, 2U>>;
          { t.x() } -> glengine::decay_same_as<float>;
          { t.y() } -> glengine::decay_same_as<float>;
     };
     template<typename T>
     concept is_WindowMove = is<T> && requires(const T &t) {
          T(0, 0);
          { t.position() } -> glengine::decay_same_as<std::array<int, 2U>>;
          { t.x() } -> glengine::decay_same_as<int>;
          { t.y() } -> glengine::decay_same_as<int>;
     };
     template<typename T>
     concept is_MouseButton = is<T> && requires(const T &t) {
          T(glengine::Mouse::Button1, glengine::Mods::None);
          { t.button() } -> glengine::decay_same_as<glengine::Mouse>;
          { t.mods() } -> glengine::decay_same_as<glengine::Mods>;
     };
     template<typename T>
     concept is_MouseScroll = is<T> && requires(const T &t) {
          T(0.F, 0.F);
          { t.offsets() } -> glengine::decay_same_as<std::array<float, 2U>>;
          { t.x_offset() } -> glengine::decay_same_as<float>;
          { t.y_offset() } -> glengine::decay_same_as<float>;
     };

     template<typename T>
     concept is_WindowResize = is<T> && requires(const T &t) {
          T(0, 0);
          { t.width() } -> glengine::decay_same_as<int>;
          { t.height() } -> glengine::decay_same_as<int>;
     };
}// namespace event
}// namespace glengine
#endif// FIELD_MAP_EDITOR_EVENTCONCEPTS_HPP