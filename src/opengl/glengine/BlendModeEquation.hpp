//
// Created by pcvii on 2/2/2022.
//

#ifndef FIELD_MAP_EDITOR_BLENDMODEEQUATION_HPP
#define FIELD_MAP_EDITOR_BLENDMODEEQUATION_HPP
#include "BlendModeGeneric.hpp"
namespace glengine
{
enum class BlendModeEquationEnum
{
  FUNC_ADD,
  FUNC_SUBTRACT,
  FUNC_REVERSE_SUBTRACT,
  MIN,
  MAX
};
consteval inline int operator+(BlendModeEquationEnum value)
{
  return static_cast<int>(value);
}
using BlendModeEquation = BlendModeGeneric<
  decltype([]() {
    using namespace std::string_view_literals;
    return std::array{ "GL_FUNC_ADD"sv,// 0
                       "GL_FUNC_SUBTRACT"sv,// 1
                       "GL_FUNC_REVERSE_SUBTRACT"sv,// 2
                       "GL_MIN"sv,// 3
                       "GL_MAX"sv };// 4
  }),
  decltype([]() {
    return std::array{
      GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN, GL_MAX
    };
  })>;
}// namespace glengine
#endif// FIELD_MAP_EDITOR_BLENDMODEEQUATION_HPP
