//
// Created by pcvii on 12/2/2021.
//

#include "OrthographicCameraController.hpp"
#include "Event/EventDispatcher.hpp"
#include "Input.hpp"

void OrthographicCameraController::OnUpdate(float ts) const
{
  if (Input::IsKeyPressed(KEY::EQUAL) || Input::IsKeyPressed(KEY::KP_ADD))
  {
    zoom(1.F);
  }
  if (Input::IsKeyPressed(KEY::MINUS) || Input::IsKeyPressed(KEY::KP_SUBTRACT))
  {
    zoom(-1.F);
  }
  if (Input::IsKeyPressed(KEY::Z))
  {
    SetZoom();
  }
  if (Input::IsKeyPressed(KEY::A) || Input::IsKeyPressed(KEY::LEFT))
  {
    m_position.x -= m_translation_speed * ts * m_zoom_level;
  }
  if (Input::IsKeyPressed(KEY::D) || Input::IsKeyPressed(KEY::RIGHT))
  {
    m_position.x += m_translation_speed * ts * m_zoom_level;
  }
  if (Input::IsKeyPressed(KEY::S) || Input::IsKeyPressed(KEY::DOWN))
  {
    m_position.y -= m_translation_speed * ts * m_zoom_level;
  }
  if (Input::IsKeyPressed(KEY::W) || Input::IsKeyPressed(KEY::UP))
  {
    m_position.y += m_translation_speed * ts * m_zoom_level;
  }
  {
    if (Input::IsKeyPressed(KEY::Q) || Input::IsKeyPressed(KEY::PAGE_UP))
    {
      m_rotation += m_rotation_speed * ts;
    }
    if (Input::IsKeyPressed(KEY::E) || Input::IsKeyPressed(KEY::PAGE_DOWN))
    {
      m_rotation -= m_rotation_speed * ts;
    }
    if (Input::IsKeyPressed(KEY::R) || Input::IsKeyPressed(KEY::HOME))
    {
      m_rotation = 0.F;
    }
    m_rotation = std::clamp(m_rotation, -180.F, 180.F);
    m_camera.SetRotation(m_rotation);
  }
  if (m_bounds)
  {
    const auto bounds = Bounds();

    const auto fix_bounds =
      [](float &pos, float low, float high, float low_max, float high_max) {
        const bool  larger_span = high - low >= high_max - low_max;
        const float span        = [=]() {
          if (larger_span)
            return (high - low) - (high_max - low_max);
          return 0.F;
        }();
        const bool bottom = low < low_max - span;
        const bool top    = high > high_max + span;
        if (bottom)
        {
          pos += low_max - span - low;
        }
        if (top)
        {
          pos -= high - (high_max + span);
        }
      };
    fix_bounds(
      m_position.x, bounds.left, bounds.right, m_bounds->left, m_bounds->right);
    fix_bounds(
      m_position.y, bounds.bottom, bounds.top, m_bounds->bottom, m_bounds->top);
  }
  m_camera.SetPosition(m_position);
}
bool OrthographicCameraController::OnImGuiUpdate() const
{
  // draw info about the camera here.
  ImGui::Text(
    "%s",
    fmt::format("X, Y: {:>4.4f}, {:>4.4f}", m_position.x, m_position.y)
      .c_str());
  ImGui::Text("%s", fmt::format("Rotation: {}", m_rotation).c_str());
  ImGui::Text("%s", fmt::format("Zoom: {}", m_zoom_level).c_str());
  ImGui::Text(
    "%s", fmt::format("Zoom Precision: {}", m_zoom_precision).c_str());
  const auto bounds = Bounds();
  ImGui::Text(
    "%s",
    fmt::format(
      "Bounds: left {:>4.4f}, right {:>4.4f}, bottom {:>4.4f}, top {:>4.4f}",
      bounds.left,
      bounds.right,
      bounds.bottom,
      bounds.top)
      .c_str());
  return false;
}

void OrthographicCameraController::OnEvent(const Event::Item &e) const
{

  Event::Dispatcher dispatcher(e);
  dispatcher.Dispatch<Event::MouseScroll>([this](const Event::MouseScroll &ms) {
    zoom(ms.YOffset());
    return true;
  });
  dispatcher.Dispatch<Event::FrameBufferResize>(
    [this](const Event::FrameBufferResize &fbr) -> bool {
      if (fbr.Width() == 0 || fbr.Height() == 0)
      {
        return false;
      }
      m_aspect_ratio =
        static_cast<float>(fbr.Width()) / static_cast<float>(fbr.Height());
      set_projection();
      return true;
    });
}
void OrthographicCameraController::zoom(const float offset) const
{
  if (
    m_zoom_level - offset * m_zoom_precision
    < std::numeric_limits<float>::epsilon())
  {
    m_zoom_precision /= 10.F;
  }
  if (
    m_zoom_level - offset * m_zoom_precision - m_zoom_precision * 10.F
    > std::numeric_limits<float>::epsilon())
  {
    m_zoom_precision *= 10.F;
    if (m_zoom_precision > 0.25F)
      m_zoom_precision = std::nearbyint(m_zoom_precision);
  }
  m_zoom_precision = (std::clamp)(m_zoom_precision, 0.0001F, 1.F);

  m_zoom_level -= offset * m_zoom_precision;
  if (m_zoom_precision - 1.F > std::numeric_limits<float>::epsilon())
    m_zoom_level = std::nearbyint(m_zoom_level);
  m_zoom_level = (std::max)(m_zoom_level, 0.0001F);
  set_projection();
}
