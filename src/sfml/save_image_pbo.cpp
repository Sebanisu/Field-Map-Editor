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

   public:
     explicit Framebuffer(GLuint in_id) noexcept
       : id(in_id)
     {
     }

     [[nodiscard]] explicit operator GLuint() const noexcept
     {
          return id;
     }

     Framebuffer(const Framebuffer &)            = default;
     Framebuffer &operator=(const Framebuffer &) = delete;

     Framebuffer(Framebuffer &&other) noexcept
       : id(other.id)
     {
          other.id = 0;
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
          if (id != 0)
          {
               glDeleteFramebuffers(1, &id);
               // Bind the default framebuffer
          }
     }
};
[[nodiscard]] Framebuffer write_texture_to_front_buffer(const sf::Texture &texture)
{
     // Create a framebuffer object
     GLuint fbo = 0U;
     glGenFramebuffers(1, &fbo);
     glBindFramebuffer(GL_FRAMEBUFFER, fbo);

     // Attach the texture to the framebuffer
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.getNativeHandle(), 0);

     // Check for framebuffer completeness
     GLenum const status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
     if (status != GL_FRAMEBUFFER_COMPLETE)
     {
          spdlog::warn("Framebuffer is incomplete (status = {})", status);
          // Handle the error here, e.g. by logging an error message or throwing an exception
     }
     return Framebuffer{ fbo };
}
sf::Image save_image_pbo(const sf::Texture &texture)
{
     const auto texture_size = texture.getSize();
     const auto backup_fbo   = backup_frame_buffer();
     const auto buffer_size  = GLsizeiptr{ texture.getSize().x } * GLsizeiptr{ texture.getSize().y } * 4;
     auto       pbo_id       = 0U;
     glGenBuffers(1, &pbo_id);
     glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
     glBufferData(GL_PIXEL_PACK_BUFFER, buffer_size, nullptr, GL_STREAM_READ);

     auto fbo = write_texture_to_front_buffer(texture);
     // Issue a request to the GPU to copy the texture to the PBO
     glReadBuffer(GL_FRONT);
     glReadPixels(
       0, 0, static_cast<GLsizei>(texture.getSize().x), static_cast<GLsizei>(texture.getSize().y), GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

     glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
     // Bind the PBO to a future that will be returned
     std::invoke(backup_fbo);

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
}