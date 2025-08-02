#include "filter_window.hpp"
#include "as_string.hpp"
#include "generic_combo.hpp"
#include "gui_labels.hpp"

fme::filter_window::filter_window(std::weak_ptr<Selections> in_selections, std::weak_ptr<map_sprite> in_map_sprite)
  : m_selections(std::move(in_selections))
  , m_map_sprite(std::move(in_map_sprite))

{
}


void fme::filter_window::collapsing_header_filters() const
{
     m_changed = false;
     if (ImGui::CollapsingHeader(gui_labels::filters.data()))
     {
          auto lock_map_sprite = m_map_sprite.lock();
          if (!lock_map_sprite)
          {
               spdlog::error("Failed to lock map_sprite: shared_ptr is expired.");
               return;
          }
          combo_filtered_pupu(lock_map_sprite);
          combo_filtered_bpps(lock_map_sprite);
          combo_filtered_palettes(lock_map_sprite);
          combo_filtered_blend_modes(lock_map_sprite);
          combo_filtered_blend_other(lock_map_sprite);
          combo_filtered_layers(lock_map_sprite);
          combo_filtered_texture_pages(lock_map_sprite);
          combo_filtered_animation_ids(lock_map_sprite);
          combo_filtered_animation_states(lock_map_sprite);
          combo_filtered_z(lock_map_sprite);
          combo_filtered_draw_bit(lock_map_sprite);
     }
}

void fme::filter_window::render() const
{
     m_changed            = false;
     auto lock_selections = m_selections.lock();
     if (!lock_selections)
     {
          spdlog::error("Failed to lock selections: shared_ptr is expired.");
          return;
     }
     bool      &visible     = lock_selections->get<ConfigKey::DisplayFiltersWindow>();
     const auto pop_visible = glengine::ScopeGuard{ [&lock_selections, &visible, was_visable = visible] {
          if (was_visable != visible)
          {
               lock_selections->update<ConfigKey::DisplayFiltersWindow>();
          }
     } };
     const auto pop_end = glengine::ScopeGuard(&ImGui::End);
     if (ImGui::Begin(gui_labels::filters.data(), &visible))
     {
          auto lock_map_sprite = m_map_sprite.lock();
          if (!lock_map_sprite)
          {
               spdlog::error("Failed to lock map_sprite: shared_ptr is expired.");
               return;
          }
          combo_filtered_pupu(lock_map_sprite);
          combo_filtered_bpps(lock_map_sprite);
          combo_filtered_palettes(lock_map_sprite);
          combo_filtered_blend_modes(lock_map_sprite);
          combo_filtered_blend_other(lock_map_sprite);
          combo_filtered_layers(lock_map_sprite);
          combo_filtered_texture_pages(lock_map_sprite);
          combo_filtered_animation_ids(lock_map_sprite);
          combo_filtered_animation_states(lock_map_sprite);
          combo_filtered_z(lock_map_sprite);
          combo_filtered_draw_bit(lock_map_sprite);
     }
}
void fme::filter_window::update(std::weak_ptr<Selections> in_selections)
{
     m_selections = std::move(in_selections);
}
void fme::filter_window::update(std::weak_ptr<map_sprite> in_map_sprite)
{
     m_map_sprite = std::move(in_map_sprite);
}




void fme::filter_window::combo_filtered_layers(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().layer_id();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::layer_id,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_layer_id; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}
void fme::filter_window::combo_filtered_texture_pages(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().texture_page_id();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::texture_page,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_texture_page_id; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}
void fme::filter_window::combo_filtered_animation_ids(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().animation_id();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::animation_id,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_animation_id; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}
void fme::filter_window::combo_filtered_blend_other(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().blend_other();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::blend_other,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_blend_other; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}


void fme::filter_window::combo_filtered_z(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().z();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::z,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_z; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

void fme::filter_window::combo_filtered_animation_states(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &map = lock_map_sprite->uniques().animation_state();
     const auto &key = lock_map_sprite->filter().animation_id.value();
     if (!map.contains(key))
     {
          return;
     }
     const auto &pair = map.at(key);
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::animation_state,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_animation_state; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}


void fme::filter_window::combo_filtered_palettes(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &map = lock_map_sprite->uniques().palette();
     const auto &key = lock_map_sprite->filter().bpp.value();
     if (!map.contains(key))
     {
          return;
     }
     const auto &pair = map.at(key);
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::palette,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_palette; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

void fme::filter_window::combo_filtered_bpps(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().bpp();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::bpp,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_bpp; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}

void fme::filter_window::combo_filtered_blend_modes(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &pair = lock_map_sprite->uniques().blend_mode();
     const auto  gcc  = fme::GenericComboWithMultiFilter(
       gui_labels::blend_mode,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       [&pair]() { return pair.strings(); },
       [&]() -> auto  &{ return lock_map_sprite->filter().multi_blend_mode; });
     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}


void fme::filter_window::combo_filtered_draw_bit(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     using namespace std::string_view_literals;
     static constexpr auto values = std::array{ ff_8::draw_bitT::all, ff_8::draw_bitT::enabled, ff_8::draw_bitT::disabled };
     const auto            gcc    = fme::GenericComboWithFilter(
       gui_labels::draw_bit,
       []() { return values; },
       []() { return values | std::views::transform(AsString{}); },
       []() {
            return std::array{ gui_labels::draw_bit_all_tooltip,
                               gui_labels::draw_bit_enabled_tooltip,
                               gui_labels::draw_bit_disabled_tooltip };
       },
       [&]() -> auto               &{ return lock_map_sprite->filter().draw_bit; });
     if (!gcc.render())
          return;
     lock_map_sprite->update_render_texture();
     m_changed = true;
}


void fme::filter_window::combo_filtered_pupu(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto gcc = GenericComboWithMultiFilter(
       gui_labels::pupu_id,
       [&]() { return lock_map_sprite->working_unique_pupu(); },
       [&]() { return lock_map_sprite->working_unique_pupu() | std::views::transform(AsString{}); },
       [&]() {
            return lock_map_sprite->working_unique_pupu()
                   | std::views::transform([](const ff_8::PupuID &pupu_id) -> decltype(auto) { return pupu_id.create_summary(); });
       },
       [&]() -> auto & { return lock_map_sprite->filter().multi_pupu; });

     if (!gcc.render())
     {
          return;
     }
     lock_map_sprite->update_render_texture();
     m_changed = true;
}
