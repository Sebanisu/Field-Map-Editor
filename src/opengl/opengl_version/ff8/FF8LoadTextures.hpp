//
// Created by pcvii on 12/1/2021.
//

#ifndef FIELD_MAP_EDITOR_FF8LOADTEXTURES_HPP
#define FIELD_MAP_EDITOR_FF8LOADTEXTURES_HPP
static const unsigned int texture_page_count   = 13U;
static const unsigned int palette_count_plus_1 = 17U;
static const unsigned int upscale_texture_count =
  palette_count_plus_1 * texture_page_count;
#include "DelayedTextures.hpp"

namespace ff_8
{
glengine::DelayedTextures<35U>
  LoadTextures(const open_viii::graphics::background::Mim &mim);

glengine::DelayedTextures<17U * 13U>
  LoadTextures(const std::filesystem::path &upscale_path);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_FF8LOADTEXTURES_HPP
