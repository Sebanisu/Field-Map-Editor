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
enum class FrameBufferTextureFormat
{
  None,
  RGBA8,
  RED_INTEGER,
};
struct FrameBufferSpecification
{
  std::array<FrameBufferTextureFormat, 4U> attachments = {
    FrameBufferTextureFormat::RGBA8,
    FrameBufferTextureFormat::RED_INTEGER,
    {},
    {}
  };
  int                      width  = {};
  int                      height = {};
  // uint32_t samples           = { 1 };
  //  bool     swap_chain_target = { false };
  FrameBufferSpecification resize(int in_width, int in_height) const
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
  FrameBuffer() = default;
  FrameBuffer(FrameBufferSpecification spec);
  void                  bind() const;
  constexpr static void unbind()
  {
    if (!std::is_constant_evaluated())
    {
      GlCall{}(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    }
  }
  const FrameBufferSpecification &specification() const;
  SubTexture get_color_attachment(std::uint32_t index = 0U) const;
             operator bool()
  {
    return std::ranges::any_of(
             m_color_attachment, [](const auto &id) { return id != 0U; })
           && m_renderer_id != 0U && m_depth_attachment != 0U;
  }
  int  read_pixel(uint32_t attachment_index, int x, int y) const;

  void clear_red_integer_color_attachment() const
  {
    for (uint8_t i{}; i != 4U; ++i)
    {
      if (
        m_specification.attachments[i] != FrameBufferTextureFormat::RED_INTEGER)
      {
        continue;
      }
      const int value = -1;

      GlCall{}(
        glClearTexImage,
        m_color_attachment[i],
        0,
        GL_RED_INTEGER,
        GL_INT,
        &value);
    }
  }

private:
  FrameBufferSpecification m_specification    = {};
  std::array<Glid, 4U>     m_color_attachment = {};
  Glid                     m_depth_attachment = {};
  Glid                     m_renderer_id      = {};
};
static_assert(Bindable<FrameBuffer>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_FRAMEBUFFER_HPP
