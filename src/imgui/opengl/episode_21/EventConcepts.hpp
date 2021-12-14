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
concept is = decay_same_as<typename T::value_type, T> && requires(const T &t)
{
  {
    t.Name()
    } -> decay_same_as<std::string_view>;
  {
    t.category()
    } -> decay_same_as<Category>;
  {
    t.CategoryName()
    } -> decay_same_as<std::string_view>;
  {
    t.Handled()
    } -> decay_same_as<bool>;
  {
    t.Data()
    } -> decay_same_as<std::string>;
};
template<typename T>
concept is_Key = is<T> && requires(const T &t)
{
  T(KEY::A);
  {
    t.Key()
    } -> decay_same_as<KEY>;
};
template<typename T>
concept is_MouseMove = is<T> && requires(const T &t)
{
  T(0.F, 0.F);
  {
    t.Position()
    } -> decay_same_as<std::array<float, 2U>>;
  {
    t.X()
    } -> decay_same_as<float>;
  {
    t.Y()
    } -> decay_same_as<float>;
};
template<typename T>
concept is_WindowMove = is<T> && requires(const T &t)
{
  T(0, 0);
  {
    t.Position()
    } -> decay_same_as<std::array<int, 2U>>;
  {
    t.X()
    } -> decay_same_as<int>;
  {
    t.Y()
    } -> decay_same_as<int>;
};
template<typename T>
concept is_MouseButton = is<T> && requires(const T &t)
{
  T(MOUSE::BUTTON_1);
  {
    t.Button()
    } -> decay_same_as<MOUSE>;
};
template<typename T>
concept is_MouseScroll = is<T> && requires(const T &t)
{
  T(0.F, 0.F);
  {
    t.Offsets()
    } -> decay_same_as<std::array<float, 2U>>;
  {
    t.XOffset()
    } -> decay_same_as<float>;
  {
    t.YOffset()
    } -> decay_same_as<float>;
};

template<typename T>
concept is_WindowResize = is<T> && requires(const T &t)
{
  T(0, 0);
  {
    t.Width()
    } -> decay_same_as<int>;
  {
    t.Height()
    } -> decay_same_as<int>;
};
}// namespace Event
#endif// MYPROJECT_EVENTCONCEPTS_HPP
