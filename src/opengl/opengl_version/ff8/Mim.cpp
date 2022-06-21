//
// Created by pcvii on 11/30/2021.
//

#include "Mim.hpp"
#include "Application.hpp"
#include "Event/EventDispatcher.hpp"
#include "FrameBuffer.hpp"
#include "FrameBufferBackup.hpp"
#include "ImGuiDisabled.hpp"
#include "ImGuiTileDisplayWindow.hpp"
#include "OrthographicCameraController.hpp"
#include "PixelBuffer.hpp"
namespace ff_8
{
static const BPPs               bpp          = {};
static const Palettes           palette      = {};
static bool                     draw_palette = false;
static bool                     draw_grid    = false;
static bool                     fit_height   = true;
static bool                     fit_width    = true;
static bool                     saving       = false;
static bool                     preview      = false;
static const glengine::Texture *texture      = nullptr;
}// namespace ff_8
void ff_8::Mim::on_update(float ts) const
{
  (void)m_delayed_textures.on_update();
  const auto &local_texture = CurrentTexture();
  m_imgui_viewport_window.set_image_bounds(
    glm::vec2{ local_texture.width(), local_texture.height() });
  m_imgui_viewport_window.on_update(ts);
  m_imgui_viewport_window.fit(fit_width, fit_height);
  m_batch_renderer.on_update(ts);
}

void ff_8::Mim::on_render() const
{
  if (texture == nullptr)
    texture = &CurrentTexture();

  if (texture->width() == 0 || texture->height() == 0)
  {
    return;
  }

  glengine::Window::default_blend();
  m_imgui_viewport_window.on_render();
  set_uniforms();
  if (!saving)
  {
    m_imgui_viewport_window.on_render([this]() { render_frame_buffer(); });
    GetViewPortPreview().on_render(
      m_imgui_viewport_window.has_hover(), [this]() {
        preview = true;
        set_uniforms();
        render_frame_buffer();
        preview = false;
      });
  }
  else
  {
    render_frame_buffer();
  }
  ff_8::ImGuiTileDisplayWindow::TakeControl(
    m_imgui_viewport_window.has_hover(), m_id);
  texture = nullptr;
}
void ff_8::Mim::on_im_gui_update() const
{
  const auto  popid         = glengine::ImGuiPushId();
  const auto &local_texture = CurrentTexture();
  {
    const auto disable = glengine::ImGuiDisabled(
      local_texture.height() == 0 || local_texture.width() == 0);


    ImGui::Checkbox("Draw Palette", &draw_palette);
    ImGui::Checkbox("Draw Grid", &draw_grid);
    ImGui::Checkbox("fit Height", &fit_height);
    ImGui::Checkbox("fit Width", &fit_width);

    if (bpp.on_im_gui_update() || palette.on_im_gui_update())
    {
    }
    if (ImGui::Button("Save"))
    {
      save();
    }
    if (ImGui::Button("Save All"))
    {
      Save_All();
    }
  }
  ImGui::Separator();


  ImGui::Text(
    "%s",
    fmt::format(
      "Texture Width: {:>5}, Height: {:>5}\n",
      local_texture.width(),
      local_texture.height())
      .c_str());
  ImGui::Separator();
  m_imgui_viewport_window.on_im_gui_update();
  ImGui::Separator();
  m_batch_renderer.on_im_gui_update();
  ff_8::ImGuiTileDisplayWindow::OnImGuiUpdateForward(m_id, [this]() {
    ImGui::Text(
      "%s", fmt::format("Mim {}", static_cast<uint32_t>(m_id)).c_str());
  });
}

ff_8::Mim::Mim(const ff_8::Fields &fields)
  : m_mim(LoadMim(fields.Field(), fields.Coo(), m_path, m_choose_coo))
{
  texture = nullptr;
  if (!std::empty(m_path))
  {
    spdlog::debug("Loaded {}", m_path);
    spdlog::debug("Loading Textures from Mim");
    m_delayed_textures = LoadTextures(m_mim);
  }
}

std::size_t ff_8::Mim::Index() const
{
  if (bpp.BPP().bpp24())
  {
    return static_cast<std::size_t>(2) * 16U + 1;
  }
  if (bpp.BPP().bpp16())
  {
    return static_cast<std::size_t>(2) * 16U;
  }
  return static_cast<std::size_t>(bpp.Index()) * 16U + palette.Palette();
}

const glengine::Texture &ff_8::Mim::CurrentTexture() const
{
  if (draw_palette)
  {
    return m_delayed_textures.textures->back();
  }
  return m_delayed_textures.textures->at(Index());
}

void ff_8::Mim::set_uniforms() const
{
  m_batch_renderer.bind();

  if (saving)
  {
    m_batch_renderer.shader().set_uniform(
      "u_MVP",
      glengine::OrthographicCamera{ { texture->width(), texture->height() } }
        .view_projection_matrix());
  }
  else if (preview)
  {

    m_batch_renderer.shader().set_uniform(
      "u_MVP", m_imgui_viewport_window.preview_view_projection_matrix());
  }
  else
  {
    m_batch_renderer.shader().set_uniform(
      "u_MVP", m_imgui_viewport_window.view_projection_matrix());
  }
  if (!draw_grid || saving)
  {
    m_batch_renderer.shader().set_uniform("u_Grid", 0.F, 0.F);
  }
  else
  {
    if (!draw_palette)
    {
      m_batch_renderer.shader().set_uniform("u_Grid", 16.F, 16.F);
    }
    else
    {
      m_batch_renderer.shader().set_uniform("u_Grid", 1.F, 1.F);
    }
  }
  m_batch_renderer.shader().set_uniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}
void ff_8::Mim::on_event(const glengine::event::Item &event) const
{
  m_imgui_viewport_window.on_event(event);
  m_batch_renderer.on_event(event);
}
void ff_8::Mim::save() const
{
  saving                                 = true;
  const glengine::Texture &local_texture = CurrentTexture();
  glengine::FrameBuffer    fb(
    { .width = local_texture.width(), .height = local_texture.height() });
  {
    const auto fbb = glengine::FrameBufferBackup{};
    fb.bind();
    glengine::Renderer::Clear();
    glViewport(0, 0, local_texture.width(), local_texture.height());
    on_render();
  }
  auto fs_path = std::filesystem::path(m_path);
  auto string  = fmt::format(
    "{}_mim_{}_{}.png",
    fs_path.stem().string(),
    bpp.String(),
    palette.String());
  if (bpp.BPP().bpp16() || bpp.BPP().bpp24())
  {
    string =
      fmt::format("{}_mim_{}.png", fs_path.stem().string(), bpp.String());
  }
  if (draw_palette)
  {
    string = fmt::format("{}_mim_clut.png", fs_path.stem().string());
  }
  glengine::PixelBuffer pixel_buffer{ fb.specification() };
  pixel_buffer.         operator()(fb, fs_path.parent_path() / string);
  while (pixel_buffer.operator()(&glengine::Texture::save))
    ;
  saving = false;
}
void ff_8::Mim::Save_All() const
{
  saving = true;
  for (int index = 0;
       index < static_cast<int>(m_delayed_textures.textures->size());
       ++index)
  {
    texture = &m_delayed_textures.textures->at(static_cast<std::size_t>(index));
    int local_bpp     = index / 16;
    int local_palette = index % 16;
    if (local_bpp == 2)
    {
      if (local_palette == 1)
      {
        local_bpp     = 24;
        local_palette = 0;
      }
      else if (local_palette == 2)
      {
        local_bpp     = -1;
        local_palette = 0;
      }
      else if (local_palette == 0)
      {
        local_bpp = 16;
      }
    }
    else if (local_bpp == 1)
    {
      local_bpp = 8;
    }
    else if (local_bpp == 0)
    {
      local_bpp = 4;
    }
    if (texture->width() == 0 || texture->height() == 0)
    {
      continue;
    }
    glengine::FrameBuffer fb(
      { .width = texture->width(), .height = texture->height() });
    {
      const auto fbb = glengine::FrameBufferBackup{};
      fb.bind();
      glengine::Renderer::Clear();
      GlCall{}(glViewport, 0, 0, texture->width(), texture->height());
      on_render();
    }
    glengine::PixelBuffer pixel_buffer{ fb.specification() };
    auto                  fs_path = std::filesystem::path(m_path);
    auto                  string  = fmt::format(
      "{}_mim_{}_{}.png", fs_path.stem().string(), local_bpp, local_palette);
    if (local_bpp == 16 || local_bpp == 24)
      string = fmt::format("{}_mim_{}.png", fs_path.stem().string(), local_bpp);
    else if (local_bpp == -1)
      string = fmt::format("{}_mim_clut.png", fs_path.stem().string());
    pixel_buffer(fb, fs_path.parent_path() / string);
    while (pixel_buffer(&glengine::Texture::save))
      ;
  }
  // RestoreViewPortToFrameBuffer();
  saving = false;
}
void ff_8::Mim::render_frame_buffer() const
{
  m_batch_renderer.clear();
  m_batch_renderer.draw_quad(
    *texture,
    glm::vec3{ -static_cast<float>(texture->width()) / 2.F,
               -static_cast<float>(texture->height()) / 2.F,
               0.F },
    glm::vec2{ texture->width(), texture->height() });
  m_batch_renderer.draw();
  m_batch_renderer.on_render();
}
