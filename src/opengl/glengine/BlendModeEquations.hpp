//
// Created by pcvii on 2/2/2022.
//

#ifndef FIELD_MAP_EDITOR_BLENDMODEEQUATIONS_HPP
#define FIELD_MAP_EDITOR_BLENDMODEEQUATIONS_HPP
#include "BlendModeEquation.hpp"
namespace glengine
{
using BlendModeEquations = BlendModeGenerics<
  decltype([]() {
    return std::array{ "modeRGB", "modeAlpha" };
  }),
  BlendModeEquation>;
}
#endif// FIELD_MAP_EDITOR_BLENDMODEEQUATIONS_HPP
