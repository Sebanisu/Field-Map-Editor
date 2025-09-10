//
// Created by pcvii on 2/2/2022.
//

#ifndef FIELD_MAP_EDITOR_BLENDMODEEQUATION_HPP
#define FIELD_MAP_EDITOR_BLENDMODEEQUATION_HPP
#include "BlendModeGeneric.hpp"
namespace glengine
{
inline namespace impl
{
     enum class BlendModeEquationEnum
     {
          FuncAdd,
          FuncSubtract,
          FuncReverseSubtract,
          Min,
          Max
     };
     consteval inline int operator+(BlendModeEquationEnum value)
     {
          return static_cast<int>(value);
     }
     class BlendModeEquationStrings
     {
          static constexpr auto strings = []()
          {
               using namespace std::string_view_literals;
               return std::array{ "GL_FUNC_ADD"sv,             // 0
                                  "GL_FUNC_SUBTRACT"sv,        // 1
                                  "GL_FUNC_REVERSE_SUBTRACT"sv,// 2
                                  "GL_MIN"sv,                  // 3
                                  "GL_MAX"sv };                // 4
          }();

        public:
          constexpr const auto &operator()() const noexcept
          {
               return strings;
          }
     };

     class BlendModeEquationValues
     {
          constexpr static auto values = std::array{ GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN, GL_MAX };

        public:
          constexpr const auto &operator()() const noexcept
          {
               return values;
          }
     };
     using BlendModeEquation = BlendModeGeneric<BlendModeEquationStrings, BlendModeEquationValues>;
}// namespace impl
}// namespace glengine
#endif// FIELD_MAP_EDITOR_BLENDMODEEQUATION_HPP
