//
// Created by pcvii on 5/30/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUITILEDISPLAYWINDOW_HPP
#define FIELD_MAP_EDITOR_IMGUITILEDISPLAYWINDOW_HPP
#include <Counter.hpp>
#include <ScopeGuard.hpp>
namespace ff_8
{
/**
 * ImGui Tile Display Window
 * Undefined behavior if you have more than 1 of these.
 */
class ImGuiTileDisplayWindow
{
public:
  ImGuiTileDisplayWindow();
  ImGuiTileDisplayWindow(const ImGuiTileDisplayWindow &)            = default;
  ImGuiTileDisplayWindow(ImGuiTileDisplayWindow &&) noexcept        = default;
  ImGuiTileDisplayWindow &operator=(const ImGuiTileDisplayWindow &) = default;
  ImGuiTileDisplayWindow &
    operator=(ImGuiTileDisplayWindow &&) noexcept = default;
  ~ImGuiTileDisplayWindow();
  void        on_update(float) const;
  void        on_im_gui_update() const;
  void        on_event(const glengine::event::Item &) const;
  void        on_render() const;
  static void take_control(const bool has_hover, const glengine::Counter id);
  static bool on_render_forward(auto &&...params)
  {
    if (get_window())
    {
      return get_window()->on_render(
        std::forward<decltype(params)>(params)...);
    }
    return false;
  }
  bool
    on_render(const glengine::Counter id, std::invocable auto &&function) const
  {
    if (m_drawn || m_current_id != id)
      return false;
    const auto the_end = glengine::ScopeGuard{ []() { ImGui::End(); } };
    if (ImGui::Begin(s_title))
    {
      m_drawn = true;
      function();
      return true;
    }
    return false;
  }

private:
  static ImGuiTileDisplayWindow            *get_window();
  static constexpr inline const char *const s_title = "Tile Display Window";
  mutable bool                              m_drawn = { false };
  mutable glengine::Counter m_current_id            = glengine::Counter{ 0U };
};
static_assert(glengine::Renderable<ImGuiTileDisplayWindow>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_IMGUITILEDISPLAYWINDOW_HPP
