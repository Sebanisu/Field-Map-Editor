//
// Created by pcvii on 1/18/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIPUSHID_HPP
#define FIELD_MAP_EDITOR_IMGUIPUSHID_HPP
#include "scope_guard.hpp"
namespace glengine
{
class [[nodiscard]] ImGuiPushID_impl
{
public:
  ImGuiPushID_impl() = default;
  ImGuiPushID_impl(const ImGuiPushID_impl &) = delete;
  ImGuiPushID_impl(ImGuiPushID_impl &&) noexcept = delete;
  ImGuiPushID_impl & operator = (const ImGuiPushID_impl &) = delete;
  ImGuiPushID_impl & operator = (ImGuiPushID_impl &&) noexcept = delete;
  [[nodiscard]] scope_guard operator()() const noexcept;
  void                                reset() const noexcept;
};
static constexpr inline ImGuiPushID_impl ImGuiPushID = {};
}
#endif// FIELD_MAP_EDITOR_IMGUIPUSHID_HPP
