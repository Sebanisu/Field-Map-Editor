#ifndef F5DCBECE_9BF1_4228_BFAE_6AA9A57273F5
#define F5DCBECE_9BF1_4228_BFAE_6AA9A57273F5
#include <imgui.h>
#include <ScopeGuard.hpp>
namespace fme
{
[[nodiscard]] int                             &get_imgui_id();
[[nodiscard]] glengine::ScopeGuard<void (*)()> PushPopID();
}// namespace fme
#endif /* F5DCBECE_9BF1_4228_BFAE_6AA9A57273F5 */
