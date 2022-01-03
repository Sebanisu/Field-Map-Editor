//
// Created by pcvii on 11/23/2021.
//

#ifndef MYPROJECT_RENDERABLE_HPP
#define MYPROJECT_RENDERABLE_HPP
namespace Event
{
class Item;
}
template<typename T>
concept Renderable =
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

// free function overloads for the member functions.
template<Renderable T>
inline auto OnImGuiUpdate(const T &t)
{
  return t.OnImGuiUpdate();
}
template<Renderable T>
inline auto OnUpdate(const T &t, const float ts)
{
  return t.OnUpdate(ts);
}
template<Renderable T>
inline auto OnRender(const T &t)
{
  return t.OnRender();
}
template<Renderable T>
inline auto OnEvent(const T &t, const Event::Item &e)
{
  return t.OnEvent(e);
}
#endif// MYPROJECT_RENDERABLE_HPP
