//
// Created by pcvii on 12/15/2021.
//

#ifndef MYPROJECT_FRAMEBUFFER_HPP
#define MYPROJECT_FRAMEBUFFER_HPP
#include "Renderer.hpp"
#include "SubTexture.hpp"
#include "unique_value.hpp"
namespace glengine
{
struct FrameBufferSpecification
{
  int width  = {};
  int height = {};
  // uint32_t samples           = { 1 };
  // bool     swap_chain_target = { false };
};
class FrameBuffer
{
public:
  FrameBuffer() = default;
  FrameBuffer(FrameBufferSpecification spec);
  void                  Bind() const;
  constexpr static void UnBind()
  {
    if (!std::is_constant_evaluated())
    {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
  }
  const FrameBufferSpecification &Specification() const;
  SubTexture                      GetColorAttachment() const;

private:
  FrameBufferSpecification m_specification    = {};
  GLID                     m_color_attachment = {};
  GLID                     m_depth_attachment = {};
  GLID                     m_renderer_id      = {};
};
static_assert(Bindable<FrameBuffer>);
}// namespace glengine
#endif// MYPROJECT_FRAMEBUFFER_HPP
