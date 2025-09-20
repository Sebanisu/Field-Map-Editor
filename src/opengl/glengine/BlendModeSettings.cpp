#include "BlendModeSettings.hpp"
#include "GLCheck.hpp"
void glengine::BlendModeSettings::enable_blending()
{
     glengine::GlCall{}(glEnable, GL_BLEND);
}

void glengine::BlendModeSettings::default_blend()
{
     GlCall{}(
       glBlendFuncSeparate, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE,
       GL_ONE);
     GlCall{}(glBlendEquationSeparate, GL_FUNC_ADD, GL_MAX);
}

void glengine::BlendModeSettings::add_blend()
{
     // GlCall{}(glBlendEquationSeparate, GL_FUNC_ADD, GL_FUNC_ADD);
     GlCall{}(glBlendFunc, GL_SRC_ALPHA, GL_ONE);
     GlCall{}(glBlendEquation, GL_FUNC_ADD);
}

void glengine::BlendModeSettings::subtract_blend()
{
     // what i had originally
     // GlCall{}(glBlendEquationSeparate, GL_FUNC_REVERSE_SUBTRACT,
     // GL_FUNC_ADD); was erasing the pixels below it.
     //  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
     //  glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
     // gpts fix.
     GlCall{}(glBlendFuncSeparate, GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
     GlCall{}(glBlendEquationSeparate, GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
}

void glengine::BlendModeSettings::multiply_blend()
{
     GlCall{}(glBlendFunc, GL_ZERO, GL_SRC_COLOR);
     GlCall{}(glBlendEquation, GL_FUNC_ADD);
}