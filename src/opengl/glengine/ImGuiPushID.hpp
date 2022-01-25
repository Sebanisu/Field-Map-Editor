//
// Created by pcvii on 1/18/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIPUSHID_HPP
#define FIELD_MAP_EDITOR_IMGUIPUSHID_HPP
#include "scope_guard.hpp"
namespace glengine
{
/**
 * Wrapper for ImGui::PushID(int) that increments an int and returns a scope
 * guarded ImGui::PopID() that will automatically Pop when the scope ends.
 */
class [[nodiscard]] ImGuiPushID_impl
{
public:
  /**
   * ImGui::PushID(int) and increment int.
   * @return ImGui::PopID() wrapped in a scope guard.
   */
  [[nodiscard]] scope_guard operator()() const noexcept;
  /**
   * reset() must be called once per frame usually at the beginning or the end.
   * It resets an int to 0.
   */
  void                      reset() const noexcept;

private:
  inline static constinit int s_id = {};
};
static constexpr inline ImGuiPushID_impl ImGuiPushID = {};
}// namespace glengine
#endif// FIELD_MAP_EDITOR_IMGUIPUSHID_HPP
