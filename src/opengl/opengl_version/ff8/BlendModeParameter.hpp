//
// Created by pcvii on 2/2/2022.
//

#ifndef FIELD_MAP_EDITOR_BLENDMODEPARAMETER_HPP
#define FIELD_MAP_EDITOR_BLENDMODEPARAMETER_HPP
using BlendModeParameter = BlendModeGeneric<
  decltype([]() {
    using namespace std::string_view_literals;
    return std::array{ "GL_ZERO"sv,// 0
                       "GL_ONE"sv,// 1
                       "GL_SRC_COLOR"sv,// 2
                       "GL_ONE_MINUS_SRC_COLOR"sv,// 3
                       "GL_DST_COLOR"sv,// 4
                       "GL_ONE_MINUS_DST_COLOR"sv,// 5
                       "GL_SRC_ALPHA"sv,// 6
                       "GL_ONE_MINUS_SRC_ALPHA"sv,// 7
                       "GL_DST_ALPHA"sv,// 8
                       "GL_ONE_MINUS_DST_ALPHA"sv,// 9
                       "GL_CONSTANT_COLOR"sv,// 10
                       "GL_ONE_MINUS_CONSTANT_COLOR"sv,// 11
                       "GL_CONSTANT_ALPHA"sv,// 12
                       "GL_ONE_MINUS_CONSTANT_ALPHA"sv,// 13
                       "GL_SRC_ALPHA_SATURATE"sv,// 14
                       "GL_SRC1_COLOR"sv,// 15
                       "GL_ONE_MINUS_SRC_COLOR"sv,// 16
                       "GL_SRC1_ALPHA"sv,// 17
                       "GL_ONE_MINUS_SRC1_ALPHA"sv };// 18
  }),
  decltype([]() {
    return std::array{ GL_ZERO,
                       GL_ONE,
                       GL_SRC_COLOR,
                       GL_ONE_MINUS_SRC_COLOR,
                       GL_DST_COLOR,
                       GL_ONE_MINUS_DST_COLOR,
                       GL_SRC_ALPHA,
                       GL_ONE_MINUS_SRC_ALPHA,
                       GL_DST_ALPHA,
                       GL_ONE_MINUS_DST_ALPHA,
                       GL_CONSTANT_COLOR,
                       GL_ONE_MINUS_CONSTANT_COLOR,
                       GL_CONSTANT_ALPHA,
                       GL_ONE_MINUS_CONSTANT_ALPHA,
                       GL_SRC_ALPHA_SATURATE,
                       GL_SRC1_COLOR,
                       GL_ONE_MINUS_SRC_COLOR,
                       GL_SRC1_ALPHA,
                       GL_ONE_MINUS_SRC1_ALPHA };
  })>;


#endif// FIELD_MAP_EDITOR_BLENDMODEPARAMETER_HPP
