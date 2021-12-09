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
void OnUpdate(const OrthographicCameraController &self, float ts)
{
  if (Input::IsKeyPressed(KEY::A) || Input::IsKeyPressed(KEY::LEFT))
    self.m_position.x -= self.m_translation_speed * ts;
  if (Input::IsKeyPressed(KEY::D) || Input::IsKeyPressed(KEY::RIGHT))
    self.m_position.x += self.m_translation_speed * ts;
  if (Input::IsKeyPressed(KEY::S) || Input::IsKeyPressed(KEY::DOWN))
    self.m_position.y -= self.m_translation_speed * ts;
  if (Input::IsKeyPressed(KEY::W) || Input::IsKeyPressed(KEY::UP))
    self.m_position.y += self.m_translation_speed * ts;
  if (self.m_rotation_enabled)
  {

    if (Input::IsKeyPressed(KEY::Q) || Input::IsKeyPressed(KEY::PAGE_UP))
      self.m_rotation += self.m_rotation_speed * ts;
    if (Input::IsKeyPressed(KEY::E) || Input::IsKeyPressed(KEY::PAGE_DOWN))
      self.m_rotation -= self.m_rotation_speed * ts;
    self.m_camera.SetRotation(self.m_rotation);
  }
  self.m_camera.SetPosition(self.m_position);
}
void OnImguiRender(const OrthographicCameraController &)
{
  // draw info about the camera here.
}
void OnEvent(const OrthographicCameraController &self, const Event::Item &e)
{
  const auto set_projection = [&self]() {
    self.m_camera.SetProjection(
      -self.m_aspect_ratio * self.m_zoom_level,
      self.m_aspect_ratio * self.m_zoom_level,
      -self.m_zoom_level,
      self.m_zoom_level);
  };
  Event::Dispatcher dispatcher(e);
  dispatcher.Dispatch<Event::MouseScroll>(
    [&self, &set_projection](const Event::MouseScroll &ms) {
      self.m_zoom_level -= ms.YOffset();
      set_projection();
      return true;
    });
  dispatcher.Dispatch<Event::FrameBufferResize>(
    [&self, &set_projection](const Event::FrameBufferResize &fbr) -> bool {
      if (fbr.Width() == 0 || fbr.Height() == 0)
      {
        return false;
      }
      self.m_aspect_ratio =
        static_cast<float>(fbr.Width()) / static_cast<float>(fbr.Height());
      set_projection();
      return true;
    });
}
