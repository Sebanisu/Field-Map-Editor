//
// Created by pcvii on 1/25/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIDISABLED_HPP
#define FIELD_MAP_EDITOR_IMGUIDISABLED_HPP
#include "ScopeGuard.hpp"
namespace glengine
{
[[nodiscard]] ScopeGuard ImGuiDisabled(bool disabled) noexcept;
}// namespace glengine
#endif// FIELD_MAP_EDITOR_IMGUIDISABLED_HPP
