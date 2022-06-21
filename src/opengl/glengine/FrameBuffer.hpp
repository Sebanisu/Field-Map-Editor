//
// Created by pcvii on 12/15/2021.
//

#ifndef FIELD_MAP_EDITOR_FRAMEBUFFER_HPP
#define FIELD_MAP_EDITOR_FRAMEBUFFER_HPP
#include "Renderer.hpp"
#include "SubTexture.hpp"
#include "UniqueValue.hpp"
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
  void                  bind() const;
  constexpr static void unbind()
  {
    if (!std::is_constant_evaluated())
    {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
  }
  const FrameBufferSpecification &specification() const;
  SubTexture                      get_color_attachment() const;
                                  operator bool()
  {
    return m_color_attachment != 0U && m_renderer_id != 0U
           && m_depth_attachment != 0U;
  }

private:
  FrameBufferSpecification m_specification    = {};
  Glid                     m_color_attachment = {};
  Glid                     m_depth_attachment = {};
  Glid                     m_renderer_id      = {};
};
static_assert(Bindable<FrameBuffer>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_FRAMEBUFFER_HPP
