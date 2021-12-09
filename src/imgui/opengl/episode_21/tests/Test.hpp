//
// Created by pcvii on 11/23/2021.
//

#ifndef MYPROJECT_TEST_HPP
#define MYPROJECT_TEST_HPP
#include "concepts.hpp"
namespace Event
{
class Item;
}
namespace test
{
template<typename T>
concept Test =
  std::default_initializable<T> && std::movable<T> &&(
    requires(const T &t, const Event::Item & e)
    {
      {
        t.OnImGuiUpdate()
        } -> std::same_as<bool>;
      {
        t.OnUpdate(float{})
        } -> Void;
      {
        t.OnRender()
        } -> Void;
      {
        t.OnEvent(e)
        } -> Void;
    } ||
    requires(const T &t, const Event::Item & e)
    {
      {
        t.OnImGuiUpdate()
        } -> Void;
      {
        t.OnUpdate(float{})
        } -> Void;
      {
        t.OnRender()
        } -> Void;
      {
        t.OnEvent(e)
        } -> Void;
    });
}// namespace test
// free function overloads for the member functions.
template<test::Test T>
inline auto OnImGuiUpdate(const T &t)
{
  return t.OnImGuiUpdate();
}
template<test::Test T>
inline auto OnUpdate(const T &t, const float ts)
{
  return t.OnUpdate(ts);
}
template<test::Test T>
inline auto OnRender(const T &t)
{
  return t.OnRender();
}
template<test::Test T>
inline auto OnEvent(const T &t, const Event::Item &e)
{
  return t.OnEvent(e);
}
#endif// MYPROJECT_TEST_HPP
