//
// Created by pcvii on 12/15/2021.
//

#include "FrameBuffer.hpp"
#include <spdlog/spdlog.h>
namespace glengine
{
static constexpr auto attachments =
  std::array<GLenum, 4>{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

static std::uint32_t AttachDepthTexture(const FrameBufferSpecification &spec)
{
     std::uint32_t tmp{};
     GlCall{}(glGenTextures, 1, &tmp);
     GlCall{}(glBindTexture, GL_TEXTURE_2D, tmp);

     GlCall{}(&glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     GlCall{}(&glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     GlCall{}(&glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
     GlCall{}(&glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
     GlCall{}(
       glTexImage2D, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, spec.width, spec.height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
     Texture::unbind();
     return tmp;
}

static std::uint32_t AttachColorTexture(int width, int height, GLint internal_format, GLenum format, GLenum type = GL_UNSIGNED_BYTE)
{
     std::uint32_t tmp{};
     GlCall{}(glGenTextures, 1, &tmp);
     GlCall{}(glBindTexture, GL_TEXTURE_2D, tmp);
     GlCall{}(&glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     GlCall{}(&glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (type == GL_INT) ? GL_NEAREST : GL_NEAREST_MIPMAP_NEAREST);
     GlCall{}(&glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
     GlCall{}(&glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


     GlCall{}(glTexImage2D, GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, nullptr);
     if (type != GL_INT)
     {
          GlCall{}(glGenerateMipmap, GL_TEXTURE_2D);
     }
     Texture::unbind();
     return tmp;
}

static std::uint32_t
  GenerateFramebuffer(const std::array<Glid, 4U> &color_attachments, [[maybe_unused]] const Glid &depth_attachment, bool first = false)
{

     std::uint32_t tmp{};
     GlCall{}(glCreateFramebuffers, 1, &tmp);
     GlCall{}(glBindFramebuffer, GL_FRAMEBUFFER, tmp);
     //        spdlog::debug(
     //          "m_color_attachment {}\n",
     //          static_cast<uint32_t>(m_color_attachment));
     std::uint8_t num_attachments{};
     if (first)
     {
          GlCall{}(
            glFramebufferTexture2D, GL_FRAMEBUFFER, attachments[num_attachments], GL_TEXTURE_2D, color_attachments[num_attachments], 0);
          ++num_attachments;
     }
     else
     {
          for (; num_attachments != 4U; ++num_attachments)
          {
               if (color_attachments[num_attachments] == 0U)
               {
                    break;
               }
               GlCall{}(
                 glFramebufferTexture2D,
                 GL_FRAMEBUFFER,
                 attachments[num_attachments],
                 GL_TEXTURE_2D,
                 color_attachments[num_attachments],
                 0);
          }
          GlCall{}(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_attachment, 0);
     }

     GLenum status = GlCall{}(glCheckFramebufferStatus, GL_FRAMEBUFFER);
     if (status != GL_FRAMEBUFFER_COMPLETE)
     {
          spdlog::critical("{}:{} NOT GL_FRAMEBUFFER_COMPLETE - {}", __FILE__, __LINE__, status);
     }
     glDrawBuffers(num_attachments, attachments.data());
     return tmp;
}

static decltype(auto) GenerateColorAttachments(const FrameBufferSpecification &spec)
{
     auto convert = [&spec](FrameBufferTextureFormat format) -> decltype(auto) {
          switch (format)
          {
               case FrameBufferTextureFormat::RGBA8: {
                    return Glid{ AttachColorTexture(spec.width, spec.height, GL_RGBA8, GL_RGBA), Texture::destroy };
               }
               case FrameBufferTextureFormat::RGBA8UI: {
                    return Glid{ AttachColorTexture(
                                   spec.width,
                                   spec.height,
                                   GL_RGBA8UI,// internal format: 8-bit unsigned int RGBA
                                   GL_RGBA_INTEGER,// format: integer RGBA
                                   GL_UNSIGNED_BYTE// type: unsigned byte per channel
                                   ),
                                 Texture::destroy };
               }
               case FrameBufferTextureFormat::RED_INTEGER: {
                    return Glid{ AttachColorTexture(spec.width, spec.height, GL_R32I, GL_RED_INTEGER, GL_INT), Texture::destroy };
               }
               case FrameBufferTextureFormat::None:
                    break;
          }
          return Glid{};
     };
     return std::array<Glid, 4U>{
          convert(spec.attachments[0]), convert(spec.attachments[1]), convert(spec.attachments[2]), convert(spec.attachments[3])
     };
}

FrameBuffer::FrameBuffer(FrameBufferSpecification spec)
  : m_specification{ std::move(spec) }
  , m_color_attachment{ GenerateColorAttachments(spec) }
  , m_depth_attachment{ AttachDepthTexture(m_specification), Texture::destroy }
{
     // Sometimes the textures wouldn't be defined before defining m_renderer_id
     // So I moved this code inside here.
     m_renderer_id = Glid{ GenerateFramebuffer(m_color_attachment, m_depth_attachment), [](std::uint32_t id) {
                               GlCall{}(glDeleteFramebuffers, 1, &id);
                               FrameBuffer::unbind();
                          } };
     m_renderer_id_first = Glid{ GenerateFramebuffer(m_color_attachment, m_depth_attachment, true), [](std::uint32_t id) {
                                     GlCall{}(glDeleteFramebuffers, 1, &id);
                                     FrameBuffer::unbind();
                                } };
     unbind();
}

void FrameBuffer::bind(bool first) const
{
     GlCall{}(glBindFramebuffer, GL_FRAMEBUFFER, first ? m_renderer_id_first : m_renderer_id);
     spdlog::trace(
       "Binding framebuffer: {}, ID: {}",
       first ? "m_renderer_id_first" : "m_renderer_id",
       first ? static_cast<uint32_t>(m_renderer_id_first) : static_cast<uint32_t>(m_renderer_id));
}

void FrameBuffer::bind_read(bool first) const
{
     GlCall{}(glBindFramebuffer, GL_READ_FRAMEBUFFER, first ? m_renderer_id_first : m_renderer_id);
}

void FrameBuffer::bind_draw(bool first) const
{
     GlCall{}(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, first ? m_renderer_id_first : m_renderer_id);
}

FrameBufferBackup FrameBuffer::backup()
{
     return {};
}

const FrameBufferSpecification &FrameBuffer::specification() const
{
     return m_specification;
}

SubTexture FrameBuffer::bind_color_attachment(std::uint32_t index) const
{
     assert(index < 4U);
     assert(m_color_attachment[index] != 0U);
     // called here to update mipmaps after texture changed.
     auto r = SubTexture(m_color_attachment[index]);
     r.bind();
     switch (m_specification.attachments[index])
     {
          case FrameBufferTextureFormat::RGBA8: {
               GlCall{}(glGenerateMipmap, GL_TEXTURE_2D);
               break;
          }
          default:
               break;
     }
     return r;
}

GlidCopy FrameBuffer::color_attachment_id(std::uint32_t index) const
{
     assert(index < 4U);
     //     assert(m_color_attachment[index] != 0U);
     return m_color_attachment[index];
}


Texture FrameBuffer::steal_color_attachment_id(std::uint32_t index)
{
     assert(index < 4U);
     //     assert(m_color_attachment[index] != 0U);
     return { std::move(m_color_attachment[index]), width(), height() };
}

FrameBuffer FrameBuffer::clone() const
{
     const auto  backup_fbo = backup();

     FrameBuffer copy(m_specification);

     // Bind this FBO for reading
     this->bind_read();

     // Bind copy FBO for drawing
     copy.bind_draw();

     // Blit COLOR_ATTACHMENT0
     GlCall{}(glReadBuffer, GL_COLOR_ATTACHMENT0);
     GlCall{}(glDrawBuffer, GL_COLOR_ATTACHMENT0);
     GlCall{}(
       glBlitFramebuffer,
       0,
       0,
       m_specification.width,
       m_specification.height,
       0,
       0,
       m_specification.width,
       m_specification.height,
       GL_COLOR_BUFFER_BIT,
       GL_NEAREST);

     // Blit COLOR_ATTACHMENT1
     GlCall{}(glReadBuffer, GL_COLOR_ATTACHMENT1);
     GlCall{}(glDrawBuffer, GL_COLOR_ATTACHMENT1);
     GlCall{}(
       glBlitFramebuffer,
       0,
       0,
       m_specification.width,
       m_specification.height,
       0,
       0,
       m_specification.width,
       m_specification.height,
       GL_COLOR_BUFFER_BIT,
       GL_NEAREST);
     (void)copy.bind_color_attachment(0);// generates mipmaps and binds the texture.
     (void)copy.bind_color_attachment(1);// generates mipmaps and binds the texture.
     return copy;
}

glm::ivec2 FrameBuffer::get_size() const
{
     return { m_specification.width, m_specification.height };
}

int FrameBuffer::width() const
{
     return m_specification.width;
}

int FrameBuffer::height() const
{
     return m_specification.height;
}

int &FrameBuffer::mutable_scale()
{
     return m_specification.scale;
}

int FrameBuffer::scale() const
{
     return m_specification.scale;
}

void FrameBuffer::set_scale(int in_scale)
{
     assert(in_scale > 0);
     m_specification.scale = in_scale;
     spdlog::trace("scale updated: {}", m_specification.scale);
}

FrameBuffer::Pixel FrameBuffer::read_pixel(uint32_t attachment_index, int x, int y) const
{
     if (attachment_index >= 4 || m_color_attachment[attachment_index] == 0)
          return std::monostate{};// invalid attachment
     switch (m_specification.attachments[attachment_index])
     {
          case FrameBufferTextureFormat::RGBA8: {
               std::array<uint8_t, 4> pixel_data = { 0 };
               GlCall{}(glReadBuffer, attachments[attachment_index]);
               GlCall{}(glReadPixels, x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel_data);
               return pixel_data;
          }
          case FrameBufferTextureFormat::RGBA8UI: {
               std::array<uint8_t, 4> pixel_data = { 0 };
               GlCall{}(glReadBuffer, attachments[attachment_index]);
               GlCall{}(glReadPixels, x, y, 1, 1, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, &pixel_data);
               return pixel_data;
          }
          case FrameBufferTextureFormat::RED_INTEGER: {
               int pixel_data = 0;
               GlCall{}(glReadBuffer, attachments[attachment_index]);
               GlCall{}(glReadPixels, x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel_data);
               break;
          }
          case FrameBufferTextureFormat::None: {
               return std::monostate{};// invalid attachment
          }
     }
     return std::monostate{};// invalid attachment
}
}// namespace glengine