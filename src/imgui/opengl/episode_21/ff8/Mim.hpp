//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_MIM_HPP
#define MYPROJECT_MIM_HPP
#include "BatchRenderer.hpp"
#include "BPPs.hpp"
#include "FF8LoadTextures.hpp"
#include "Fields.hpp"
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
  BPPs                                 m_bpp              = {};
  Palettes                             m_palette          = {};
  BatchRenderer                        m_batch_renderer   = {};
  mutable float                        m_zoom             = { 1.F };
  mutable glm::vec3                    m_view_offset      = {};
};
static_assert(test::Test<Mim>);
inline void
  ff8::Mim::SetUniforms() const
{
  const float window_width  = 16.F;
  const float window_height = 9.F;
  const auto  proj          = glm::ortho(
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
inline void
  OnUpdate(const Mim &self, float ts)
{
  self.m_delayed_textures.check();
  OnUpdate(self.m_batch_renderer, ts);
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
}
inline void
  OnImGuiRender(const Mim &self)
{
  if (OnImGuiRender(self.m_bpp) || OnImGuiRender(self.m_palette))
  {
  }
  const auto &texture = self.CurrentTexture();
  ImGui::Text(
    "%s",
    fmt::format(
      "Texture Width: {:>5}, Height: {:>5}", texture.width(), texture.height())
      .c_str());

  ImGui::Separator();
  int window_width = 16;
  int id           = 0;

  {
    const auto pop = scope_guard(&ImGui::PopID);
    ImGui::PushID(++id);
    if (ImGui::SliderFloat2(
          "View Offset",
          &self.m_view_offset.x,
          0.F,
          static_cast<float>(window_width)))
    {
    }
  }
  {
    const auto pop = scope_guard{ &ImGui::PopID };
    ImGui::PushID(++id);
    if (ImGui::SliderFloat("Zoom", &self.m_zoom, 16.F, 0.0625F))
    {
    }
  }
  OnImGuiRender(self.m_batch_renderer);
}

}// namespace ff8
#endif// MYPROJECT_MIM_HPP
