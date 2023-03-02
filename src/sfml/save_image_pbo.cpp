//
// Created by pcvii on 2/27/2023.
//
#include "save_image_pbo.hpp"
#include "scope_guard.hpp"
#include <GL/glew.h>
#include <spdlog/spdlog.h>
#include <version>
[[nodiscard]] auto backup_frame_buffer()
{
     // Save the current framebuffer
     GLint current_framebuffer = 0;
     glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current_framebuffer);
     return scope_guard([=]() {
          // Restore the previous framebuffer
          glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(current_framebuffer));
     });
}
class [[nodiscard]] Framebuffer
{
     GLuint id;

     GLuint color_attachment_id;

   public:
     explicit Framebuffer(const sf::Texture &texture)
     {
          // create a new texture object
          glGenTextures(1, &color_attachment_id);

          // bind the new texture object
          glBindTexture(GL_TEXTURE_2D, color_attachment_id);

          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

          // allocate memory for the new texture
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

          // Create a framebuffer object
          glCreateFramebuffers(1, &id);
          glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
          // bind the new texture to the framebuffer
          glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment_id, 0);

          // Check for framebuffer completeness
          if (GLenum const status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE)
          {
               spdlog::warn("Framebuffer is incomplete (status = {})", status);
               // Handle the error here, e.g. by logging an error message or throwing an exception
          }

          GLuint tmp;
          glCreateFramebuffers(1, &tmp);
          glBindFramebuffer(GL_READ_FRAMEBUFFER, tmp);
          // bind the original texture
          glBindTexture(GL_TEXTURE_2D, texture.getNativeHandle());
          // bind the new texture to the framebuffer
          glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.getNativeHandle(), 0);

          // Check for framebuffer completeness
          if (GLenum const status = glCheckFramebufferStatus(GL_READ_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE)
          {
               spdlog::warn("Framebuffer is incomplete (status = {})", status);
               // Handle the error here, e.g. by logging an error message or throwing an exception
          }

          // copy the data from the original texture to the new texture// Set up the blit parameters
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
          glBindTexture(GL_TEXTURE_2D, 0);
          // Unbind the textures from their framebuffer attachments
          glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
          glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);
          glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
          glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
          glDeleteFramebuffers(1, &tmp);

          bind();
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment_id, 0);
          // Check for framebuffer completeness
          if (GLenum const status = glCheckFramebufferStatus(GL_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE)
          {
               spdlog::warn("Framebuffer is incomplete (status = {})", status);
               // Handle the error here, e.g. by logging an error message or throwing an exception
          }
     }
     explicit Framebuffer(GLuint in_id, GLuint in_color_attachment_id) noexcept
       : id(in_id)
       , color_attachment_id(in_color_attachment_id)
     {
     }

     [[nodiscard]] operator GLuint() const noexcept
     {
          return id;
     }

     Framebuffer(const Framebuffer &)            = delete;
     Framebuffer &operator=(const Framebuffer &) = delete;

     Framebuffer(Framebuffer &&other) noexcept
       : id(other.id)
       , color_attachment_id(other.color_attachment_id)
     {
          other.id                  = 0;
          other.color_attachment_id = 0;
     }

     Framebuffer &operator=(Framebuffer &&other) noexcept
     {
          if (this != &other)
          {
               id       = other.id;
               other.id = 0;
          }
          return *this;
     }
     static void bind(GLuint in_id)
     {
          glBindFramebuffer(GL_FRAMEBUFFER, in_id);
     }
     void bind() const
     {
          glBindFramebuffer(GL_FRAMEBUFFER, id);
     }

     static void unbind()
     {
          glBindFramebuffer(GL_FRAMEBUFFER, 0);
     }
     ~Framebuffer()
     {
          destory();
     }
     void destory()
     {
          if (id != 0)
          {
               // get the handle to the currently bound framebuffer
               GLint currentFramebuffer;
               glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFramebuffer);
               if (std::cmp_equal(currentFramebuffer, id))
               {
                    // unbind the framebuffer
                    unbind();
               }
               // delete the framebuffer
               glDeleteFramebuffers(1, &id);

               // delete the color attachment texture
               glDeleteTextures(1, &color_attachment_id);
               id                  = 0;
               color_attachment_id = 0;
          }
     }
};
std::future<sf::Image> save_image_pbo(const sf::Texture &texture)
{
     const auto texture_size = texture.getSize();
     const auto backup_fbo   = backup_frame_buffer();
     const auto buffer_size  = GLsizeiptr{ texture.getSize().x } * GLsizeiptr{ texture.getSize().y } * 4;
     auto       pbo_id       = 0U;
     glGenBuffers(1, &pbo_id);
     glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
     glBufferData(GL_PIXEL_PACK_BUFFER, buffer_size, nullptr, GL_STREAM_READ);

     auto fbo = Framebuffer(texture);
     // Issue a request to the GPU to copy the texture to the PBO
     // glReadBuffer(GL_FRONT);
     // glNamedFramebufferReadBuffer(fbo, GL_FRONT);
     glReadPixels(
       0, 0, static_cast<GLsizei>(texture.getSize().x), static_cast<GLsizei>(texture.getSize().y), GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

     glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
     // Bind the PBO to a future that will be returned
     return std::async(
       std::launch::deferred,
       [pbo_id, buffer_size, texture_size]([[maybe_unused]] Framebuffer fbo_temp) -> sf::Image {

#ifdef __cpp_lib_smart_ptr_for_overwrite
            const auto pixels = std::make_unique_for_overwrite<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
#else
            const auto pixels = std::make_unique<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
#endif

            glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
            glGetBufferSubData(GL_PIXEL_PACK_BUFFER, 0, buffer_size, pixels.get());
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
            fbo_temp.destory();
            glDeleteBuffers(1, &pbo_id);
            sf::Image image;
            image.create(texture_size.x, texture_size.y, pixels.get());
            image.flipVertically();

            return image;
       },
       std::move(fbo));
}