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
static const Bpps               Bpp         = {};
static const Palettes           Palette     = {};
static bool                     DrawPalette = false;
static bool                     DrawGrid    = false;
static bool                     FitHeight   = true;
static bool                     FitWidth    = true;
static bool                     Saving      = false;
static bool                     Preview     = false;
static const glengine::Texture *Texture     = nullptr;
}// namespace ff_8
void ff_8::Mim::on_update(float ts) const
{
  (void)GetMim().on_update();
  const auto &local_texture = current_texture();
  m_imgui_viewport_window.set_image_bounds(
    glm::vec2{ local_texture.width(), local_texture.height() });
  m_imgui_viewport_window.on_update(ts);
  m_imgui_viewport_window.fit(FitWidth, FitHeight);
  m_batch_renderer.on_update(ts);
}

void ff_8::Mim::on_render() const
{
  const auto null_texture = glengine::ScopeGuard([]() { Texture = nullptr; });
  if (Texture == nullptr)
    Texture = &current_texture();

  if (Texture->width() == 0 || Texture->height() == 0)
  {
    return;
  }

  glengine::Window::default_blend();
  m_imgui_viewport_window.on_render();
  set_uniforms();
  if (!Saving)
  {
    m_imgui_viewport_window.on_render([this]() { render_frame_buffer(); });
    GetViewPortPreview().on_render(m_imgui_viewport_window, [this]() {
      Preview                = true;
      const auto pop_preview = glengine::ScopeGuard([]() { Preview = false; });
      set_uniforms();
      render_frame_buffer();
    });
  }
  else
  {
    render_frame_buffer();
  }
  ff_8::ImGuiTileDisplayWindow::take_control(
    m_imgui_viewport_window.has_hover(), m_id);
}
void ff_8::Mim::on_im_gui_update() const
{
  const auto  pop_id        = glengine::ImGuiPushId();
  const auto &local_texture = current_texture();
  {
    const auto disable = glengine::ImGuiDisabled(
      local_texture.height() == 0 || local_texture.width() == 0);


    ImGui::Checkbox("draw Palette", &DrawPalette);
    ImGui::Checkbox("draw Grid", &DrawGrid);
    ImGui::Checkbox("fit Height", &FitHeight);
    ImGui::Checkbox("fit Width", &FitWidth);

    if (Bpp.on_im_gui_update() || Palette.on_im_gui_update())
    {
    }
    if (ImGui::Button("Save"))
    {
      save();
    }
    if (ImGui::Button("Save All"))
    {
      save_all();
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
  ff_8::ImGuiTileDisplayWindow::on_render_forward(m_id, [this]() {
    ImGui::Text(
      "%s", fmt::format("Mim {}", static_cast<uint32_t>(m_id)).c_str());
  });
}

ff_8::Mim::Mim(const ff_8::Fields &)
:Mim()
{
}

std::size_t ff_8::Mim::index() const
{
  if (Bpp->bpp24())
  {
    return static_cast<std::size_t>(2) * 16U + 1;
  }
  if (Bpp->bpp16())
  {
    return static_cast<std::size_t>(2) * 16U;
  }
  return static_cast<std::size_t>(Bpp.index()) * 16U + Palette;
}

const glengine::Texture &ff_8::Mim::current_texture() const
{
  if (DrawPalette)
  {
    return GetMim().delayed_textures.textures->back();
  }
  return GetMim().delayed_textures.textures->at(index());
}

void ff_8::Mim::set_uniforms() const
{
  m_batch_renderer.bind();

  if (Saving)
  {
    m_batch_renderer.shader().set_uniform(
      "u_MVP",
      glengine::OrthographicCamera{ { Texture->width(), Texture->height() } }
        .view_projection_matrix());
  }
  else if (Preview)
  {

    m_batch_renderer.shader().set_uniform(
      "u_MVP", m_imgui_viewport_window.preview_view_projection_matrix());
  }
  else
  {
    m_batch_renderer.shader().set_uniform(
      "u_MVP", m_imgui_viewport_window.view_projection_matrix());
  }
  if (!DrawGrid || Saving)
  {
    m_batch_renderer.shader().set_uniform("u_Grid", 0.F, 0.F);
  }
  else
  {
    if (!DrawPalette)
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
  Saving                                 = true;
  const glengine::Texture &local_texture = current_texture();
  glengine::FrameBuffer    fb(
    { .width = local_texture.width(), .height = local_texture.height() });
  {
    const auto fbb = glengine::FrameBufferBackup{};
    fb.bind();
    glengine::Renderer::Clear();
    fb.clear_red_integer_color_attachment();
    glViewport(0, 0, local_texture.width(), local_texture.height());
    on_render();
  }
  auto fs_path = std::filesystem::path(GetMim().path);
  auto string  = fmt::format(
    "{}_mim_{}_{}.png",
    fs_path.stem().string(),
    Bpp.string(),
    Palette.string());
  if (Bpp->bpp16() || Bpp->bpp24())
  {
    string =
      fmt::format("{}_mim_{}.png", fs_path.stem().string(), Bpp.string());
  }
  if (DrawPalette)
  {
    string = fmt::format("{}_mim_clut.png", fs_path.stem().string());
  }
  glengine::PixelBuffer pixel_buffer{ fb.specification() };
  pixel_buffer.         operator()(fb, fs_path.parent_path() / string);
  while (pixel_buffer.operator()(&glengine::Texture::save))
    ;
  Saving = false;
}
void ff_8::Mim::save_all() const
{
  Saving = true;
  for (int index = 0;
       index < static_cast<int>(GetMim().delayed_textures.textures->size());
       ++index)
  {
    Texture = &GetMim().delayed_textures.textures->at(static_cast<std::size_t>(index));
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
    if (Texture->width() == 0 || Texture->height() == 0)
    {
      continue;
    }
    glengine::FrameBuffer fb(
      { .width = Texture->width(), .height = Texture->height() });
    {
      const auto fbb = glengine::FrameBufferBackup{};
      fb.bind();
      glengine::Renderer::Clear();
      fb.clear_red_integer_color_attachment();
      GlCall{}(glViewport, 0, 0, Texture->width(), Texture->height());
      on_render();
    }
    glengine::PixelBuffer pixel_buffer{ fb.specification() };
    auto                  fs_path = std::filesystem::path(GetMim().path);
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
  Saving = false;
}
void ff_8::Mim::render_frame_buffer() const
{
  m_batch_renderer.clear();
  m_batch_renderer.draw_quad(
    *Texture,
    glm::vec3{ -static_cast<float>(Texture->width()) / 2.F,
               -static_cast<float>(Texture->height()) / 2.F,
               0.F },
    glm::vec2{ Texture->width(), Texture->height() });
  m_batch_renderer.draw();
  m_batch_renderer.on_render();
}
