//
// Created by pcvii on 11/30/2021.
//

#include "Map.hpp"

ff8::Map::Map(const ff8::Fields &fields)
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

void ff8::Map::OnUpdate(float) const
{
  m_delayed_textures.check();
}
