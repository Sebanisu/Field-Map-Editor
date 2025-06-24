//
// Created by pcvii on 12/27/2021.
//

#ifndef FIELD_MAP_EDITOR_FRAMEBUFFERBACKUP_HPP
#define FIELD_MAP_EDITOR_FRAMEBUFFERBACKUP_HPP
#include "GLCheck.hpp"
#include "ScopeGuard.hpp"
#include "UniqueValue.hpp"
namespace glengine
{
/**
 * Back up the currently bound framebuffer and rebind it on destruction.
 */
class [[nodiscard]] FrameBufferBackup final : public AnyScopeGuard
{
     Glid m_render_id       = {};

     Glid m_render_id_read  = {};

     Glid m_render_id_draw  = {};

   public:
     FrameBufferBackup();
};
}// namespace glengine
#endif// FIELD_MAP_EDITOR_FRAMEBUFFERBACKUP_HPP
