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
  Map(const Fields &fields)
    : m_mim(LoadMim(fields.Field(), fields.Coo(), m_mim_path, m_mim_choose_coo))
    , m_map(LoadMap(
        fields.Field(),
        fields.Coo(),
        m_mim,
        m_map_path,
        m_map_choose_coo))
    , m_delayed_textures(LoadTextures(m_mim))
  {
  }

  friend void
    OnUpdate(const Map &, float);
  friend void
    OnRender(const Map &);
  friend void
    OnImGuiRender(const Map &);

private:
  std::string                          m_mim_path         = {};
  std::string                          m_map_path         = {};
  bool                                 m_mim_choose_coo   = {};
  bool                                 m_map_choose_coo   = {};
  open_viii::graphics::background::Mim m_mim              = {};
  open_viii::graphics::background::Map m_map              = {};
  DelayedTextures                      m_delayed_textures = {};
};
static_assert(test::Test<Map>);
inline void
  OnUpdate(const Map &self, float)
{
  self.m_delayed_textures.check();
}
inline void
  OnRender(const Map &)
{
}
inline void
  OnImGuiRender(const Map &)
{
}
}// namespace ff8
#endif// MYPROJECT_MAP_HPP
