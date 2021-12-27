//
// Created by pcvii on 12/27/2021.
//

#ifndef MYPROJECT_FRAMEBUFFERBACKUP_HPP
#define MYPROJECT_FRAMEBUFFERBACKUP_HPP
#include "GLCheck.hpp"
#include "unique_value.hpp"
/**
 * Back up the currently bound framebuffer and rebind it on destruction.
 */
class [[nodiscard]] FrameBufferBackup
{
  GLID m_render_id = {};

public:
  FrameBufferBackup()
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
};
#endif// MYPROJECT_FRAMEBUFFERBACKUP_HPP
