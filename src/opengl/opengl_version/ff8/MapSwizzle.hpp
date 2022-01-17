//
// Created by pcvii on 1/3/2022.
//

#ifndef MYPROJECT_MAPSWIZZLE_HPP
#define MYPROJECT_MAPSWIZZLE_HPP
#include "Fields.hpp"
#include "FrameBuffer.hpp"
#include "Map.hpp"
#include "Mim.hpp"
#include "Texture.hpp"

namespace ff8
{
struct TileFunctionsSwizzle
{
  template<typename T>
  struct Bounds
  {
    [[nodiscard]] static constexpr auto x() noexcept
    {
      using tileT = std::decay_t<T>;
      using xT =
        typename std::invoke_result_t<decltype(&tileT::source_x), tileT>;
      return [](const tileT &tile) -> xT { return tile.source_x(); };
    }
    [[nodiscard]] static constexpr auto y() noexcept
    {
      using tileT = std::decay_t<T>;
      using yT =
        typename std::invoke_result_t<decltype(&tileT::source_y), tileT>;
      return [](const tileT &tile) -> yT { return tile.source_y(); };
    }
    [[nodiscard]] static constexpr auto texture_page() noexcept
    {
      using tileT = std::decay_t<T>;
      using texture_pageT =
        typename std::invoke_result_t<decltype(&tileT::texture_id), tileT>;
      return
        [](const tileT &tile) -> texture_pageT { return tile.texture_id(); };
    }
    static constexpr auto all() noexcept
    {
      return std::tuple(x(), y(), texture_page());
    }
  };
};
using MapSwizzle = Map<TileFunctionsSwizzle>;
static_assert(glengine::Renderable<MapSwizzle>);
}// namespace ff8
#endif// MYPROJECT_MAPSWIZZLE_HPP
