//
// Created by pcvii on 3/6/2023.
//

#ifndef FIELD_MAP_EDITOR_FUTURE_OPERATIONS_HPP
#define FIELD_MAP_EDITOR_FUTURE_OPERATIONS_HPP
#include "safedir.hpp"
#include <filesystem>
#include <future>
#include <glengine/Texture.hpp>
#include <map>
#include <open_viii/graphics/Color.hpp>
#include <spdlog/spdlog.h>
#include <vector>
namespace fme
{
namespace future_operations
{
     struct LoadColorsIntoTexture
     {
          std::weak_ptr<void>                                   weak_ptr;
          glengine::Texture                                    *texture;
          mutable std::vector<open_viii::graphics::Color32RGBA> colors;
          glm::uvec2                                            size;

          void operator()() const;
     };
     struct LoadImageIntoTexture
     {
          std::weak_ptr<void>      weak_ptr;
          glengine::Texture *const texture;
          mutable glengine::Image  image;

          void                     operator()() const;
     };
     struct GetImageFromPathCreateFuture
     {
          std::weak_ptr<void>           weak_ptr;
          glengine::Texture *const      texture;
          mutable std::filesystem::path path;

          std::future<void>             operator()() const;
     };

     struct GetImageFromFromFirstValidPathCreateFuture
     {
          std::weak_ptr<void>      weak_ptr;
          glengine::Texture *const texture;
          mutable std::move_only_function<std::vector<std::filesystem::path>()>
                            paths_get;

          std::future<void> operator()() const;
     };
}// namespace future_operations
}// namespace fme
#endif// FIELD_MAP_EDITOR_FUTURE_OPERATIONS_HPP
