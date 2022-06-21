//
// Created by pcvii on 12/15/2021.
//

#include "FrameBuffer.hpp"
namespace glengine
{
FrameBuffer::FrameBuffer(FrameBufferSpecification spec)
  : m_specification{ std::move(spec) }
  , m_color_attachment{ [this]() {
                         std::uint32_t tmp{};
                         GlCall{}(glGenTextures, 1, &tmp);
                         GlCall{}(glBindTexture, GL_TEXTURE_2D, tmp);
                         GlCall{}(
                           &glTexParameteri,
                           GL_TEXTURE_2D,
                           GL_TEXTURE_MAG_FILTER,
                           GL_NEAREST);
                         GlCall{}(
                           &glTexParameteri,
                           GL_TEXTURE_2D,
                           GL_TEXTURE_MIN_FILTER,
                           GL_NEAREST_MIPMAP_NEAREST);
                         GlCall{}(
                           &glTexParameteri,
                           GL_TEXTURE_2D,
                           GL_TEXTURE_WRAP_S,
                           GL_CLAMP_TO_BORDER);
                         GlCall{}(
                           &glTexParameteri,
                           GL_TEXTURE_2D,
                           GL_TEXTURE_WRAP_T,
                           GL_CLAMP_TO_BORDER);

                         glTexImage2D(
                           GL_TEXTURE_2D,
                           0,
                           GL_RGBA8,
                           m_specification.width,
                           m_specification.height,
                           0,
                           GL_RGBA,
                           GL_UNSIGNED_BYTE,
                           nullptr);
                         GlCall{}(glGenerateMipmap, GL_TEXTURE_2D);
                         GlCall{}(glBindTexture, GL_TEXTURE_2D, 0);
                         return tmp;
                       }(),
                        Texture::destroy }
  , m_depth_attachment{
    [this]() {
      std::uint32_t tmp{};
      GlCall{}(glGenTextures, 1, &tmp);
      GlCall{}(glBindTexture, GL_TEXTURE_2D, tmp);

      GlCall{}(
        &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      GlCall{}(
        &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      GlCall{}(
        &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      GlCall{}(
        &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      GlCall{}(
        glTexImage2D,
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT,
        m_specification.width,
        m_specification.height,
        0,
        GL_DEPTH_COMPONENT,
        GL_UNSIGNED_BYTE,
        nullptr);
      GlCall{}(glBindTexture, GL_TEXTURE_2D, 0);
      return tmp;
    }(),
    Texture::destroy
  }
{
  // Sometimes the textures wouldn't be defined before defining m_renderer_id
  // So I moved this code inside here.
  m_renderer_id = Glid(
    [this]() {
      std::uint32_t tmp{};
      GlCall{}(glCreateFramebuffers, 1, &tmp);
      GlCall{}(glBindFramebuffer, GL_FRAMEBUFFER, tmp);
      //        spdlog::debug(
      //          "m_color_attachment {}\n",
      //          static_cast<uint32_t>(m_color_attachment));
      GlCall{}(
        glFramebufferTexture2D,
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        m_color_attachment,
        0);
      GlCall{}(
        glFramebufferTexture2D,
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        m_depth_attachment,
        0);

      GLenum status = GlCall{}(glCheckFramebufferStatus, GL_FRAMEBUFFER);
      if (status != GL_FRAMEBUFFER_COMPLETE)
      {
        spdlog::critical(
          "{}:{} NOT GL_FRAMEBUFFER_COMPLETE - {}", __FILE__, __LINE__, status);
      }
      return tmp;
    }(),
    [](std::uint32_t id) {
      GlCall{}(glDeleteFramebuffers, 1, &id);
      unbind();
    });
  unbind();
}
void FrameBuffer::bind() const
{
  GlCall{}(glBindFramebuffer, GL_FRAMEBUFFER, m_renderer_id);
}
const FrameBufferSpecification &FrameBuffer::specification() const
{
  return m_specification;
}
SubTexture FrameBuffer::get_color_attachment() const
{
  // called here to update mipmaps after texture changed.
  auto r = SubTexture(m_color_attachment);
  r.bind();
  GlCall{}(glGenerateMipmap, GL_TEXTURE_2D);
  return r;
}
}// namespace glengine