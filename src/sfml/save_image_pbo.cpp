//
// Created by pcvii on 2/27/2023.
//
#include "save_image_pbo.hpp"
#include <GL/glew.h>
#include <version>
std::future<sf::Image> save_image_pbo(const sf::Texture &texture)
{

     const auto buffer_size = GLsizeiptr{ texture.getSize().x } * GLsizeiptr{ texture.getSize().y } * 4;
     auto       pbo_id      = 0U;
     glGenBuffers(1, &pbo_id);
     glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
     glBufferData(GL_PIXEL_PACK_BUFFER, buffer_size, nullptr, GL_STREAM_READ);

     // Issue a request to the GPU to copy the texture to the PBO
     glReadBuffer(GL_FRONT);
     glReadPixels(
       0, 0, static_cast<GLsizei>(texture.getSize().x), static_cast<GLsizei>(texture.getSize().y), GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

     glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
     // Bind the PBO to a future that will be returned
     return std::async(std::launch::deferred, [texture_size = texture.getSize(), pbo_id, buffer_size]() -> sf::Image {
#ifdef __cpp_lib_smart_ptr_for_overwrite
          const auto pixels = std::make_unique_for_overwrite<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
#else
          const auto pixels = std::make_unique<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
#endif

          glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
          glGetBufferSubData(GL_PIXEL_PACK_BUFFER, 0, buffer_size, pixels.get());
          glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

          sf::Image image;
          image.create(texture_size.x, texture_size.y, pixels.get());
          glDeleteBuffers(1, &pbo_id);

          return image;
     });
}