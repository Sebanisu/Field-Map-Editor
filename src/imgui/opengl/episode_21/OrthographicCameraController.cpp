//
// Created by pcvii on 12/2/2021.
//

#include "OrthographicCameraController.hpp"

OrthographicCameraController::OrthographicCameraController(float aspect_ratio)
  : m_aspect_ratio(aspect_ratio)
  , m_camera{ -m_aspect_ratio * m_zoom_level,
              m_aspect_ratio * m_zoom_level,
              -m_zoom_level,
              m_zoom_level }
{
  // todo optional rotation?
}
void
  OnUpdate(const OrthographicCameraController &self, float)
{
  // todo movespeed
  //  todo keyboard command
  // a left = x -= speed * ts;
  // d right = x += speed * ts;
  // s down = y -= speed * ts;
  // w up = y += speed * ts;
  if (self.m_rotation)
  {
    // q rot_left rot += rot_speed * ts
    // e rot_right rot -= rot_speed * ts
    self.m_camera.SetRotation(self.m_rotation);
  }
  self.m_camera.SetPosition(self.m_position);
  // todo events
  // void OnEvent(event& e);
  // bool OnMouseScrolled(event& e);
  //  {
  //    m_zoom_level -= e.get_y_offset();
  //  m_camera = { -m_aspect_ratio * m_zoom_level,
  //               m_aspect_ratio * m_zoom_level,
  //               -m_zoom_level,
  //               m_zoom_level };
  //  }
  // bool OnWindowResized(event& e);
  //  {
  //  if(e.get_width == 0 || e.get_height == 0)
  //    return;
  //    m_aspect_ratio = e.get_width / e.get_height;
  // move this to it's own function?
  //  self.m_camera.SetProjection( -self.m_aspect_ratio * self.m_zoom_level,
  //                 self.m_aspect_ratio * self.m_zoom_level,
  //                 -self.m_zoom_level,
  //                 self.m_zoom_level);
  //  }
}
void
  OnImguiRender(const OrthographicCameraController &)
{
}
