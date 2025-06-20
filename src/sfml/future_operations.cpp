//
// Created by pcvii on 3/6/2023.
//

#include "future_operations.hpp"
#include <Image.hpp>
#include <span>
future_operations::LoadColorsIntoTexture::LoadColorsIntoTexture(
  glengine::Texture *const                        in_texture,
  std::vector<open_viii::graphics::Color32RGBA> &&in_colors,
  glm::uvec2                                      in_size)
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
          assert(m_size.x * m_size.y == std::ranges::size(m_colors));
          if (std::ranges::empty(m_colors) || m_size.x == 0 || m_size.y == 0)
          {
               return;
          }
          *m_texture = glengine::Texture(m_colors, m_size.x, m_size.y);
          // m_texture->create(m_size.x, m_size.y);
          // m_texture->update(m_colors.front().data());
          // m_texture->setSmooth(false);
          // m_texture->setRepeated(false);
          // m_texture->generateMipmap();
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
/// TODO fix loading from images. I was removing sf::Image so some of this code was commented out.
// future_operations::save_image_to_path::save_image_to_path(std::filesystem::path in_path, const sf::Image &in_image)
//   : m_path(std::move(in_path))
//   , m_image(in_image)
// {
// }
// void future_operations::save_image_to_path::operator()() const
// {
//      std::error_code error_code{};
//      std::filesystem::create_directories(m_path.parent_path(), error_code);
//      const std::string &string = m_path.string();
//      if (error_code)
//      {
//           spdlog::error("error {}:{} - {}: {} - path: {}", __FILE__, __LINE__, error_code.value(), error_code.message(), string);
//           error_code.clear();
//      }
//      // if (m_image.getSize().x == 0 || m_image.getSize().y == 0 || m_image.getPixelsPtr() == nullptr)
//      // {
//      //      spdlog::error(
//      //        "error {}:{} Invalid image: \"{}\" - ({},{})", __FILE__, __LINE__, string, m_image.getSize().x, m_image.getSize().y);
//      //      return;
//      // }
//      using namespace std::chrono_literals;
//      std::size_t           count            = { 0U };
//      static constexpr auto error_delay_time = 1000ms;
//      // for (; count != 10; count++)
//      // {
//      //      if (m_image.saveToFile(string))
//      //      {
//      //           spdlog::info("Saved \"{}\"", string);
//      //           return;
//      //      }
//      //      else
//      //      {
//      //           spdlog::error("{}:{} Looping on fail {:>2} times", __FILE__, __LINE__, count);
//      //           std::this_thread::sleep_for(error_delay_time);
//      //      }
//      // }
// }
