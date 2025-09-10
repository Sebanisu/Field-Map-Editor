//
// Created by pcvii on 3/6/2023.
//

#ifndef FIELD_MAP_EDITOR_FUTURE_OPERATIONS_HPP
#define FIELD_MAP_EDITOR_FUTURE_OPERATIONS_HPP
#include "safedir.hpp"
#include <filesystem>
#include <future>
#include <open_viii/graphics/Color.hpp>
#include <spdlog/spdlog.h>
#include <Texture.hpp>
#include <vector>
namespace fme
{
namespace future_operations
{
     class LoadColorsIntoTexture
     {
          glengine::Texture                            *m_texture;
          std::vector<open_viii::graphics::Color32RGBA> m_colors;
          glm::uvec2                                    m_size{};

        public:
          LoadColorsIntoTexture(
            glengine::Texture *const                      in_texture,
            std::vector<open_viii::graphics::Color32RGBA> in_colors,
            glm::uvec2                                    in_size);
          void operator()() const;
     };
     class LoadImageIntoTexture
     {
          glengine::Texture *const m_texture;
          glengine::Image          m_image;

        public:
          LoadImageIntoTexture(glengine::Texture *const in_texture, glengine::Image in_image);
          void operator()();
     };
     class GetImageFromPathCreateFuture
     {
          glengine::Texture *const m_texture;
          std::filesystem::path    m_path;

        public:
          GetImageFromPathCreateFuture(glengine::Texture *const in_texture, std::filesystem::path in_path);
          std::future<void> operator()();
     };

     class GetImageFromFromFirstValidPathCreateFuture
     {
          glengine::Texture *const                                              m_texture;
          mutable std::move_only_function<std::vector<std::filesystem::path>()> m_paths_get;

        public:
          GetImageFromFromFirstValidPathCreateFuture(
            glengine::Texture *const                                        in_texture,
            std::move_only_function<std::vector<std::filesystem::path>()> &&in_paths_get);
          std::future<void> operator()() const;
     };
}// namespace future_operations
}// namespace fme
#endif// FIELD_MAP_EDITOR_FUTURE_OPERATIONS_HPP
