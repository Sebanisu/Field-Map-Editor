//
// Created by pcvii on 9/7/2021.
//

#include "gui.hpp"
#include "gui_batch.hpp"
#include "gui_labels.hpp"
#include "safedir.hpp"
#include <open_viii/paths/Paths.hpp>
#include <SFML/Window/Mouse.hpp>
#include <utility>
using namespace open_viii::graphics::background;
using namespace open_viii::graphics;
using namespace open_viii::graphics::literals;
using namespace std::string_literals;
/**
 * @see https://godbolt.org/z/xce9jEbqh
 * @tparam T enforced std::filesystem::path
 * @tparam U anything that converts to std::filesystem::path
 * @param lhs T value
 * @param rhs U value
 * @return lhs + rhs
 */
template<typename T, typename U>
     requires std::same_as<std::remove_cvref_t<T>, std::filesystem::path> && (!std::same_as<std::remove_cvref_t<U>, std::filesystem::path>)
              && std::convertible_to<std::remove_cvref_t<U>, std::filesystem::path>
inline std::filesystem::path operator+(const T &lhs, const U &rhs)
{
     auto tmp = lhs;
     tmp += rhs;
     return tmp;
}
/**
 * @see https://godbolt.org/z/xce9jEbqh
 * @tparam T enforced std::filesystem::path
 * @param lhs anything that converts to std::filesystem::path
 * @param rhs T value
 * @return lhs + rhs
 */
template<typename T>
     requires std::same_as<std::remove_cvref_t<T>, std::filesystem::path>
              || std::same_as<std::remove_cvref_t<T>, std::filesystem::directory_entry>
inline std::filesystem::path operator+(const std::filesystem::path &lhs, const T &rhs)
{
     auto tmp = lhs;
     tmp += rhs;
     return tmp;
}

namespace fme
{
void gui::start()
{
     if (m_window.isOpen())
     {
          scale_window(static_cast<float>(m_selections.window_width), static_cast<float>(m_selections.window_height));
          do
          {
               m_changed      = false;
               get_imgui_id() = {};
               loop_events();
               const sf::Time        &delta_time       = m_delta_clock.restart();

               static constexpr float scroll_time_fast = 4000.F;
               static constexpr float scroll_time_slow = 1000.F;
               m_scrolling.total_scroll_time[0] =
                 m_selections.draw_swizzle || (!m_selections.draw_palette && mim_test()) ? scroll_time_fast : scroll_time_slow;
               if (m_scrolling.scroll(xy, delta_time))
               {
                    m_changed                     = false;
                    m_mouse_positions.mouse_moved = true;
                    if (mim_test())
                    {
                         move_camera(m_mim_sprite);
                    }
                    else if (map_test())
                    {
                         move_camera(m_map_sprite);
                    }
               }
               ImGui::SFML::Update(m_window, delta_time);
               loop();
          } while (m_window.isOpen());
          ImGui::SFML::Shutdown();
     }
}
void gui::render_dockspace()
{
     // If you strip some features of, this demo is pretty much equivalent to
     // calling DockSpaceOverViewport()! In most cases you should be able to just
     // call DockSpaceOverViewport() and ignore all the code below! In this
     // specific demo, we are not using DockSpaceOverViewport() because:
     // - we allow the host window to be floating/moveable instead of filling the
     // viewport (when opt_fullscreen == false)
     // - we allow the host window to have padding (when opt_padding == true)
     // - we have a local menu bar in the host window (vs. you could use
     // BeginMainMenuBar() + DockSpaceOverViewport() in your code!) TL;DR; this
     // demo is more complicated than what you would normally use. If we removed
     // all the options we are showcasing, this demo would become:
     //     void ShowExampleAppDockSpace()
     //     {
     //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
     //     }

     static constexpr bool     opt_fullscreen  = true;
     static constexpr bool     opt_padding     = false;
     static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

     // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window
     // not dockable into, because it would be confusing to have two docking
     // targets within each others.
     ImGuiWindowFlags          window_flags    = ImGuiWindowFlags_NoDocking;// ImGuiWindowFlags_MenuBar
     if constexpr (opt_fullscreen)
     {
          const ImGuiViewport *viewport = ImGui::GetMainViewport();
          ImGui::SetNextWindowPos(viewport->WorkPos);
          ImGui::SetNextWindowSize(viewport->WorkSize);
          ImGui::SetNextWindowViewport(viewport->ID);
          ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
          ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);
          window_flags = bitwise_or(
            window_flags,
            ImGuiWindowFlags_NoTitleBar,
            ImGuiWindowFlags_NoCollapse,
            ImGuiWindowFlags_NoResize,
            ImGuiWindowFlags_NoMove,
            ImGuiWindowFlags_NoBringToFrontOnFocus,
            ImGuiWindowFlags_NoNavFocus);
     }
     else
     {
          dockspace_flags = bitwise_and(dockspace_flags, bitwise_not(ImGuiDockNodeFlags_PassthruCentralNode));
     }

     // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render
     // our background and handle the pass-thru hole, so we ask Begin() to not
     // render a background.
     if (bitwise_and(dockspace_flags, ImGuiDockNodeFlags_PassthruCentralNode) != ImGuiDockNodeFlags{})
     {
          window_flags = bitwise_or(window_flags, ImGuiWindowFlags_NoBackground);
     }

     // Important: note that we proceed even if Begin() returns false (aka window
     // is collapsed). This is because we want to keep our DockSpace() active. If
     // a DockSpace() is inactive, all active windows docked into it will lose
     // their parent and become undocked. We cannot preserve the docking
     // relationship between an active window and an inactive docking, otherwise
     // any change of dockspace/settings would lead to windows being stuck in
     // limbo and never being visible.
     if constexpr (!opt_padding)
     {
          ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0F, 0.0F));
     }

     const auto imgui_end = scope_guard(&ImGui::End);
     ImGui::Begin("DockSpace Demo", nullptr, window_flags);
     if constexpr (!opt_padding)
     {
          ImGui::PopStyleVar();
     }

     if constexpr (opt_fullscreen)
     {
          ImGui::PopStyleVar(2);
     }

     dockspace_flags         = bitwise_or(dockspace_flags, ImGuiDockNodeFlags_PassthruCentralNode);
     dockspace_flags         = bitwise_and(dockspace_flags, bitwise_not(ImGuiDockNodeFlags_NoResize));
     // Submit the DockSpace
     const ImGuiIO &imgui_io = ImGui::GetIO();
     if (bitwise_and(imgui_io.ConfigFlags, ImGuiConfigFlags_DockingEnable) != ImGuiConfigFlags{})
     {
          const ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
          ImGui::DockSpace(dockspace_id, ImVec2(0.0F, 0.0F), dockspace_flags);
     }
     //  else
     //  {
     //    ShowDockingDisabledMessage();
     //  }
}
void gui::control_panel_window()
{
     const auto imgui_end = scope_guard(&ImGui::End);
     if (!ImGui::Begin(
           gui_labels::control_panel.data(),
           nullptr,
           static_cast<ImGuiWindowFlags>(static_cast<uint32_t>(ImGuiWindowFlags_AlwaysAutoResize))))
     {
          m_mouse_positions.mouse_enabled = handle_mouse_cursor();
          return;
     }
     //    if (m_first)
     //    {
     //      ImGui::SetWindowPos({ 0U, 0U });
     //    }
     background_color_picker();
     ImGui::SameLine();
     frame_rate();
     combo_draw();
     if (m_paths.empty())
     {
          return;
     }
     combo_path();
     combo_coo();
     combo_field();
     if (mim_test())
     {
          control_panel_window_mim();
     }
     else if (map_test())
     {
          control_panel_window_map();
     }

     m_mouse_positions.mouse_enabled = handle_mouse_cursor();
     text_mouse_position();
}
void gui::control_panel_window_mim()
{
     checkbox_mim_palette_texture();
     if (ImGui::CollapsingHeader(gui_labels::filters.data()))
     {
          if (!m_mim_sprite.draw_palette())
          {
               combo_bpp();
               combo_palette();
          }
          if (!m_mim_sprite.draw_palette())
          {
               format_imgui_text("{} == {}", gui_labels::width, gui_labels::max_tiles);
          }
     }
     if (m_changed)
     {
          scale_window();
     }
     slider_xy_sprite(m_mim_sprite);
}
void gui::control_panel_window_map()
{
     combo_upscale_path();
     combo_deswizzle_path();
     checkbox_map_swizzle();
     checkbox_render_imported_image();
     checkbox_map_disable_blending();
     compact_flatten_buttons();
     collapsing_header_filters();
     if (m_changed)
     {
          scale_window();
     }
     slider_xy_sprite(m_map_sprite);
}
void gui::frame_rate()
{
     const auto framerate = ImGui::GetIO().Framerate;
     format_imgui_text("   {:>3.2f} fps", framerate);
}
void gui::compact_flatten_buttons()
{
     format_imgui_text("{}: ", gui_labels::compact);
     ImGui::SameLine();
     if (ImGui::Button(gui_labels::rows.data()))
     {
          m_map_sprite.compact_rows();
     }
     ImGui::SameLine();
     if (ImGui::Button(gui_labels::all.data()))
     {
          m_map_sprite.compact_all();
     }
     ImGui::SameLine();
     format_imgui_text("{}: ", gui_labels::flatten);
     ImGui::SameLine();
     if (ImGui::Button(gui_labels::bpp.data()))
     {
          m_map_sprite.flatten_bpp();
     }
     ImGui::SameLine();
     if (ImGui::Button(gui_labels::palette.data()))
     {
          m_map_sprite.flatten_palette();
     }
}
void gui::collapsing_header_filters()
{
     if (ImGui::CollapsingHeader(gui_labels::filters.data()))
     {
          combo_pupu();
          combo_filtered_bpps();
          combo_filtered_palettes();
          combo_blend_modes();
          combo_blend_other();
          combo_layers();
          combo_texture_pages();
          combo_animation_ids();
          combo_animation_frames();
          combo_z();
          combo_draw_bit();
     }
}
void gui::background_color_picker()
{
     static std::array<float, 3U> clear_color_f{};
     if (ImGui::ColorEdit3(gui_labels::background.data(), clear_color_f.data(), ImGuiColorEditFlags_DisplayRGB))
     {
          // changed color
          static constexpr auto lerp = [](float percent) {
               return static_cast<sf::Uint8>(
                 std::lerp((std::numeric_limits<sf::Uint8>::min)(), (std::numeric_limits<sf::Uint8>::max)(), percent));
          };
          clear_color = { lerp(clear_color_f[0]), lerp(clear_color_f[1]), lerp(clear_color_f[2]), (std::numeric_limits<sf::Uint8>::max)() };
     }
}
void gui::loop()
{
     using namespace std::string_view_literals;

     menu_bar();
     file_browser_locate_ff8();
     file_browser_save_texture();
     render_dockspace();
     popup_batch_deswizzle();
     popup_batch_reswizzle();
     control_panel_window();
     batch_ops_ask_menu();
     begin_batch_embed_map_warning_window();
     popup_batch_embed();
     import_image_window();
     // Begin non imgui drawing.
     on_click_not_imgui();
     m_window.clear(clear_color);
     if (mim_test())
     {
          m_window.draw(m_mim_sprite.toggle_grids(m_selections.draw_grid, m_selections.draw_texture_page_grid));
     }
     else if (map_test())
     {
          m_window.draw(m_map_sprite.toggle_grid(m_selections.draw_grid, m_selections.draw_texture_page_grid));
     }
     //  m_mouse_positions.cover.setColor(clear_color);
     //  m_window.draw(m_mouse_positions.cover);

     sf::RenderStates states = {};
     if (m_drag_sprite_shader)
     {
          m_drag_sprite_shader->setUniform("texture", *m_mouse_positions.sprite.getTexture());
          static constexpr float border_width = 2.F;
          m_drag_sprite_shader->setUniform("borderWidth", border_width * static_cast<float>(m_map_sprite.get_map_scale()));
          states.shader = m_drag_sprite_shader.get();
     }
     m_window.draw(m_mouse_positions.sprite, states);
     ImGui::SFML::Render(m_window);
     m_window.display();
}
void gui::checkbox_render_imported_image()
{
     if (loaded_image_texture.getSize() != sf::Vector2u{})
     {
          if (ImGui::Checkbox("Render Imported Image", &m_selections.render_imported_image))
          {
               Configuration config{};
               config->insert_or_assign("selections_render_imported_image", m_selections.render_imported_image);
               config.save();
               // pass texture and map and tile_size
               update_imported_render_texture();
               if (!m_selections.render_imported_image)
               {
                    m_map_sprite.update_render_texture(nullptr, {}, tile_sizes::default_size);
               }
               m_changed = true;
          }
     }
}
void gui::update_imported_render_texture()
{
     if (m_selections.render_imported_image)
     {
          m_map_sprite.update_render_texture(&loaded_image_render_texture.getTexture(), import_image_map, m_selections.tile_size_value);
     }
}
void gui::popup_batch_common_filter_start(
  ff_8::filter_old<std::filesystem::path> &filter,
  std::string_view                         prefix,
  std::string_view                         base_name)
{
     if (filter.enabled())
     {
          filter.update(filter.value() / prefix / base_name);
          safedir const path = filter.value();
          if (!path.is_exists() || !path.is_dir())
          {
               filter.disable();
          }
     }
}


void gui::popup_batch_deswizzle()
{
     popup_batch_common(
       m_batch_deswizzle,
       [](ff_8::filters &filters) -> ff_8::filter_old<std::filesystem::path> & { return filters.upscale; },
       [this](ff_8::filter_old<std::filesystem::path> &filter) {
            // do I need to do generate_upscale_paths(m_field->get_base_name(),
            // get_coo()); here?
            if (combo_upscale_path(filter))//, ""
            {
            }

            return ImGui::Button(gui_labels::start.data());
       },
       [&](const auto &selected_path, auto &map) {
            map.save_pupu_textures(selected_path);
            const std::filesystem::path map_path = selected_path / map.map_filename();
            map.save_modified_map(map_path);
            format_imgui_text(
              "{} {} {}: {}", gui_labels::saving, open_viii::graphics::background::Map::EXT, gui_labels::file, map_path.string());
       });
}
void gui::popup_batch_reswizzle()
{
     popup_batch_common(
       m_batch_reswizzle,
       [](ff_8::filters &filters) -> ff_8::filter_old<std::filesystem::path> & { return filters.deswizzle; },
       [this](ff_8::filter_old<compact_type> &compact, bool &flatten_bpp, bool &flatten_palette) {
            combo_compact_type(compact);
            ImGui::Separator();
            format_imgui_text("Flatten: ");
            ImGui::Checkbox("BPP", &flatten_bpp);
            ImGui::SameLine();
            ImGui::Checkbox("Palette", &flatten_palette);
            return ImGui::Button("Start");
       },
       [&](
         const auto                           &selected_path,
         map_sprite                           &map,
         const ff_8::filter_old<compact_type> &compact,
         const bool                           &flatten_bpp,
         const bool                           &flatten_palette) {
            const auto compact_function = [&] {
                 if (compact.enabled())
                 {
                      if (compact.value() == compact_type::rows)
                      {
                           map.compact_rows();
                      }
                      if (compact.value() == compact_type::all)
                      {
                           map.compact_all();
                      }
                 }
            };
            compact_function();
            if (flatten_bpp)
            {
                 map.flatten_bpp();
                 compact_function();
            }
            if (flatten_palette)
            {
                 map.flatten_palette();
                 compact_function();
            }
            const std::filesystem::path map_path = selected_path / map.map_filename();
            map.save_new_textures(selected_path);
            map.save_modified_map(map_path);
            format_imgui_text("Saving Map file: {}", map_path.string());
       });
}

void gui::on_click_not_imgui()
{
     if (m_mouse_positions.mouse_enabled)
     {
          m_mouse_positions.update_sprite_pos(m_selections.draw_swizzle);
          if (m_mouse_positions.left_changed())
          {
               if (map_test())
               {
                    if (m_mouse_positions.left)
                    {
                         // left mouse down
                         // m_mouse_positions.cover =
                         m_mouse_positions.sprite = m_map_sprite.save_intersecting(m_mouse_positions.pixel, m_mouse_positions.texture_page);
                         m_mouse_positions.down_pixel = m_mouse_positions.pixel;
                         m_mouse_positions.max_tile_x = m_map_sprite.max_x_for_saved();
                    }
                    else
                    {
                         // left mouse up
                         m_map_sprite.update_position(
                           m_mouse_positions.pixel, m_mouse_positions.texture_page, m_mouse_positions.down_pixel);


                         // m_mouse_positions.cover =
                         m_mouse_positions.sprite     = {};
                         m_mouse_positions.max_tile_x = {};
                    }
               }
          }
     }
     else
     {
          if (m_mouse_positions.left_changed() && !m_mouse_positions.left)
          {
               // m_mouse_positions.cover =
               m_mouse_positions.sprite = {};
               // mouse up off-screen ?
          }
     }
}
void gui::text_mouse_position() const
{
     if (m_mouse_positions.mouse_enabled)
     {
          format_imgui_text("Mouse Pos: ({:4}, {:3})", m_mouse_positions.pixel.x, m_mouse_positions.pixel.y);
          ImGui::SameLine();
          if (map_test() || !m_selections.draw_palette)
          {
               const int tile_size = 16;
               format_imgui_text("Tile Pos: ({:2}, {:2})", m_mouse_positions.pixel.x / tile_size, m_mouse_positions.pixel.y / tile_size);
               if (m_selections.draw_swizzle)
               {
                    format_imgui_text("Page: {:2}", m_mouse_positions.texture_page);
               }
          }
     }
     if (!map_test())
     {
          return;
     }
     if (!ImGui::CollapsingHeader("Hovered Tiles") || !m_mouse_positions.mouse_enabled)
     {
          return;
     }
     m_map_sprite.const_visit_tiles([&](const auto &tiles) {
          static std::vector<std::size_t> indices = {};
          if (m_mouse_positions.mouse_moved)
          {
               indices = m_map_sprite.find_intersecting(tiles, m_mouse_positions.pixel, m_mouse_positions.texture_page);
          }
          if (std::ranges::empty(indices))
          {
               return;
          }
          format_imgui_text("Number of Tiles {:4}", std::ranges::size(indices));
          static constexpr int columns = 8;
          static_assert(columns % 2 == 0);
          if (!ImGui::BeginTable("##table", columns))
          {
               return;
          }
          for (const auto index : indices)
          {
               ImGui::TableNextColumn();
               format_imgui_text("{:4}", index);
               ImGui::TableNextColumn();
               (void)create_tile_button(tiles[index]);
          }
          ImGui::EndTable();
     });
}
bool gui::handle_mouse_cursor()
{
     bool           mouse_enabled = false;
     const auto    &mouse_pos     = sf::Mouse::getPosition(m_window);
     const auto    &win_size      = m_window.getSize();
     constexpr auto in_bounds     = [](std::integral auto value, std::integral auto low, std::integral auto high) {
          return std::cmp_greater_equal(value, low) && std::cmp_less_equal(value, high);
     };
     if (!in_bounds(mouse_pos.x, 0, win_size.x) || !in_bounds(mouse_pos.y, 0, win_size.y) || !m_window.hasFocus())
     {
          return mouse_enabled;
     }
     const sf::Vector2i clamped_mouse_pos = { std::clamp(mouse_pos.x, 0, static_cast<int>(win_size.x)),
                                              std::clamp(mouse_pos.y, 0, static_cast<int>(win_size.y)) };

     const auto         pixel_pos         = m_window.mapPixelToCoords(clamped_mouse_pos);

     m_mouse_positions.pixel              = { static_cast<int>(pixel_pos.x), static_cast<int>(pixel_pos.y) };
     const auto &pixel_x                  = m_mouse_positions.pixel.x;
     const auto &pixel_y                  = m_mouse_positions.pixel.y;
     auto        imgui_io                 = ImGui::GetIO();
     const bool  mim_or_map               = mim_test() || map_test();
     const bool  is_in_bounds             = in_bounds(pixel_x, 0, m_mim_sprite.width()) && in_bounds(pixel_y, 0, m_mim_sprite.height());
     if (!mim_or_map || !is_in_bounds || imgui_io.WantCaptureMouse)
     {
          return mouse_enabled;
     }
     mouse_enabled = true;
     if (!m_mouse_positions.mouse_moved)
     {
          return mouse_enabled;
     }
     auto              &tilex              = m_mouse_positions.pixel.x;
     const std::int16_t texture_page_width = 256;
     m_mouse_positions.texture_page        = static_cast<std::uint8_t>(tilex / texture_page_width);// for 4bit swizzle.
     return mouse_enabled;
}
void gui::combo_coo()
{
     if (generic_combo(
           get_imgui_id(),
           gui_labels::language,
           []() { return std::views::iota(static_cast<int>(open_viii::LangT::begin), static_cast<int>(open_viii::LangT::end)); },
           []() {
                static constexpr auto coos_string_array = open_viii::LangCommon::to_string_array();
                return coos_string_array;
           },
           m_selections.coo))
     {
          Configuration config{};
          config->insert_or_assign("selections_palette", m_selections.palette);
          config.save();
          if (mim_test())
          {
               m_mim_sprite = m_mim_sprite.with_coo(get_coo());
          }
          else if (map_test())
          {
               m_map_sprite = m_map_sprite.with_coo(get_coo());
          }
          if (m_field)
          {
               generate_upscale_paths(m_field->get_base_name(), get_coo());
          }
          m_changed = true;
     }
     if (ImGui::IsItemHovered())
     {
          ImGui::BeginTooltip();
          format_imgui_text(
            "{}",
            "This Language dropdown won't refresh archives unless you toggle the\n"
            "FF8 Path. Also it might not change anything unless it's the remaster\n"
            "version of the fields archive because they have all the languages in\n"
            "the same file. You could change the path directly to a lang- path.\n"
            "Then this will override this dropdown for older versions of FF8.");
          ImGui::EndTooltip();
     }
}
const open_viii::LangT &gui::get_coo() const
{

     static constexpr auto coos = open_viii::LangCommon::to_array();
     return coos.at(static_cast<size_t>(m_selections.coo));
}
void gui::combo_field()
{
     if (generic_combo(
           get_imgui_id(),
           gui_labels::field,
           [this]() { return std::views::iota(0, static_cast<int>(std::ranges::ssize(m_archives_group.mapdata()))); },
           [this]() { return m_archives_group.mapdata(); },
           m_selections.field))
     //  static constexpr auto items = 20;
     //  if (ImGui::Combo("Field",
     //        &m_selections.field,
     //        m_archives_group.mapdata_c_str().data(),
     //        static_cast<int>(m_archives_group.mapdata_c_str().size()),
     //        items))
     {
          Configuration config{};
          const auto   &maps = m_archives_group.mapdata();
          config->insert_or_assign("starter_field", *std::next(maps.begin(), m_selections.field));
          config.save();
          update_field();
     }
}

void gui::update_field()
{
     m_field = m_archives_group.field(m_selections.field);
     if (m_selections.draw == 0)
     {
          m_mim_sprite = m_mim_sprite.with_field(m_field);
     }
     else if (m_selections.draw == 1)
     {
          m_map_sprite = m_map_sprite.with_field(m_field);
     }

     m_loaded_swizzle_texture_path   = std::filesystem::path{};
     m_loaded_deswizzle_texture_path = std::filesystem::path{};

     m_changed                       = true;
     if (m_field)
     {
          generate_upscale_paths(m_field->get_base_name(), get_coo());
     }
}

void gui::checkbox_map_swizzle()
{
     if (ImGui::Checkbox("Swizzle", &m_selections.draw_swizzle))
     {
          Configuration config{};
          config->insert_or_assign("selections_draw_swizzle", m_selections.draw_swizzle);
          config.save();
          if (m_selections.draw_swizzle)
          {
               m_map_sprite.enable_disable_blends();
               m_map_sprite.enable_draw_swizzle();
          }
          else
          {
               m_map_sprite.disable_draw_swizzle();
               if (!m_selections.draw_disable_blending)
               {
                    m_map_sprite.disable_disable_blends();
               }
          }
          m_changed = true;
     }
}
void gui::checkbox_map_disable_blending()
{
     if (!m_selections.draw_swizzle && ImGui::Checkbox("Disable Blending", &m_selections.draw_disable_blending))
     {
          Configuration config{};
          config->insert_or_assign("selections_draw_disable_blending", m_selections.draw_disable_blending);
          config.save();
          if (m_selections.draw_disable_blending)
          {
               m_map_sprite.enable_disable_blends();
          }
          else
          {
               m_map_sprite.disable_disable_blends();
          }
          m_changed = true;
     }
}
void gui::checkbox_mim_palette_texture()
{
     if (ImGui::Checkbox("draw Palette Texture", &m_selections.draw_palette))
     {
          Configuration config{};
          config->insert_or_assign("selections_draw_palette", m_selections.draw_palette);
          config.save();
          m_mim_sprite = m_mim_sprite.with_draw_palette(m_selections.draw_palette);
          m_changed    = true;
     }
}
static void update_bpp(mim_sprite &sprite, BPPT bpp)
{
     sprite = sprite.with_bpp(bpp);
}
static void update_bpp(map_sprite &sprite, BPPT bpp)
{
     if (sprite.filter().bpp.update(bpp).enabled())
     {
          sprite.update_render_texture();
     }
}
void gui::combo_bpp()
{
     {
          const auto                  pop_pushed = PushPop();
          static constexpr std::array bpp_items  = Mim::bpp_selections_c_str();
          if (ImGui::Combo(
                "BPP", &m_selections.bpp, bpp_items.data(), static_cast<int>(bpp_items.size()), static_cast<int>(bpp_items.size())))
          {
               Configuration config{};
               config->insert_or_assign("selections_bpp", m_selections.bpp);
               config.save();
               if (mim_test())
               {
                    update_bpp(m_mim_sprite, bpp());
               }
               if (map_test())
               {
                    update_bpp(m_map_sprite, bpp());
               }
               m_changed = true;
          }
     }
     {
          const auto  pop_pushed            = PushPop();
          static bool enable_palette_filter = false;
          if (map_test())
          {
               ImGui::SameLine();
               if (ImGui::Checkbox("", &enable_palette_filter))
               {
                    if (enable_palette_filter)
                    {
                         m_map_sprite.filter().bpp.enable();
                    }
                    else
                    {
                         m_map_sprite.filter().bpp.disable();
                    }
                    m_map_sprite.update_render_texture();
                    m_changed = true;
               }
          }
     }
}
std::uint8_t gui::palette() const
{
     return static_cast<uint8_t>(Mim::palette_selections().at(static_cast<size_t>(m_selections.palette)));
}
static void update_palette(mim_sprite &sprite, uint8_t palette)
{
     sprite = sprite.with_palette(palette);
}
static void update_palette(map_sprite &sprite, uint8_t palette)
{
     if (sprite.filter().palette.update(palette).enabled())
     {
          sprite.update_render_texture();
     }
}
void gui::combo_palette()
{
     if (m_selections.bpp != 2)
     {
          static constexpr std::array palette_items = Mim::palette_selections_c_str();
          {
               const auto pop_pushed = PushPop();
               if (ImGui::Combo(
                     "Palette",
                     &m_selections.palette,
                     palette_items.data(),
                     static_cast<int>(palette_items.size()),
                     static_cast<int>(palette_items.size())))
               {
                    if (mim_test())
                    {
                         update_palette(m_mim_sprite, palette());
                    }
                    if (map_test())
                    {
                         update_palette(m_map_sprite, palette());
                    }
                    Configuration config{};
                    config->insert_or_assign("selections_palette", m_selections.palette);
                    config.save();
                    m_changed = true;
               }
          }
          if (map_test())
          {
               ImGui::SameLine();
               {
                    static bool enable_palette_filter = false;
                    const auto  pop_pushed            = PushPop();
                    if (ImGui::Checkbox("", &enable_palette_filter))
                    {
                         if (enable_palette_filter)
                         {
                              m_map_sprite.filter().palette.enable();
                         }
                         else
                         {
                              m_map_sprite.filter().palette.disable();
                         }
                         m_map_sprite.update_render_texture();
                         m_changed = true;
                    }
               }
          }
     }
}

void gui::menu_bar()
{
     if (!ImGui::BeginMainMenuBar())
     {
          return;
     }
     const auto end_menu_bar = scope_guard(&ImGui::EndMainMenuBar);
     //  if (!ImGui::BeginMenuBar())
     //    return;
     //  const auto end_menu_bar = scope_guard(&ImGui::EndMenuBar);
     if (ImGui::BeginMenu("File"))
     {
          file_menu();
     }
     if (ImGui::BeginMenu("Edit"))
     {
          edit_menu();
     }
     if (ImGui::BeginMenu("Batch Operation Test"))
     {
          batch_operation_test_menu();
     }
     if (ImGui::BeginMenu("Import"))
     {
          import_menu();
     }
}
void gui::import_menu()
{
     const auto end_menu = scope_guard(&ImGui::EndMenu);
     if (ImGui::MenuItem("Import Image", nullptr, &m_selections.display_import_image))
     {
          Configuration config{};
          config->insert_or_assign("selections_display_import_image", m_selections.display_import_image);
          config.save();
     }
}
void gui::batch_operation_test_menu()
{
     const auto end_menu = scope_guard(&ImGui::EndMenu);
     if (ImGui::MenuItem("Deswizzle", nullptr, false, static_cast<bool>(m_archives_group)))
     {
          m_directory_browser.Open();
          m_directory_browser.SetTitle("Choose directory to save textures");
          m_directory_browser.SetPwd(Configuration{}["deswizzle_path"].value_or(std::filesystem::current_path().string()));
          m_directory_browser.SetTypeFilters({ ".map", ".png" });
          m_modified_directory_map = map_directory_mode::batch_save_deswizzle_textures;
     }
     if (ImGui::MenuItem("Reswizzle", nullptr, false, static_cast<bool>(m_archives_group)))
     {
          m_directory_browser.Open();
          m_directory_browser.SetTitle(
            "Choose source directory of deswizzled textures (contains two letter "
            "directories)");
          m_directory_browser.SetPwd(Configuration{}["reswizzle_path"].value_or(std::filesystem::current_path().string()));
          m_directory_browser.SetTypeFilters({ ".map", ".png" });
          m_modified_directory_map = map_directory_mode::batch_load_deswizzle_textures;
     }

     if (ImGui::MenuItem("Embed .map files into Archives", nullptr, false, static_cast<bool>(m_archives_group)))
     {
          m_selections.batch_embed_map_warning_window = true;
     }
     if (ImGui::MenuItem("Test Batch Window", nullptr, &m_selections.test_batch_window))
     {
          Configuration config{};
          config->insert_or_assign("selections_test_batch_window", m_selections.test_batch_window);
          config.save();
     }
}
void gui::edit_menu()
{
     const auto end_menu = scope_guard(&ImGui::EndMenu);
     if (ImGui::MenuItem("Undo", "Control + Z", false, m_map_sprite.undo_enabled()))
     {
          m_map_sprite.undo();
     }
     if (ImGui::MenuItem("Redo", "Control + Y", false, m_map_sprite.redo_enabled()))
     {
          m_map_sprite.redo();
     }
     ImGui::Separator();
     if (ImGui::MenuItem("Undo All", "Shift + Control + Z", false, m_map_sprite.undo_enabled()))
     {
          m_map_sprite.undo_all();
     }
     if (ImGui::MenuItem("Redo All", "Shift + Control + Y", false, m_map_sprite.redo_enabled()))
     {
          m_map_sprite.redo_all();
     }
     ImGui::Separator();
     if (ImGui::MenuItem("draw Tile Grid", nullptr, &m_selections.draw_grid))
     {
          Configuration config{};
          config->insert_or_assign("selections_draw_grid", m_selections.draw_grid);
          config.save();
     }
     if ((map_test() && m_selections.draw_swizzle) || mim_test())
     {
          if (ImGui::MenuItem("draw Texture Page Grid", nullptr, &m_selections.draw_texture_page_grid))
          {
               Configuration config{};
               config->insert_or_assign("selections_draw_texture_page_grid", m_selections.draw_texture_page_grid);
               config.save();
          }
     }
}
void gui::file_menu()
{
     const auto end_menu = scope_guard(&ImGui::EndMenu);
     menuitem_locate_ff8();
     if (map_test())
     {
          menuitem_locate_custom_upscale();
     }
     ImGui::Separator();
     menuitem_save_texture(save_texture_path(), mim_test() || map_test());
     if (mim_test())
     {
          ImGui::Separator();
          menuitem_save_mim_file(m_mim_sprite.mim_filename());
     }
     if (map_test())
     {
          ImGui::Separator();
          menuitem_save_map_file(m_map_sprite.map_filename());
          menuitem_save_map_file_modified(m_map_sprite.map_filename());
          menuitem_load_map_file(m_map_sprite.map_filename());
          ImGui::Separator();
          menuitem_save_swizzle_textures();
          menuitem_load_swizzle_textures();
          ImGui::Separator();
          menuitem_save_deswizzle_textures();
          menuitem_load_deswizzle_textures();
     }
}
void gui::browse_for_embed_map_dir()
{
     m_directory_browser.Open();
     m_directory_browser.SetTitle(
       "Choose source directory of your textures and .map files "
       "(contains two letter directories)");
     m_directory_browser.SetPwd(Configuration{}["embed_source_path"].value_or(std::filesystem::current_path().string()));
     m_directory_browser.SetTypeFilters({ ".map" });
     m_modified_directory_map = map_directory_mode::batch_embed_map_files;
}
bool gui::map_test() const
{
     return !m_map_sprite.fail() && m_selections.draw == 1;
}
bool gui::mim_test() const
{
     return !m_mim_sprite.fail() && m_selections.draw == 0;
}
std::string gui::save_texture_path() const
{
     if (m_archives_group.mapdata().empty())
     {
          return {};
     }
     const std::string &field_name = m_archives_group.mapdata().at(static_cast<size_t>(m_selections.field));
     if (mim_test())// MIM
     {
          if (m_mim_sprite.draw_palette())
          {
               return fmt::format("{}_mim_palettes.png", field_name);
          }
          const int bpp = static_cast<int>(Mim::bpp_selections().at(static_cast<size_t>(m_selections.bpp)));
          return fmt::format("{}_mim_{}bpp_{}.png", field_name, bpp, m_selections.palette);
     }
     else if (map_test())
     {
          return fmt::format("{}_map.png", field_name);
     }
     return {};
}
void gui::file_browser_locate_ff8()
{
     m_directory_browser.Display();
     if (m_directory_browser.HasSelected())
     {
          static std::filesystem::path reswizzle_src = {};
          auto                         selected_path = m_directory_browser.GetSelected();
          if (m_modified_directory_map == map_directory_mode::ff8_install_directory)
          {
               m_paths.push_back(selected_path.string());
               sort_paths();
               m_selections.path = selected_path.string();
               update_path();
          }
          else if (m_modified_directory_map == map_directory_mode::custom_upscale_directory)
          {
               m_custom_upscale_paths.push_back(selected_path.string());
               // todo remove paths that don't exist.
               Configuration config{};
               config->insert_or_assign("custom_upscale_paths_vector", m_custom_upscale_paths);
               config.save();
               if (m_field)
               {
                    generate_upscale_paths(m_field->get_base_name(), get_coo());
               }
               // todo toggle filter enabled?
          }
          else if (m_modified_directory_map == map_directory_mode::save_swizzle_textures)
          {
               Configuration config{};
               config->insert_or_assign("reswizzle_path", m_directory_browser.GetPwd().string());
               selected_path = path_with_prefix_and_base_name(std::move(selected_path));
               std::filesystem::create_directories(selected_path);
               // todo modify these two functions :P to use the imported image.
               m_map_sprite.save_new_textures(selected_path);// done.
               m_map_sprite.save_modified_map(selected_path / m_map_sprite.map_filename());// done.
          }
          else if (m_modified_directory_map == map_directory_mode::load_swizzle_textures)
          {
               Configuration config{};
               config->insert_or_assign("single_swizzle_or_deswizzle_path", m_directory_browser.GetPwd().string());
               config.save();
               m_loaded_swizzle_texture_path = selected_path;
               if (m_field)
               {
                    generate_upscale_paths(m_field->get_base_name(), get_coo());
               }
               m_map_sprite.filter().deswizzle.disable();
               m_map_sprite.filter().upscale.update(m_loaded_swizzle_texture_path).enable();
               auto          map_path      = m_loaded_swizzle_texture_path / m_map_sprite.map_filename();
               safedir const safe_map_path = map_path;
               if (safe_map_path.is_exists())
               {
                    m_map_sprite.load_map(map_path);
               }
               m_map_sprite.update_render_texture(true);
          }
          else if (m_modified_directory_map == map_directory_mode::save_deswizzle_textures)
          {
               Configuration config{};
               config->insert_or_assign("deswizzle_path", m_directory_browser.GetPwd().string());
               config.save();
               selected_path = path_with_prefix_and_base_name(std::move(selected_path));
               std::filesystem::create_directories(selected_path);
               m_map_sprite.save_pupu_textures(selected_path);
               m_map_sprite.save_modified_map(selected_path / m_map_sprite.map_filename());
          }
          else if (m_modified_directory_map == map_directory_mode::load_deswizzle_textures)
          {
               Configuration config{};
               config->insert_or_assign("single_swizzle_or_deswizzle_path", m_directory_browser.GetPwd().string());
               config.save();
               m_loaded_deswizzle_texture_path = selected_path;
               m_map_sprite.filter().upscale.disable();
               m_map_sprite.filter().deswizzle.update(m_loaded_deswizzle_texture_path).enable();
               auto          map_path      = m_loaded_deswizzle_texture_path / m_map_sprite.map_filename();
               safedir const safe_map_path = map_path;
               if (safe_map_path.is_exists())
               {
                    m_map_sprite.load_map(map_path);
               }
               m_map_sprite.update_render_texture(true);
          }
          else if (m_modified_directory_map == map_directory_mode::batch_save_deswizzle_textures)
          {
               Configuration config{};
               config->insert_or_assign("deswizzle_path", m_directory_browser.GetPwd().string());
               config.save();
               m_batch_deswizzle.enable(selected_path);
          }
          else if (m_modified_directory_map == map_directory_mode::batch_embed_map_files)
          {
               Configuration config{};
               config->insert_or_assign("embed_source_path", m_directory_browser.GetPwd().string());
               config.save();
               m_batch_embed.enable(selected_path);
          }
          else if (m_modified_directory_map == map_directory_mode::batch_save_swizzle_textures)
          {
               Configuration config{};
               config->insert_or_assign("reswizzle_path", m_directory_browser.GetPwd().string());
               config.save();
               m_batch_reswizzle.enable(std::move(reswizzle_src), std::move(selected_path));
          }
          else if (m_modified_directory_map == map_directory_mode::batch_load_deswizzle_textures)
          {
               Configuration config{};
               config->insert_or_assign("deswizzle_path", m_directory_browser.GetPwd().string());
               reswizzle_src = std::move(selected_path);
               m_directory_browser.Open();
               m_directory_browser.SetTitle(
                 "Choose destination directory where reswizzled textures will be "
                 "saved.");
               m_directory_browser.SetTypeFilters({ ".map", ".png" });
               m_modified_directory_map = map_directory_mode::batch_save_swizzle_textures;
               config->insert_or_assign("reswizzle_path", m_directory_browser.GetPwd().string());
               config.save();
          }
          m_directory_browser.ClearSelected();
     }
}
std::filesystem::path gui::path_with_prefix_and_base_name(std::filesystem::path selected_path) const
{
     std::string const      base_name = m_map_sprite.get_base_name();
     std::string_view const prefix    = std::string_view{ base_name }.substr(0U, 2U);
     selected_path                    = selected_path / prefix / base_name;
     return selected_path;
}
void gui::sort_paths()
{// todo remove paths that don't exist.
     std::vector<std::string> tmp = {};
     tmp.reserve(std::ranges::size(m_paths));
     std::ranges::transform(m_paths, std::back_inserter(tmp), [](const toml::node &node) { return node.value_or(std::string{}); });
     std::ranges::sort(tmp);
     const auto removal = std::ranges::unique(tmp);
     tmp.erase(removal.begin(), removal.end());
     m_paths.clear();
     std::ranges::for_each(tmp, [this](std::string &str) { m_paths.push_back(std::move(str)); });
     Configuration config{};
     config->insert_or_assign("paths_vector", m_paths);
     config.save();
}
void gui::file_browser_save_texture()
{
     m_save_file_browser.Display();
     if (m_save_file_browser.HasSelected())
     {
          [[maybe_unused]] const auto selected_path = m_save_file_browser.GetSelected();
          if (mim_test())
          {
               const auto str_path = selected_path.string();
               if (open_viii::tools::i_ends_with(str_path, Mim::EXT))
               {
                    m_mim_sprite.mim_save(selected_path);
                    Configuration config{};
                    config->insert_or_assign("mim_path", m_save_file_browser.GetPwd().string());
                    config.save();
               }
               else
               {
                    m_mim_sprite.save(selected_path);
                    Configuration config{};
                    config->insert_or_assign("save_image_path", m_save_file_browser.GetPwd().string());
                    config.save();
               }
          }
          else if (map_test())
          {
               const auto str_path = selected_path.string();
               if (open_viii::tools::i_ends_with(str_path, Map::EXT))
               {
                    switch (m_modified_map)
                    {
                         case map_dialog_mode::save_modified: {
                              m_map_sprite.save_modified_map(selected_path);
                              Configuration config{};
                              config->insert_or_assign("map_path", m_save_file_browser.GetPwd().string());
                              config.save();
                         }
                         break;
                         case map_dialog_mode::save_unmodified: {
                              m_map_sprite.map_save(selected_path);
                              Configuration config{};
                              config->insert_or_assign("map_path", m_save_file_browser.GetPwd().string());
                              config.save();
                         }
                         break;
                         case map_dialog_mode::load: {
                              m_map_sprite.load_map(selected_path);
                              Configuration config{};
                              config->insert_or_assign("map_path", m_save_file_browser.GetPwd().string());
                              config.save();
                              m_changed = true;
                         }
                         break;
                    }
               }
               else
               {
                    m_map_sprite.save(selected_path);
                    Configuration config{};
                    config->insert_or_assign("save_image_path", m_save_file_browser.GetPwd().string());
                    config.save();
               }
          }
          m_save_file_browser.ClearSelected();
     }
}
void gui::menuitem_locate_ff8()
{
     if (ImGui::MenuItem("Locate a FF8 install"))
     {
          open_locate_ff8_filebrowser();
     }
}
void gui::open_locate_ff8_filebrowser()
{
     m_directory_browser.Open();
     m_directory_browser.SetTitle("Choose FF8 install directory");
     m_directory_browser.SetTypeFilters({ ".exe" });
     m_modified_directory_map = map_directory_mode::ff8_install_directory;
}
void gui::menuitem_locate_custom_upscale()
{
     if (ImGui::MenuItem("Locate a Custom Upscale directory"))
     {
          open_locate_custom_upscale();
     }
}
void gui::open_locate_custom_upscale()
{
     m_directory_browser.Open();
     m_directory_browser.SetTitle("Choose Custom Upscale directory");
     m_modified_directory_map = map_directory_mode::custom_upscale_directory;
}
void gui::menuitem_save_swizzle_textures()
{
     if (ImGui::MenuItem("Save Swizzled Textures", nullptr, false, true))
     {
          save_swizzle_textures();
     }
}
std::string gui::appends_prefix_base_name(std::string_view title) const
{
     std::string const      base_name = m_map_sprite.get_base_name();
     std::string_view const prefix    = std::string_view{ base_name }.substr(0U, 2U);
     return fmt::format("{} (appends {}{}{})", title, prefix, char{ std::filesystem::path::preferred_separator }, base_name);
}
void gui::save_swizzle_textures()
{
     m_directory_browser.Open();
     static constexpr std::string_view title = "Choose directory to save reswizzle textures";
     m_directory_browser.SetTitle(appends_prefix_base_name(title));
     m_directory_browser.SetPwd(Configuration{}["reswizzle_path"].value_or(std::filesystem::current_path().string()));
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_modified_directory_map = map_directory_mode::save_swizzle_textures;
}
void gui::menuitem_save_deswizzle_textures()
{
     if (ImGui::MenuItem("Save Deswizzled Textures (Pupu)", nullptr, false, true))
     {
          m_directory_browser.Open();
          static constexpr std::string_view title = "Choose directory to save deswizzle textures";
          m_directory_browser.SetTitle(appends_prefix_base_name(title));
          m_directory_browser.SetPwd(Configuration{}["deswizzle_path"].value_or(std::filesystem::current_path().string()));
          m_directory_browser.SetTypeFilters({ ".map", ".png" });
          m_modified_directory_map = map_directory_mode::save_deswizzle_textures;
     }
}
void gui::menuitem_load_swizzle_textures()
{
     if (ImGui::MenuItem("Load Swizzled Textures", nullptr, false, true))
     {
          open_swizzle_filebrowser();
     }
}
void gui::open_swizzle_filebrowser()
{
     m_directory_browser.Open();
     m_directory_browser.SetTitle("Choose directory to load textures from");
     m_directory_browser.SetPwd(Configuration{}["single_swizzle_or_deswizzle_path"].value_or(std::filesystem::current_path().string()));
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_modified_directory_map = map_directory_mode::load_swizzle_textures;
}
void gui::menuitem_load_deswizzle_textures()
{
     if (ImGui::MenuItem("Load Deswizzled Textures", nullptr, false, true))
     {
          open_deswizzle_filebrowser();
     }
}
void gui::open_deswizzle_filebrowser()
{
     m_directory_browser.Open();
     m_directory_browser.SetTitle("Choose directory to load textures from");
     m_directory_browser.SetPwd(Configuration{}["single_swizzle_or_deswizzle_path"].value_or(std::filesystem::current_path().string()));
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_modified_directory_map = map_directory_mode::load_deswizzle_textures;
}
void gui::menuitem_save_texture(const std::string &path, bool enabled)
{
     if (ImGui::MenuItem("Save Displayed Texture", nullptr, false, enabled))
     {
          m_save_file_browser.Open();
          m_save_file_browser.SetTitle("Save Texture as...");
          m_save_file_browser.SetPwd(Configuration{}["save_image_path"].value_or(std::filesystem::current_path().string()));
          m_save_file_browser.SetTypeFilters({ ".png", ".ppm" });
          m_save_file_browser.SetInputName(path.c_str());
     }
}
void gui::menuitem_save_mim_file(const std::string &path, bool enabled)
{
     if (ImGui::MenuItem("Save Mim File", nullptr, false, enabled))
     {
          m_save_file_browser.Open();
          m_save_file_browser.SetTitle("Save Mim as...");
          m_save_file_browser.SetPwd(Configuration{}["mim_path"].value_or(std::filesystem::current_path().string()));
          m_save_file_browser.SetTypeFilters({ Mim::EXT.data() });
          m_save_file_browser.SetInputName(path);
     }
}
void gui::menuitem_save_map_file(const std::string &path, bool enabled)
{
     if (ImGui::MenuItem("Save Map File (unmodified)", nullptr, false, enabled))
     {
          m_save_file_browser.Open();
          m_save_file_browser.SetTitle("Save Map as...");
          m_save_file_browser.SetPwd(Configuration{}["map_path"].value_or(std::filesystem::current_path().string()));
          m_save_file_browser.SetTypeFilters({ Map::EXT.data() });
          m_save_file_browser.SetInputName(path);
          m_modified_map = map_dialog_mode::save_unmodified;
     }
}
void gui::menuitem_save_map_file_modified(const std::string &path, bool enabled)
{
     if (ImGui::MenuItem("Save Map File (modified)", nullptr, false, enabled))
     {
          m_save_file_browser.Open();
          m_save_file_browser.SetTitle("Save Map as...");
          m_save_file_browser.SetPwd(Configuration{}["map_path"].value_or(std::filesystem::current_path().string()));
          m_save_file_browser.SetTypeFilters({ Map::EXT.data() });
          m_save_file_browser.SetInputName(path);
          m_modified_map = map_dialog_mode::save_modified;
     }
}
void gui::menuitem_load_map_file(const std::string &path, bool enabled)
{
     if (ImGui::MenuItem("Load Map File", nullptr, false, enabled))
     {
          m_save_file_browser.Open();
          m_save_file_browser.SetTitle("Load Map...");
          m_save_file_browser.SetPwd(Configuration{}["map_path"].value_or(std::filesystem::current_path().string()));
          m_save_file_browser.SetTypeFilters({ Map::EXT.data() });
          m_save_file_browser.SetInputName(path);
          m_modified_map = map_dialog_mode::load;
     }
}
void gui::combo_draw()
{
     if (generic_combo(
           get_imgui_id(),
           gui_labels::draw,
           []() { return std::views::iota(0, static_cast<int>(std::size(m_draw_selections))); },
           [this]() { return this->m_draw_selections; },
           m_selections.draw))
     {
          Configuration config{};
          config->insert_or_assign("selections_draw", m_selections.draw);
          config.save();
          if (m_selections.draw == 0)
          {
               m_mim_sprite = get_mim_sprite();
          }
          else if (m_selections.draw == 1)
          {
               m_map_sprite = m_map_sprite.update(m_field, get_coo(), m_selections.draw_swizzle);
          }
          m_changed = true;
     }
}
bool gui::combo_path()
{
     const auto browse_button = scope_guard([this]() {
          ImGui::SameLine();
          if (ImGui::Button("Browse"))
          {
               open_locate_ff8_filebrowser();
          }
     });
     if (generic_combo(
           get_imgui_id(),
           gui_labels::path,
           [this]() {
                return m_paths
                       | std::ranges::views::transform([](toml::node &item) -> std::string { return item.value_or<std::string>({}); });
           },
           [this]() {
                return m_paths
                       | std::ranges::views::transform([](toml::node &item) -> std::string { return item.value_or<std::string>({}); });
           },
           m_selections.path))
     {
          Configuration config{};
          config->insert_or_assign("selections_path", m_selections.path);
          config.save();
          update_path();
          return true;
     }
     return false;
}

toml::array gui::get_paths()
{
     const char   *paths_vector = "paths_vector";
     Configuration config{};
     if (!config->contains(paths_vector))
     {
          const auto &default_paths = open_viii::Paths::get();
          // todo get all default paths for linux and windows.
          toml::array paths_array{};
          paths_array.reserve(default_paths.size());
          for (const auto &path : default_paths)
          {
               paths_array.push_back(path);
          }
          config->insert_or_assign(paths_vector, std::move(paths_array));
          config.save();
     }
     return *(config->get_as<toml::array>(paths_vector));

     //  std::vector<std::string> paths{};
     //  open_viii::Paths::for_each_path([&paths](const std::filesystem::path &p) {
     //    paths.emplace_back(p.string());
     //  });
     //  return paths;
}
toml::array gui::get_custom_upscale_paths_vector()
{
     const char   *paths_vector = "custom_upscale_paths_vector";
     Configuration config{};
     if (!config->contains(paths_vector))
     {
          return {};
     }
     return *(config->get_as<toml::array>(paths_vector));

     //  std::vector<std::string> paths{};
     //  open_viii::Paths::for_each_path([&paths](const std::filesystem::path &p) {
     //    paths.emplace_back(p.string());
     //  });
     //  return paths;
}
void gui::loop_events()
{
     m_mouse_positions.update();
     while (m_window.pollEvent(m_event))
     {
          ImGui::SFML::ProcessEvent(m_event);
          const auto event_variant = events::get(m_event);
          std::visit(
            events::make_visitor(
              [this](const sf::Event::SizeEvent &size) {
                   scale_window(static_cast<float>(size.width), static_cast<float>(size.height));
                   m_changed = true;
              },
              [this](const sf::Event::MouseMoveEvent &) {
                   m_mouse_positions.mouse_moved = true;
                   // TODO move setting mouse pos code here?
                   // m_changed = true;
              },
              [this](const sf::Event::KeyEvent &key) {
                   if (ImGui::GetIO().WantCaptureKeyboard)
                   {
                        m_scrolling.reset();
                        return;
                   }
                   const auto &type = m_event.type;
                   if (type == sf::Event::EventType::KeyReleased)
                   {
                        event_type_key_released(key);
                   }
                   else if (type == sf::Event::EventType::KeyPressed)
                   {
                        event_type_key_pressed(key);
                   }
              },
              [this](const sf::Event::MouseButtonEvent &mouse) {
                   const sf::Mouse::Button &button = mouse.button;
                   if (!m_mouse_positions.mouse_enabled)
                   {
                        m_mouse_positions.left = false;
                        return;
                   }
                   switch (m_event.type)
                   {
                        case sf::Event::EventType::MouseButtonPressed:
                             ///< A mouse button was pressed (data in event.mouseButton)
                             {
                                  event_type_mouse_button_pressed(button);
                             }
                             break;
                        case sf::Event::EventType::MouseButtonReleased:
                             ///< A mouse button was released (data in
                             ///< event.mouseButton)
                             {
                                  event_type_mouse_button_released(button);
                             }
                             break;
                        default:
                             break;
                   }
              },
              [this]([[maybe_unused]] const std::monostate &) {
                   if (m_event.type == sf::Event::Closed)
                   {
                        m_window.close();
                   }
              },
              []([[maybe_unused]] const auto &) {}),
            event_variant);
     }
}
void gui::event_type_mouse_button_released(const sf::Mouse::Button &button)
{
     switch (button)
     {
          case sf::Mouse::Left: {
               m_mouse_positions.left = false;
               spdlog::trace("{}", "Left Mouse Button Up");
          }
          break;
          default:
               break;
     }
}
void gui::event_type_mouse_button_pressed(const sf::Mouse::Button &button)
{
     switch (button)
     {
          case sf::Mouse::Left: {
               m_mouse_positions.left = true;
               spdlog::trace("{}", "Left Mouse Button Down");
          }
          break;
          default:
               break;
     }
}
void gui::event_type_key_pressed(const sf::Event::KeyEvent &key)
{
     if (key.code == sf::Keyboard::Up)
     {
          m_scrolling.up = true;
     }
     else if (key.code == sf::Keyboard::Down)
     {
          m_scrolling.down = true;
     }
     else if (key.code == sf::Keyboard::Left)
     {
          m_scrolling.left = true;
     }
     else if (key.code == sf::Keyboard::Right)
     {
          m_scrolling.right = true;
     }
}
void gui::event_type_key_released(const sf::Event::KeyEvent &key)
{
     if (key.shift && key.control && key.code == sf::Keyboard::Z)
     {
          m_map_sprite.undo_all();
     }
     else if (key.shift && key.control && key.code == sf::Keyboard::Y)
     {
          m_map_sprite.redo_all();
     }
     else if (key.control && key.code == sf::Keyboard::Z)
     {
          m_map_sprite.undo();
     }
     else if (key.control && key.code == sf::Keyboard::Y)
     {
          m_map_sprite.redo();
     }
     else if (key.code == sf::Keyboard::Up)
     {
          m_scrolling.up = false;
     }
     else if (key.code == sf::Keyboard::Down)
     {
          m_scrolling.down = false;
     }
     else if (key.code == sf::Keyboard::Left)
     {
          m_scrolling.left = false;
     }
     else if (key.code == sf::Keyboard::Right)
     {
          m_scrolling.right = false;
     }
}
std::uint32_t gui::image_height() const
{
     if (map_test())
     {
          return m_map_sprite.height();
     }
     return m_mim_sprite.height();
}
float gui::scaled_menubar_gap() const
{
     if (saved_window_height < std::numeric_limits<float>::epsilon())
     {
          return {};
     }
     return ImGui::GetFrameHeight() * static_cast<float>(image_height()) / saved_window_height;
}
void gui::scale_window(float width, float height)
{
     const auto            img_height = static_cast<float>(image_height());
     static constexpr auto load       = [](auto &saved, auto &not_saved) {
          if (not_saved < std::numeric_limits<float>::epsilon())
          {
               not_saved = saved;
          }
          else
          {
               saved = not_saved;
          }
     };
     load(saved_window_width, width);
     load(saved_window_height, height);
     const float adjusted_height = height - ImGui::GetFrameHeight();
     // this scales up the elements without losing the horizontal space. so
     // going from 4:3 to 16:9 will end up with wide screen.
     m_scale_width               = std::round(width / adjusted_height * img_height);
     float const scaled_gap      = scaled_menubar_gap();
     m_window.setView(sf::View(
       sf::FloatRect(std::round(m_cam_pos.x), std::round(m_cam_pos.y - scaled_gap), m_scale_width, std::round(img_height + scaled_gap))));
}
archives_group gui::get_archives_group() const
{
     if (!std::ranges::empty(m_selections.path))
     {
          return { open_viii::LangCommon::to_array().front(), m_selections.path };
     }
     return {};
}
sf::RenderWindow gui::get_render_window() const
{
     return sf::RenderWindow{ sf::VideoMode(m_selections.window_width, m_selections.window_height),
                              sf::String{ gui_labels::window_title } };
}
void gui::update_path()
{
     m_archives_group = m_archives_group.with_path(m_selections.path);
     update_field();
     if (m_batch_embed4.enabled())
     {
          m_batch_embed4.enable(m_selections.path, m_batch_embed4.start_time());
     }
}
mim_sprite gui::get_mim_sprite() const
{
     return { m_field,
              Mim::bpp_selections().at(static_cast<std::size_t>(m_selections.bpp)),
              static_cast<std::uint8_t>(Mim::palette_selections().at(static_cast<std::size_t>(m_selections.palette))),
              get_coo(),
              m_selections.draw_palette };
}
void gui::init_and_get_style()
{
     //  static constexpr auto fps_lock = 360U;
     //  m_window.setFramerateLimit(fps_lock);
     m_window.setVerticalSyncEnabled(true);
     (void)ImGui::SFML::Init(m_window);
     ImGuiIO &imgui_io    = ImGui::GetIO();
     imgui_io.ConfigFlags = bitwise_or(imgui_io.ConfigFlags, ImGuiConfigFlags_DockingEnable);
     if (m_field)
     {
          generate_upscale_paths(m_field->get_base_name(), get_coo());
     }
     if (!m_drag_sprite_shader)
     {
          m_drag_sprite_shader                       = std::make_shared<sf::Shader>();
          static const std::string border_shader_raw = R"(#version 130
uniform sampler2D texture;
uniform float borderWidth;

void main()
{
    vec4 pixel = texture2D(texture, gl_TexCoord[0].st);
    vec2 texelSize = vec2(1.0/textureSize(texture,0).x, 1.0/textureSize(texture,0).y);
    float alpha = 0;
    int space = int(borderWidth);
    int threshold = (2*space) * (2*space);
    int count = 0;
    for(int y=-space;y<=space;++y)
    {
      for(int x=-space;x<=space;++x)
      {
        if(texture2D(texture, gl_TexCoord[0].st + vec2(x*texelSize.x, y*texelSize.y)).a > 0.5)
        {
            count++;
        }
      }
    }
    if(pixel.a > 0.5)
    {
      gl_FragColor = pixel;
    }
    else
    {
      gl_FragColor = vec4(1, 0, 0, float(count)/float(threshold));
    }
}
)";
          m_drag_sprite_shader->loadFromMemory(border_shader_raw, sf::Shader::Fragment);
     }
}
gui::gui()
  : m_window(get_render_window())
  , m_paths(get_paths())
  , m_custom_upscale_paths(get_custom_upscale_paths_vector())
  , m_archives_group(get_archives_group())
  , m_field(init_field())
  , m_mim_sprite(get_mim_sprite())
  , m_map_sprite(get_map_sprite())

{
     sort_paths();
     init_and_get_style();
}
std::shared_ptr<open_viii::archive::FIFLFS<false>> gui::init_field()
{
     m_selections.field = get_selected_field();
     return m_archives_group.field(m_selections.field);
}
map_sprite gui::get_map_sprite() const
{
     return { m_field, get_coo(), m_selections.draw_swizzle, {}, m_selections.draw_disable_blending };
}
int gui::get_selected_field()
{
     if (const int field = m_archives_group.find_field(starter_field()); field != -1)
     {
          return field;
     }
     return 0;
}
std::string gui::starter_field()
{
     return Configuration{}["starter_field"].value_or(std::string("ecenter3"));
}


void gui::combo_pupu()
{
     const auto &pair = m_map_sprite.uniques().pupu();
     if (generic_combo(
           get_imgui_id(),
           gui_labels::pupu_id,
           [&pair]() { return pair.values(); },
           [&pair]() { return pair.strings(); },
           [this]() -> auto & { return m_map_sprite.filter().pupu; }))
     {
          m_map_sprite.update_render_texture();
          m_changed = true;
     }
}

void gui::combo_draw_bit()
{
     using namespace std::string_view_literals;
     if (generic_combo(
           get_imgui_id(),
           gui_labels::draw_bit,
           []() {
                return std::array{ ff_8::draw_bitT::all, ff_8::draw_bitT::enabled, ff_8::draw_bitT::disabled };
           },
           []() {
                return std::array{ "all"sv, "enabled"sv, "disabled"sv };
           },
           [this]() -> auto & { return m_map_sprite.filter().draw_bit; }))
     {
          m_map_sprite.update_render_texture();
          m_changed = true;
     }
}
void gui::combo_filtered_palettes()
{
     const auto &map = m_map_sprite.uniques().palette();
     const auto &key = m_map_sprite.filter().bpp.value();
     if (map.contains(key))
     {
          const auto &pair = map.at(key);
          if (generic_combo(
                get_imgui_id(),
                gui_labels::palette,
                [&pair]() { return pair.values(); },
                [&pair]() { return pair.strings(); },
                [this]() -> auto & { return m_map_sprite.filter().palette; }))
          {
               m_map_sprite.update_render_texture();
               m_selections.palette = m_map_sprite.filter().palette.value();
               Configuration config{};
               config->insert_or_assign("selections_palette", m_selections.palette);
               config.save();
               m_changed = true;
          }
     }
}

void gui::combo_filtered_bpps()
{
     const auto &pair = m_map_sprite.uniques().bpp();
     if (generic_combo(
           get_imgui_id(),
           gui_labels::bpp,
           [&pair]() { return pair.values(); },
           [&pair]() { return pair.strings(); },
           [this]() -> auto & { return m_map_sprite.filter().bpp; }))
     {
          m_map_sprite.update_render_texture();
          m_selections.bpp = static_cast<int>(m_map_sprite.filter().bpp.value().raw() & 3U);
          Configuration config{};
          config->insert_or_assign("selections_bpp", m_selections.bpp);
          config.save();
          m_changed = true;
     }
}

void gui::combo_blend_modes()
{
     const auto &pair = m_map_sprite.uniques().blend_mode();
     if (generic_combo(
           get_imgui_id(),
           gui_labels::blend_mode,
           [&pair]() { return pair.values(); },
           [&pair]() { return pair.strings(); },
           [this]() -> auto & { return m_map_sprite.filter().blend_mode; }))
     {
          m_map_sprite.update_render_texture();
          m_changed = true;
     }
}

void gui::combo_layers()
{
     const auto &pair = m_map_sprite.uniques().layer_id();
     if (generic_combo(
           get_imgui_id(),
           gui_labels::layers,
           [&pair]() { return pair.values(); },
           [&pair]() { return pair.strings(); },
           [this]() -> auto & { return m_map_sprite.filter().layer_id; }))
     {
          m_map_sprite.update_render_texture();
          m_changed = true;
     }
}
void gui::combo_texture_pages()
{
     const auto &pair = m_map_sprite.uniques().texture_page_id();
     if (generic_combo(
           get_imgui_id(),
           gui_labels::texture_page,
           [&pair]() { return pair.values(); },
           [&pair]() { return pair.strings(); },
           [this]() -> auto & { return m_map_sprite.filter().texture_page_id; }))
     {
          m_map_sprite.update_render_texture();
          m_changed = true;
     }
}
void gui::combo_animation_ids()
{
     const auto &pair = m_map_sprite.uniques().animation_id();
     if (generic_combo(
           get_imgui_id(),
           gui_labels::animation_id,
           [&pair]() { return pair.values(); },
           [&pair]() { return pair.strings(); },
           [this]() -> auto & { return m_map_sprite.filter().animation_id; }))
     {
          m_map_sprite.update_render_texture();
          m_changed = true;
     }
}
void gui::combo_blend_other()
{
     const auto &pair = m_map_sprite.uniques().blend_other();
     if (generic_combo(
           get_imgui_id(),
           gui_labels::blend_other,
           [&pair]() { return pair.values(); },
           [&pair]() { return pair.strings(); },
           [this]() -> auto & { return m_map_sprite.filter().blend_other; }))
     {
          m_map_sprite.update_render_texture();
          m_changed = true;
     }
}
void gui::combo_compact_type(ff_8::filter_old<compact_type> &compact) const
{
     if (generic_combo(
           get_imgui_id(),
           gui_labels::compact,
           []() {
                return std::array{ compact_type::rows, compact_type::all };
           },
           []() {
                return std::array{ gui_labels::rows, gui_labels::all };
           },
           [&]() -> auto & { return compact; }))
     {
     }
}

void gui::combo_z()
{
     const auto &pair = m_map_sprite.uniques().z();
     if (generic_combo(
           get_imgui_id(),
           gui_labels::z,
           [&pair]() { return pair.values(); },
           [&pair]() { return pair.strings(); },
           [this]() -> auto & { return m_map_sprite.filter().z; }))
     {
          m_map_sprite.update_render_texture();
          m_changed = true;
     }
}

void gui::combo_animation_frames()
{
     const auto &map = m_map_sprite.uniques().animation_frame();
     const auto &key = m_map_sprite.filter().animation_id.value();
     if (map.contains(key))
     {
          const auto &pair = map.at(key);
          if (generic_combo(
                get_imgui_id(),
                gui_labels::animation_frame,
                [&pair]() { return pair.values(); },
                [&pair]() { return pair.strings(); },
                [this]() -> auto & { return m_map_sprite.filter().animation_frame; }))
          {
               m_map_sprite.update_render_texture();
               m_changed = true;
          }
     }
}
BPPT gui::bpp() const
{
     return Mim::bpp_selections().at(static_cast<size_t>(m_selections.bpp));
}
void gui::combo_deswizzle_path()
{
     if (const safedir deswizzle_texture_path = m_loaded_deswizzle_texture_path; !deswizzle_texture_path.is_exists() || !m_field)
     {
          return;
     }
     // std::vector<std::filesystem::path> values = {
     // m_loaded_deswizzle_texture_path };
     std::vector<std::string> strings = { m_loaded_deswizzle_texture_path.string() };
     if (generic_combo(
           get_imgui_id(),
           gui_labels::deswizzle_path,
           //[&values]() { return values; },
           [&strings]() { return strings; },
           [&strings]() { return strings; },
           [this]() -> auto & { return m_map_sprite.filter().deswizzle; }))
     {
          if (m_map_sprite.filter().deswizzle.enabled())
          {
               m_map_sprite.filter().upscale.disable();
          }
          m_map_sprite.update_render_texture(true);
          m_changed = true;
     }
}
void gui::combo_upscale_path()
{
     if (!m_field)
     {
          return;
     }
     if (!combo_upscale_path(m_map_sprite.filter().upscale))
     {
          return;
     }
     if (m_map_sprite.filter().upscale.enabled())
     {
          m_map_sprite.filter().deswizzle.disable();
     }
     m_map_sprite.update_render_texture(true);
     m_changed = true;
}
void gui::generate_upscale_paths(const std::string &field_name, open_viii::LangT coo)
{
     m_upscale_paths.clear();
     auto transform_paths = m_paths
                            | std::views::transform([](const toml::node &item) -> std::string { return item.value_or(std::string{}); })
                            | std::views::transform([this, &field_name, &coo](const std::string &path) {
                                   if (m_field)
                                   {
                                        return upscales(std::filesystem::path(path), field_name, coo).get_paths();
                                   }
                                   return upscales{}.get_paths();
                              });
     // std::views::join; broken in msvc.
     auto process = [this](const auto &temp_paths) {
          auto filter_paths = temp_paths | std::views::filter([](safedir path) { return path.is_exists() && path.is_dir(); });
          for (auto &path : filter_paths)
          {
               m_upscale_paths.emplace_back(path.string());
          }
     };
     for (auto temp_paths : transform_paths)
     {
          process(temp_paths);
     }
     if (safedir(m_loaded_swizzle_texture_path).is_exists())
     {
          m_upscale_paths.push_back(m_loaded_swizzle_texture_path.string());
     }
     if (m_field)
     {
          process(upscales(std::filesystem::current_path(), field_name, coo).get_paths());
          for (const auto &upscale_path : m_custom_upscale_paths)
          {
               process(upscales(upscale_path.value_or(std::string{}), field_name, coo).get_paths());
          }
     }
     std::ranges::sort(m_upscale_paths);
     const auto to_remove = std::ranges::unique(m_upscale_paths);
     m_upscale_paths.erase(to_remove.begin(), to_remove.end());
}
bool gui::combo_upscale_path(ff_8::filter_old<std::filesystem::path> &filter) const
{
     return m_field
            && generic_combo(
              get_imgui_id(),
              gui_labels::upscale_path,
              [this]() { return m_upscale_paths; },
              [this]() { return m_upscale_paths; },
              [&filter]() -> auto & { return filter; });
}

bool gui::combo_upscale_path(std::filesystem::path &path, const std::string &field_name, open_viii::LangT coo) const
{
     std::vector<std::string> paths           = {};
     auto                     transform_paths = m_paths
                            | std::views::transform([](const toml::node &item) -> std::string { return item.value_or(std::string{}); })
                            | std::views::transform([this, &field_name, &coo](const std::string &in_path) {
                                   if (m_field)
                                   {
                                        return upscales(std::filesystem::path(in_path), field_name, coo).get_paths();
                                   }
                                   return upscales{}.get_paths();
                              });
     // std::views::join; broken in msvc.
     auto process = [&paths](const auto &temp_paths) {
          auto filter_paths = temp_paths | std::views::filter([](safedir in_path) { return in_path.is_exists() && in_path.is_dir(); });
          for (auto &in_path : filter_paths)
          {
               paths.emplace_back(in_path.string());
          }
     };
     for (auto temp_paths : transform_paths)
     {
          process(temp_paths);
     }
     if (safedir(m_loaded_swizzle_texture_path).is_exists())
     {
          paths.push_back(m_loaded_swizzle_texture_path.string());
     }
     if (m_field)
     {
          process(upscales(std::filesystem::current_path(), field_name, coo).get_paths());

          if (generic_combo(
                get_imgui_id(), gui_labels::upscale_path, [&paths]() { return paths; }, [&paths]() { return paths; }, path))
          {
               return true;
          }
     }
     return false;
}

std::vector<std::filesystem::path> gui::find_maps_in_directory(const std::filesystem::path &src, size_t reserve)
{
     std::vector<std::filesystem::path> paths{};
     paths.reserve(reserve);
     for (const auto &path : std::filesystem::recursive_directory_iterator{ src })
     {
          if (
            path.path().has_extension()
            && open_viii::tools::i_ends_with(path.path().extension().string(), open_viii::graphics::background::Map::EXT))
          {
               paths.emplace_back(path.path());
          }
     }
     return paths;
}


//[[nodiscard]] inline bool any_matches(
//  const std::vector<std::filesystem::path>        &paths,
//  const std::vector<open_viii::archive::FileData> &all_file_data)
//{
//  return std::ranges::any_of(
//    all_file_data, [&paths](const open_viii::archive::FileData &file_data) {
//      const auto in_path = file_data.get_path();
//      return in_path.has_filename()
//             && open_viii::archive::any_matches(paths, in_path);
//    });
//}
template<bool Nested>
std::vector<std::filesystem::path>
  gui::replace_entries(const open_viii::archive::FIFLFS<Nested> &field, const std::vector<std::filesystem::path> &paths) const
{
     auto tmp = open_viii::archive::replace_files<Nested>(field, paths);
     return tmp;
}

void gui::popup_batch_embed()
{
     if (m_batch_embed(m_archives_group.mapdata()))
     {
          m_batch_embed2.enable({}, m_batch_embed.start_time());
     }
     else if (m_batch_embed2(std::array{ "fields" }))
     {
          if (open_viii::archive::fiflfs_in_main_zzz(m_archives_group.archives()))
          {
               m_batch_embed3.enable({}, m_batch_embed2.start_time());
          }
          else
          {
               m_batch_embed4.enable(m_selections.path, m_batch_embed2.start_time());
          }
     }
     else if (m_batch_embed3(std::array{ "fields" }))
     {
          m_batch_embed4.enable(m_selections.path, m_batch_embed3.start_time());
     }
     else if (m_batch_embed4(std::array{ "save" }))
     {
          if (open_viii::archive::fiflfs_in_main_zzz(m_archives_group.archives()))
          {
          }
     }
}

bool gui::check_futures()
{
     const auto removal = std::ranges::remove_if(m_futures, [](const std::future<void> &future) {
          return !future.valid() || future.wait_for(std::chrono::seconds{}) == std::future_status::ready;
     });
     m_futures.erase(removal.begin(), removal.end());
     return !std::ranges::empty(m_futures);
}
void gui::batch_ops_ask_menu() const
{
#if 1
     if (m_selections.test_batch_window)
     {
          static gui_batch test{ m_archives_group };
          test(&get_imgui_id());
     }
#else
     using namespace std::string_view_literals;
     if (ImGui::Begin("Batch Operations", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
     {
          if (ImGui::CollapsingHeader("Source", ImGuiTreeNodeFlags_DefaultOpen))
          {
               static int            selected_src_type = {};

               static constexpr auto src_types         = std::array{ "Fields Archive"sv, "Upscales or Swizzles"sv, "Deswizzles"sv };
               generic_combo(
                 get_imgui_id(),
                 "Type",
                 []() { return std::views::iota(0, static_cast<int>(std::ranges::ssize(src_types))); },
                 []() { return src_types; },
                 selected_src_type);
               static std::filesystem::path selected_src_swizzle_path   = {};
               static std::filesystem::path selected_src_deswizzle_path = {};
               if (selected_src_type == 0)
               {
                    combo_path();
                    {
                         const auto sg = PushPop();
                         if (ImGui::Button("Browse"))
                         {
                              open_locate_ff8_filebrowser();
                         }
                    }
               }
               else if (selected_src_type == 1)
               {
                    if (combo_upscale_path(selected_src_swizzle_path, "", {}))
                    {
                    }
                    {
                         const auto sg = PushPop();
                         if (ImGui::Button("Browse"))
                         {
                              open_swizzle_filebrowser();
                         }
                    }
               }
               else if (selected_src_type == 2)
               {
                    format_imgui_text("Directory: {}", selected_src_deswizzle_path);
                    {
                         const auto sg = PushPop();
                         if (ImGui::Button("Browse"))
                         {
                              open_deswizzle_filebrowser();
                         }
                    }
               }
          }
          static constexpr auto       task_types      = std::array{ "None"sv, "Deswizzle"sv, "Swizzle"sv };
          static int                  task_type       = {};
          static filter<compact_type> compact         = {};
          static bool                 flatten_bpp     = {};
          static bool                 flatten_palette = {};
          if (ImGui::CollapsingHeader("Transformation", ImGuiTreeNodeFlags_DefaultOpen))
          {
               generic_combo(
                 get_imgui_id(),
                 "Task"sv,
                 []() { return std::views::iota(0, static_cast<int>(std::ranges::ssize(task_types))); },
                 []() { return task_types; },
                 task_type);
               combo_compact_type(compact);
               format_imgui_text("Flatten: ");
               ImGui::SameLine();
               ImGui::Checkbox("BPP", &flatten_bpp);
               ImGui::SameLine();
               ImGui::Checkbox("Palette", &flatten_palette);
               ImGui::Separator();
          }
          if (task_type != 0 && ImGui::CollapsingHeader("Destination", ImGuiTreeNodeFlags_DefaultOpen))
          {
               static std::filesystem::path selected_dst_path{};
               format_imgui_text("Directory: {}", selected_dst_path);
               {
                    const auto sg = PushPop();
                    if (ImGui::Button("Browse"))
                    {
                    }
               }
          }
          if (ImGui::CollapsingHeader("Embed .map(s) into archives", ImGuiTreeNodeFlags_DefaultOpen))
          {
               static bool embed_maps   = {};
               static bool reload_after = { true };
               {
                    const auto sg = PushPop();
                    if (ImGui::Checkbox("", &embed_maps))
                    {
                    }
               }
               if (embed_maps)
               {
                    ImGui::SameLine();
                    combo_path();
                    {
                         const auto sg = PushPop();
                         if (ImGui::Checkbox("Reload after?", &reload_after))
                         {
                         }
                    }
               }
          }
          ImGui::Separator();
          {
               const auto sg = PushPop();
               if (ImGui::Button("Start"))
               {
               }
          }
          ImGui::SameLine();
          {
               const auto sg = PushPop();
               if (ImGui::Button("Cancel"))
               {
               }
          }
     }
     ImGui::End();
#endif
}

gui::variant_tile_t &gui::combo_selected_tile(bool &changed)
{
     const auto end_action = scope_guard(
       [&changed, current_tile_id = m_selections.selected_tile, this]() { changed = current_tile_id != m_selections.selected_tile; });
     // combo box with all the tiles.
     static std::string    current_item_str = {};
     static variant_tile_t current_tile     = { std::monostate{} };
     find_selected_tile_for_import(current_tile, current_item_str);


     ImVec2 const combo_pos    = ImGui::GetCursorScreenPos();
     const auto   the_end_id_0 = PushPop();
     static bool  was_hovered  = false;
     if (ImGui::BeginCombo("Select Existing Tile", "", ImGuiComboFlags_HeightLargest))
     {
          const auto the_end_combo = scope_guard([]() { ImGui::EndCombo(); });
          m_map_sprite.const_visit_tiles([this](const auto &tiles) {
               for (int tile_id{}; const auto &tile : tiles)
               {
                    const auto the_end_id_1 = PushPop();
                    const auto iterate      = scope_guard([&tile_id]() { ++tile_id; });
                    bool       is_selected  = (m_selections.selected_tile == tile_id);// You can store your selection however you
                                                                               // want, outside or inside your objects
                    if (std::ranges::any_of(
                          std::array{ [&is_selected, this, &tile, &tile_id]() -> bool {
                                          bool const selected = ImGui::Selectable("", is_selected);
                                          if (ImGui::IsItemHovered())
                                          {
                                               ImGui::BeginTooltip();
                                               const auto end_tooltip = scope_guard(&ImGui::EndTooltip);
                                               format_imgui_text("{}", tile_id);
                                               constexpr float tile_size = 256.F;
                                               (void)create_tile_button(tile, sf::Vector2f(tile_size, tile_size));
                                               m_map_sprite.enable_square(tile);
                                               was_hovered = true;
                                          }
                                          return selected;
                                     }(),
                                      []() -> bool {
                                           ImGui::SameLine();
                                           return false;
                                      }(),
                                      create_tile_button(tile),
                                      []() -> bool {
                                           ImGui::SameLine();
                                           return false;
                                      }(),
                                      [&tile_id]() -> bool {
                                           format_imgui_text("{}", tile_id);
                                           return false;
                                      }() },
                          std::identity{}))
                    {
                         m_selections.selected_tile = tile_id;
                         Configuration config{};
                         config->insert_or_assign("selections_selected_tile", m_selections.selected_tile);
                         config.save();
                         current_item_str = fmt::format("{}", tile_id);
                         current_tile     = tile;
                    }
                    if (is_selected)
                    {
                         ImGui::SetItemDefaultFocus();// You may set the initial focus when
                                                      // opening the combo (scrolling + for
                                                      // keyboard navigation support)
                    }
               }
          });
     }
     else if (was_hovered)
     {
          was_hovered = false;
          m_map_sprite.disable_square();
     }
     ImVec2 const      backup_pos = ImGui::GetCursorScreenPos();
     ImGuiStyle const &style      = ImGui::GetStyle();
     ImGui::SetCursorScreenPos(ImVec2(combo_pos.x + style.FramePadding.x, combo_pos.y + style.FramePadding.y));
     (void)std::visit(
       [this](const auto &tile) -> bool {
            if constexpr (!std::is_same_v<std::decay_t<decltype(tile)>, std::monostate>)
            {
                 return create_tile_button(tile);
            }
            else
            {
                 return false;
            }
       },
       current_tile);
     ImGui::SameLine();
     format_imgui_text("{}", current_item_str);
     ImGui::SetCursorScreenPos(backup_pos);
     return current_tile;
}
void gui::find_selected_tile_for_import(gui::variant_tile_t &current_tile, std::string &current_item_str) const
{
     m_map_sprite.const_visit_tiles([&](const auto &tiles) {
          if (m_selections.selected_tile < 0 || std::cmp_greater_equal(m_selections.selected_tile, tiles.size()))
          {
               current_tile = std::monostate{};
               return;
          }
          std::visit(
            [&](const auto &tile) {
                 if (std::cmp_less(m_selections.selected_tile, tiles.size()))
                 {
                      const auto &tmp_tile = tiles[static_cast<size_t>(m_selections.selected_tile)];
                      if constexpr (std::is_same_v<std::decay_t<decltype(tile)>, std::decay_t<decltype(tmp_tile)>>)
                      {
                           if (tile != tmp_tile)
                           {
                                current_tile     = tmp_tile;
                                current_item_str = fmt::format("{}", m_selections.selected_tile);
                           }
                      }
                      else if constexpr (!is_tile<std::decay_t<decltype(tile)>>)
                      {
                           current_tile     = tmp_tile;
                           current_item_str = fmt::format("{}", m_selections.selected_tile);
                      }
                 }
            },
            current_tile);
     });
}
void gui::begin_batch_embed_map_warning_window()
{
     if (!m_selections.batch_embed_map_warning_window)
     {
          return;
     }
     // begin imgui window
     const auto             pop_id     = PushPop();
     static constexpr float min_width  = 500.F;
     static constexpr float min_height = 150.F;
     static constexpr float half       = 0.5F;
     ImGui::SetNextWindowSizeConstraints(
       ImVec2{ min_width, min_height }, ImVec2{ std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() });
     ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(half, half));
     static const char *title = "Batch embed '.map' files.";
     ImGui::OpenPopup(title);
     if (!ImGui::BeginPopupModal(
           title, nullptr, bitwise_or(ImGuiWindowFlags{}, ImGuiWindowFlags_AlwaysAutoResize, ImGuiWindowFlags_NoSavedSettings)))
     {
          return;
     }
     const auto end_popup = scope_guard([]() { ImGui::EndPopup(); });
     format_imgui_wrapped_text(
       "This will take the currently loaded Final Fantasy 8 archive in \"{}\" "
       "and create new `field.fi`, `field.fl`, and `field.fs` files. "
       "Replacing any `.map` file with ones found in the chosen path.",
       m_selections.path);
     if (ImGui::Button("Browse to begin..."))
     {
          browse_for_embed_map_dir();
          ImGui::CloseCurrentPopup();
          m_selections.batch_embed_map_warning_window = false;
     }
     ImGui::SameLine();
     if (ImGui::Button("Close"))
     {
          ImGui::CloseCurrentPopup();
          m_selections.batch_embed_map_warning_window = false;
     }
}
void gui::import_image_window()
{
     if (!m_selections.display_import_image)
     {
          return;
     }
     // begin imgui window
     const auto the_end = scope_guard([]() { ImGui::End(); });
     if (!ImGui::Begin("Import Image", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
     {
          return;
     }
     bool                         changed      = false;
     //   * So I need to choose an existing tile to base the new tiles on.
     [[maybe_unused]] const auto &current_tile = combo_selected_tile(changed);
     // add text showing the tile's info.
     collapsing_tile_info(current_tile);
     //   * I need to browse for an image file.
     changed = browse_for_image_display_preview() || changed;
     //   * We need to adjust the scale to fit
     // maybe i can just create an imgui window filled with the image
     // scale the image to be the selected tile size. 16,32,64,128,256.
     changed = combo_tile_size() || changed;
     //   * We need to adjust the position
     // have a px offset? or something?
     generate_map_for_imported_image(current_tile, changed);
     collapsing_header_generated_tiles();
     // I need to detect the last used texture page and the highest source_y.
     m_map_sprite.const_visit_tiles([this, &changed](const auto &tiles) {
          if (std::ranges::empty(tiles))
          {
               return;
          }
          const auto [next_source_y, next_texture_page] = get_next_unused_y_and_texture_page(tiles);
          if (changed)
          {
               adjust_source_xy_texture_page_for_import_map(next_source_y, next_texture_page);
               update_scaled_up_render_texture();
               update_imported_render_texture();
          }
     });
     // I think I have the new tiles coords generated correctly for the swizzle and
     // for drawing in game. just need to render it into an image and merge the new
     // tiles into the `.map`
     //    * Then we can swap between swizzle and deswizzle views to show what they
     //    look like
     //    * At the end we need to be able to save and merge them with the '.map'
     //    file.
     //    * They'll probably insert before the last tile.
     //  Save button that'll save the swizzled images with new '.map'
     if (ImGui::Button("Save Swizzle"))
     {
          save_swizzle_textures();
     }
     // have a cancel button to hide window.
     ImGui::SameLine();
     if (ImGui::Button("Cancel"))
     {
          // hide window and save that it's hidden.
          m_selections.display_import_image = false;
          Configuration config{};
          config->insert_or_assign("selections_display_import_image", m_selections.display_import_image);
          config.save();
          reset_imported_image();
     }
     // have a reset button to reset window state?
     ImGui::SameLine();
     if (ImGui::Button("Reset"))
     {
          reset_imported_image();
     }
}
void gui::adjust_source_xy_texture_page_for_import_map(uint8_t next_source_y, const uint8_t next_texture_page)
{
     import_image_map.visit_tiles([&next_texture_page, &next_source_y](auto &&import_tiles) {
          auto       tile_i   = import_tiles.begin();
          const auto tile_end = import_tiles.end();
          for (uint8_t tp = next_texture_page; tp < tile_size_px; ++tp)
          {
               for (uint8_t pixel_y = next_source_y; pixel_y < tile_size_px; ++pixel_y)
               {
                    next_source_y = 0;
                    for (uint8_t pixel_x = 0; pixel_x < tile_size_px; ++pixel_x)
                    {
                         if (tile_i == tile_end)
                         {
                              return;
                         }
                         *tile_i = tile_i->with_source_xy(pixel_x * tile_size_px, pixel_y * tile_size_px).with_texture_id(tp);
                         ++tile_i;
                    }
               }
          }
     });
}
void gui::collapsing_header_generated_tiles() const
{
     if (ImGui::CollapsingHeader(
           import_image_map.visit_tiles([](auto &&tiles) { return fmt::format("Generated Tiles: {}", std::size(tiles)); }).c_str()))
     {
          static constexpr int columns = 9;
          if (ImGui::BeginTable("import_tiles_table", columns))
          {
               const auto the_end_tile_table = scope_guard([]() { ImGui::EndTable(); });
               import_image_map.visit_tiles([this](auto &tiles) {
                    for (const auto &tile : tiles)
                    {
                         ImGui::TableNextColumn();
                         sf::Sprite const sprite(
                           loaded_image_texture,
                           sf::IntRect(
                             static_cast<int>(tile.x() / tile_size_px * m_selections.tile_size_value),
                             static_cast<int>(tile.y() / tile_size_px * m_selections.tile_size_value),
                             static_cast<int>(m_selections.tile_size_value),
                             static_cast<int>(m_selections.tile_size_value)));
                         const auto             the_end_tile_table_tile = PushPop();
                         static constexpr float button_size             = 32.F;
                         ImGui::ImageButton(sprite, sf::Vector2f(button_size, button_size), 0);
                    }
               });
          }
     }
}
void gui::generate_map_for_imported_image(const variant_tile_t &current_tile, bool changed)
{//   * I'd probably store the new tiles in their own map.
     const auto tiles_wide =
       static_cast<uint32_t>(ceil(static_cast<float>(loaded_image_texture.getSize().x) / static_cast<float>(m_selections.tile_size_value)));
     const auto tiles_high =
       static_cast<uint32_t>(ceil(static_cast<float>(loaded_image_texture.getSize().y) / static_cast<float>(m_selections.tile_size_value)));
     format_imgui_text("Possible Tiles: {} wide, {} high, {} total", tiles_wide, tiles_high, tiles_wide * tiles_high);
     if (changed && tiles_wide * tiles_high != 0U && loaded_image_texture.getSize() != sf::Vector2u{})
     {
          import_image_map = open_viii::graphics::background::Map(
            [&current_tile, x_tile = uint8_t{}, y_tile = uint8_t{}, &tiles_high, &tiles_wide]() mutable {
                 return std::visit(
                   [&](auto tile) -> variant_tile_t {
                        if constexpr (is_tile<std::decay_t<decltype(tile)>>)
                        {
                             if (x_tile == tiles_wide)
                             {
                                  x_tile = 0;
                                  ++y_tile;
                             }
                             if (y_tile == tiles_high)
                             {
                                  return std::monostate{};
                             }
                             //   * Set new tiles to 4 bit to get max amount of tiles.
                             tile =
                               tile.with_depth(BPPT::BPP4_CONST())
                                 .with_source_xy({ static_cast<uint8_t>(x_tile * tile_size_px_unsigned),
                                                   static_cast<uint8_t>(y_tile * tile_size_px_unsigned) })
                                 .with_xy({ static_cast<int16_t>(x_tile * tile_size_px), static_cast<int16_t>(y_tile * tile_size_px) });

                             // iterate
                             ++x_tile;
                             return tile;
                        }
                        else
                        {
                             return std::monostate{};
                        }
                   },
                   current_tile);
            });
          filter_empty_import_tiles();
     }
}
void gui::filter_empty_import_tiles()
{//* Filter empty tiles
     loaded_image_cpu = loaded_image_texture.copyToImage();
     import_image_map.visit_tiles([this](auto &tiles) {
          const auto rem_range = std::ranges::remove_if(tiles, [this](const auto &tile) -> bool {
               const auto          x_start = tile.x() / tile_size_px * m_selections.tile_size_value;
               const auto          y_start = tile.y() / tile_size_px * m_selections.tile_size_value;
               const int           x_max   = x_start + m_selections.tile_size_value;
               const sf::Vector2u &imgsize = loaded_image_cpu.getSize();
               const auto          x_end   = (std::min)(static_cast<int>(imgsize.x), x_max);
               const int           y_max   = y_start + m_selections.tile_size_value;
               const auto          y_end   = (std::min)(static_cast<int>(imgsize.y), y_max);
               for (auto pixel_x = x_start; std::cmp_less(pixel_x, x_end); ++pixel_x)
               {
                    for (auto pixel_y = y_start; std::cmp_less(pixel_y, y_end); ++pixel_y)
                    {
                         const auto color =
                           loaded_image_cpu.getPixel(static_cast<unsigned int>(pixel_x), static_cast<unsigned int>(pixel_y));
                         if (std::cmp_greater(color.a, 0U))
                         {
                              return false;
                         }
                    }
               }
               return true;
          });
          tiles.erase(rem_range.begin(), rem_range.end());
     });
     loaded_image_cpu = {};
}
void gui::reset_imported_image()
{
     m_map_sprite.update_render_texture(nullptr, {}, tile_sizes::default_size);
     import_image_map                   = {};
     loaded_image_texture               = {};
     loaded_image_cpu                   = {};
     m_import_image_path                = {};
     m_selections.render_imported_image = false;
     Configuration config{};
     config->insert_or_assign("selections_render_imported_image", m_selections.render_imported_image);
     config.save();
}
bool gui::combo_tile_size()
{
     if (!generic_combo(
           get_imgui_id(),
           std::string_view("Tile Size"),
           []() -> decltype(auto) {
                static constexpr auto sizes = std::array{
                     tile_sizes::default_size, tile_sizes::x_2_size, tile_sizes::x_4_size, tile_sizes::x_8_size, tile_sizes::x_16_size
                };
                return sizes;
           },
           []() -> decltype(auto) {
                static constexpr auto size_strings = std::array{ std::string_view{ " 1x  16 px" },
                                                                 std::string_view{ " 2x  32 px" },
                                                                 std::string_view{ " 4x  64 px" },
                                                                 std::string_view{ " 8x 128 px" },
                                                                 std::string_view{ "16x 256 px" } };
                return size_strings;
           },
           m_selections.tile_size_value))
     {
          return false;
     }
     Configuration config{};
     config->insert_or_assign("selections_tile_size_value", static_cast<std::underlying_type_t<tile_sizes>>(m_selections.tile_size_value));
     config.save();
     return true;
}
bool gui::browse_for_image_display_preview()
{
     bool changed = false;
     ImGui::InputText("##image_path", m_import_image_path.data(), m_import_image_path.size(), ImGuiInputTextFlags_ReadOnly);
     ImGui::SameLine();
     if (ImGui::Button("Browse"))
     {
          m_load_file_browser.Open();
          m_load_file_browser.SetTitle("Load Image File...");
          m_load_file_browser.SetTypeFilters({ ".png" });
          m_load_file_browser.SetPwd(Configuration{}["load_image_path"].value_or(std::filesystem::current_path().string()));
          m_load_file_browser.SetInputName(m_import_image_path.data());
     }
     m_load_file_browser.Display();

     if (m_load_file_browser.HasSelected())
     {
          Configuration config{};
          config->insert_or_assign("load_image_path", m_load_file_browser.GetPwd().string());
          config.save();
          [[maybe_unused]] const auto selected_path = m_load_file_browser.GetSelected();
          m_import_image_path                       = selected_path.string();
          m_load_file_browser.ClearSelected();
          loaded_image_texture.loadFromFile(m_import_image_path);// stored on gpu.
          loaded_image_texture.setRepeated(false);
          loaded_image_texture.setSmooth(false);
          loaded_image_texture.generateMipmap();
          changed = true;
     }
     if (loaded_image_texture.getSize().x == 0 || loaded_image_texture.getSize().y == 0)
     {
          return false;
     }
     if (ImGui::CollapsingHeader("Selected Image Preview"))
     {
          sf::Sprite const sprite(loaded_image_texture);
          const float      width             = std::max((ImGui::GetContentRegionAvail().x), 1.0F);
          const auto       size              = loaded_image_texture.getSize();

          float const      scale             = width / static_cast<float>(size.x);
          const float      height            = static_cast<float>(size.y) * scale;
          ImVec2 const     cursor_screen_pos = ImGui::GetCursorScreenPos();
          const auto       pop_id            = PushPop();
          ImGui::ImageButton(sprite, sf::Vector2f(width, height), 0);
          if (ImGui::Checkbox("Draw Grid", &m_selections.import_image_grid))
          {
               Configuration config{};
               config->insert_or_assign("selections_import_image_grid", m_selections.import_image_grid);
               config.save();
          }
          if (m_selections.import_image_grid)
          {
               static constexpr float thickness = 2.0F;
               static const ImU32     color_32  = imgui_color32(sf::Color::Red);
               for (auto x_pos = static_cast<std::uint32_t>(m_selections.tile_size_value); x_pos < size.x;
                    x_pos += static_cast<std::underlying_type_t<tile_sizes>>(m_selections.tile_size_value))
               {
                    ImGui::GetWindowDrawList()->AddLine(
                      ImVec2(cursor_screen_pos.x + (static_cast<float>(x_pos) * scale), cursor_screen_pos.y),
                      ImVec2(
                        cursor_screen_pos.x + (static_cast<float>(x_pos) * scale),
                        cursor_screen_pos.y + (static_cast<float>(size.y) * scale)),
                      color_32,
                      thickness);
               }

               for (auto y_pos = static_cast<std::uint32_t>(m_selections.tile_size_value); y_pos < size.y;
                    y_pos += static_cast<std::underlying_type_t<tile_sizes>>(m_selections.tile_size_value))
               {
                    ImGui::GetWindowDrawList()->AddLine(
                      ImVec2(cursor_screen_pos.x, cursor_screen_pos.y + (static_cast<float>(y_pos) * scale)),
                      ImVec2(
                        cursor_screen_pos.x + (static_cast<float>(size.x) * scale),
                        cursor_screen_pos.y + (static_cast<float>(y_pos) * scale)),
                      color_32,
                      thickness);
               }
          }
     }
     return changed;
}
void gui::update_scaled_up_render_texture()
{
     const auto scale_up_dim = [this](uint32_t dim) {
          return static_cast<uint32_t>(
            ceil(static_cast<double>(dim) / static_cast<double>(m_selections.tile_size_value))
            * static_cast<double>(m_selections.tile_size_value));
     };
     const auto size = loaded_image_texture.getSize();
     if (size == decltype(size){})
     {
          return;
     }
     loaded_image_render_texture.create(scale_up_dim(size.x), scale_up_dim(size.y));
     loaded_image_render_texture.setActive(true);
     loaded_image_render_texture.clear(sf::Color::Transparent);
     sf::Sprite sprite = sf::Sprite(loaded_image_texture);
     sprite.setScale(1.F, -1.F);
     sprite.setPosition(0.F, static_cast<float>(loaded_image_render_texture.getSize().y));
     loaded_image_render_texture.draw(sprite);
     loaded_image_render_texture.setRepeated(false);
     loaded_image_render_texture.setSmooth(false);
     loaded_image_render_texture.generateMipmap();
}
void gui::collapsing_tile_info(const variant_tile_t &current_tile) const
{
     std::visit(
       [this](const auto &tile) {
            if constexpr (is_tile<std::decay_t<decltype(tile)>>)
            {
                 if (ImGui::CollapsingHeader("Selected Tile Info"))
                 {
                      if (ImGui::BeginTable("table_tile_info", 2))
                      {
                           const auto the_end_table = scope_guard([]() { ImGui::EndTable(); });
                           m_map_sprite.format_tile_text(tile, [](const std::string_view text, const auto value) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                format_imgui_text("{}", text.data());
                                ImGui::TableNextColumn();
                                format_imgui_text("{}", value);
                           });
                      }

                      static constexpr float width_max             = 1.0F;
                      static constexpr float half                  = 0.5F;
                      const auto             width                 = std::max((ImGui::GetContentRegionAvail().x), width_max) * half;
                      ImVec2 const           backup_pos            = ImGui::GetCursorScreenPos();
                      ImGuiStyle const      &style                 = ImGui::GetStyle();
                      static constexpr float position_width_scale  = 1.1F;
                      static constexpr float position_height_scale = 0.9F;
                      static constexpr float padding_height_scale  = 2.0F;
                      static constexpr float tile_scale            = 0.9F;
                      ImGui::SetCursorScreenPos(ImVec2(
                        backup_pos.x + width * position_width_scale,
                        backup_pos.y - width * position_height_scale - style.FramePadding.y * padding_height_scale));
                      (void)create_tile_button(tile, { width * tile_scale, width * tile_scale });
                      ImGui::SetCursorScreenPos(backup_pos);
                 }
            }
       },
       current_tile);
}
}// namespace fme