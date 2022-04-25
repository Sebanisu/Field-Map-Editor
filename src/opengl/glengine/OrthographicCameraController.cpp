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
    FitHeight();
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
    const auto bounds     = CurrentBounds();

    const auto fix_bounds = [](
                              float                     &pos,
                              [[maybe_unused]] glm::vec2 img_max,
                              [[maybe_unused]] glm::vec2 viewport_scaled) {
      float minpos = (std::min)(
        viewport_scaled.x + img_max.y, viewport_scaled.y + img_max.x);
      float maxpos = (std::max)(
        viewport_scaled.x + img_max.y, viewport_scaled.y + img_max.x);
      pos = std::clamp(pos, minpos, maxpos);
      /*
       * How to fix camera?
       * Screenbounds? ViewPort -width/2 and -height/2 scaled to same as zoom.
       * objectbounds? image width/2 and height/2;
       * x =Clamp (x, screenbounds.x +objectbounds.x, screenbounds.x * -1 -
       * objectbounds.x); y =Clamp (y, screenbounds.y +objectbounds.y,
       * screenbounds.y * -1 - objectbounds.y);
       */
    };
    //    float left       = (std::min)(bounds.left, bounds.right);
    //    float right      = (std::max)(bounds.left, bounds.right);
    //    float bottom     = (std::min)(bounds.bottom, bounds.top);
    //    float top        = (std::max)(bounds.bottom, bounds.top);
    float max_left      = (std::min)(m_bounds->left, m_bounds->right);
    float max_right     = (std::max)(m_bounds->left, m_bounds->right);
    float max_bottom    = (std::min)(m_bounds->bottom, m_bounds->top);
    float max_top       = (std::max)(m_bounds->bottom, m_bounds->top);
    float scaled_left   = m_camera.Bounds().x;
    float scaled_right  = m_camera.Bounds().y;
    float scaled_bottom = m_camera.Bounds().z;
    float scaled_top    = m_camera.Bounds().w;

    fix_bounds(
      m_position.x,
      glm::vec2(max_left, max_right),
      glm::vec2(scaled_left, scaled_right));
    fix_bounds(
      m_position.y,
      glm::vec2(max_bottom, max_top),
      glm::vec2(scaled_bottom, scaled_top));
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
  ImGui::Text(
    "%s",
    fmt::format("Viewport AspectRatio: {}", m_viewport_aspect_ratio).c_str());
  ImGui::Text(
    "%s",
    fmt::format("Image    AspectRatio: {}", m_image_aspect_ratio).c_str());

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
    ImGui::Text(
      "%s",
      fmt::format(
        "Scaled Bounds:\nleft {:>4.4f}, right {:>4.4f}, bottom {:>4.4f}, top "
        "{:>4.4f}",
        m_camera.Bounds().x,
        m_camera.Bounds().y,
        m_camera.Bounds().z,
        m_camera.Bounds().w)
        .c_str());
    ImGui::Separator();
    m_camera.OnImGuiUpdate();
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
      RefreshAspectRatio();
      // m_aspect_ratio =
      // static_cast<float>(fbr.Width()) / static_cast<float>(fbr.Height());
      SetProjection();
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
  SetProjection();
}
OrthographicCameraController::return_values
  OrthographicCameraController::CurrentBounds() const
{
  return return_values{ .left   = m_camera.Bounds().x + m_position.x,
                        .right  = m_camera.Bounds().y + m_position.x,
                        .bottom = m_camera.Bounds().z + m_position.y,
                        .top    = m_camera.Bounds().w + m_position.y };
}
std::optional<OrthographicCameraController::return_values>
  OrthographicCameraController::MaxBounds() const
{
  //  if (m_bounds.has_value())
  //    return m_bounds.value() * m_zoom_level;
  return m_bounds;
}
void OrthographicCameraController::SetImageBounds(
  OrthographicCameraController::return_values bounds) const
{
  if (
    std::abs(bounds.left) + std::abs(bounds.right)
      > std::numeric_limits<float>::epsilon()
    && std::abs(bounds.bottom) + std::abs(bounds.top)
         > std::numeric_limits<float>::epsilon())
  {
    m_image_aspect_ratio = (std::abs(bounds.left) + std::abs(bounds.right))
                           / (std::abs(bounds.bottom) + std::abs(bounds.top));
    m_bounds = std::move(bounds);
  }
  else
  {
    DisableBounds();
  }
}
void OrthographicCameraController::DisableBounds() const
{
  m_bounds = std::nullopt;
}
void OrthographicCameraController::RefreshAspectRatio() const
{
  RefreshAspectRatio(Get_Frame_Buffer_Aspect_Ratio());
}
template<class T>
static bool almost_equal(T x, T y, int ulp = 5)
{
  // the machine epsilon has to be scaled to the magnitude of the values used
  // and multiplied by the desired precision in ULPs (units in the last place)
  return std::fabs(x - y)
           <= std::numeric_limits<T>::epsilon() * std::fabs(x + y) * ulp
         // unless the result is subnormal
         || std::fabs(x - y) < std::numeric_limits<T>::min();
}
void OrthographicCameraController::RefreshAspectRatio(
  float new_aspect_ratio) const
{
  if (!almost_equal(new_aspect_ratio, m_viewport_aspect_ratio))
  {
    m_viewport_aspect_ratio = new_aspect_ratio;
    SetProjection();
  }
}
void OrthographicCameraController::FitHeight() const
{
  if (m_bounds)
  {
    m_zoom_level = (m_bounds->top - m_bounds->bottom);
    zoom(0.F);
    m_position.y = {};// center height
    m_camera.SetPosition(m_position);
  }
}
void OrthographicCameraController::FitWidth() const
{
  if (m_bounds)
  {
    m_zoom_level = (m_bounds->right - m_bounds->left) / m_viewport_aspect_ratio;
    zoom(0.F);
    m_position.x = {};// center width
    m_camera.SetPosition(m_position);
  }
}
void OrthographicCameraController::FitBoth() const
{
  if (m_bounds)
  {
    m_zoom_level = (std::max)(
      m_bounds->top - m_bounds->bottom,
      (m_bounds->right - m_bounds->left) / m_viewport_aspect_ratio);
    zoom(0.F);
    m_position = {};// center
    m_camera.SetPosition(m_position);
  }
}
void OrthographicCameraController::SetZoom(float new_zoom) const
{
  m_zoom_level = new_zoom;
  zoom(0.F);
}

void OrthographicCameraController::SetProjection() const
{
  m_camera.SetProjection(m_zoom_level, m_viewport_aspect_ratio);
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
