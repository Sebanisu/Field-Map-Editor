//
// Created by pcvii on 2/27/2023.
//
#include "save_image_pbo.hpp"
#include "scope_guard.hpp"
#include <GL/glew.h>
#include <spdlog/spdlog.h>
#include <version>

/**
 * @brief Saves and restores the currently bound OpenGL framebuffer.
 *
 * Captures the currently bound framebuffer and returns a scope guard
 * that, when destroyed, restores the framebuffer binding automatically.
 *
 * @return A scope_guard that restores the previous framebuffer binding.
 */
[[nodiscard]] auto backup_frame_buffer()
{
     // Save the currently bound framebuffer
     GLint current_framebuffer = 0;
     glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current_framebuffer);

     // Create a scope guard to restore the framebuffer when it goes out of scope
     return scope_guard([=]() { glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(current_framebuffer)); });
}

/**
 * @brief RAII wrapper around an OpenGL framebuffer and its color attachment texture.
 *
 * Manages the lifetime of an OpenGL framebuffer object (FBO) and its attached texture.
 * Provides functionality for creation, binding, and destruction.
 */
class [[nodiscard]] Framebuffer
{
     GLuint id;///< Framebuffer object ID
     GLuint color_attachment_id;///< Color attachment texture ID

   public:
     /**
      * @brief Constructs a Framebuffer by copying data from an existing sf::Texture.
      *
      * @param texture The texture to copy into the framebuffer.
      */
     explicit Framebuffer(const sf::Texture &texture)
     {
          // Create and configure a new texture for the framebuffer color attachment
          glGenTextures(1, &color_attachment_id);
          glBindTexture(GL_TEXTURE_2D, color_attachment_id);

          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

          glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            static_cast<GLsizei>(texture.getSize().x),
            static_cast<GLsizei>(texture.getSize().y),
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr);

          // Create and set up a framebuffer object
          glCreateFramebuffers(1, &id);
          glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
          glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment_id, 0);

          // Check framebuffer completeness
          if (GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE)
          {
               spdlog::warn("Framebuffer is incomplete (status = {})", status);
          }

          // Create a temporary framebuffer for reading the original texture
          GLuint tmp;
          glCreateFramebuffers(1, &tmp);
          glBindFramebuffer(GL_READ_FRAMEBUFFER, tmp);
          glBindTexture(GL_TEXTURE_2D, texture.getNativeHandle());
          glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.getNativeHandle(), 0);

          // Check read framebuffer completeness
          if (GLenum status = glCheckFramebufferStatus(GL_READ_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE)
          {
               spdlog::warn("Framebuffer is incomplete (status = {})", status);
          }

          // Copy (blit) the texture from the original to the new texture
          glBlitFramebuffer(
            0,
            0,
            static_cast<GLsizei>(texture.getSize().x),
            static_cast<GLsizei>(texture.getSize().y),
            0,
            0,
            static_cast<GLsizei>(texture.getSize().x),
            static_cast<GLsizei>(texture.getSize().y),
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST);

          // Clean up bindings
          glBindTexture(GL_TEXTURE_2D, 0);
          glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
          glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);
          glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
          glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
          glDeleteFramebuffers(1, &tmp);

          // Rebind and finalize framebuffer setup
          bind();
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment_id, 0);

          if (GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE)
          {
               spdlog::warn("Framebuffer is incomplete (status = {})", status);
          }
     }

     /**
      * @brief Constructs a Framebuffer from existing OpenGL framebuffer and texture IDs.
      *
      * @param in_id Framebuffer ID.
      * @param in_color_attachment_id Texture ID attached to framebuffer.
      */
     explicit Framebuffer(GLuint in_id, GLuint in_color_attachment_id) noexcept
       : id(in_id)
       , color_attachment_id(in_color_attachment_id)
     {
     }

     // Deleted copy constructor and assignment (non-copyable)
     Framebuffer(const Framebuffer &)            = delete;
     Framebuffer &operator=(const Framebuffer &) = delete;

     /**
      * @brief Move constructor.
      *
      * Transfers ownership of OpenGL resources.
      */
     Framebuffer(Framebuffer &&other) noexcept
       : id(other.id)
       , color_attachment_id(other.color_attachment_id)
     {
          other.id                  = 0;
          other.color_attachment_id = 0;
     }

     /**
      * @brief Move assignment operator.
      *
      * Transfers ownership of OpenGL resources.
      */
     Framebuffer &operator=(Framebuffer &&other) noexcept
     {
          if (this != &other)
          {
               id                        = other.id;
               color_attachment_id       = other.color_attachment_id;
               other.id                  = 0;
               other.color_attachment_id = 0;
          }
          return *this;
     }

     /**
      * @brief Implicit conversion to GLuint.
      *
      * @return The underlying OpenGL framebuffer ID.
      */
     [[nodiscard]] operator GLuint() const noexcept
     {
          return id;
     }

     /**
      * @brief Binds the specified framebuffer ID.
      *
      * @param in_id The framebuffer ID to bind.
      */
     static void bind(GLuint in_id)
     {
          glBindFramebuffer(GL_FRAMEBUFFER, in_id);
     }

     /**
      * @brief Binds this framebuffer.
      */
     void bind() const
     {
          glBindFramebuffer(GL_FRAMEBUFFER, id);
     }

     /**
      * @brief Unbinds any currently bound framebuffer.
      */
     static void unbind()
     {
          glBindFramebuffer(GL_FRAMEBUFFER, 0);
     }

     /**
      * @brief Destructor. Cleans up OpenGL resources.
      */
     ~Framebuffer()
     {
          destory();
     }

     /**
      * @brief Manually destroys the framebuffer and texture resources.
      */
     void destory()
     {
          if (id != 0)
          {
               GLint currentFramebuffer;
               glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFramebuffer);
               if (std::cmp_equal(currentFramebuffer, id))
               {
                    // Unbind if currently bound
                    unbind();
               }

               glDeleteFramebuffers(1, &id);
               glDeleteTextures(1, &color_attachment_id);
               id                  = 0;
               color_attachment_id = 0;
          }
     }
};

/**
 * @brief Asynchronously saves an OpenGL texture to an sf::Image using a Pixel Buffer Object (PBO).
 *
 * This function uses a PBO to asynchronously retrieve texture pixel data from GPU memory,
 * allowing the CPU to continue processing without blocking while the GPU completes the readback.
 * The returned future will produce an sf::Image once the pixel data is ready.
 *
 * @param texture The sf::Texture to be saved into an sf::Image.
 * @return A deferred std::future that will contain the sf::Image once pixel data is read back.
 */
std::future<sf::Image> save_image_pbo(const sf::Texture &texture)
{
     // Get the size of the texture
     const auto texture_size = texture.getSize();

     // Backup the currently bound framebuffer (to restore it later)
     const auto backup_fbo   = backup_frame_buffer();

     // Calculate the size needed for the pixel buffer: width * height * 4 bytes (RGBA)
     const auto buffer_size  = GLsizeiptr{ texture.getSize().x } * GLsizeiptr{ texture.getSize().y } * 4;

     // Create and bind a Pixel Buffer Object (PBO)
     auto       pbo_id       = 0U;
     glGenBuffers(1, &pbo_id);
     glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
     glBufferData(GL_PIXEL_PACK_BUFFER, buffer_size, nullptr, GL_STREAM_READ);

     // Create a framebuffer object and attach the texture
     auto fbo = Framebuffer(texture);

     // Request GPU to copy the texture pixels into the PBO
     glReadPixels(
       0,
       0,
       static_cast<GLsizei>(texture.getSize().x),
       static_cast<GLsizei>(texture.getSize().y),
       GL_RGBA,
       GL_UNSIGNED_BYTE,
       nullptr// Offset into PBO
     );

     // Unbind the PBO after issuing the readback
     glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

     // Return a deferred future to perform the CPU-side readback later
     return std::async(
       std::launch::deferred,
       [pbo_id, buffer_size, texture_size]([[maybe_unused]] Framebuffer fbo_temp) -> sf::Image {
#ifdef __cpp_lib_smart_ptr_for_overwrite
            const auto pixels = std::make_unique_for_overwrite<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
#else
            const auto pixels = std::make_unique<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
#endif

            // Bind PBO and retrieve pixel data
            glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
            glGetBufferSubData(GL_PIXEL_PACK_BUFFER, 0, buffer_size, pixels.get());
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

            // Clean up resources
            fbo_temp.destory();
            glDeleteBuffers(1, &pbo_id);

            // Create an image from retrieved pixel data
            sf::Image image;
            image.create(texture_size.x, texture_size.y, pixels.get());

            // Flip image vertically because OpenGL origin is bottom-left
            image.flipVertically();

            return image;
       },
       std::move(fbo)// Pass the framebuffer into the lambda
     );
}