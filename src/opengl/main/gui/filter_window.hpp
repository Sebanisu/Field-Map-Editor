#ifndef E8C0CF82_630C_44AE_895F_D090A6AD3983
#define E8C0CF82_630C_44AE_895F_D090A6AD3983
#include "map_sprite.hpp"
#include "Selections.hpp"
#include <functional>
#include <imgui.h>
#include <memory>
namespace fme
{
struct table_move
{
     toml::table *root;  // pointer to the root table
     toml::table *nested;// pointer to the nested table

     table_move(
       toml::table *r,
       toml::table *n)
       : root(r)
       , nested(n)
     {
     }
};
struct filter_window
{
     using OuterFilter = std::move_only_function<
       bool(const ff_8::PupuID &, const std::span<const ff_8::PupuID>)>;
     using PupuMatchPredicate = std::move_only_function<
       bool(const ff_8::PupuID &, const ff_8::PupuID &)>;
     filter_window(
       std::weak_ptr<Selections>,
       std::weak_ptr<map_sprite>);
     void               collapsing_header_filters() const;
     [[nodiscard]] bool shortcut(const ImGuiKeyChord);
     void               render() const;
     void               menu() const;
     void               update(std::weak_ptr<Selections>);
     void               update(std::weak_ptr<map_sprite>);
     void               register_change_field_callback(
       std::move_only_function<void(const std::string &)>);
     void clear_change_field_callback();
     void register_change_coo_callback(
       std::move_only_function<void(const std::string &)>);
     void clear_change_coo_callback();
     void register_is_remaster_callback(std::move_only_function<bool()>);
     void clear_is_remaster_callback();

   private:
     [[nodiscard]] toml::table *
       get_root_table(const std::shared_ptr<Selections> &lock_selections) const;
     void root_table_to_imgui_tree(toml::table *root_table) const;
     void root_table_to_imgui_tree(
       toml::table                                  *root_table,
       const bool                                    skip_search,
       const std::uint32_t                           current_depth,
       std::vector<std::move_only_function<void()>> &callbacks) const;
     [[nodiscard]] bool begin_window(const std::shared_ptr<Selections> &) const;
     [[nodiscard]] bool contains_key_recursive(const toml::table *tbl) const;
     void               handle_remove_queue(
                     const std::shared_ptr<Selections> &,
                     const std::shared_ptr<map_sprite> &) const;
     void handle_rename_queue(
       const std::shared_ptr<Selections> &,
       const std::shared_ptr<map_sprite> &) const;
     void handle_regenerate(
       const std::shared_ptr<Selections> &,
       const std::shared_ptr<map_sprite> &) const;
     void cleanup_invalid_selections() const;
     void handle_thumbnail_size_adjustment() const;
     void
       reload_thumbnails_if_needed(const std::shared_ptr<map_sprite> &) const;
     void save_config(const std::shared_ptr<Selections> &) const;
     void render_list_view(
       const std::shared_ptr<Selections> &,
       const std::shared_ptr<map_sprite> &) const;
     [[nodiscard]] int calc_column_count(float) const;
     void              select_file(
                    const std::string &,
                    const std::shared_ptr<map_sprite> &) const;
     [[nodiscard]] std::vector<ff_8::PupuID>  get_unused_ids() const;
     [[nodiscard]] std::optional<std::string> prev_key() const;
     [[nodiscard]] std::optional<std::string> next_key() const;
     void draw_thumbnail_label(const std::string &) const;
     void draw_add_new_button(
       const std::shared_ptr<Selections> &,
       const std::shared_ptr<map_sprite> &) const;
     void add_new_entry(
       const std::shared_ptr<Selections> &,
       const std::shared_ptr<map_sprite> &) const;
     [[nodiscard]] std::string generate_file_name(
       const std::shared_ptr<map_sprite> &,
       const std::optional<int> index = std::nullopt) const;
     void render_detail_view(
       const std::shared_ptr<Selections> &,
       const std::shared_ptr<map_sprite> &) const;
     void display_stats(
       const std::shared_ptr<Selections> &,
       const std::shared_ptr<map_sprite> &) const;
     void combo_failover(const std::shared_ptr<Selections> &) const;
     void render_multi_select_toolbar(
       const std::shared_ptr<Selections> &,
       const std::shared_ptr<map_sprite> &) const;
     void render_attribute_combine_controls(
       const std::shared_ptr<Selections> &,
       const std::shared_ptr<map_sprite> &) const;
     void render_thumbnails(
       const std::shared_ptr<Selections> &,
       const std::shared_ptr<map_sprite> &) const;
     void draw_filename_controls(
       const std::shared_ptr<Selections> &,
       const std::shared_ptr<map_sprite> &) const;
     void unselect_file() const;
     void draw_filter_controls(const std::shared_ptr<map_sprite> &) const;
     void popup_combo_filtered_pupu(
       const std::shared_ptr<Selections> &,
       const std::shared_ptr<map_sprite> &) const;
     void combo_filtered_pupu(const std::shared_ptr<map_sprite> &) const;
     void combo_filtered_bpps(const std::shared_ptr<map_sprite> &) const;
     void combo_filtered_palettes(const std::shared_ptr<map_sprite> &) const;
     void combo_filtered_blend_modes(const std::shared_ptr<map_sprite> &) const;
     void combo_filtered_blend_other(const std::shared_ptr<map_sprite> &) const;
     void combo_filtered_layers(const std::shared_ptr<map_sprite> &) const;
     void
       combo_filtered_texture_pages(const std::shared_ptr<map_sprite> &) const;
     void combo_exclude_animation_id_from_state(
       const std::shared_ptr<map_sprite> &) const;
     void
       combo_filtered_animation_ids(const std::shared_ptr<map_sprite> &) const;
     void combo_filtered_animation_states(
       const std::shared_ptr<map_sprite> &) const;
     void combo_filtered_z(const std::shared_ptr<map_sprite> &) const;
     void combo_filtered_draw_bit(const std::shared_ptr<map_sprite> &) const;
     void menu_filtered_pupu(const std::shared_ptr<map_sprite> &) const;
     void menu_filtered_bpps(const std::shared_ptr<map_sprite> &) const;
     void menu_filtered_palettes(const std::shared_ptr<map_sprite> &) const;
     void menu_filtered_blend_modes(const std::shared_ptr<map_sprite> &) const;
     void menu_filtered_blend_other(const std::shared_ptr<map_sprite> &) const;
     void menu_filtered_layers(const std::shared_ptr<map_sprite> &) const;
     void
       menu_filtered_texture_pages(const std::shared_ptr<map_sprite> &) const;
     void
       menu_filtered_animation_ids(const std::shared_ptr<map_sprite> &) const;
     void menu_filtered_animation_states(
       const std::shared_ptr<map_sprite> &) const;
     void menu_filtered_z(const std::shared_ptr<map_sprite> &) const;
     void menu_filtered_draw_bit(const std::shared_ptr<map_sprite> &) const;

     void draw_thumbnail(
       const std::shared_ptr<Selections> &,
       const std::shared_ptr<map_sprite> &,
       const std::string &,
       const std::optional<glengine::FrameBuffer> &,
       std::move_only_function<void()>) const;

     const std::string &get_thumbnail_tooltip(
       const std::shared_ptr<map_sprite> &,
       const std::string &) const;
     void render_thumbnail_button(
       const std::string &,
       const std::optional<glengine::FrameBuffer> &,
       const bool,
       std::move_only_function<void()>) const;
     void render_thumbnail_popup(
       const std::shared_ptr<Selections> &,
       const std::shared_ptr<map_sprite> &,
       const std::string &) const;

     [[nodiscard]] bool is_excluded(const ff_8::PupuID &) const;
     void               process_combine(
                     const std::shared_ptr<Selections> &,
                     const std::shared_ptr<map_sprite> &) const;
     void process_combine(
       toml::table *,
       const std::vector<ff_8::PupuID> &,
       OuterFilter,
       PupuMatchPredicate) const;

     mutable bool              m_changed                = { false };
     std::weak_ptr<Selections> m_selections             = {};
     std::weak_ptr<map_sprite> m_map_sprite             = {};

     mutable bool              m_search_open            = { false };
     mutable bool              m_reload_thumbnail       = { false };
     mutable bool              m_regenerate_items       = { false };
     mutable bool              m_was_focused            = { false };

     mutable bool              m_checkoffset            = { false };
     mutable bool              m_checklayer_id          = { false };
     mutable bool              m_checkanimation         = { false };
     mutable bool              m_checkanimation_id      = { false };
     mutable bool              m_checkanimation_state   = { false };
     mutable bool              m_checkallow_same_blend  = { false };
     mutable bool              m_checkanimation_fill_in = { false };
     mutable float             m_aspect_ratio           = { 1.f };
     float                     m_tool_button_size_width = { 152.f };
     mutable float             m_thumb_size_width       = { 96.f };
     mutable ff_8::Filter<ff_8::FilterTag::MultiPupu> m_multi_select_filter
       = { ff_8::FilterSettings::All_Disabled };
     mutable ff_8::Filter<ff_8::FilterTag::MultiAnimationId>
       m_excluded_animation_id_from_state
       = { ff_8::FilterSettings::All_Disabled };
     mutable std::map<std::string, table_move> m_select_for_fix_names = {};
     mutable std::vector<std::string>          m_multi_select         = {};
     mutable std::vector<std::string>          m_reload_list          = {};
     mutable std::string                       m_search_field         = {};
     mutable std::string                       m_selected_file_name   = {};
     mutable std::string                       m_hovered_file_name    = {};
     mutable std::optional<std::string>        m_previous_file_name   = {};
     mutable std::optional<std::string>        m_next_file_name       = {};
     mutable toml::table                      *m_selected_toml_table  = {};
     mutable std::vector<std::string>          m_remove_queue         = {};
     mutable std::move_only_function<void(const std::string &)>
       m_change_field_callback;
     mutable std::move_only_function<void(const std::string &)>
                                             m_change_coo_callback;
     mutable std::move_only_function<bool()> m_is_remaster_callback;
     mutable std::vector<std::pair<std::string, std::string>> m_rename_queue
       = {};
     mutable std::array<char, 128> m_file_name_buffer = {};
     mutable std::map<std::string, std::optional<glengine::FrameBuffer>>
                                  *m_textures_map  = {};
     mutable std::string           m_last_selected = {};
     static const constexpr size_t s_max_chars
       = std::ranges::size(std::remove_cvref_t<decltype(m_file_name_buffer)>{})
         - 1U;// space for null terminator
};
}// namespace fme

#endif /* E8C0CF82_630C_44AE_895F_D090A6AD3983 */
