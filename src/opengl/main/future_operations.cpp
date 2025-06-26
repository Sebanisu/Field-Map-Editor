//
// Created by pcvii on 3/6/2023.
//

#include "future_operations.hpp"
#include <Image.hpp>
#include <iostream>
#include <span>
#include <stacktrace>
future_operations::LoadColorsIntoTexture::LoadColorsIntoTexture(
  glengine::Texture *const                      in_texture,
  std::vector<open_viii::graphics::Color32RGBA> in_colors,
  glm::uvec2                                    in_size)
  : m_texture(in_texture)
  , m_colors(std::move(in_colors))
  , m_size(in_size)
{
}
void future_operations::LoadColorsIntoTexture::operator()() const
{
     if (!m_texture)
     {
          spdlog::error("Failed to lock texture weak_ptr: texture is expired or not set.");
          return;
     }
     try
     {
          spdlog::debug("Trying to pass colors[{}] into texture ({}, {})", std::ranges::size(m_colors), m_size.x, m_size.y);
          assert(m_size.x * m_size.y == std::ranges::size(m_colors));
          if (std::ranges::empty(m_colors) || m_size.x == 0 || m_size.y == 0)
          {
               return;
          }
          *m_texture       = glengine::Texture(m_colors, m_size.x, m_size.y);
          //const auto stack = std::stacktrace::current();
          //std::cout << stack << std::endl;
     }
     catch (const std::exception &e)
     {
          // Handle the exception and log the error message using spdlog
          spdlog::error("Exception caught while creating texture: {}", e.what());
     }
}
future_operations::LoadImageIntoTexture::LoadImageIntoTexture(glengine::Texture *const in_texture, glengine::Image in_image)
  : m_texture(in_texture)
  , m_image(std::move(in_image))
{
}
void future_operations::LoadImageIntoTexture::operator()()
{
     if (!m_texture)
     {
          spdlog::error("m_texture is a nullptr");
          return;
     }
     try
     {

          spdlog::info("Trying to pass image[{}] into texture ({}, {})", m_image.path.string(), m_image.width, m_image.height);
          if (m_image.width == 0 || m_image.height == 0)
          {
               return;
          }
          *m_texture = glengine::Texture(std::move(m_image));

     }
     catch (const std::exception &e)
     {
          // Handle the exception and log the error message using spdlog
          spdlog::error("Exception caught while creating texture: {}", e.what());
     }
}
future_operations::GetImageFromPathCreateFuture::GetImageFromPathCreateFuture(
  glengine::Texture *const in_texture,
  std::filesystem::path    in_path)
  : m_texture(in_texture)
  , m_path(std::move(in_path))
{
}
std::future<void> future_operations::GetImageFromPathCreateFuture::operator()()
{
     try
     {
          spdlog::info("texture path: \"{}\"", m_path.string());

          return { std::async(std::launch::deferred, LoadImageIntoTexture{ m_texture, glengine::Image(std::move(m_path), false) }) };
     }
     catch (const std::exception &e)
     {
          // Handle the exception and log the error message using spdlog
          spdlog::error("Exception caught while loading image: {}", e.what());
          return {};
     }
}
