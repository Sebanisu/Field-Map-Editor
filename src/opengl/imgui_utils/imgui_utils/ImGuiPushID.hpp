//
// Created by pcvii on 1/18/2022.
//

#ifndef FIELD_MAP_EDITOR_IMGUIPUSHID_HPP
#define FIELD_MAP_EDITOR_IMGUIPUSHID_HPP
#include <glengine/ScopeGuard.hpp>
#include <imgui.h>
namespace imgui_utils
{
namespace impl
{
     /**
      * Wrapper for ImGui::PushID(int) that increments an int and returns a
      * scope guarded ImGui::PopID() that will automatically Pop when the scope
      * ends.
      */
     class [[nodiscard]] ImGuiPushId final
     {
        public:
          /**
           * ImGui::PushID(int) and increment int.
           * @return ImGui::PopID() wrapped in a scope guard.
           */
          [[nodiscard]] glengine::ScopeGuard<void (*)()>
               operator()() const noexcept;
          /**
           * reset() must be called once per frame usually at the beginning or
           * the end. It resets an int to 0.
           */
          void reset() const noexcept;

          int  id() const noexcept;
     };
}// namespace impl
static constexpr inline impl::ImGuiPushId ImGuiPushId = {};

}// namespace imgui_utils
#endif// FIELD_MAP_EDITOR_IMGUIPUSHID_HPP
