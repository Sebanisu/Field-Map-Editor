#ifndef D6C77062_9D81_4B19_94F6_558AE8A3D90C
#define D6C77062_9D81_4B19_94F6_558AE8A3D90C
#include "create_tile_button.hpp"
#include "map_sprite.hpp"
#include <cstdint>
#include <limits>
#include <open_viii/graphics/background/Map.hpp>
#include <variant>
namespace fme
{

template<typename... B>
[[nodiscard]] inline auto make_visitor(B &&...b) noexcept
{
     struct visitor : public std::remove_cvref_t<B>...
     {
          using B::operator()...;
     };
     return visitor{ std::forward<B>(b)... };
}
[[nodiscard]] bool collapsing_tile_info(
  std::weak_ptr<const map_sprite>                           map,
  const open_viii::graphics::background::Map::variant_tile &original_tile,
  const open_viii::graphics::background::Map::variant_tile &working_tile,
  const tile_button_options                                &options = {},
  const std::size_t index = std::numeric_limits<size_t>::max());
bool browse_for_image_display_preview();
}// namespace fme
#endif /* D6C77062_9D81_4B19_94F6_558AE8A3D90C */
