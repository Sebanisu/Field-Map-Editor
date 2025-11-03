//
// Created by pcvii on 12/27/2021.
//

#include "FrameBufferBackup.hpp"
#include "GLCheck.hpp"
glengine::FrameBufferBackup::FrameBufferBackup()
  : m_render_id{ []() -> std::uint32_t
                 {
                      GLint id = 0;
                      GlCall{}(glGetIntegerv, GL_FRAMEBUFFER_BINDING, &id);
                      return static_cast<std::uint32_t>(id);
                 }(),
                 [](std::uint32_t id)
                 { GlCall{}(glBindFramebuffer, GL_FRAMEBUFFER, id); } }
  , m_render_id_read{
       []() -> std::uint32_t
       {
            GLint id = 0;
            GlCall{}(glGetIntegerv, GL_READ_FRAMEBUFFER_BINDING, &id);
            return static_cast<std::uint32_t>(id);
       }(),
       [](std::uint32_t id)
       { GlCall{}(glBindFramebuffer, GL_READ_FRAMEBUFFER, id); }
  }
  , m_render_id_draw{
       []() -> std::uint32_t
       {
            GLint id = 0;
            GlCall{}(glGetIntegerv, GL_DRAW_FRAMEBUFFER_BINDING, &id);
            return static_cast<std::uint32_t>(id);
       }(),
       [](std::uint32_t id)
       { GlCall{}(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, id); }
  }
{
}
