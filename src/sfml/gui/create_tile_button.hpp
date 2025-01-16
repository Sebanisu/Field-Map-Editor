#ifndef DF4C3AF7_C3A6_4F62_8336_B0EF72752E11
#define DF4C3AF7_C3A6_4F62_8336_B0EF72752E11

#include "map_sprite.hpp"
#include "push_pop_id.hpp"
#include <fmt/format.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <open_viii/graphics/background/Map.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Vertex.hpp>
namespace fme
{
template<open_viii::graphics::background::is_tile tileT>
[[nodiscard]] bool create_tile_button(std::weak_ptr<const map_sprite> map, const tileT &tile, sf::Vector2f image_size = {});
}// namespace fme
#endif /* DF4C3AF7_C3A6_4F62_8336_B0EF72752E11 */
