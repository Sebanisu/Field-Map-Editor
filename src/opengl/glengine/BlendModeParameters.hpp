//
// Created by pcvii on 2/2/2022.
//

#ifndef FIELD_MAP_EDITOR_BLENDMODEPARAMETERS_HPP
#define FIELD_MAP_EDITOR_BLENDMODEPARAMETERS_HPP
#include "BlendModeParameter.hpp"
namespace glengine
{
using BlendModeParameters = BlendModeGenerics<
  decltype([]() {
    return std::array{ "srcRGB", "dstRGB", "srcAlpha", "dstAlpha" };
  }),
  BlendModeParameter>;
}
#endif// FIELD_MAP_EDITOR_BLENDMODEPARAMETERS_HPP
