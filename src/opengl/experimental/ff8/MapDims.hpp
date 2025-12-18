//
// Created by pcvii on 6/15/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPDIMS_HPP
#define FIELD_MAP_EDITOR_MAPDIMS_HPP
#include <ff_8/TileOperations.hpp>
#include <glm/glm.hpp>
namespace ff_8
{
namespace map_dims_statics
{
     static constexpr auto TexturePageWidth = std::int16_t{ 256 };
     static constexpr auto TileSize         = float{ 16.F };
};// namespace map_dims_statics
template<typename TileFunctions>
class MapDims
{
     using MapT = open_viii::graphics::background::Map;

   public:
     glm::vec2 scaled_size() const noexcept
     {
          return size * tile_scale;
     }
     glm::vec3 scaled_position() const noexcept
     {
          return position * tile_scale;
     }
     glm::vec2 scaled_offset() const noexcept
     {
          return offset * tile_scale;
     }
     glm::vec2 scaled_tile_size() const noexcept
     {
          return glm::vec2{ map_dims_statics::TileSize * tile_scale };
     }
     glm::vec2 offset   = { 0.F, -map_dims_statics::TileSize };
     glm::vec3 position = {};
     glm::vec2 true_min = {};
     glm::vec2 true_max = {};
     glm::vec2 size     = {};
     glm::vec2 min      = {};
     glm::vec2 max      = {};
     mutable std::optional<glm::ivec3> pressed_mouse_location  = std::nullopt;
     mutable std::optional<glm::ivec3> dragging_mouse_location = std::nullopt;
     mutable std::optional<glm::ivec3> released_mouse_location = std::nullopt;
     mutable float                     tile_scale              = { 1.F };
     MapDims(const MapT &map)
     {
          map.visit_tiles(
            [&](const auto &tiles)
            {
                 {
                      auto f_tiles = tiles
                                     | std::views::filter(
                                       TileOperations::NotInvalidTile{});
                      get_x(f_tiles);
                      get_y(f_tiles);
                      get_true_x(f_tiles);
                      get_true_y(f_tiles);
                 }
            });
          size = glm::vec2{ max.x - min.x + map_dims_statics::TileSize,
                            max.y - min.y + map_dims_statics::TileSize };
          offset
            = glm::vec2{ size.x / 2.F + min.x,
                         size.y / 2.F + min.y - map_dims_statics::TileSize };
          position = glm::vec3{ -size.x / 2.F, -size.y / 2.F, 0.F };
     }

   private:
     template<std::ranges::range TilesR>
     void get_x(TilesR &&f_tiles)
     {
          {
               static constexpr typename TileFunctions::X x = {};
               const auto [i_min_x, i_max_x]
                 = std::ranges::minmax_element(f_tiles, {}, x);
               if (i_min_x == i_max_x)
               {
                    return;
               }
               static constexpr typename TileFunctions::TexturePage texture_page
                 = {};
               const auto i_max_texture_page
                 = std::ranges::max_element(f_tiles, {}, texture_page);
               if (i_max_texture_page == std::ranges::end(f_tiles))
               {
                    return;
               }
               min.x = x(*i_min_x);
               max.x = static_cast<float>(
                 [&, i_max_x = i_max_x]()
                 {
                      if constexpr (typename TileFunctions::UseTexturePage{})
                      {
                           return x(*i_max_x);
                      }
                      else
                      {
                           return (texture_page(*i_max_texture_page) + 1)
                                  * map_dims_statics::TexturePageWidth;
                      }
                 }());
          }
     }
     template<std::ranges::range TilesR>
     void get_y(TilesR &&f_tiles)
     {
          static constexpr typename TileFunctions::Y y = {};
          const auto [i_min_y, i_max_y]
            = std::ranges::minmax_element(f_tiles, {}, y);
          if (i_min_y == i_max_y)
          {
               return;
          }
          min.y = y(*i_min_y);
          max.y = y(*i_max_y);
     }
     template<std::ranges::range TilesR>
     void get_true_x(TilesR &&f_tiles)
     {
          static constexpr TileOperations::X true_x = {};
          const auto [true_i_min_x, true_i_max_x]
            = std::ranges::minmax_element(f_tiles, {}, true_x);
          if (true_i_min_x == true_i_max_x)
          {
               return;
          }
          true_min.x = true_x(*true_i_min_x);
          true_max.x = true_x(*true_i_max_x);
     }
     template<std::ranges::range TilesR>
     void get_true_y(TilesR &&f_tiles)
     {
          static constexpr TileOperations::Y true_y = {};
          const auto [true_i_min_y, true_i_max_y]
            = std::ranges::minmax_element(f_tiles, {}, true_y);
          if (true_i_min_y == true_i_max_y)
          {
               return;
          }
          true_min.y = true_y(*true_i_min_y);
          true_max.y = true_y(*true_i_max_y);
     }
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPDIMS_HPP
