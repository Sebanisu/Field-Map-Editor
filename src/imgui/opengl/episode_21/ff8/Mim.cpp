//
// Created by pcvii on 11/30/2021.
//

#include "Mim.hpp"

void ff8::Mim::OnUpdate(float ts) const
{
  m_delayed_textures.check();
  const auto &texture = CurrentTexture();
  m_batch_renderer.Camera().SetMaxBounds(
    { 0.F,
      static_cast<float>(texture.width()),
      0.F,
      static_cast<float>(texture.height()) });
  m_batch_renderer.OnUpdate(ts);
}

void ff8::Mim::OnRender() const
{
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
    ImGui::Checkbox("Draw Palette", &m_draw_palette);
    ImGui::Checkbox("Draw Grid", &m_draw_grid);
    if (m_bpp.OnImGuiUpdate() || m_palette.OnImGuiUpdate())
    {
    }
  }
  ImGui::Text(
    "%s",
    fmt::format(
      "Texture Width: {:>5}, Height: {:>5}", texture.width(), texture.height())
      .c_str());
  ImGui::Separator();
  m_batch_renderer.OnImGuiUpdate();
}

ff8::Mim::Mim(const ff8::Fields &fields)
  : m_mim(LoadMim(fields.Field(), fields.Coo(), m_path, m_choose_coo))
  , m_delayed_textures(LoadTextures(m_mim))
{
}

std::size_t ff8::Mim::Index() const
{
  return static_cast<std::size_t>(m_bpp.Index()) * 16U + m_palette.Palette();
}

const Texture &ff8::Mim::CurrentTexture() const
{
  if (m_draw_palette)
  {
    return m_delayed_textures.textures->back();
  }
  return m_delayed_textures.textures->at(Index());
}

void ff8::Mim::SetUniforms() const
{
  m_batch_renderer.Bind();
  if(!m_draw_grid)
  {
    m_batch_renderer.Shader().SetUniform("u_Grid", 0.F, 0.F);
  }
  else
  {
    if (!m_draw_palette)
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
  m_batch_renderer.OnEvent(e);
}
