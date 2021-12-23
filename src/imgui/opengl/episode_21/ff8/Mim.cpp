//
// Created by pcvii on 11/30/2021.
//

#include "Mim.hpp"
#include "FrameBuffer.hpp"
#include "OrthographicCameraController.hpp"
#include "PixelBuffer.hpp"
namespace ff8
{
static const BPPs                   bpp                 = {};
static const Palettes               palette             = {};
static bool                         draw_palette        = false;
static bool                         draw_grid           = false;
static bool                         snap_zoom_to_height = true;
static bool                         saving              = false;
static OrthographicCameraController camera              = { 16 / 9 };
}// namespace ff8
void ff8::Mim::OnUpdate(float ts) const
{
  m_delayed_textures.check();
  const auto &texture = CurrentTexture();
  camera.SetMaxBounds({ 0.F,
                        static_cast<float>(texture.width()),
                        0.F,
                        static_cast<float>(texture.height()) });
  if (snap_zoom_to_height)
  {
    camera.SetZoom();
  }
  camera.OnUpdate(ts);
  m_batch_renderer.OnUpdate(ts);
}

void ff8::Mim::OnRender() const
{
  camera.OnRender();
  SetUniforms();
  const auto &texture = CurrentTexture();
  if (texture.width() == 0 || texture.height() == 0)
  {
    return;
  }
  glm::vec2 size = { texture.width(), texture.height() };
  m_batch_renderer.Clear();
  m_batch_renderer.DrawQuad(glm::vec2{ 0.F }, texture, size);
  m_batch_renderer.Draw();
  m_batch_renderer.OnRender();
}
void ff8::Mim::OnImGuiUpdate() const
{
  const auto &texture = CurrentTexture();
  {
    const auto disable = scope_guard(&ImGui::EndDisabled);
    ImGui::BeginDisabled(texture.height() == 0 || texture.width() == 0);
    ImGui::Checkbox("Draw Palette", &draw_palette);
    ImGui::Checkbox("Draw Grid", &draw_grid);
    ImGui::Checkbox("Snap Zoom to Height", &snap_zoom_to_height);
    if (bpp.OnImGuiUpdate() || palette.OnImGuiUpdate())
    {
    }
  }
  ImGui::Text(
    "%s",
    fmt::format(
      "Texture Width: {:>5}, Height: {:>5}", texture.width(), texture.height())
      .c_str());
  ImGui::Separator();
  if (camera.OnImGuiUpdate())
  {
  }
  if (ImGui::Button("Save"))
  {
    Save();
  }
  ImGui::Separator();
  m_batch_renderer.OnImGuiUpdate();
}

ff8::Mim::Mim(const ff8::Fields &fields)
  : m_mim(LoadMim(fields.Field(), fields.Coo(), m_path, m_choose_coo))
  , m_delayed_textures(LoadTextures(m_mim))
{
  camera.RefreshAspectRatio();
}

std::size_t ff8::Mim::Index() const
{
  return static_cast<std::size_t>(bpp.Index()) * 16U + palette.Palette();
}

const Texture &ff8::Mim::CurrentTexture() const
{
  if (draw_palette)
  {
    return m_delayed_textures.textures->back();
  }
  return m_delayed_textures.textures->at(Index());
}

void ff8::Mim::SetUniforms() const
{
  m_batch_renderer.Bind();
  if (saving)
  {
    m_batch_renderer.Shader().SetUniform(
      "u_MVP",
      OrthographicCamera{
        { 0, 0 }, { CurrentTexture().width(), CurrentTexture().height() } }
        .ViewProjectionMatrix());
  }
  else
  {
    m_batch_renderer.Shader().SetUniform(
      "u_MVP", camera.Camera().ViewProjectionMatrix());
  }
  if (!draw_grid)
  {
    m_batch_renderer.Shader().SetUniform("u_Grid", 0.F, 0.F);
  }
  else
  {
    if (!draw_palette)
    {
      m_batch_renderer.Shader().SetUniform("u_Grid", 16.F, 16.F);
    }
    else
    {
      m_batch_renderer.Shader().SetUniform("u_Grid", 1.F, 1.F);
    }
  }
  m_batch_renderer.Shader().SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}
void ff8::Mim::OnEvent(const Event::Item &e) const
{
  camera.OnEvent(e);
  m_batch_renderer.OnEvent(e);
}
void ff8::Mim::Save() const
{
  saving                 = true;
  const Texture &texture = CurrentTexture();
  FrameBuffer    fb({ .width = texture.width(), .height = texture.height() });
  glViewport(0, 0, texture.width(), texture.height());
  fb.Bind();
  OnRender();
  fb.UnBind();
  glViewport(
    0,
    0,
    Application::CurrentWindow()->ViewWindowData().frame_buffer_width,
    Application::CurrentWindow()->ViewWindowData().frame_buffer_height);
  PixelBuffer  pixel_buffer{ fb.Specification() };
  auto         fs_path = std::filesystem::path(m_path);
  pixel_buffer.operator()(
    fb,
    fs_path.parent_path()
      / fmt::format(
        "{}_{}_{}.png",
        fs_path.stem().string(),
        bpp.String(),
        palette.String()));
  while (pixel_buffer.operator()(&Texture::save))
    ;
  saving = false;
}
