//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_MIM_HPP
#define FIELD_MAP_EDITOR_MIM_HPP
#include "BatchRenderer.hpp"
#include "BPPs.hpp"
#include "FF8LoadTextures.hpp"
#include "Fields.hpp"
#include "ImGuiViewPortWindow.hpp"
#include "Palettes.hpp"
#include <Counter.hpp>
namespace ff8
{
class Mim
{
public:
  Mim() = default;
  Mim(const Fields &fields);
  void                     OnUpdate(float) const;
  void                     OnRender() const;
  void                     OnImGuiUpdate() const;
  void                     OnEvent(const glengine::Event::Item &) const;
  std::size_t              Index() const;
  const glengine::Texture &CurrentTexture() const;
  void                     Save() const;


private:
  void                                 RenderFrameBuffer() const;
  void                                 SetUniforms() const;
  void                                 Save_All() const;
  std::string                          m_path                  = {};
  bool                                 m_choose_coo            = {};
  open_viii::graphics::background::Mim m_mim                   = {};
  // 3 bpp x 16 palettes 48 possible textures + 1 for palette texture
  glengine::DelayedTextures<35U>       m_delayed_textures      = {};
  glengine::BatchRenderer              m_batch_renderer        = {};
  glengine::ImGuiViewPortWindow        m_imgui_viewport_window = { "Mim" };
  glengine::Counter                    m_id                    = {};
};
static_assert(glengine::Renderable<Mim>);
}// namespace ff8
#endif// FIELD_MAP_EDITOR_MIM_HPP
