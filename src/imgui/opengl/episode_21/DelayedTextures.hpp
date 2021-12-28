//
// Created by pcvii on 12/1/2021.
//

#ifndef MYPROJECT_DELAYEDTEXTURES_HPP
#define MYPROJECT_DELAYEDTEXTURES_HPP
#include "Texture.hpp"

struct DelayedTexturesData
{
  std::vector<open_viii::graphics::Color32RGBA> colors{};
  std::int32_t                                  width{};
  std::int32_t                                  height{};
  Texture                                      *out{};
};
template<std::size_t sizeT>
struct DelayedTextures
{

  mutable std::unique_ptr<std::array<Texture, sizeT>> textures{
    std::make_unique<std::array<Texture, sizeT>>()
  };
  mutable std::vector<std::future<DelayedTexturesData>> futures{};
  void                                                  check() const
  {
    for (std::future<DelayedTexturesData> &future : futures)
    {
      if (
        future.valid()
        && future.wait_for(std::chrono::seconds(0))
             == std::future_status::ready)
      {
        DelayedTexturesData rd = future.get();
        *rd.out                = Texture(rd.colors, rd.width, rd.height);
        fmt::print(
          "Finished Loading Texture: {:>2}\n",
          std::distance(textures->data(), rd.out));
        return;
      }
    }
    remove_invalid();
  }
  void remove_invalid() const
  {
    if (!std::ranges::empty(futures))
    {
      const auto [first, last] = std::ranges::remove_if(
        futures, [](const std::future<DelayedTexturesData> &future) {
          return !future.valid();
        });
      futures.erase(first, last);
    }
  }
};
#endif// MYPROJECT_DELAYEDTEXTURES_HPP
