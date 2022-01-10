//
// Created by pcvii on 12/2/2021.
//

#include "OrthographicCameraController.hpp"
#include "Event/EventDispatcher.hpp"
#include "GLCheck.hpp"
#include "Input.hpp"

extern float Get_Frame_Buffer_Aspect_Ratio();
namespace glengine
{
void OrthographicCameraController::OnUpdate(float ts) const
{
  using glengine::Input;
  using glengine::KEY;
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
    m_position = {};
    SetZoom();
  }

  const auto lateral_speed  = m_translation_speed * ts;
  const auto relative_speed = [&]() {
    auto r = lateral_speed * m_zoom_level;
    if (m_bounds)
    {
      const auto height = std::abs(m_bounds->top - m_bounds->bottom);
      if (height > 0 && r > 0)
      {
        const auto default_zoom_level = height / 2.F;
        r /= default_zoom_level;
      }
    }
    return r;
  }();

  if (Input::IsKeyPressed(KEY::A) || Input::IsKeyPressed(KEY::LEFT))
  {
    m_position.x -= relative_speed;
  }
  if (Input::IsKeyPressed(KEY::D) || Input::IsKeyPressed(KEY::RIGHT))
  {
    m_position.x += relative_speed;
  }
  if (Input::IsKeyPressed(KEY::S) || Input::IsKeyPressed(KEY::DOWN))
  {
    m_position.y -= relative_speed;
  }
  if (Input::IsKeyPressed(KEY::W) || Input::IsKeyPressed(KEY::UP))
  {
    m_position.y += relative_speed;
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
    const auto bounds = CurrentBounds();

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
  {
    const auto bounds = CurrentBounds();
    ImGui::Text(
      "%s",
      fmt::format(
        "Current Display Bounds:\nleft {:>4.4f}, right {:>4.4f}, bottom "
        "{:>4.4f}, top {:>4.4f}",
        bounds.left,
        bounds.right,
        bounds.bottom,
        bounds.top)
        .c_str());
  }
  {
    const auto &temp = MaxBounds();
    if (temp)
    {
      const auto &bounds = *temp;
      ImGui::Text(
        "%s",
        fmt::format(
          "Max Bounds:\nleft {:>4.4f}, right {:>4.4f}, bottom {:>4.4f}, top "
          "{:>4.4f}",
          bounds.left,
          bounds.right,
          bounds.bottom,
          bounds.top)
          .c_str());
    }
  }
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
OrthographicCameraController::return_values
  OrthographicCameraController::CurrentBounds() const
{
  return return_values{ .left   = -m_zoom_level * m_aspect_ratio + m_position.x,
                        .right  = m_zoom_level * m_aspect_ratio + m_position.x,
                        .bottom = -m_zoom_level + m_position.y,
                        .top    = m_zoom_level + m_position.y };
}
const std::optional<OrthographicCameraController::return_values> &
  OrthographicCameraController::MaxBounds() const
{
  return m_bounds;
}
void OrthographicCameraController::SetMaxBounds(
  OrthographicCameraController::return_values bounds) const
{
  if (
    std::abs(bounds.left) + std::abs(bounds.right)
      > std::numeric_limits<float>::epsilon()
    && std::abs(bounds.bottom) + std::abs(bounds.top)
         > std::numeric_limits<float>::epsilon())
    m_bounds = std::move(bounds);
  else
    DisableBounds();
}
void OrthographicCameraController::DisableBounds() const
{
  m_bounds = std::nullopt;
}
void OrthographicCameraController::RefreshAspectRatio() const
{
  RefreshAspectRatio(Get_Frame_Buffer_Aspect_Ratio());
}
void OrthographicCameraController::RefreshAspectRatio(
  float new_aspect_ratio) const
{
  m_aspect_ratio = new_aspect_ratio;
  set_projection();
}
void OrthographicCameraController::SetZoom() const
{
  if (m_bounds)
  {
    m_zoom_level = (m_bounds->top - m_bounds->bottom) / 2.F;
    zoom(0.F);
  }
}
void OrthographicCameraController::SetZoom(float new_zoom) const
{
  m_zoom_level = new_zoom;
  zoom(0.F);
}

void OrthographicCameraController::set_projection() const
{
  m_camera.SetProjection(
    -m_aspect_ratio * m_zoom_level,
    m_aspect_ratio * m_zoom_level,
    -m_zoom_level,
    m_zoom_level);
}
OrthographicCameraController::OrthographicCameraController()
  : OrthographicCameraController(Get_Frame_Buffer_Aspect_Ratio(), 1.F)
{
}
OrthographicCameraController::OrthographicCameraController(float zoom)
  : OrthographicCameraController(Get_Frame_Buffer_Aspect_Ratio(), zoom)
{
}
/**
 * Convert bounds to viewport.
 * @param bounds only computes the width and height. recommended to use a fixed
 * bounds value.
 */
void MakeViewPortMatchBounds(
  const OrthographicCameraController::return_values &bounds)
{
  GLCall{}(
    glViewport,
    GLint{ 0 },
    GLint{ 0 },
    std::abs(
      static_cast<GLsizei>(bounds.right) - static_cast<GLsizei>(bounds.left)),
    std::abs(
      static_cast<GLsizei>(bounds.top) - static_cast<GLsizei>(bounds.bottom)));
}
}// namespace glengine
