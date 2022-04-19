//
// Created by pcvii on 2/2/2022.
//

#ifndef FIELD_MAP_EDITOR_BLENDMODEEQUATIONS_HPP
#define FIELD_MAP_EDITOR_BLENDMODEEQUATIONS_HPP
#include "BlendModeEquation.hpp"
namespace glengine
{
inline namespace impl
{
  class BlendModeEquationsStrings
  {
  public:
    constexpr auto operator()() const noexcept
    {
      using namespace std::string_view_literals;
      return std::array{ "modeRGB"sv, "modeAlpha"sv };
    }
  };
  using BlendModeEquations =
    BlendModeGenerics<BlendModeEquationsStrings, BlendModeEquation>;
}// namespace impl
}// namespace glengine
#endif// FIELD_MAP_EDITOR_BLENDMODEEQUATIONS_HPP
