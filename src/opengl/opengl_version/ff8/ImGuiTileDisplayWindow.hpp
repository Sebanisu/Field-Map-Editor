//
// Created by pcvii on 5/30/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUITILEDISPLAYWINDOW_HPP
#define FIELD_MAP_EDITOR_IMGUITILEDISPLAYWINDOW_HPP
#include <Counter.hpp>
#include <scope_guard.hpp>
namespace ff8
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
  void        OnUpdate(float) const;
  void        OnImGuiUpdate() const;
  void        OnEvent(const glengine::Event::Item &) const;
  void        OnRender() const;
  static void TakeControl(const bool has_hover, const glengine::Counter id);
  static bool OnImGuiUpdateForward(auto &&...params)
  {
    if (GetWindow())
    {
      return GetWindow()->OnImGuiUpdate(
        std::forward<decltype(params)>(params)...);
    }
    return false;
  }
  bool OnImGuiUpdate(const glengine::Counter id, std::invocable auto &&function)
    const
  {
    if (m_drawn || m_current_id != id)
      return false;
    const auto the_end = glengine::scope_guard{ []() { ImGui::End(); } };
    if (ImGui::Begin(s_title))
    {
      m_drawn = true;
      function();
      return true;
    }
    return false;
  }

private:
  static ImGuiTileDisplayWindow  *GetWindow();
  static inline const char *const s_title      = "Tile Display Window";
  mutable bool                    m_drawn      = { false };
  mutable glengine::Counter       m_current_id = glengine::Counter{ 0U };
};
static_assert(glengine::Renderable<ImGuiTileDisplayWindow>);
}// namespace ff8
#endif// FIELD_MAP_EDITOR_IMGUITILEDISPLAYWINDOW_HPP
