#ifndef D606CA11_FB30_432B_BF0C_B3184209EE6E
#define D606CA11_FB30_432B_BF0C_B3184209EE6E
#include "map_sprite.hpp"
#include "Selections.hpp"
#include <imgui.h>
#include <memory>
namespace fme
{
class [[nodiscard]] textures_window
{
     mutable std::weak_ptr<map_sprite> m_map_sprite = {};
     mutable std::weak_ptr<Selections> m_selections = {};
     void                              draw_table() const;

   public:
     textures_window() = default;
     void render() const;
     void update(const std::shared_ptr<map_sprite> &new_map_sprite) const;
     void update(const std::shared_ptr<Selections> &new_selections) const;
};
}// namespace fme
#endif /* D606CA11_FB30_432B_BF0C_B3184209EE6E */
