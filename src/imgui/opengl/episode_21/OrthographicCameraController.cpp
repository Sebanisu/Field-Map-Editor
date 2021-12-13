//
// Created by pcvii on 12/2/2021.
//

#include "OrthographicCameraController.hpp"
#include "EventDispatcher.hpp"
#include "Input.hpp"

OrthographicCameraController::OrthographicCameraController(float aspect_ratio)
  : m_aspect_ratio(aspect_ratio)
  , m_camera{ -m_aspect_ratio * m_zoom_level,
              m_aspect_ratio * m_zoom_level,
              -m_zoom_level,
              m_zoom_level }
{
  // todo optional rotation?
}
void OrthographicCameraController::OnUpdate(float ts) const
{
  if (Input::IsKeyPressed(KEY::A) || Input::IsKeyPressed(KEY::LEFT))
  {
    m_position.x -= m_translation_speed * ts;
  }
  if (Input::IsKeyPressed(KEY::D) || Input::IsKeyPressed(KEY::RIGHT))
  {
    m_position.x += m_translation_speed * ts;
  }
  if (Input::IsKeyPressed(KEY::S) || Input::IsKeyPressed(KEY::DOWN))
  {
    m_position.y -= m_translation_speed * ts;
  }
  if (Input::IsKeyPressed(KEY::W) || Input::IsKeyPressed(KEY::UP))
  {
    m_position.y += m_translation_speed * ts;
  }
  if (m_rotation_enabled)
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
  return false;
}
void OrthographicCameraController::OnEvent(const Event::Item &e) const
{
  const auto set_projection = [this]() {
    m_camera.SetProjection(
      -m_aspect_ratio * m_zoom_level,
      m_aspect_ratio * m_zoom_level,
      -m_zoom_level,
      m_zoom_level);
  };
  Event::Dispatcher dispatcher(e);
  dispatcher.Dispatch<Event::MouseScroll>(
    [&set_projection, this](const Event::MouseScroll &ms) {
      if (
        m_zoom_level - ms.YOffset() * m_zoom_precision
        < std::numeric_limits<float>::epsilon())
      {
        m_zoom_precision /= 10.F;
      }
      if (
        m_zoom_level - ms.YOffset() * m_zoom_precision - m_zoom_precision * 10.F
        > std::numeric_limits<float>::epsilon())
      {
        m_zoom_precision *= 10.F;
        if (m_zoom_precision > 0.25F)
          m_zoom_precision = std::nearbyint(m_zoom_precision);
      }
      m_zoom_precision = (std::clamp)(m_zoom_precision, 0.0001F, 1.F);

      m_zoom_level -= ms.YOffset() * m_zoom_precision;
      if (m_zoom_precision - 1.F > std::numeric_limits<float>::epsilon())
        m_zoom_level = std::nearbyint(m_zoom_level);
      m_zoom_level = (std::max)(m_zoom_level, 0.0001F);
      set_projection();
      return true;
    });
  dispatcher.Dispatch<Event::FrameBufferResize>(
    [&set_projection, this](const Event::FrameBufferResize &fbr) -> bool {
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
