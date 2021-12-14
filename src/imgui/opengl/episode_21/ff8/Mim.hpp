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
DelayedTextures LoadTextures(const open_viii::graphics::background::Mim &mim);
class Mim
{
public:
  void OnUpdate(float) const;
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const Event::Item &) const;
  Mim() = default;
  Mim(const Fields &fields);
  std::size_t    Index() const;
  const Texture &CurrentTexture() const;

private:
  void                                 SetUniforms() const;
  std::string                          m_path                = {};
  bool                                 m_choose_coo          = {};
  open_viii::graphics::background::Mim m_mim                 = {};
  // 3 bpp x 16 palettes 48 possible textures + 1 for palette texture
  DelayedTextures                      m_delayed_textures    = {};
  inline static const BPPs             m_bpp                 = {};
  inline static const Palettes         m_palette             = {};
  BatchRenderer                        m_batch_renderer      = {};
  inline static bool                   m_draw_palette        = false;
  inline static bool                   m_draw_grid           = false;
  inline static bool                   m_snap_zoom_to_height = true;
};
static_assert(Renderable<Mim>);
}// namespace ff8
#endif// MYPROJECT_MIM_HPP
