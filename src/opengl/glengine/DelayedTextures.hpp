//
// Created by pcvii on 12/1/2021.
//

#ifndef FIELD_MAP_EDITOR_DELAYEDTEXTURES_HPP
#define FIELD_MAP_EDITOR_DELAYEDTEXTURES_HPP
#include "Texture.hpp"
#include <future>
namespace glengine
{
struct DelayedTexturesData
{
     std::filesystem::path                  path   = {};
     std::vector<glm::vec<4, std::uint8_t>> colors = {};
     std::int32_t                           width  = {};
     std::int32_t                           height = {};
     Texture                               *out    = {};
};
template<std::size_t sizeT>
struct DelayedTextures
{

     mutable std::unique_ptr<std::array<Texture, sizeT>> textures{
          std::make_unique<std::array<Texture, sizeT>>()
     };
     mutable std::vector<std::future<DelayedTexturesData>> futures{};
     [[nodiscard]] bool                                    on_update() const
     {
          for (std::future<DelayedTexturesData> &future : futures)
          {
               if (
                 future.valid()
                 && future.wait_for(std::chrono::seconds(0))
                      == std::future_status::ready)
               {
                    DelayedTexturesData rd = future.get();
                    *rd.out = Texture(rd.colors, rd.width, rd.height);
                    spdlog::debug(
                      "Finished Loading Texture: {:>2}\t{}",
                      std::distance(textures->data(), rd.out),
                      rd.path.string());
                    return true;
               }
          }
          remove_invalid();
          return false;
     }
     void remove_invalid() const
     {
          if (!std::ranges::empty(futures))
          {
               const auto [first, last] = std::ranges::remove_if(
                 futures, [](const std::future<DelayedTexturesData> &future)
                 { return !future.valid(); });
               futures.erase(first, last);
          }
     }
};
}// namespace glengine
#endif// FIELD_MAP_EDITOR_DELAYEDTEXTURES_HPP
