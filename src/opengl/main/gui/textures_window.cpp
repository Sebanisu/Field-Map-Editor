#include "textures_window.hpp"

void fme::textures_window::render() const
{
     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error(
            "m_selections is no longer valid. File: {}, Line: {}",
            __FILE__,
            __LINE__);
          return;
     }
     auto map_sprite = m_map_sprite.lock();
     if (!map_sprite)
     {
          spdlog::error(
            "m_map_sprite is no longer valid. File: {}, Line: {}",
            __FILE__,
            __LINE__);
          return;
     }
}

void fme::textures_window::update(
  const std::shared_ptr<map_sprite> &new_map_sprite) const
{
     m_map_sprite = new_map_sprite;
}

void fme::textures_window::update(
  const std::shared_ptr<Selections> &new_selections) const
{
     m_selections = new_selections;
}