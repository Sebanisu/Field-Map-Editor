//
// Created by pcvii on 12/7/2021.
//

#ifndef MYPROJECT_EVENTCONCEPTS_HPP
#define MYPROJECT_EVENTCONCEPTS_HPP

#include "EventEnums.hpp"
#include "KeyCodes.hpp"
#include "MouseButtonCodes.hpp"


namespace Event
{
template<typename T>
concept is =
  glengine::decay_same_as<typename T::value_type, T> && requires(const T &t)
{
  {
    t.Name()
    } -> glengine::decay_same_as<std::string_view>;
  {
    t.category()
    } -> glengine::decay_same_as<Category>;
  {
    t.CategoryName()
    } -> glengine::decay_same_as<std::string_view>;
  {
    t.Handled()
    } -> glengine::decay_same_as<bool>;
  {
    t.Data()
    } -> glengine::decay_same_as<std::string>;
};
template<typename T>
concept is_Key = is<T> && requires(const T &t)
{
  T(glengine::KEY::A);
  {
    t.Key()
    } -> glengine::decay_same_as<glengine::KEY>;
};
template<typename T>
concept is_MouseMove = is<T> && requires(const T &t)
{
  T(0.F, 0.F);
  {
    t.Position()
    } -> glengine::decay_same_as<std::array<float, 2U>>;
  {
    t.X()
    } -> glengine::decay_same_as<float>;
  {
    t.Y()
    } -> glengine::decay_same_as<float>;
};
template<typename T>
concept is_WindowMove = is<T> && requires(const T &t)
{
  T(0, 0);
  {
    t.Position()
    } -> glengine::decay_same_as<std::array<int, 2U>>;
  {
    t.X()
    } -> glengine::decay_same_as<int>;
  {
    t.Y()
    } -> glengine::decay_same_as<int>;
};
template<typename T>
concept is_MouseButton = is<T> && requires(const T &t)
{
  T(glengine::MOUSE::BUTTON_1);
  {
    t.Button()
    } -> glengine::decay_same_as<glengine::MOUSE>;
};
template<typename T>
concept is_MouseScroll = is<T> && requires(const T &t)
{
  T(0.F, 0.F);
  {
    t.Offsets()
    } -> glengine::decay_same_as<std::array<float, 2U>>;
  {
    t.XOffset()
    } -> glengine::decay_same_as<float>;
  {
    t.YOffset()
    } -> glengine::decay_same_as<float>;
};

template<typename T>
concept is_WindowResize = is<T> && requires(const T &t)
{
  T(0, 0);
  {
    t.Width()
    } -> glengine::decay_same_as<int>;
  {
    t.Height()
    } -> glengine::decay_same_as<int>;
};
}// namespace Event
#endif// MYPROJECT_EVENTCONCEPTS_HPP
