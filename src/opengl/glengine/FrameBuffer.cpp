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
                         GLCall{}(glGenTextures, 1, &tmp);
                         GLCall{}(glBindTexture, GL_TEXTURE_2D, tmp);
                         GLCall{}(
                           &glTexParameteri,
                           GL_TEXTURE_2D,
                           GL_TEXTURE_MAG_FILTER,
                           GL_NEAREST);
                         GLCall{}(
                           &glTexParameteri,
                           GL_TEXTURE_2D,
                           GL_TEXTURE_MIN_FILTER,
                           GL_NEAREST_MIPMAP_NEAREST);
                         GLCall{}(
                           &glTexParameteri,
                           GL_TEXTURE_2D,
                           GL_TEXTURE_WRAP_S,
                           GL_CLAMP_TO_BORDER);
                         GLCall{}(
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
                         GLCall{}(glGenerateMipmap, GL_TEXTURE_2D);
                         GLCall{}(glBindTexture, GL_TEXTURE_2D, 0);
                         return tmp;
                       }(),
                        Texture::Destroy }
  , m_depth_attachment{
    [this]() {
      std::uint32_t tmp{};
      GLCall{}(glGenTextures, 1, &tmp);
      GLCall{}(glBindTexture, GL_TEXTURE_2D, tmp);

      GLCall{}(
        &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      GLCall{}(
        &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      GLCall{}(
        &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      GLCall{}(
        &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      GLCall{}(
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
      GLCall{}(glBindTexture, GL_TEXTURE_2D, 0);
      return tmp;
    }(),
    Texture::Destroy
  }
{
  // Sometimes the textures wouldn't be defined before defining m_renderer_id
  // So I moved this code inside here.
  m_renderer_id = GLID(
    [this]() {
      std::uint32_t tmp{};
      GLCall{}(glCreateFramebuffers, 1, &tmp);
      GLCall{}(glBindFramebuffer, GL_FRAMEBUFFER, tmp);
      //        fmt::print(
      //          "m_color_attachment {}\n",
      //          static_cast<uint32_t>(m_color_attachment));
      GLCall{}(
        glFramebufferTexture2D,
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        m_color_attachment,
        0);
      GLCall{}(
        glFramebufferTexture2D,
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        m_depth_attachment,
        0);

      GLenum status = GLCall{}(glCheckFramebufferStatus, GL_FRAMEBUFFER);
      if (status != GL_FRAMEBUFFER_COMPLETE)
      {
        fmt::print(
          stderr,
          "Error {}:{} NOT GL_FRAMEBUFFER_COMPLETE - {}",
          __FILE__,
          __LINE__,
          status);
      }
      return tmp;
    }(),
    [](std::uint32_t id) {
      GLCall{}(glDeleteFramebuffers, 1, &id);
      UnBind();
    });
  UnBind();
}
void FrameBuffer::Bind() const
{
  GLCall{}(glBindFramebuffer, GL_FRAMEBUFFER, m_renderer_id);
}
const FrameBufferSpecification &FrameBuffer::Specification() const
{
  return m_specification;
}
SubTexture FrameBuffer::GetColorAttachment() const
{
  auto r = SubTexture(m_color_attachment);
  r.Bind();
  GLCall{}(glGenerateMipmap, GL_TEXTURE_2D);
  return r;
}
}// namespace glengine