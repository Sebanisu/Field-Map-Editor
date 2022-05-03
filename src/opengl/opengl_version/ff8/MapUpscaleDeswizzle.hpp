//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_MAPUPSCALEDESWIZZLE_HPP
#define FIELD_MAP_EDITOR_MAPUPSCALEDESWIZZLE_HPP
#include "Fields.hpp"
#include "FrameBuffer.hpp"
#include "Map.hpp"
#include "Mim.hpp"
#include "Texture.hpp"

namespace ff8
{
struct TileFunctionsUpscaleDeswizzle
{
  template<typename T>
  struct Bounds
  {
  private:
    using tileT = std::decay_t<T>;
    using xT    = typename std::invoke_result_t<decltype(&tileT::x), tileT>;
    using yT    = typename std::invoke_result_t<decltype(&tileT::y), tileT>;
    using texture_pageT =
      typename std::invoke_result_t<decltype(&tileT::texture_id), tileT>;

  public:
    using x = decltype([](const tileT &tile) -> xT { return tile.x(); });
    using y = decltype([](const tileT &tile) -> yT { return tile.y(); });
    using texture_page     = decltype([](const tileT &) -> texture_pageT {
      return {};
    });
    using use_texture_page = std::true_type;
  };
  static constexpr const char *Label = "Map (Upscale Deswizzle)";
};
using MapUpscaleDeswizzle = Map<TileFunctionsUpscaleDeswizzle>;
static_assert(glengine::Renderable<MapUpscaleDeswizzle>);
}// namespace ff8
#endif// FIELD_MAP_EDITOR_MAPDESWIZZLE_HPP