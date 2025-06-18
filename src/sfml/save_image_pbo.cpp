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
std::future<void> save_image_pbo(const std::filesystem::path &path, glengine::FrameBuffer fbo)
{
     // Backup the currently bound framebuffer (to restore it later)
     const auto backup_fbo = fbo.backup();
     fbo.bind();

     // Calculate the size needed for the pixel buffer: width * height * 4 bytes (RGBA)
     const auto buffer_size = GLsizeiptr{ fbo.width() } * GLsizeiptr{ fbo.height() } * GLsizeiptr{ 4 };

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
       static_cast<GLsizei>(fbo.width()),
       static_cast<GLsizei>(fbo.height()),
       GL_RGBA,
       GL_UNSIGNED_BYTE,
       nullptr// Offset into PBO
     );

     // Unbind the PBO after issuing the readback
     glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

     // Return a deferred future to perform the CPU-side readback later
     return std::async(std::launch::deferred, [pbo_id, buffer_size, path, fbo_moved = std::move(fbo)]() {
          const auto texture_size = fbo_moved.get_size();
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