//
// Created by pcvii on 11/23/2021.
//

#ifndef FIELD_MAP_EDITOR_RENDERABLE_HPP
#define FIELD_MAP_EDITOR_RENDERABLE_HPP
namespace glengine
{
namespace event
{
     class Item;
}
template<typename T>
concept Renderable =
  std::default_initializable<T> && std::movable<T> &&(
    requires(const T &t, const event::Item & e)
    {
      {
        t.on_im_gui_update()
        } -> std::same_as<bool>;
      {
        t.on_update(float{})
        } -> Void;
      {
        t.on_render()
        } -> Void;
      {
        t.on_event(e)
        } -> Void;
    } ||
    requires(const T &t, const event::Item & e)
    {
      {
        t.on_im_gui_update()
        } -> Void;
      {
        t.on_update(float{})
        } -> Void;
      {
        t.on_render()
        } -> Void;
      {
        t.on_event(e)
        } -> Void;
    });

// free function overloads for the member functions.
template<Renderable T>
inline auto OnImGuiUpdate(const T &t)
{
     return t.on_im_gui_update();
}
template<Renderable T>
inline auto OnUpdate(const T &t, const float ts)
{
     return t.on_update(ts);
}
template<Renderable T>
inline auto OnRender(const T &t)
{
     return t.on_render();
}
template<Renderable T>
inline auto OnEvent(const T &t, const event::Item &e)
{
     return t.on_event(e);
}
}// namespace glengine
#endif// FIELD_MAP_EDITOR_RENDERABLE_HPP
