//
// Created by pcvii on 3/6/2023.
//

#ifndef FIELD_MAP_EDITOR_FUTURE_OPERATIONS_HPP
#define FIELD_MAP_EDITOR_FUTURE_OPERATIONS_HPP
#include "safedir.hpp"
#include <filesystem>
#include <future>
#include <open_viii/graphics/Color.hpp>
#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>
#include <vector>
namespace future_operations
{
class LoadColorsIntoTexture
{
     sf::Texture                                  *m_texture;
     std::vector<open_viii::graphics::Color32RGBA> m_colors;
     sf::Vector2u                                  m_size{};

   public:
     LoadColorsIntoTexture(sf::Texture *const in_texture, std::vector<open_viii::graphics::Color32RGBA> &&in_colors, sf::Vector2u in_size);
     void operator()() const;
};
class LoadImageIntoTexture
{
     sf::Texture *m_texture;
     sf::Image    m_image;

   public:
     LoadImageIntoTexture(sf::Texture *const in_texture, sf::Image in_image);
     void operator()() const;
};
class GetImageFromPathCreateFuture
{
     sf::Texture          *m_texture;
     std::filesystem::path m_path;

   public:
     GetImageFromPathCreateFuture(sf::Texture *const in_texture, std::filesystem::path in_path);
     std::future<void> operator()() const;
};
class save_image_to_path
{
     std::filesystem::path m_path;
     sf::Image             m_image;

   public:
     save_image_to_path(std::filesystem::path in_path, const sf::Image &in_image);
     void operator()() const;
};
template<std::ranges::contiguous_range range_t>
class GetImageFromFromFirstValidPathCreateFuture
{
     sf::Texture *m_texture;
     range_t      m_paths;

   public:
     GetImageFromFromFirstValidPathCreateFuture(sf::Texture *const in_texture, range_t &&in_paths)
       : m_texture(in_texture)
       , m_paths(std::move(in_paths))
     {
     }
     std::future<void> operator()() const
     {
          try
          {
               auto filtered_paths =
                 m_paths
                 | std::ranges::views::transform([](auto &&path) -> std::filesystem::path { return std::forward<decltype(path)>(path); })
                 | std::views::filter([](safedir path) { return path.is_exists() && !path.is_dir(); });
               if (filtered_paths.begin() == filtered_paths.end())
               {
                    return {};
               }
               return GetImageFromPathCreateFuture{ m_texture, *filtered_paths.begin() }();
          }
          catch (const std::exception &e)
          {
               // Handle the exception and log the error message using spdlog
               spdlog::error("Exception caught while loading image: {}", e.what());
               return {};
          }
     }
};
}// namespace future_operations
#endif// FIELD_MAP_EDITOR_FUTURE_OPERATIONS_HPP
