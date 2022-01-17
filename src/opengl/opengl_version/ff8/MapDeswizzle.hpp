//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_MAP_HPP
#define MYPROJECT_MAP_HPP
#include "Fields.hpp"
#include "FrameBuffer.hpp"
#include "Mim.hpp"
#include "Map.hpp"
#include "Texture.hpp"

namespace ff8
{struct TileFunctionsDeswizzle
{
  template<typename T>
  struct Bounds
  {
    [[nodiscard]] static constexpr auto x() noexcept
    {
      using tileT = std::decay_t<T>;
      using xT    = typename std::invoke_result_t<decltype(&tileT::x), tileT>;
      return [](const tileT &tile) -> xT { return tile.x(); };
    }
    [[nodiscard]] static constexpr auto y() noexcept
    {
      using tileT = std::decay_t<T>;
      using yT    = typename std::invoke_result_t<decltype(&tileT::y), tileT>;
      return [](const tileT &tile) -> yT { return tile.y(); };
    }
    [[nodiscard]] static constexpr auto texture_page() noexcept
    {
      using tileT = std::decay_t<T>;
      using texture_pageT =
        typename std::invoke_result_t<decltype(&tileT::texture_id), tileT>;
      return [](const tileT &) -> texture_pageT {
        return 0;// tile.texture_id();
      };
    }
    static constexpr auto all() noexcept
    {
      return std::tuple(x(), y(), texture_page());
    }
  };
};
using MapDeswizzle = Map<TileFunctionsDeswizzle>;
static_assert(glengine::Renderable<MapDeswizzle>);
}// namespace ff8
#endif// MYPROJECT_MAPDESWIZZLE_HPP
