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
template<open_viii::graphics::background::is_tile tileT>
[[nodiscard]] inline bool create_tile_button(const map_sprite &map, const tileT &tile, sf::Vector2f image_size = {})
{
     const auto *texture = map.get_texture(tile);
     if (texture == nullptr)
     {
          return false;
     }
     const auto tile_texture_size = map.get_tile_texture_size(texture);
     const auto src_x             = [&]() -> std::uint32_t {
          if (map.filter().deswizzle.enabled())
          {
               return static_cast<std::uint32_t>(tile.x());
          }
          auto       source_texture_page_width = tileT::texture_page_width(tile.depth());
          const auto texture_page_x_offset     = [&]() -> std::uint32_t {
               if (map.filter().upscale.enabled())
               {
                    return 0;
               }
               return tile.texture_id() * source_texture_page_width;
          }();
          return tile.source_x() + texture_page_x_offset;
     }();
     const auto src_y = [&]() -> std::uint32_t {
          if (map.filter().deswizzle.enabled())
          {
               return static_cast<std::uint32_t>(tile.y());
          }
          return tile.source_y();
     }();
     static constexpr float tile_size = 16.F;
     sf::Sprite             sprite(
       *texture,
       sf::IntRect(
         static_cast<int>((static_cast<float>(src_x) / tile_size) * static_cast<float>(tile_texture_size.x)),
         static_cast<int>((static_cast<float>(src_y) / tile_size) * static_cast<float>(tile_texture_size.y)),
         static_cast<int>(tile_texture_size.x),
         static_cast<int>(tile_texture_size.y)));
     if (image_size == sf::Vector2f{})
     {
          image_size = sf::Vector2f(ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight());
     }


     const auto str_id = fmt::format("id269{}", get_imgui_id());
     return ImGui::ImageButton(str_id.c_str(), sprite, image_size);
}

#endif /* DF4C3AF7_C3A6_4F62_8336_B0EF72752E11 */
