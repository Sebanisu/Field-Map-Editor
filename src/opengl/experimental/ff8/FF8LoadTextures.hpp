//
// Created by pcvii on 12/1/2021.
//

#ifndef FIELD_MAP_EDITOR_FF8LOADTEXTURES_HPP
#define FIELD_MAP_EDITOR_FF8LOADTEXTURES_HPP
#include "DelayedTextures.hpp"
#include <open_viii/graphics/background/Mim.hpp>

namespace ff_8
{
static const unsigned int TexturePageCount  = 13U;
static const unsigned int PaletteCountPlus1 = 17U;
static const unsigned int UpscaleTextureCount
  = PaletteCountPlus1 * TexturePageCount;
glengine::DelayedTextures<35U>
  LoadTextures(const open_viii::graphics::background::Mim &mim);

glengine::DelayedTextures<UpscaleTextureCount>
  LoadTextures(const std::filesystem::path &swizzle_path);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_FF8LOADTEXTURES_HPP
