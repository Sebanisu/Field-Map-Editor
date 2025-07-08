#include "push_pop_id.hpp"
namespace fme
{
[[nodiscard]] int &get_imgui_id()
{
     static int imgui_id = {};
     return imgui_id;
}

[[nodiscard]] glengine::ScopeGuard<void (*)()> PushPopID()
{
     ImGui::PushID(++get_imgui_id());
     return glengine::ScopeGuard{ &ImGui::PopID };
}
}// namespace fme