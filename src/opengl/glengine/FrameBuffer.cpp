//
// Created by pcvii on 12/15/2021.
//

#include "FrameBuffer.hpp"
namespace glengine
{
static std::uint32_t AttachDepthTexture(const FrameBufferSpecification &spec)
{
  std::uint32_t tmp{};
  GlCall{}(glGenTextures, 1, &tmp);
  GlCall{}(glBindTexture, GL_TEXTURE_2D, tmp);

  GlCall{}(&glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  GlCall{}(&glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  GlCall{}(
    &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  GlCall{}(
    &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  GlCall{}(
    glTexImage2D,
    GL_TEXTURE_2D,
    0,
    GL_DEPTH_COMPONENT,
    spec.width,
    spec.height,
    0,
    GL_DEPTH_COMPONENT,
    GL_UNSIGNED_BYTE,
    nullptr);
  Texture::unbind();
  return tmp;
}

static std::uint32_t AttachColorTexture(const FrameBufferSpecification &spec)
{
  std::uint32_t tmp{};
  GlCall{}(glGenTextures, 1, &tmp);
  GlCall{}(glBindTexture, GL_TEXTURE_2D, tmp);
  GlCall{}(&glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  GlCall{}(
    &glTexParameteri,
    GL_TEXTURE_2D,
    GL_TEXTURE_MIN_FILTER,
    GL_NEAREST_MIPMAP_NEAREST);
  GlCall{}(
    &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  GlCall{}(
    &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA8,
    spec.width,
    spec.height,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    nullptr);
  GlCall{}(glGenerateMipmap, GL_TEXTURE_2D);
  Texture::unbind();
  return tmp;
}
static std::uint32_t GenerateFramebuffer(
  const std::array<Glid, 4U> &color_attachments,
  const Glid                 &depth_attachment)
{
  static constexpr auto attachments =
    std::array<GLenum, 4>{ GL_COLOR_ATTACHMENT0,
                           GL_COLOR_ATTACHMENT1,
                           GL_COLOR_ATTACHMENT2,
                           GL_COLOR_ATTACHMENT3 };

  std::uint32_t tmp{};
  GlCall{}(glCreateFramebuffers, 1, &tmp);
  GlCall{}(glBindFramebuffer, GL_FRAMEBUFFER, tmp);
  //        spdlog::debug(
  //          "m_color_attachment {}\n",
  //          static_cast<uint32_t>(m_color_attachment));

  for (std::uint8_t i{}; i != 4U; ++i)
  {
    if (color_attachments[i] == 0U)
      break;
    GlCall{}(
      glFramebufferTexture2D,
      GL_FRAMEBUFFER,
      attachments[i],
      GL_TEXTURE_2D,
      color_attachments[i],
      0);
  }
  GlCall{}(
    glFramebufferTexture2D,
    GL_FRAMEBUFFER,
    GL_DEPTH_ATTACHMENT,
    GL_TEXTURE_2D,
    depth_attachment,
    0);

  GLenum status = GlCall{}(glCheckFramebufferStatus, GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
  {
    spdlog::critical(
      "{}:{} NOT GL_FRAMEBUFFER_COMPLETE - {}", __FILE__, __LINE__, status);
  }
  return tmp;
}
FrameBuffer::FrameBuffer(FrameBufferSpecification spec)
  : m_specification{ std::move(spec) }
  , m_color_attachment{ Glid{ AttachColorTexture(m_specification),
                              Texture::destroy },
                        {},
                        {},
                        {} }
  , m_depth_attachment{ AttachDepthTexture(m_specification), Texture::destroy }
{
  // Sometimes the textures wouldn't be defined before defining m_renderer_id
  // So I moved this code inside here.
  m_renderer_id =
    Glid{ GenerateFramebuffer(m_color_attachment, m_depth_attachment),
          [](std::uint32_t id) {
            GlCall{}(glDeleteFramebuffers, 1, &id);
            FrameBuffer::unbind();
          } };
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
  auto r = SubTexture(m_color_attachment.front());
  r.bind();
  GlCall{}(glGenerateMipmap, GL_TEXTURE_2D);
  return r;
}
}// namespace glengine