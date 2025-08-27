#ifndef E8C0CF82_630C_44AE_895F_D090A6AD3983
#define E8C0CF82_630C_44AE_895F_D090A6AD3983
#include "map_sprite.hpp"
#include "Selections.hpp"
#include <functional>
#include <memory>
namespace fme
{
struct filter_window
{
     filter_window(std::weak_ptr<Selections>, std::weak_ptr<map_sprite>);
     void collapsing_header_filters() const;
     void render() const;
     void menu() const;
     void update(std::weak_ptr<Selections>);
     void update(std::weak_ptr<map_sprite>);

   private:
     [[nodiscard]] bool begin_window(const std::shared_ptr<Selections> &) const;
     void               handle_remove_queue(const std::shared_ptr<Selections> &, const std::shared_ptr<map_sprite> &) const;
     void               save_config(const std::shared_ptr<Selections> &) const;
     void               render_list_view(const std::shared_ptr<Selections> &, const std::shared_ptr<map_sprite> &) const;
     [[nodiscard]] int  calc_column_count(float) const;
     void               select_file(const std::string &, const std::shared_ptr<map_sprite> &) const;
     [[nodiscard]] std::vector<ff_8::PupuID>  get_unused_ids() const;
     [[nodiscard]] std::optional<std::string> prev_key() const;
     [[nodiscard]] std::optional<std::string> next_key() const;
     void                                     draw_thumbnail_label(const std::string &) const;
     void                      draw_add_new_button(const std::shared_ptr<Selections> &, const std::shared_ptr<map_sprite> &) const;
     void                      add_new_entry(const std::shared_ptr<Selections> &, const std::shared_ptr<map_sprite> &) const;
     [[nodiscard]] std::string generate_file_name(const std::shared_ptr<map_sprite> &, const std::optional<int> index = std::nullopt) const;
     void                      render_detail_view(const std::shared_ptr<Selections> &, const std::shared_ptr<map_sprite> &) const;
     void                      draw_filename_controls(const std::shared_ptr<Selections> &, const std::shared_ptr<map_sprite> &) const;
     void                      unselect_file() const;
     void                      draw_filter_controls(const std::shared_ptr<map_sprite> &) const;
     void                      popup_combo_filtered_pupu(const std::shared_ptr<Selections> &, const std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_pupu(const std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_bpps(const std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_palettes(const std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_blend_modes(const std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_blend_other(const std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_layers(const std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_texture_pages(const std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_animation_ids(const std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_animation_states(const std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_z(const std::shared_ptr<map_sprite> &) const;
     void                      combo_filtered_draw_bit(const std::shared_ptr<map_sprite> &) const;
     void                      menu_filtered_pupu(const std::shared_ptr<map_sprite> &) const;
     void                      menu_filtered_bpps(const std::shared_ptr<map_sprite> &) const;
     void                      menu_filtered_palettes(const std::shared_ptr<map_sprite> &) const;
     void                      menu_filtered_blend_modes(const std::shared_ptr<map_sprite> &) const;
     void                      menu_filtered_blend_other(const std::shared_ptr<map_sprite> &) const;
     void                      menu_filtered_layers(const std::shared_ptr<map_sprite> &) const;
     void                      menu_filtered_texture_pages(const std::shared_ptr<map_sprite> &) const;
     void                      menu_filtered_animation_ids(const std::shared_ptr<map_sprite> &) const;
     void                      menu_filtered_animation_states(const std::shared_ptr<map_sprite> &) const;
     void                      menu_filtered_z(const std::shared_ptr<map_sprite> &) const;
     void                      menu_filtered_draw_bit(const std::shared_ptr<map_sprite> &) const;

     void                      draw_thumbnail(
                            const std::shared_ptr<map_sprite>          &lock_map_sprite,
                            const std::string                          &file_name,
                            const std::optional<glengine::FrameBuffer> &framebuffer,
                            std::move_only_function<void()>             on_click) const;

     mutable bool                                                         m_changed                = { false };
     std::weak_ptr<Selections>                                            m_selections             = {};
     std::weak_ptr<map_sprite>                                            m_map_sprite             = {};


     mutable bool                                                         m_reload_thumbnail       = { false };
     mutable float                                                        m_aspect_ratio           = { 1.f };
     float                                                                m_tool_button_size_width = { 152.f };
     mutable float                                                        m_thumb_size_width       = { 96.f };
     mutable ff_8::filter_old<ff_8::FilterTag::MultiPupu>                 m_multi_select_filter    = { ff_8::FilterSettings::All_Disabled };
     mutable std::vector<std::string>                                     m_multi_select           = {};
     mutable std::string                                                  m_selected_file_name     = {};
     mutable std::string                                                  m_hovered_file_name      = {};
     mutable std::optional<std::string>                                   m_previous_file_name     = {};
     mutable std::optional<std::string>                                   m_next_file_name         = {};
     mutable toml::table                                                 *m_selected_toml_table    = {};
     mutable std::vector<std::string>                                     m_remove_queue           = {};
     mutable std::array<char, 128>                                        m_file_name_buffer       = {};
     mutable std::map<std::string, std::optional<glengine::FrameBuffer>> *m_textures_map           = {};
     mutable std::string                                                  m_last_selected          = {};
     static const constexpr size_t                                        s_max_chars =
       std::ranges::size(std::remove_cvref_t<decltype(m_file_name_buffer)>{}) - 1U;// space for null terminator
};
}// namespace fme

#endif /* E8C0CF82_630C_44AE_895F_D090A6AD3983 */
