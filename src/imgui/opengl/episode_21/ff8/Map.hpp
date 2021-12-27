//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_MAP_HPP
#define MYPROJECT_MAP_HPP
#include "Fields.hpp"
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
  std::string                          m_mim_path         = {};
  std::string                          m_map_path         = {};
  bool                                 m_mim_choose_coo   = {};
  bool                                 m_map_choose_coo   = {};
  open_viii::graphics::background::Mim m_mim              = {};
  open_viii::graphics::background::Map m_map              = {};
  DelayedTextures                      m_delayed_textures = {};
  void                                 SetUniforms() const;
  BatchRenderer                        m_batch_renderer{};
  void                                 RenderTiles() const;
};
static_assert(Renderable<Map>);
}// namespace ff8
#endif// MYPROJECT_MAP_HPP
