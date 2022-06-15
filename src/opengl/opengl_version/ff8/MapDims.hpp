//
// Created by pcvii on 6/15/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPDIMS_HPP
#define FIELD_MAP_EDITOR_MAPDIMS_HPP
#include "tile_operations.hpp"
namespace ff8
{
namespace MapDimsStatics
{
  static constexpr auto texture_page_width = std::int16_t{ 256 };
  static constexpr auto tile_size          = float{ 16.F };
};// namespace MapDimsStatics
template<typename TileFunctions>
class MapDims
{
  using MapT = open_viii::graphics::background::Map;

public:
  glm::vec2 scaled_size() const noexcept
  {
    return size * tile_scale;
  }
  glm::vec2     offset     = { 0.F, -MapDimsStatics::tile_size };
  glm::vec3     position   = {};
  glm::vec2     true_min   = {};
  glm::vec2     true_max   = {};
  glm::vec2     size       = {};
  glm::vec2     min        = {};
  glm::vec2     max        = {};
  mutable float tile_scale = { 1.F };
  MapDims(const MapT &map)
  {
    map.visit_tiles([&](const auto &tiles) {
      {
        auto f_tiles =
          tiles | std::views::filter(tile_operations::invalid_tile{});
        GetX(f_tiles);
        GetY(f_tiles);
        GetTrueX(f_tiles);
        GetTrueY(f_tiles);
      }
    });
    size     = glm::vec2{ max.x - min.x + MapDimsStatics::tile_size,
                      max.y - min.y + MapDimsStatics::tile_size };
    offset   = glm::vec2{ size.x / 2.F + min.x,
                        size.y / 2.F + min.y - MapDimsStatics::tile_size };
    position = glm::vec3{ -size.x / 2.F, -size.y / 2.F, 0.F };
  }

private:
  template<std::ranges::range TilesR>
  void GetX(TilesR &&f_tiles)
  {
    {
      static constexpr typename TileFunctions::x x = {};
      const auto [i_min_x, i_max_x] =
        std::ranges::minmax_element(f_tiles, {}, x);
      if (i_min_x == i_max_x)
      {
        return;
      }
      static constexpr typename TileFunctions::texture_page texture_page = {};
      const auto                                            i_max_texture_page =
        std::ranges::max_element(f_tiles, {}, texture_page);
      if (i_max_texture_page == std::ranges::end(f_tiles))
      {
        return;
      }
      min.x = x(*i_min_x);
      max.x = static_cast<float>([&, i_max_x = i_max_x]() {
        if constexpr (typename TileFunctions::use_texture_page{})
        {
          return x(*i_max_x);
        }
        else
        {
          return (texture_page(*i_max_texture_page) + 1)
                 * MapDimsStatics::texture_page_width;
        }
      }());
    }
  }
  template<std::ranges::range TilesR>
  void GetY(TilesR &&f_tiles)
  {
    static constexpr typename TileFunctions::y y = {};
    const auto [i_min_y, i_max_y] = std::ranges::minmax_element(f_tiles, {}, y);
    if (i_min_y == i_max_y)
    {
      return;
    }
    min.y = y(*i_min_y);
    max.y = y(*i_max_y);
  }
  template<std::ranges::range TilesR>
  void GetTrueX(TilesR &&f_tiles)
  {
    static constexpr tile_operations::x true_x = {};
    const auto [true_i_min_x, true_i_max_x] =
      std::ranges::minmax_element(f_tiles, {}, true_x);
    if (true_i_min_x == true_i_max_x)
    {
      return;
    }
    true_min.x = true_x(*true_i_min_x);
    true_max.x = true_x(*true_i_max_x);
  }
  template<std::ranges::range TilesR>
  void GetTrueY(TilesR &&f_tiles)
  {
    static constexpr tile_operations::y true_y = {};
    const auto [true_i_min_y, true_i_max_y] =
      std::ranges::minmax_element(f_tiles, {}, true_y);
    if (true_i_min_y == true_i_max_y)
    {
      return;
    }
    true_min.y = true_y(*true_i_min_y);
    true_max.y = true_y(*true_i_max_y);
  }
};
}// namespace ff8
#endif// FIELD_MAP_EDITOR_MAPDIMS_HPP
