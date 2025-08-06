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
     static std::array<char,128> file_name_buffer{};
     static bool reload_thumbnail = false;
     m_changed                    = false;
     auto lock_selections         = m_selections.lock();
     if (!lock_selections)
     {
          spdlog::error("Failed to lock selections: shared_ptr is expired.");
          return;
     }
     if (!lock_selections->get<ConfigKey::DisplayFiltersWindow>())
     {
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
     if (!ImGui::Begin(gui_labels::filters.data(), &visible))
     {
          return;
     }
     auto lock_map_sprite = m_map_sprite.lock();
     if (!lock_map_sprite)
     {
          spdlog::error("Failed to lock map_sprite: shared_ptr is expired.");
          return;
     }
     if (lock_map_sprite->fail())
     {
          format_imgui_text("The `.map` is in an invalid state.\nSo no filters are avalible.");
          return;
     }
     if (lock_selections->get<ConfigKey::DrawMode>() != draw_mode::draw_map)
     {
          format_imgui_text("The draw mode is not set to `.map`.\nFilter changes won't show on draw window.");
     }
     static float thumb_size_width = 96.f;
     ImGui::SliderFloat("Thumbnail Size", &thumb_size_width, 96.f, 1024.f);
     const ImVec2        region_size         = ImGui::GetContentRegionAvail();
     const float         padding             = ImGui::GetStyle().FramePadding.x * 2.0f + ImGui::GetStyle().ItemSpacing.x;
     const int           col_count           = static_cast<int>(region_size.x / (thumb_size_width + padding));
     static std::string  selected_file_name  = {};
     static toml::table *selected_toml_table = {};
     auto               &textures_map        = lock_map_sprite->get_deswizzle_combined_textures();
     if (reload_thumbnail)
     {
          reload_thumbnail = false;
          textures_map.erase(selected_file_name);
          (void)lock_map_sprite->get_deswizzle_combined_textures();
     }
     static float aspect_ratio  = 1.f;
     const auto   draw_elements = [&](auto &&file_name, auto &&framebuffer, auto &&on_click) {
          if (framebuffer.has_value())
          {
               ImTextureID tex_id = glengine::ConvertGliDtoImTextureId<ImTextureID>(framebuffer.value().color_attachment_id());
               aspect_ratio       = static_cast<float>(framebuffer.value().height()) / static_cast<float>(framebuffer.value().width());
               const ImVec2 thumb_size = { thumb_size_width, thumb_size_width * aspect_ratio };
               if (ImGui::ImageButton(file_name.c_str(), tex_id, thumb_size))
               {
                    on_click();
               }
               else if (lock_map_sprite->get_deswizzle_combined_textures_tooltips().contains(file_name))
               {
                    tool_tip(lock_map_sprite->get_deswizzle_combined_textures_tooltips().at(file_name));
               }
          }
          else
          {
               const auto pop_id            = PushPopID();
               ImVec2     padded_thumb_size = { thumb_size_width + ImGui::GetStyle().FramePadding.x * 2.0f,
                                                thumb_size_width * aspect_ratio + ImGui::GetStyle().FramePadding.y * 2.0f };
               if (ImGui::Button("##Empty", padded_thumb_size))
               {
                    on_click();
               }
               else if (lock_map_sprite->get_deswizzle_combined_textures_tooltips().contains(file_name))
               {
                    tool_tip(lock_map_sprite->get_deswizzle_combined_textures_tooltips().at(file_name));
               }
          }
     };
     if (selected_file_name.empty() || !textures_map.contains(selected_file_name))
     {
          ImGui::Columns(col_count > 0 ? col_count : 1, "##get_deswizzle_combined_textures", false);


          for (const auto &[file_name, framebuffer] : textures_map)
          {
               const auto action = [&]() {
                    if (auto *ptr = lock_map_sprite->get_deswizzle_combined_toml_table(file_name); ptr)
                    {
                         selected_file_name  = file_name;
                         constexpr size_t max_chars = file_name_buffer.size() - 1;// space for null terminator
                         std::ranges::copy_n(
                           selected_file_name.begin(),
                           (std::min)(max_chars, static_cast<size_t>(selected_file_name.size())),
                           file_name_buffer.begin());
                         selected_toml_table = ptr;
                         lock_map_sprite->filter().reload(*ptr);
                         lock_map_sprite->update_render_texture();
                    }
               };
               draw_elements(file_name, framebuffer, action);

               // Label under image (optional)
               ImGui::TextWrapped("%s", file_name.c_str());
               ImGui::NextColumn();
          }
          ImGui::Columns(1);
     }
     else if (textures_map.contains(selected_file_name))
     {
          const auto action = [&]() {
               selected_file_name = {};
               file_name_buffer ={};
          };
          const auto &framebuffer = textures_map.at(selected_file_name);
          draw_elements(selected_file_name, framebuffer, action);
          if (ImGui::InputText("##Empty", file_name_buffer.data(), file_name_buffer.size()-1U))
          {
               
          }
          //ImGui::Button

          ImGui::Separator();

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

          if (m_changed)
          {
               reload_thumbnail = true;
               lock_map_sprite->filter().update(*selected_toml_table);
               const key_value_data        config_path_values = { .ext = ".toml" };
               const std::filesystem::path config_path =
                 config_path_values.replace_tags("{selected_path}/res/deswizzle{ext}", lock_selections, "{current_path}");
               auto config = Configuration(config_path);
               config.save();
          }
     }
}

void fme::filter_window::menu() const
{
     m_changed            = false;
     auto lock_map_sprite = m_map_sprite.lock();
     if (!lock_map_sprite)
     {
          spdlog::error("Failed to lock map_sprite: shared_ptr is expired.");
          return;
     }
     menu_filtered_pupu(lock_map_sprite);
     menu_filtered_bpps(lock_map_sprite);
     menu_filtered_palettes(lock_map_sprite);
     menu_filtered_blend_modes(lock_map_sprite);
     menu_filtered_blend_other(lock_map_sprite);
     menu_filtered_layers(lock_map_sprite);
     menu_filtered_texture_pages(lock_map_sprite);
     menu_filtered_animation_ids(lock_map_sprite);
     menu_filtered_animation_states(lock_map_sprite);
     menu_filtered_z(lock_map_sprite);
     menu_filtered_draw_bit(lock_map_sprite);
}

void fme::filter_window::update(std::weak_ptr<Selections> in_selections)
{
     m_selections = std::move(in_selections);
}
void fme::filter_window::update(std::weak_ptr<map_sprite> in_map_sprite)
{
     m_map_sprite = std::move(in_map_sprite);
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

void fme::filter_window::combo_filtered_palettes(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &map         = lock_map_sprite->uniques().palette();
     const auto &keys        = lock_map_sprite->filter().multi_bpp.value();
     const auto  join_vector = [](auto &&pairs) {
          auto transform_pairs = pairs | std::views::transform([](const auto &pair) { return pair.zip(); });
          auto join_pairs      = std::ranges::join_view(transform_pairs);
          return join_pairs | std::ranges::to<std::vector>();
     };

     auto value_string_pairs = [&]() {
          if (keys.empty() || !lock_map_sprite->filter().multi_bpp.enabled())
          {
               return join_vector(map | std::views::values);
          }
          else
          {
               auto keys_filter = keys | std::views::filter([&](const auto &key) { return map.contains(key); });
               auto keys_transform =
                 keys_filter | std::views::transform([&](const auto &key) { return map.at(key); }) | std::ranges::to<std::vector>();

               return join_vector(keys_transform);
          }
     }();


     // Deduplicate based on value
     std::ranges::sort(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     const auto unique_range = std::ranges::unique(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     value_string_pairs.erase(unique_range.begin(), unique_range.end());

     // Extract values and strings into separate views
     auto       values  = value_string_pairs | std::views::transform([](const auto &pair) { return std::get<0>(pair); });
     auto       strings = value_string_pairs | std::views::transform([](const auto &pair) { return std::get<1>(pair); });

     const auto gcc     = fme::GenericComboWithMultiFilter(
       gui_labels::palette,
       [&values]() { return values; },
       [&strings]() { return strings; },
       [&strings]() { return strings; },
       [&]() -> auto     &{ return lock_map_sprite->filter().multi_palette; });

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

void fme::filter_window::combo_filtered_animation_states(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &map         = lock_map_sprite->uniques().animation_state();
     const auto &keys        = lock_map_sprite->filter().multi_animation_id.value();
     const auto  join_vector = [](auto &&pairs) {
          auto transform_pairs = pairs | std::views::transform([](const auto &pair) { return pair.zip(); });
          auto join_pairs      = std::ranges::join_view(transform_pairs);
          return join_pairs | std::ranges::to<std::vector>();
     };

     auto value_string_pairs = [&]() {
          if (keys.empty() || !lock_map_sprite->filter().multi_animation_id.enabled())
          {
               return join_vector(map | std::views::values);
          }
          else
          {
               auto keys_filter = keys | std::views::filter([&](const auto &key) { return map.contains(key); });
               auto keys_transform =
                 keys_filter | std::views::transform([&](const auto &key) { return map.at(key); }) | std::ranges::to<std::vector>();

               return join_vector(keys_transform);
          }
     }();

     std::ranges::sort(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     const auto unique_range = std::ranges::unique(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     value_string_pairs.erase(unique_range.begin(), unique_range.end());

     auto       values  = value_string_pairs | std::views::transform([&](const auto &pair) { return std::get<0>(pair); });
     auto       strings = value_string_pairs | std::views::transform([&](const auto &pair) { return std::get<1>(pair); });
     const auto gcc     = fme::GenericComboWithMultiFilter(
       gui_labels::animation_state,
       [&values]() { return values; },
       [&strings]() { return strings; },
       [&strings]() { return strings; },
       [&]() -> auto     &{ return lock_map_sprite->filter().multi_animation_state; });
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

struct map_pupu_id
{
     std::shared_ptr<fme::map_sprite> m_map_sprite = {};
     const auto                      &values() const
     {
          return m_map_sprite->working_unique_pupu();
     }
     auto strings() const
     {
          return m_map_sprite->working_unique_pupu() | std::views::transform(fme::AsString{});
     }
     auto tooltips() const
     {
          return m_map_sprite->working_unique_pupu()
                 | std::views::transform([](const ff_8::PupuID &pupu_id) -> decltype(auto) { return pupu_id.create_summary(); });
     }
     auto zip() const
     {
          return std::ranges::views::zip(values(), strings(), tooltips());
     }
};

void fme::filter_window::menu_filtered_pupu(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(gui_labels::pupu_id, map_pupu_id{ lock_map_sprite }, lock_map_sprite->filter().multi_pupu, [&]() {
          lock_map_sprite->update_render_texture();
          m_changed = true;
     })();
}

void fme::filter_window::menu_filtered_bpps(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(gui_labels::bpp, lock_map_sprite->uniques().bpp(), lock_map_sprite->filter().multi_bpp, [&]() {
          lock_map_sprite->update_render_texture();
          m_changed = true;
     })();
}

void fme::filter_window::menu_filtered_palettes(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &map         = lock_map_sprite->uniques().palette();
     const auto &keys        = lock_map_sprite->filter().multi_bpp.value();

     const auto  join_vector = [](auto &&pairs) {
          auto transform_pairs = pairs | std::views::transform([](const auto &pair) { return pair.zip(); });
          auto join_pairs      = std::ranges::join_view(transform_pairs);
          return join_pairs | std::ranges::to<std::vector>();
     };

     auto value_string_pairs = [&]() {
          if (keys.empty() || !lock_map_sprite->filter().multi_bpp.enabled())
          {
               return join_vector(map | std::views::values);
          }
          else
          {
               auto keys_filter = keys | std::views::filter([&](const auto &key) { return map.contains(key); });
               auto keys_transform =
                 keys_filter | std::views::transform([&](const auto &key) { return map.at(key); }) | std::ranges::to<std::vector>();

               return join_vector(keys_transform);
          }
     }();

     std::ranges::sort(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     const auto unique_range = std::ranges::unique(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     value_string_pairs.erase(unique_range.begin(), unique_range.end());

     const auto unique_palettes = ff_8::unique_values_and_strings<std::uint8_t>(
       value_string_pairs | std::views::transform([&](const auto &pair) { return std::get<0>(pair); }) | std::ranges::to<std::vector>(),
       value_string_pairs | std::views::transform([&](const auto &pair) { return std::get<1>(pair); }) | std::ranges::to<std::vector>());

     GenericMenuWithMultiFilter(gui_labels::palette, unique_palettes, lock_map_sprite->filter().multi_palette, [&]() {
          lock_map_sprite->update_render_texture();
          m_changed = true;
     })();
}

void fme::filter_window::menu_filtered_blend_modes(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::blend_mode, lock_map_sprite->uniques().blend_mode(), lock_map_sprite->filter().multi_blend_mode, [&]() {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_blend_other(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::blend_other, lock_map_sprite->uniques().blend_other(), lock_map_sprite->filter().multi_blend_other, [&]() {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_layers(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::layer_id, lock_map_sprite->uniques().layer_id(), lock_map_sprite->filter().multi_layer_id, [&]() {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_texture_pages(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::texture_page, lock_map_sprite->uniques().texture_page_id(), lock_map_sprite->filter().multi_texture_page_id, [&]() {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_animation_ids(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(
       gui_labels::animation_id, lock_map_sprite->uniques().animation_id(), lock_map_sprite->filter().multi_animation_id, [&]() {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_animation_states(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     const auto &map         = lock_map_sprite->uniques().animation_state();
     const auto &keys        = lock_map_sprite->filter().multi_animation_id.value();

     const auto  join_vector = [](auto &&pairs) {
          auto transform_pairs = pairs | std::views::transform([](const auto &pair) { return pair.zip(); });
          auto join_pairs      = std::ranges::join_view(transform_pairs);
          return join_pairs | std::ranges::to<std::vector>();
     };

     auto value_string_pairs = [&]() {
          if (keys.empty() || !lock_map_sprite->filter().multi_animation_id.enabled())
          {
               return join_vector(map | std::views::values);
          }
          else
          {
               auto keys_filter = keys | std::views::filter([&](const auto &key) { return map.contains(key); });
               auto keys_transform =
                 keys_filter | std::views::transform([&](const auto &key) { return map.at(key); }) | std::ranges::to<std::vector>();

               return join_vector(keys_transform);
          }
     }();

     std::ranges::sort(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     const auto unique_range = std::ranges::unique(value_string_pairs, {}, [](const auto &pair) { return std::get<0>(pair); });
     value_string_pairs.erase(unique_range.begin(), unique_range.end());

     const auto unique_animation_state = ff_8::unique_values_and_strings<std::uint8_t>(
       value_string_pairs | std::views::transform([&](const auto &pair) { return std::get<0>(pair); }) | std::ranges::to<std::vector>(),
       value_string_pairs | std::views::transform([&](const auto &pair) { return std::get<1>(pair); }) | std::ranges::to<std::vector>());

     GenericMenuWithMultiFilter(
       gui_labels::animation_state, unique_animation_state, lock_map_sprite->filter().multi_animation_state, [&]() {
            lock_map_sprite->update_render_texture();
            m_changed = true;
       })();
}

void fme::filter_window::menu_filtered_z(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithMultiFilter(gui_labels::z, lock_map_sprite->uniques().z(), lock_map_sprite->filter().multi_z, [&]() {
          lock_map_sprite->update_render_texture();
          m_changed = true;
     })();
}

struct map_draw_bit
{
   private:
     static constexpr auto m_values   = std::array{ ff_8::draw_bitT::all, ff_8::draw_bitT::enabled, ff_8::draw_bitT::disabled };
     static constexpr auto m_tooltips = std::array{ fme::gui_labels::draw_bit_all_tooltip,
                                                    fme::gui_labels::draw_bit_enabled_tooltip,
                                                    fme::gui_labels::draw_bit_disabled_tooltip };

   public:
     auto values() const
     {
          return m_values;
     }
     auto strings() const
     {
          return m_values | std::views::transform(fme::AsString{});
     }
     auto tooltips() const
     {
          return m_tooltips;
     }
     auto zip() const
     {
          return std::ranges::views::zip(values(), strings(), tooltips());
     }
};

void fme::filter_window::menu_filtered_draw_bit(std::shared_ptr<map_sprite> &lock_map_sprite) const
{
     GenericMenuWithFilter(gui_labels::draw_bit, map_draw_bit{}, lock_map_sprite->filter().draw_bit, [&]() {
          lock_map_sprite->update_render_texture();
          m_changed = true;
     })();
}