//
// Created by pcvii on 12/27/2021.
//

#include "FrameBufferBackup.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
glengine::FrameBufferBackup::FrameBufferBackup()
  : m_render_id{ []() -> std::uint32_t {
                  int32_t id = {};
                  GLCall{}(glGetIntegerv, GL_FRAMEBUFFER_BINDING, &id);
                  return static_cast<std::uint32_t>(id);
                }(),
                 [](std::uint32_t id) {
                   GLCall{}(glBindFramebuffer, GL_FRAMEBUFFER, id);
                 } }
{
}
