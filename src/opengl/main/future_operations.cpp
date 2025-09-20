//
// Created by pcvii on 3/6/2023.
//

#include "future_operations.hpp"
#include <Image.hpp>
#include <iostream>
#include <span>
#include <stacktrace>
namespace fme
{
void future_operations::LoadColorsIntoTexture::operator()() const
{
     if (weak_ptr.expired())
     {
          spdlog::warn(
            "{}:{} - LoadColorsIntoTexture weak_ptr "
            "is expired.",
            __FILE__,
            __LINE__);
          return;
     }
     if (!texture)
     {
          spdlog::error(
            "Failed to lock texture weak_ptr: texture is expired or not set.");
          return;
     }
     try
     {
          spdlog::debug(
            "Trying to pass colors[{}] into texture ({}, {})",
            std::ranges::size(colors),
            size.x,
            size.y);
          assert(size.x * size.y == std::ranges::size(colors));
          if (std::ranges::empty(colors) || size.x == 0 || size.y == 0)
          {
               return;
          }
          *texture = glengine::Texture(
            std::move(colors),
            static_cast<int>(size.x),
            static_cast<int>(size.y));
     }
     catch (const std::exception &e)
     {
          // Handle the exception and log the error message using spdlog
          spdlog::error(
            "Exception caught while creating texture: {}", e.what());
     }
}

void future_operations::LoadImageIntoTexture::operator()() const
{
     if (weak_ptr.expired())
     {
          spdlog::warn(
            "{}:{} - LoadImageIntoTexture weak_ptr "
            "is expired.",
            __FILE__,
            __LINE__);
          return;
     }
     if (!texture)
     {
          spdlog::error("m_texture is a nullptr");
          return;
     }
     try
     {

          spdlog::info(
            "Trying to pass image[{}] into texture ({}, {})",
            image.path.string(),
            image.width,
            image.height);
          if (image.width == 0 || image.height == 0)
          {
               return;
          }
          *texture = glengine::Texture(std::move(image));
     }
     catch (const std::exception &e)
     {
          // Handle the exception and log the error message using spdlog
          spdlog::error(
            "Exception caught while creating texture: {}", e.what());
     }
}
std::future<void>
  future_operations::GetImageFromPathCreateFuture::operator()() const
{
     try
     {
          if (weak_ptr.expired())
          {
               spdlog::warn(
                 "{}:{} - GetImageFromPathCreateFuture weak_ptr "
                 "is expired.",
                 __FILE__,
                 __LINE__);
               return {};
          }
          spdlog::info(
            "{}:{} - texture path: \"{}\"", __FILE__, __LINE__, path.string());
          return { std::async(
            std::launch::deferred,
            LoadImageIntoTexture{
              .weak_ptr = weak_ptr,
              .texture  = texture,
              .image    = glengine::Image(std::move(path), false) }) };
     }
     catch (const std::exception &e)
     {
          // Handle the exception and log the error message using spdlog
          spdlog::error("Exception caught while loading image: {}", e.what());
          return {};
     }
}

std::future<void>
  future_operations::GetImageFromFromFirstValidPathCreateFuture::operator()()
    const
{
     try
     {
          if (weak_ptr.expired())
          {
               spdlog::warn(
                 "{}:{} - GetImageFromFromFirstValidPathCreateFuture weak_ptr "
                 "is expired.",
                 __FILE__,
                 __LINE__);
               return {};
          }
          std::vector<std::filesystem::path> paths = paths_get();
          auto                               filtered_paths
            = paths
              | std::ranges::views::transform(
                [](auto &&path) -> std::filesystem::path
                { return std::forward<decltype(path)>(path); })
              | std::views::filter(
                [](safedir path)
                { return path.is_exists() && !path.is_dir(); });
          if (filtered_paths.begin() == filtered_paths.end())
          {
               spdlog::warn(
                 "{}:{} - filtered_paths empty. m_paths.size() = {}",
                 __FILE__,
                 __LINE__,
                 std::ranges::size(paths));
               return {};
          }
          return GetImageFromPathCreateFuture{ .weak_ptr = weak_ptr,
                                               .texture  = texture,
                                               .path
                                               = *filtered_paths.begin() }();
     }
     catch (const std::exception &e)
     {
          // Handle the exception and log the error message using spdlog
          spdlog::error("Exception caught while loading image: {}", e.what());
          return {};
     }
}
}// namespace fme