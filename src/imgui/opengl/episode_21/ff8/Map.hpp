//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_MAP_HPP
#define MYPROJECT_MAP_HPP
#include "Fields.hpp"
#include "FrameBuffer.hpp"
#include "Mim.hpp"
#include "Texture.hpp"

namespace ff8
{
class Map
{
public:
  Map() = default;
  Map(const Fields &fields);
  void OnUpdate(float) const;
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const Event::Item &) const;

private:
  // set uniforms
  void                                 SetUniforms() const;
  // draws tiles
  void                                 RenderTiles() const;
  // internal mim file path
  std::string                          m_mim_path         = {};
  // internal map file path
  std::string                          m_map_path         = {};
  // if coo was chosen instead of default.
  bool                                 m_mim_choose_coo   = {};
  // if coo was chosen instead of default.
  bool                                 m_map_choose_coo   = {};
  // container for field textures
  open_viii::graphics::background::Mim m_mim              = {};
  // container for field tile information
  open_viii::graphics::background::Map m_map              = {};
  // loads the textures overtime instead of forcing them to load at start.
  glengine::DelayedTextures<35U>       m_delayed_textures = {};
  // takes quads and draws them to the frame buffer or screen.
  glengine::BatchRenderer              m_batch_renderer   = {};
  // holds rendered image at 1:1 scale to prevent gaps when scaling.
  glengine::FrameBuffer                m_frame_buffer     = {};
  void                                 RenderFrameBuffer() const;
  void                                 Save() const;
};
static_assert(Renderable<Map>);
}// namespace ff8
#endif// MYPROJECT_MAP_HPP
