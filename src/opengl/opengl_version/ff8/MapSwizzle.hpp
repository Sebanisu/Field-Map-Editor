//
// Created by pcvii on 1/3/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPSWIZZLE_HPP
#define FIELD_MAP_EDITOR_MAPSWIZZLE_HPP
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
  private:
    using tileT = std::decay_t<T>;
    using xT = typename std::invoke_result_t<decltype(&tileT::source_x), tileT>;
    using yT = typename std::invoke_result_t<decltype(&tileT::source_y), tileT>;
    using texture_pageT =
      typename std::invoke_result_t<decltype(&tileT::texture_id), tileT>;

  public:
    using x = decltype([](const tileT &tile) -> xT { return tile.source_x(); });
    using y = decltype([](const tileT &tile) -> yT { return tile.source_y(); });
    using texture_page     = decltype([](const tileT &tile) -> texture_pageT {
      return tile.texture_id();
    });
    using use_texture_page = std::false_type;
  };
  static constexpr const char *Label = "Map (Swizzle)";
};
using MapSwizzle = Map<TileFunctionsSwizzle>;
static_assert(glengine::Renderable<MapSwizzle>);
}// namespace ff8
#endif// FIELD_MAP_EDITOR_MAPSWIZZLE_HPP
