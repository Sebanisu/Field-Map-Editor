#ifndef AB58D01A_1172_4330_A796_897047556559
#define AB58D01A_1172_4330_A796_897047556559
#include "map_sprite.hpp"
#include "Selections.hpp"
#include <imgui.h>
#include <memory>
namespace fme
{
class [[nodiscard]] history_window
{
     mutable std::weak_ptr<map_sprite> m_map_sprite = {};
     mutable std::weak_ptr<Selections> m_selections = {};
     void                              draw_table() const;

   public:
     history_window() = default;
     history_window(
       const std::shared_ptr<Selections> &new_selections,
       const std::shared_ptr<map_sprite> &new_map_sprite);
     void on_im_gui_update() const;
     void on_im_gui_window_menu() const;
     void update(const std::shared_ptr<map_sprite> &new_map_sprite) const;
     void update(const std::shared_ptr<Selections> &new_selections) const;
};
}// namespace fme

#endif /* AB58D01A_1172_4330_A796_897047556559 */
