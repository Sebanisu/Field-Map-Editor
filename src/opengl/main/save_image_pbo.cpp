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
     // Backup currently bound framebuffer (restored by your helper)
     const auto backup_fbo = in_fbo.backup();
     in_fbo.bind();

     // Save GL state we’ll touch
     GLint prev_read_buffer = GL_COLOR_ATTACHMENT0;
     glengine::GlCall{}(glGetIntegerv, GL_READ_BUFFER, &prev_read_buffer);

     GLint prev_pack_alignment = 4;
     glengine::GlCall{}(glGetIntegerv, GL_PACK_ALIGNMENT, &prev_pack_alignment);

     GLint prev_pack_row_length = 0;
     glengine::GlCall{}(glGetIntegerv, GL_PACK_ROW_LENGTH, &prev_pack_row_length);

     // Set correct read target for RGBA8 normalized data
     glengine::GlCall{}(glReadBuffer, GL_COLOR_ATTACHMENT0);
     glengine::GlCall{}(glPixelStorei, GL_PACK_ALIGNMENT, 1);
     glengine::GlCall{}(glPixelStorei, GL_PACK_ROW_LENGTH, 0);

     const GLsizei    w           = static_cast<GLsizei>(in_fbo.width());
     const GLsizei    h           = static_cast<GLsizei>(in_fbo.height());
     const GLint      channels    = 4;
     const GLsizeiptr buffer_size = GLsizeiptr{ w } * GLsizeiptr{ h } * GLsizeiptr{ channels };

     // Create and bind PBO
     GLuint           pbo_id      = 0;
     glGenBuffers(1, &pbo_id);
     glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
     glBufferData(GL_PIXEL_PACK_BUFFER, buffer_size, nullptr, GL_STREAM_READ);

     // Issue the readback (normalized RGBA8)
     glengine::GlCall{}(glReadPixels, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

     if (GLenum err = glGetError(); err != GL_NO_ERROR)
     {
          spdlog::error("save_image_pbo: glReadPixels error: 0x{:X}", static_cast<unsigned>(err));
     }

     // Unbind PBO
     glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

     // Restore GL state
     glengine::GlCall{}(glReadBuffer, prev_read_buffer);
     glengine::GlCall{}(glPixelStorei, GL_PACK_ALIGNMENT, prev_pack_alignment);
     glengine::GlCall{}(glPixelStorei, GL_PACK_ROW_LENGTH, prev_pack_row_length);

     // Return deferred CPU-side save
     return std::async(std::launch::deferred, [pbo_id, buffer_size, w, h, channels, path = std::move(in_path), fbo = std::move(in_fbo)]() {
          // Ensure directory exists
          if (!path.parent_path().empty())
          {
               std::error_code ec;
               std::filesystem::create_directories(path.parent_path(), ec);
               if (ec)
               {
                    spdlog::error("Failed to create directories for '{}': {}", path.string(), ec.message());
               }
          }

#ifdef __cpp_lib_smart_ptr_for_overwrite
          auto pixels = std::make_unique_for_overwrite<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
#else
        auto pixels = std::make_unique<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
#endif

          // Retrieve from PBO
          glengine::GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, pbo_id);
          glengine::GlCall{}(glGetBufferSubData, GL_PIXEL_PACK_BUFFER, 0, buffer_size, pixels.get());
          if (GLenum err = glGetError(); err != GL_NO_ERROR)
          {
               spdlog::error("save_image_pbo: glGetBufferSubData error: 0x{:X}", static_cast<unsigned>(err));
          }
          glengine::GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, 0);

          // Delete PBO
          glengine::GlCall{}(glDeleteBuffers, 1, &pbo_id);

          // Flip vertically
          std::unique_ptr<std::uint8_t[]> flipped   = std::make_unique<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
          const std::size_t               row_bytes = static_cast<std::size_t>(w) * static_cast<std::size_t>(channels);
          for (GLint y = 0; y < h; ++y)
          {
               const std::size_t src = static_cast<std::size_t>(h - 1 - y) * row_bytes;
               const std::size_t dst = static_cast<std::size_t>(y) * row_bytes;
               std::memcpy(flipped.get() + dst, pixels.get() + src, row_bytes);
          }

          // Save PNG
          int ok = stbi_write_png(path.string().c_str(), w, h, channels, flipped.get(), static_cast<int>(row_bytes));
          if (!ok)
          {
               spdlog::error("stbi_write_png failed for '{}'", path.string());
          }
          else
          {
               spdlog::info("Wrote RGBA8 attachment to '{}'", path.string());
          }
     });
}

std::future<void> save_rgba8ui_attachment_as_png(std::filesystem::path in_path, glengine::FrameBuffer in_fbo)
{
     // Backup FBO (your helper should restore on scope exit)
     const auto backup_fbo = in_fbo.backup();
     in_fbo.bind();

     // Save GL state we’ll touch
     GLint prev_read_buffer = GL_COLOR_ATTACHMENT0;
     glengine::GlCall{}(glGetIntegerv, GL_READ_BUFFER, &prev_read_buffer);

     GLint prev_pack_alignment = 4;
     glengine::GlCall{}(glGetIntegerv, GL_PACK_ALIGNMENT, &prev_pack_alignment);

     GLint prev_pack_row_length = 0;
     glengine::GlCall{}(glGetIntegerv, GL_PACK_ROW_LENGTH, &prev_pack_row_length);

     // Switch to the integer color attachment we want to read
     glengine::GlCall{}(glReadBuffer, GL_COLOR_ATTACHMENT1);

     // Make sure pack state is sane for tightly-packed bytes
     glengine::GlCall{}(glPixelStorei, GL_PACK_ALIGNMENT, 1);
     glengine::GlCall{}(glPixelStorei, GL_PACK_ROW_LENGTH, 0);

     const GLsizei    w           = static_cast<GLsizei>(in_fbo.width());
     const GLsizei    h           = static_cast<GLsizei>(in_fbo.height());
     const GLint      channels    = 4;
     const GLsizeiptr buffer_size = GLsizeiptr{ w } * GLsizeiptr{ h } * GLsizeiptr{ channels };

     // Create a PBO and kick off the GPU read (integer format!)
     GLuint           pbo_id      = 0;
     glGenBuffers(1, &pbo_id);
     glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
     glBufferData(GL_PIXEL_PACK_BUFFER, buffer_size, nullptr, GL_STREAM_READ);

     glengine::GlCall{}(
       glReadPixels,
       0,
       0,
       w,
       h,
       GL_RGBA_INTEGER,// reading from *integer* attachment
       GL_UNSIGNED_BYTE,// 8-bit unsigned integer components
       nullptr// offset into bound PBO
     );

     // Capture any immediate GL errors (bad attachment/format mismatch, etc.)
     if (GLenum err = glGetError(); err != GL_NO_ERROR)
     {
          spdlog::error("save_rgba8ui_attachment_as_png: glReadPixels error: 0x{:X}", static_cast<unsigned>(err));
     }

     // Unbind PBO from pack target on the GL thread
     glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

     // Restore GL state we changed
     glengine::GlCall{}(glReadBuffer, prev_read_buffer);
     glengine::GlCall{}(glPixelStorei, GL_PACK_ALIGNMENT, prev_pack_alignment);
     glengine::GlCall{}(glPixelStorei, GL_PACK_ROW_LENGTH, prev_pack_row_length);

     // Defer CPU-side readback & write (note: std::launch::deferred requires .get() to execute)
     return std::async(std::launch::deferred, [pbo_id, buffer_size, w, h, channels, path = std::move(in_path), fbo = std::move(in_fbo)]() {
          // Ensure directory exists
          if (!path.parent_path().empty())
          {
               std::error_code ec;
               std::filesystem::create_directories(path.parent_path(), ec);
               if (ec)
               {
                    spdlog::error("Failed to create directories for '{}': {}", path.string(), ec.message());
               }
          }

          // Allocate CPU buffer
#ifdef __cpp_lib_smart_ptr_for_overwrite
          auto pixels = std::make_unique_for_overwrite<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
#else
        auto pixels = std::make_unique<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
#endif

          // Bind PBO and pull data to CPU
          glengine::GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, pbo_id);
          glengine::GlCall{}(glGetBufferSubData, GL_PIXEL_PACK_BUFFER, 0, buffer_size, pixels.get());

          if (GLenum err = glGetError(); err != GL_NO_ERROR)
          {
               spdlog::error("save_rgba8ui_attachment_as_png: glGetBufferSubData error: 0x{:X}", static_cast<unsigned>(err));
          }

          glengine::GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, 0);

          // Delete PBO now that data is on CPU
          glengine::GlCall{}(glDeleteBuffers, 1, &pbo_id);

          // Flip vertically (OpenGL origin is bottom-left)
          std::unique_ptr<std::uint8_t[]> flipped   = std::make_unique<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
          const std::size_t               row_bytes = static_cast<std::size_t>(w) * static_cast<std::size_t>(channels);
          for (GLint y = 0; y < h; ++y)
          {
               const std::size_t src = static_cast<std::size_t>(h - 1 - y) * row_bytes;
               const std::size_t dst = static_cast<std::size_t>(y) * row_bytes;
               std::memcpy(flipped.get() + dst, pixels.get() + src, row_bytes);
          }

          // Write PNG (stbi expects 0..255 bytes; RGBA8UI matches that)
          int ok = stbi_write_png(path.string().c_str(), w, h, channels, flipped.get(), static_cast<int>(row_bytes));
          if (!ok)
          {
               spdlog::error("stbi_write_png failed for '{}'", path.string());
          }
          else
          {
               spdlog::info("Wrote RGBA8UI attachment to '{}'", path.string());
          }
     });
}