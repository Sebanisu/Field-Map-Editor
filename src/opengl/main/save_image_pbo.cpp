//
// Created by pcvii on 2/27/2023.
//
#include "save_image_pbo.hpp"
#include <GL/glew.h>
#include <ScopeGuard.hpp>
#include <spdlog/spdlog.h>
#include <stb_image_write.h>
#include <version>

/**
 * @brief Asynchronously saves an OpenGL texture to an sf::Image using a Pixel Buffer Object (PBO).
 *
 * This function uses a PBO to asynchronously retrieve texture pixel data from GPU memory,
 * allowing the CPU to continue processing without blocking while the GPU completes the readback.
 * The returned future will produce an sf::Image once the pixel data is ready.
 *
 * @param texture The glengine::Texture to be saved into an sf::Image.
 * @return A deferred std::future that will contain the sf::Image once pixel data is read back.
 */
std::future<void> save_image_pbo(std::filesystem::path in_path, glengine::FrameBuffer in_fbo)
{
     // Backup the currently bound framebuffer (to restore it later)
     const auto backup_fbo = in_fbo.backup();
     in_fbo.bind();

     // Calculate the size needed for the pixel buffer: width * height * 4 bytes (RGBA)
     const auto buffer_size = GLsizeiptr{ in_fbo.width() } * GLsizeiptr{ in_fbo.height() } * GLsizeiptr{ 4 };

     // Create and bind a Pixel Buffer Object (PBO)
     auto       pbo_id      = 0U;
     glGenBuffers(1, &pbo_id);
     glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
     glBufferData(GL_PIXEL_PACK_BUFFER, buffer_size, nullptr, GL_STREAM_READ);

     // Request GPU to copy the texture pixels into the PBO
     glengine::GlCall{}(
       glReadPixels,
       0,
       0,
       in_fbo.width(),
       in_fbo.height(),
       GL_RGBA,
       GL_UNSIGNED_BYTE,
       nullptr// Offset into PBO
     );

     // Unbind the PBO after issuing the readback
     glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

     // Return a deferred future to perform the CPU-side readback later
     return std::async(std::launch::deferred, [pbo_id, buffer_size, path = std::move(in_path), fbo = std::move(in_fbo)]() {
          const auto texture_size = fbo.get_size();
#ifdef __cpp_lib_smart_ptr_for_overwrite
          const auto pixels = std::make_unique_for_overwrite<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
#else
            const auto pixels = std::make_unique<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
#endif

          // Bind PBO and retrieve pixel data
          glengine::GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, pbo_id);
          glengine::GlCall{}(glGetBufferSubData, GL_PIXEL_PACK_BUFFER, 0, buffer_size, pixels.get());
          glengine::GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, 0);

          // Clean up resources
          glengine::GlCall{}(glDeleteBuffers, 1, &pbo_id);
          const int channels = 4;// RGBA

          // Create an image from retrieved pixel data
          // Flip the image vertically for stb
          // std::vector<std::uint8_t> flipped(buffer_size);
          // for (int y = 0; y < texture_size.y; ++y)
          // {
          //      std::memcpy(
          //        &flipped[y * texture_size.x * channels],
          //        &pixels[(texture_size.y - 1 - y) * texture_size.x * channels],
          //        texture_size.x * channels);
          // }

          // Save as PNG
          stbi_write_png(path.string().c_str(), texture_size.x, texture_size.y, channels, pixels.get(), texture_size.x * channels);
     });
}


std::future<void> save_rgba8ui_attachment_as_png(std::filesystem::path in_path, glengine::FrameBuffer in_fbo)
{
     // Backup the currently bound framebuffer (to restore it later)
     const auto backup_fbo = in_fbo.backup();
     in_fbo.bind();

     // Make sure to read from the second attachment (index 1)
     glengine::GlCall{}(glReadBuffer, GL_COLOR_ATTACHMENT1);

     // Calculate buffer size: width * height * 4 channels * 1 byte per channel (RGBA8UI)
     const auto buffer_size = GLsizeiptr{ in_fbo.width() } * GLsizeiptr{ in_fbo.height() } * 4;

     // Create and bind a Pixel Buffer Object (PBO)
     auto       pbo_id      = 0U;
     glGenBuffers(1, &pbo_id);
     glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
     glBufferData(GL_PIXEL_PACK_BUFFER, buffer_size, nullptr, GL_STREAM_READ);

     // Request GPU to copy pixels from the RGBA8UI attachment
     glengine::GlCall{}(
       glReadPixels,
       0,
       0,
       in_fbo.width(),
       in_fbo.height(),
       GL_RGBA_INTEGER,// Note the INTEGER format for unsigned integer texture
       GL_UNSIGNED_BYTE,// 8 bits per channel
       nullptr// Offset into PBO
     );

     // Unbind the PBO
     glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

     // Return a deferred future to do CPU readback & save later
     return std::async(std::launch::deferred, [pbo_id, buffer_size, path = std::move(in_path), fbo = std::move(in_fbo)]() {
          const auto texture_size = fbo.get_size();

#ifdef __cpp_lib_smart_ptr_for_overwrite
          auto pixels = std::make_unique_for_overwrite<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
#else
        auto pixels = std::make_unique<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
#endif

          // Bind PBO and retrieve pixel data
          glengine::GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, pbo_id);
          glengine::GlCall{}(glGetBufferSubData, GL_PIXEL_PACK_BUFFER, 0, buffer_size, pixels.get());
          glengine::GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, 0);

          // Clean up the PBO
          glengine::GlCall{}(glDeleteBuffers, 1, &pbo_id);

          const int                       channels = 4;// RGBA

          // Flip the image vertically because OpenGL's origin is bottom-left
          std::unique_ptr<std::uint8_t[]> flipped  = std::make_unique<std::uint8_t[]>(buffer_size);
          for (int y = 0; y < texture_size.y; ++y)
          {
               std::memcpy(
                 &flipped[y * texture_size.x * channels],
                 &pixels[(texture_size.y - 1 - y) * texture_size.x * channels],
                 texture_size.x * channels);
          }

          // Save as PNG using flipped data
          stbi_write_png(path.string().c_str(), texture_size.x, texture_size.y, channels, flipped.get(), texture_size.x * channels);
     });
}