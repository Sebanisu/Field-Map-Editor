//
// Created by pcvii on 12/1/2021.
//

#ifndef MYPROJECT_DELAYEDTEXTURES_HPP
#define MYPROJECT_DELAYEDTEXTURES_HPP
#include "Texture.hpp"
#include <fmt/format.h>
#include <future>
#include <open_viii/graphics/color/Color32.hpp>
struct DelayedTextures
{
  struct return_data
  {
    std::vector<open_viii::graphics::Color32RGBA> colors{};
    std::int32_t                                  width{};
    std::int32_t                                  height{};
    Texture                                      *out{};
  };
  mutable std::unique_ptr<std::array<Texture, 49U>> textures{
    std::make_unique<std::array<Texture, 49U>>()
  };
  mutable std::vector<std::future<return_data>> futures{};
  void
    check() const
  {
    for (std::future<return_data> &future : futures)
    {
      if (
        future.valid()
        && future.wait_for(std::chrono::seconds(0))
             == std::future_status::ready)
      {
        return_data rd = future.get();
        *rd.out        = Texture(rd.colors, rd.width, rd.height);
        fmt::print(
          "Finished Loading Texture: {:>2}\n",
          std::distance(textures->data(), rd.out));
        return;
      }
    }
    remove_invalid();
  }
  void
    remove_invalid() const
  {
    if (!std::ranges::empty(futures))
    {
      const auto [first, last] = std::ranges::remove_if(
        futures,
        [](const std::future<return_data> &future) { return !future.valid(); });
      futures.erase(first, last);
    }
  }
};
#endif// MYPROJECT_DELAYEDTEXTURES_HPP
