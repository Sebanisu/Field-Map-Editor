//
// Created by pcvii on 12/1/2021.
//

#ifndef FIELD_MAP_EDITOR_FF8LOADTEXTURES_HPP
#define FIELD_MAP_EDITOR_FF8LOADTEXTURES_HPP
#include "DelayedTextures.hpp"

namespace ff8
{
glengine::DelayedTextures<35U>
  LoadTextures(const open_viii::graphics::background::Mim &mim);

glengine::DelayedTextures<17U * 13U>
  LoadTextures(const std::filesystem::path &upscale_path);
}
#endif// FIELD_MAP_EDITOR_FF8LOADTEXTURES_HPP
