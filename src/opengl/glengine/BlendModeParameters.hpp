//
// Created by pcvii on 2/2/2022.
//

#ifndef FIELD_MAP_EDITOR_BLENDMODEPARAMETERS_HPP
#define FIELD_MAP_EDITOR_BLENDMODEPARAMETERS_HPP
#include "BlendModeParameter.hpp"
namespace glengine
{
inline namespace impl
{
     class BlendModeParametersStrings
     {
          static constexpr auto strings = []() {
               using namespace std::string_view_literals;
               return std::array{ "srcRGB"sv, "dstRGB"sv, "srcAlpha"sv, "dstAlpha"sv };
          }();

        public:
          constexpr const auto &operator()() const noexcept
          {
               return strings;
          }
     };
     using BlendModeParameters = BlendModeGenerics<BlendModeParametersStrings, BlendModeParameter>;
}// namespace impl
}// namespace glengine
#endif// FIELD_MAP_EDITOR_BLENDMODEPARAMETERS_HPP
