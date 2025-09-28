//
// Created by pcvii on 12/15/2021.
//

#ifndef FIELD_MAP_EDITOR_FRAMEBUFFER_HPP
#define FIELD_MAP_EDITOR_FRAMEBUFFER_HPP
#include "FrameBufferBackup.hpp"
#include "Renderer.hpp"
#include "SubTexture.hpp"
#include "UniqueValue.hpp"
#include <algorithm>
#include <variant>
namespace glengine
{
enum class FrameBufferTextureFormat
{
     None,
     RGBA8,
     RGBA8UI,
     RED_INTEGER,
};
struct FrameBufferSpecification
{
     std::array<FrameBufferTextureFormat, 4U> attachments
       = { FrameBufferTextureFormat::RGBA8,
           FrameBufferTextureFormat::RGBA8,
           FrameBufferTextureFormat::RED_INTEGER,
           {} };
     int                      width  = {};
     int                      height = {};
     int                      scale  = { 1 };
     // uint32_t samples           = { 1 };
     //  bool     swap_chain_target = { false };
     FrameBufferSpecification resize(
       int in_width,
       int in_height) const
     {
          auto ret   = *this;
          ret.width  = in_width;
          ret.height = in_height;
          return ret;
     }
};
class FrameBuffer
{
   public:
     using Pixel   = std::variant<std::monostate, int, std::array<uint8_t, 4>>;
     FrameBuffer() = default;
     FrameBuffer(FrameBufferSpecification spec);
     /**
      * @brief Binds the framebuffer to both the read and draw framebuffer
      * targets.
      *
      * This is a convenience method that binds the framebuffer to
      * `GL_FRAMEBUFFER`, which affects both `GL_READ_FRAMEBUFFER` and
      * `GL_DRAW_FRAMEBUFFER`.
      *
      * @param first If true, binds the `m_renderer_id_first`; otherwise, binds
      * `m_renderer_id`.
      */
     void                  bind(bool first = false) const;

     /**
      * @brief Binds the framebuffer to the OpenGL read framebuffer target.
      *
      * This is used when reading from this framebuffer, such as during blitting
      * operations.
      *
      * @param first If true, binds the `m_renderer_id_first`; otherwise, binds
      * `m_renderer_id`.
      */
     void                  bind_read(bool first = false) const;

     /**
      * @brief Binds the framebuffer to the OpenGL draw framebuffer target.
      *
      * This is used when writing (drawing) to this framebuffer, such as in
      * rendering passes or when receiving blitted content.
      *
      * @param first If true, binds the `m_renderer_id_first`; otherwise, binds
      * `m_renderer_id`.
      */
     void                  bind_draw(bool first = false) const;

     constexpr static void unbind()
     {
          if (!std::is_constant_evaluated())
          {
               GlCall{}(glBindFramebuffer, GL_FRAMEBUFFER, 0);
          }
     }
     static FrameBufferBackup                      backup();
     [[nodiscard]] const FrameBufferSpecification &specification() const;
     SubTexture bind_color_attachment(std::uint32_t index = 0U) const;
     [[nodiscard]] GlidCopy color_attachment_id(std::uint32_t index = 0) const;
     [[nodiscard]] Texture  steal_color_attachment_id(std::uint32_t index);
     /**
      * @brief Creates a deep copy of this FrameBuffer, including GPU resources.
      *
      * This performs a full framebuffer blit of color attachments with format
      * RGBA8. Attachments using RED_INTEGER or other unsupported formats will
      * be skipped.
      *
      * @note Depth/stencil attachments are not copied.
      * @note RED_INTEGER attachments are skipped intentionally. If needed in
      * the future, they can be copied manually via glReadPixels +
      * glTexSubImage2D.
      *
      * @return A new FrameBuffer instance that is a deep copy of this one.
      */
     [[nodiscard]] FrameBuffer clone() const;
                               operator bool()
     {
          return std::ranges::any_of(
                   m_color_attachment, [](const auto &id) { return id != 0U; })
                 && m_renderer_id != 0U && m_depth_attachment != 0U;
     }
     [[nodiscard]] glm::ivec2 get_size() const;
     [[nodiscard]] int        width() const;
     [[nodiscard]] int        height() const;
     [[nodiscard]] int        scale() const;
     [[nodiscard]] int       &mutable_scale();
     void                     set_scale(int);
     [[nodiscard]] Pixel      read_pixel(
            uint32_t attachment_index,
            int      x,
            int      y) const;

     void clear_non_standard_color_attachments() const
     {
          for (uint8_t i{}; i != 4U; ++i)
          {
               switch (m_specification.attachments[i])
               {
                    case FrameBufferTextureFormat::RGBA8UI:
                    {
                         static constexpr const std::array<GLuint, 4> clearValue
                           = {};
                         GlCall{}(
                           glClearTexImage,
                           m_color_attachment[1],
                           0,
                           GL_RGBA_INTEGER,
                           GL_UNSIGNED_INT,
                           clearValue.data());
                         break;
                    }
                    case FrameBufferTextureFormat::RED_INTEGER:
                    {
                         static constexpr const int value = -1;
                         GlCall{}(
                           glClearTexImage,
                           m_color_attachment[i],
                           0,
                           GL_RED_INTEGER,
                           GL_INT,
                           &value);
                         break;
                    }
                    default:
                         // unhandled.
                         break;
               }
          }
     }

   private:
     FrameBufferSpecification m_specification     = {};
     std::array<Glid, 4U>     m_color_attachment  = {};
     Glid                     m_depth_attachment  = {};
     Glid                     m_renderer_id       = {};
     Glid                     m_renderer_id_first = {};
};
static_assert(Bindable<FrameBuffer>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_FRAMEBUFFER_HPP
