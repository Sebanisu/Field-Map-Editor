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
concept HasOnUpdate = requires(const T &t, float ts) {
     { t.on_update(ts) } -> Void;
};

template<typename T>
concept HasOnRender = requires(const T &t) {
     { t.on_render() } -> Void;
};

template<typename T>
concept HasOnEvent = requires(const T &t, const event::Item &e) {
     { t.on_event(e) } -> Void;
};


template<typename T>
concept HasOnImGuiUpdate = requires(const T &t) {
     { t.on_im_gui_update() } -> std::same_as<bool>;
} || requires(const T &t) {
     { t.on_im_gui_update() } -> Void;
};

template<typename T>
concept HasOnImGuiFileMenu = requires(const T &t) {
     { t.on_im_gui_file_menu() } -> std::same_as<bool>;
} || requires(const T &t) {
     { t.on_im_gui_file_menu() } -> Void;
};

template<typename T>
concept HasOnImGuiEditMenu = requires(const T &t) {
     { t.on_im_gui_edit_menu() } -> std::same_as<bool>;
} || requires(const T &t) {
     { t.on_im_gui_edit_menu() } -> Void;
};

template<typename T>
concept HasOnImGuiWindowMenu = requires(const T &t) {
     { t.on_im_gui_window_menu() } -> std::same_as<bool>;
} || requires(const T &t) {
     { t.on_im_gui_window_menu() } -> Void;
};

template<typename T>
concept HasOnImGuiHelpMenu = requires(const T &t) {
     { t.on_im_gui_help_menu() } -> std::same_as<bool>;
} || requires(const T &t) {
     { t.on_im_gui_help_menu() } -> Void;
};

template<typename T>
concept Renderable
  = std::default_initializable<T> && std::movable<T>
    && (HasOnUpdate<T> || HasOnRender<T> || HasOnEvent<T> || HasOnImGuiUpdate<T> || HasOnImGuiFileMenu<T> || HasOnImGuiEditMenu<T> || HasOnImGuiWindowMenu<T> || HasOnImGuiHelpMenu<T>);


// free function overloads for the member functions.
template<Renderable T>
inline auto OnImGuiUpdate(const T &t)
{
     if constexpr (HasOnImGuiUpdate<T>)
          return t.on_im_gui_update();
}

template<Renderable T>
inline auto OnUpdate(
  const T &t,
  float    ts)
{
     if constexpr (HasOnUpdate<T>)
          return t.on_update(ts);
}

template<Renderable T>
inline auto OnRender(const T &t)
{
     if constexpr (HasOnRender<T>)
          return t.on_render();
}

template<Renderable T>
inline auto OnEvent(
  const T           &t,
  const event::Item &e)
{
     if constexpr (HasOnEvent<T>)
          return t.on_event(e);
}

template<Renderable T>
inline auto OnImGuiFileMenu(const T &t)
{
     if constexpr (HasOnImGuiFileMenu<T>)
          return t.on_im_gui_file_menu();
}

template<Renderable T>
inline auto OnImGuiEditMenu(const T &t)
{
     if constexpr (HasOnImGuiEditMenu<T>)
          return t.on_im_gui_edit_menu();
}

template<Renderable T>
inline auto OnImGuiWindowMenu(const T &t)
{
     if constexpr (HasOnImGuiWindowMenu<T>)
          return t.on_im_gui_window_menu();
}

template<Renderable T>
inline auto OnImGuiHelpMenu(const T &t)
{
     if constexpr (HasOnImGuiHelpMenu<T>)
          return t.on_im_gui_help_menu();
}

}// namespace glengine
#endif// FIELD_MAP_EDITOR_RENDERABLE_HPP
