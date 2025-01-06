#ifndef D6C77062_9D81_4B19_94F6_558AE8A3D90C
#define D6C77062_9D81_4B19_94F6_558AE8A3D90C
#include <cstdint>
#include <open_viii/graphics/background/Map.hpp>
#include <variant>
#include "map_sprite.hpp"
#include <limits>
void collapsing_tile_info(const map_sprite & map,
  const std::variant<
    open_viii::graphics::background::Tile1,
    open_viii::graphics::background::Tile2,
    open_viii::graphics::background::Tile3,
    std::monostate> &current_tile,
  const std::size_t  index = std::numeric_limits<size_t>::max());
bool browse_for_image_display_preview();

#endif /* D6C77062_9D81_4B19_94F6_558AE8A3D90C */
