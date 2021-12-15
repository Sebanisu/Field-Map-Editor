//
// Created by pcvii on 12/15/2021.
//

#ifndef MYPROJECT_FRAMEBUFFER_HPP
#define MYPROJECT_FRAMEBUFFER_HPP
#include "Renderer.hpp"
#include "unique_value.hpp"
#include "SubTexture.hpp"
struct FrameBufferSpecification
{
  int      width             = {};
  int      height            = {};
  //uint32_t samples           = { 1 };
  //bool     swap_chain_target = { false };
};
class FrameBuffer
{
public:
  FrameBuffer() = default;
  FrameBuffer(FrameBufferSpecification spec)
    : m_specification(std::move(spec))
    , m_color_attachment(
        [this]() {
          std::uint32_t tmp{};
          GLCall{}(glCreateTextures, GL_TEXTURE_2D, 1, &tmp);
          GLCall{}(glBindTexture, GL_TEXTURE_2D, tmp);
          GLCall{}(
            glTexImage2D,
            GL_TEXTURE_2D,
            0,
            GL_RGB8,
            m_specification.width,
            m_specification.height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr);

          GLCall{}(
            &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          GLCall{}(
            &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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
          GLCall{}(glBindTexture, GL_TEXTURE_2D, 0);
          return tmp;
        }(),
        [](std::uint32_t id) { GLCall{}(glDeleteTextures, 1, &id); })
    , m_depth_attachment(
        [this]() {
          std::uint32_t tmp{};
          GLCall{}(glCreateTextures, GL_TEXTURE_2D, 1, &tmp);
          GLCall{}(glBindTexture, GL_TEXTURE_2D, tmp);
          GLCall{}(
            glTexImage2D,
            GL_TEXTURE_2D,
            0,
            GL_DEPTH24_STENCIL8,
            m_specification.width,
            m_specification.height,
            0,
            GL_DEPTH_STENCIL,
            GL_UNSIGNED_INT_24_8,
            nullptr);

          GLCall{}(
            &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          GLCall{}(
            &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          GLCall{}(
            &glTexParameteri,
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_S,
            GL_CLAMP_TO_EDGE);
          GLCall{}(
            &glTexParameteri,
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_T,
            GL_CLAMP_TO_EDGE);
          GLCall{}(glBindTexture, GL_TEXTURE_2D, 0);
          return tmp;
        }(),
        [](std::uint32_t id) { GLCall{}(glDeleteTextures, 1, &id); })
    , m_renderer_id(
        [this]() {
          std::uint32_t tmp{};
          GLCall{}(glCreateFramebuffers, 1, &tmp);
          GLCall{}(glBindFramebuffer, GL_FRAMEBUFFER, tmp);
          glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            m_color_attachment,
            0);
          glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_TEXTURE_2D,
            m_depth_attachment,
            0);

          GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
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
        })
  {
    UnBind();
  }
  void Bind() const
  {
    GLCall{}(glBindFramebuffer, GL_FRAMEBUFFER, m_renderer_id);
  }
  static void UnBind()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  const FrameBufferSpecification &Specification() const
  {
    return m_specification;
  }
SubTexture GetColorAttachment() const {
  return SubTexture(m_depth_attachment);
}
private:
  FrameBufferSpecification m_specification    = {};
  GLID                     m_color_attachment = {};
  GLID                     m_depth_attachment = {};
  GLID                     m_renderer_id      = {};
};
static_assert(Bindable<FrameBuffer>);
#endif// MYPROJECT_FRAMEBUFFER_HPP
