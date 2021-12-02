//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_MIM_HPP
#define MYPROJECT_MIM_HPP
#include "BatchRenderer.hpp"
#include "BPPs.hpp"
#include "FF8LoadTextures.hpp"
#include "Fields.hpp"
#include "MVP.hpp"
#include "Palettes.hpp"
namespace ff8
{
DelayedTextures
  LoadTextures(const open_viii::graphics::background::Mim &mim);
class Mim
{
public:
  friend void
    OnUpdate(const Mim &, float);
  friend void
    OnRender(const Mim &);
  friend void
    OnImGuiRender(const Mim &);
  Mim() = default;
  Mim(const Fields &fields)
    : m_mim(LoadMim(fields.Field(), fields.Coo(), m_path, m_choose_coo))
    , m_delayed_textures(LoadTextures(m_mim))
  {
  }
  std::size_t
    Index() const
  {
    return static_cast<std::size_t>(m_bpp.Index()) * 16U + m_palette.Palette();
  }
  const Texture &
    CurrentTexture() const
  {
    if (m_draw_palette)
    {
      return m_delayed_textures.textures->back();
    }
    return m_delayed_textures.textures->at(Index());
  }

private:
  void
                                       SetUniforms() const;
  std::string                          m_path             = {};
  bool                                 m_choose_coo       = {};
  open_viii::graphics::background::Mim m_mim              = {};
  // 3 bpp x 16 palettes 48 possible textures + 1 for palette texture
  DelayedTextures                      m_delayed_textures = {};
  inline static const BPPs             m_bpp              = {};
  inline static const Palettes         m_palette          = {};
  BatchRenderer                        m_batch_renderer   = {};
  inline static float                  m_zoom             = { 1.F };
  inline static glm::vec3              m_view_offset      = {};
  inline static glm::vec2              m_view_percent     = { 0.F, 0.5F };
  inline static bool                   m_draw_palette     = false;
  inline static MVP                    m_mvp              = {};
};
static_assert(test::Test<Mim>);
inline void
  ff8::Mim::SetUniforms() const
{
  const auto &texture       = CurrentTexture();
  float       window_height = texture.height() / 16.F;
  using ::SetUniforms;
  SetUniforms(m_mvp, m_batch_renderer.Shader(), window_height);
  //  open_viii::graphics::Rectangle<int> m_viewport{};
  //  GLCall{ glGetIntegerv, GL_VIEWPORT, reinterpret_cast<int *>(&m_viewport)
  //  }; const auto &texture       = CurrentTexture(); float       window_height
  //  = texture.height() / 16.F; float window_width = window_height *
  //  (m_viewport.width() - m_viewport.x())
  //                       / (m_viewport.height() - m_viewport.y());
  //
  //  const auto proj = glm::ortho(
  //    m_view_offset.x / m_zoom,
  //    (m_view_offset.x + window_width) / m_zoom,
  //    m_view_offset.y / m_zoom,
  //    (m_view_offset.y + window_height) / m_zoom,
  //    -1.F,
  //    1.F);
  //
  //  const auto mvp = proj;
  //  m_batch_renderer.Shader().Bind();
  //  m_batch_renderer.Shader().SetUniform("u_MVP", mvp);
  //  m_batch_renderer.Shader().SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}
inline void
  OnUpdate(const Mim &self, float ts)
{
  self.m_delayed_textures.check();
  OnUpdate(self.m_batch_renderer, ts);
  OnUpdate(self.m_mvp, ts);
}
inline void
  OnRender(const Mim &self)
{
  self.SetUniforms();

  const auto &texture = self.CurrentTexture();
  if (texture.width() == 0 || texture.height() == 0)
  {
    return;
  }
  glm::vec2 size = { texture.width() / 16, texture.height() / 16 };
  self.m_batch_renderer.Clear();
  self.m_batch_renderer.DrawQuad(glm::vec2{ 0.F }, texture, size);
  self.m_batch_renderer.Draw();
  OnRender(self.m_batch_renderer);
  OnRender(self.m_mvp);
}
inline void
  OnImGuiRender(const Mim &self)
{
  int                                 id = 0;
  open_viii::graphics::Rectangle<int> m_viewport{};
  GLCall{ glGetIntegerv, GL_VIEWPORT, reinterpret_cast<int *>(&m_viewport) };
  {
    ImGui::Checkbox("Draw Palette", &self.m_draw_palette);
  }
  const auto &texture       = self.CurrentTexture();
  float       window_height = texture.height() / 16.F;
  float window_width = window_height * (m_viewport.width() - m_viewport.x())
                       / (m_viewport.height() - m_viewport.y());

  {
    const auto disable = scope_guard(&ImGui::EndDisabled);
    ImGui::BeginDisabled(texture.height() == 0 || texture.width() == 0);
    if (OnImGuiRender(self.m_bpp))
    {
      const glm::vec2 new_max = {
        static_cast<float>(texture.width() * self.m_zoom) / 16.F - window_width,
        static_cast<float>(texture.height() * self.m_zoom) / 16.F
          - window_height
      };
      self.m_view_offset.x = self.m_view_percent.x * new_max.x;
      self.m_view_offset.y = self.m_view_percent.y * new_max.y;
    }
  }
  {
    const auto disable = scope_guard(&ImGui::EndDisabled);
    ImGui::BeginDisabled(texture.height() == 0 || texture.width() == 0);
    if (OnImGuiRender(self.m_palette))
    {
      const glm::vec2 new_max = {
        static_cast<float>(texture.width() * self.m_zoom) / 16.F - window_width,
        static_cast<float>(texture.height() * self.m_zoom) / 16.F
          - window_height
      };
      self.m_view_offset.x = self.m_view_percent.x * new_max.x;
      self.m_view_offset.y = self.m_view_percent.y * new_max.y;
    }
  }
  ImGui::Text(
    "%s",
    fmt::format(
      "Texture Width: {:>5}, Height: {:>5}", texture.width(), texture.height())
      .c_str());

  ImGui::Separator();
  const glm::vec2 max = {
    static_cast<float>(texture.width() * self.m_zoom) / window_height
      - window_width,
    static_cast<float>(texture.height() * self.m_zoom) / window_height
      - window_height
  };
  {
    ImGui::PushID(++id);
    const auto pop = scope_guard(&ImGui::PopID);

    if (max.x >= 0.F)
    {
      self.m_view_offset.x = std::clamp(self.m_view_offset.x, 0.F, max.x);
    }
    const auto disable = scope_guard(&ImGui::EndDisabled);
    ImGui::BeginDisabled(max.x <= 0.F);
    if (ImGui::SliderFloat("", &self.m_view_percent.x, 0.F, 1.F))
    {
      self.m_view_offset.x = self.m_view_percent.x * max.x;
    }
  }
  ImGui::SameLine();
  {
    ImGui::PushID(++id);
    const auto pop = scope_guard(&ImGui::PopID);

    if (max.y >= 0.F)
    {
      self.m_view_offset.y = std::clamp(self.m_view_offset.y, 0.F, max.y);
    }
    const auto disable = scope_guard(&ImGui::EndDisabled);
    ImGui::BeginDisabled(max.y <= 0.F);
    if (ImGui::SliderFloat("", &self.m_view_percent.y, 0.F, 1.F))
    {
      self.m_view_offset.y = self.m_view_percent.y * max.y;
    }
  }
  {
    const auto pop = scope_guard{ &ImGui::PopID };
    ImGui::PushID(++id);
    const auto disable = scope_guard(&ImGui::EndDisabled);
    ImGui::BeginDisabled(texture.height() == 0 || texture.width() == 0);


    if (ImGui::SliderFloat("Zoom", &self.m_zoom, 32.F, 1.F))
    {// || (std::abs(self.m_view_offset.y) <
     // std::numeric_limits<float>::epsilon() && std::abs(self.m_zoom) >
     // std::numeric_limits<float>::epsilon())
      const glm::vec2 new_max = {
        static_cast<float>(texture.width() * self.m_zoom) / 16.F - window_width,
        static_cast<float>(texture.height() * self.m_zoom) / 16.F
          - window_height
      };

      self.m_view_offset.x = self.m_view_percent.x * new_max.x;
      self.m_view_offset.y = self.m_view_percent.y * new_max.y;
    }
  }
  OnImGuiRender(self.m_batch_renderer);
  OnImGuiRender(self.m_mvp);
}

}// namespace ff8
#endif// MYPROJECT_MIM_HPP
