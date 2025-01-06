#ifndef F5DCBECE_9BF1_4228_BFAE_6AA9A57273F5
#define F5DCBECE_9BF1_4228_BFAE_6AA9A57273F5
#include "scope_guard.hpp"
#include <imgui.h>
[[nodiscard]] int        &get_imgui_id();

[[nodiscard]] inline static auto PushPopID()
{
     ImGui::PushID(++get_imgui_id());
     return scope_guard{ &ImGui::PopID };
}

#endif /* F5DCBECE_9BF1_4228_BFAE_6AA9A57273F5 */
