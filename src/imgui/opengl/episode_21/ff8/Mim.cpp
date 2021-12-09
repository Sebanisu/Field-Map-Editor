//
// Created by pcvii on 11/30/2021.
//

#include "Mim.hpp"

void ff8::Mim::OnUpdate(float ts) const
{
  m_delayed_textures.check();
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
  glm::vec2 size = { texture.width() / 16, texture.height() / 16 };
  m_batch_renderer.Clear();
  m_batch_renderer.DrawQuad(glm::vec2{ 0.F }, texture, size);
  m_batch_renderer.Draw();
  m_batch_renderer.OnRender();
}
void ff8::Mim::OnImGuiUpdate() const
{
  int                                 id = 0;
  open_viii::graphics::Rectangle<int> m_viewport{};
  GLCall{ glGetIntegerv, GL_VIEWPORT, reinterpret_cast<int *>(&m_viewport) };
  {
    ImGui::Checkbox("Draw Palette", &m_draw_palette);
  }
  const auto &texture       = CurrentTexture();
  float       window_height = static_cast<float>(texture.height()) / 16.F;
  float       window_width =
    window_height * (static_cast<float>(m_viewport.width() - m_viewport.x()))
    / (static_cast<float>(m_viewport.height() - m_viewport.y()));

  {
    const auto disable = scope_guard(&ImGui::EndDisabled);
    ImGui::BeginDisabled(texture.height() == 0 || texture.width() == 0);
    if (m_bpp.OnImGuiUpdate())
    {
      const glm::vec2 new_max = {
        static_cast<float>(texture.width()) * m_zoom / 16.F - window_width,
        static_cast<float>(texture.height()) * m_zoom / 16.F - window_height
      };
      m_view_offset.x = m_view_percent.x * new_max.x;
      m_view_offset.y = m_view_percent.y * new_max.y;
    }
  }
  {
    const auto disable = scope_guard(&ImGui::EndDisabled);
    ImGui::BeginDisabled(texture.height() == 0 || texture.width() == 0);
    if (m_palette.OnImGuiUpdate())
    {
      const glm::vec2 new_max = {
        static_cast<float>(texture.width()) * m_zoom / 16.F - window_width,
        static_cast<float>(texture.height()) * m_zoom / 16.F - window_height
      };
      m_view_offset.x = m_view_percent.x * new_max.x;
      m_view_offset.y = m_view_percent.y * new_max.y;
    }
  }
  ImGui::Text(
    "%s",
    fmt::format(
      "Texture Width: {:>5}, Height: {:>5}", texture.width(), texture.height())
      .c_str());

  ImGui::Separator();
  const glm::vec2 max = {
    static_cast<float>(texture.width()) * m_zoom / window_height - window_width,
    static_cast<float>(texture.height()) * m_zoom / window_height
      - window_height
  };
  {
    ImGui::PushID(++id);
    const auto pop = scope_guard(&ImGui::PopID);

    if (max.x >= 0.F)
    {
      m_view_offset.x = std::clamp(m_view_offset.x, 0.F, max.x);
    }
    const auto disable = scope_guard(&ImGui::EndDisabled);
    ImGui::BeginDisabled(max.x <= 0.F);
    if (ImGui::SliderFloat("", &m_view_percent.x, 0.F, 1.F))
    {
      m_view_offset.x = m_view_percent.x * max.x;
    }
  }
  ImGui::SameLine();
  {
    ImGui::PushID(++id);
    const auto pop = scope_guard(&ImGui::PopID);

    if (max.y >= 0.F)
    {
      m_view_offset.y = std::clamp(m_view_offset.y, 0.F, max.y);
    }
    const auto disable = scope_guard(&ImGui::EndDisabled);
    ImGui::BeginDisabled(max.y <= 0.F);
    if (ImGui::SliderFloat("", &m_view_percent.y, 0.F, 1.F))
    {
      m_view_offset.y = m_view_percent.y * max.y;
    }
  }
  {
    const auto pop = scope_guard{ &ImGui::PopID };
    ImGui::PushID(++id);
    const auto disable = scope_guard(&ImGui::EndDisabled);
    ImGui::BeginDisabled(texture.height() == 0 || texture.width() == 0);


    if (ImGui::SliderFloat("Zoom", &m_zoom, 32.F, 1.F))
    {// || (std::abs(m_view_offset.y) <
     // std::numeric_limits<float>::epsilon() && std::abs(m_zoom) >
     // std::numeric_limits<float>::epsilon())
      const glm::vec2 new_max = {
        static_cast<float>(texture.width()) * m_zoom / 16.F - window_width,
        static_cast<float>(texture.height()) * m_zoom / 16.F - window_height
      };

      m_view_offset.x = m_view_percent.x * new_max.x;
      m_view_offset.y = m_view_percent.y * new_max.y;
    }
  }
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
  const auto &texture       = CurrentTexture();
  float       window_height = static_cast<float>(texture.height()) / 16.F;
  open_viii::graphics::Rectangle<int> m_viewport{};
  GLCall{ glGetIntegerv, GL_VIEWPORT, reinterpret_cast<int *>(&m_viewport) };
  float window_width =
    window_height * (static_cast<float>(m_viewport.width() - m_viewport.x()))
    / (static_cast<float>(m_viewport.height() - m_viewport.y()));

  const auto proj = glm::ortho(
    m_view_offset.x / m_zoom,
    (m_view_offset.x + window_width) / m_zoom,
    m_view_offset.y / m_zoom,
    (m_view_offset.y + window_height) / m_zoom,
    -1.F,
    1.F);

  const auto mvp = proj;
  m_batch_renderer.Shader().Bind();
  m_batch_renderer.Shader().SetUniform("u_MVP", mvp);
  m_batch_renderer.Shader().SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}
