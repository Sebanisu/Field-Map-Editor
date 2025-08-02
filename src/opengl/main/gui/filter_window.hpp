#ifndef E8C0CF82_630C_44AE_895F_D090A6AD3983
#define E8C0CF82_630C_44AE_895F_D090A6AD3983
#include "map_sprite.hpp"
#include "Selections.hpp"
#include <memory>
namespace fme
{
struct filter_window
{
     filter_window(std::weak_ptr<Selections>, std::weak_ptr<map_sprite>);
     void collapsing_header_filters() const;
     void render() const;
     void update(std::weak_ptr<Selections>);
     void update(std::weak_ptr<map_sprite>);

   private:
     void                      combo_filtered_pupu(std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_draw_bit(std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_blend_modes(std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_layers(std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_texture_pages(std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_animation_ids(std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_animation_states(std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_palettes(std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_bpps(std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_blend_other(std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_z(std::shared_ptr<map_sprite> &) const;
     mutable bool              m_changed    = false;
     std::weak_ptr<Selections> m_selections = {};
     std::weak_ptr<map_sprite> m_map_sprite = {};
};
}// namespace fme

#endif /* E8C0CF82_630C_44AE_895F_D090A6AD3983 */
