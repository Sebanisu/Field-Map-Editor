//
// Created by pcvii on 12/27/2021.
//

#ifndef MYPROJECT_FRAMEBUFFERBACKUP_HPP
#define MYPROJECT_FRAMEBUFFERBACKUP_HPP
#include "GLCheck.hpp"
#include "unique_value.hpp"
namespace glengine
{
/**
 * Back up the currently bound framebuffer and rebind it on destruction.
 */
class [[nodiscard]] FrameBufferBackup
{
  GLID m_render_id = {};

public:
  FrameBufferBackup();
};
}// namespace glengine
#endif// MYPROJECT_FRAMEBUFFERBACKUP_HPP
