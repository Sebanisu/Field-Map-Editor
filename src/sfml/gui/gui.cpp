//
// Created by pcvii on 9/7/2021.
//
#include "gui.hpp"
#include "as_string.hpp"
#include "collapsing_tile_info.hpp"
#include "create_tile_button.hpp"
#include "EmptyStringIterator.hpp"
#include "gui_labels.hpp"
#include "imgui_color.hpp"
#include "open_file_explorer.hpp"
#include "push_pop_id.hpp"
#include "safedir.hpp"
#include "tool_tip.hpp"
#include <open_viii/paths/Paths.hpp>
#include <SFML/Window/Mouse.hpp>
#include <utility>

static void DebugCallback(
  [[maybe_unused]] GLenum      source,
  [[maybe_unused]] GLenum      type,
  [[maybe_unused]] GLuint      id,
  GLenum                       severity,
  [[maybe_unused]] GLsizei     length,
  const GLchar                *message,
  [[maybe_unused]] const void *userParam)
{
     switch (severity)
     {
          case GL_DEBUG_SEVERITY_HIGH:
               spdlog::error("OpenGL high: {}", message);
               throw;
               break;
          case GL_DEBUG_SEVERITY_MEDIUM:
               spdlog::debug("OpenGL medium: {}", message);
               break;
          case GL_DEBUG_SEVERITY_LOW:
               spdlog::debug("OpenGL low: {}", message);
               break;
          case GL_DEBUG_SEVERITY_NOTIFICATION:
          default:
               spdlog::debug("OpenGL notification: {}", message);
               break;
     }
}
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
          scale_window(static_cast<float>(m_selections->window_width), static_cast<float>(m_selections->window_height));
          (void)icons_font();
          do
          {
               m_changed      = false;
               get_imgui_id() = {};
               loop_events();
               m_elapsed_time                          = m_delta_clock.restart();

               static constexpr float scroll_time_fast = 4000.F;
               static constexpr float scroll_time_slow = 1000.F;
               m_scrolling.total_scroll_time[0] =
                 m_selections->draw_swizzle || (!m_selections->draw_palette && mim_test()) ? scroll_time_fast : scroll_time_slow;
               if (m_scrolling.scroll(xy, m_elapsed_time))
               {
                    m_changed                     = false;
                    m_mouse_positions.mouse_moved = true;
               }
               ImGui::SFML::Update(m_window, m_elapsed_time);
               m_batch.update(m_elapsed_time);
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
     (void)ImGui::Begin("##DockSpace Demo", nullptr, window_flags);
     if constexpr (!opt_padding)
     {
          ImGui::PopStyleVar();
     }

     if constexpr (opt_fullscreen)
     {
          ImGui::PopStyleVar(2);
     }

     dockspace_flags                            = bitwise_or(dockspace_flags, ImGuiDockNodeFlags_PassthruCentralNode);
     dockspace_flags                            = bitwise_and(dockspace_flags, bitwise_not(ImGuiDockNodeFlags_NoResize));
     // Submit the DockSpace
     ImGuiIO &imgui_io                          = ImGui::GetIO();
     imgui_io.ConfigWindowsMoveFromTitleBarOnly = true;
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
     if (!m_selections->display_control_panel_window)
     {
          return;
     }
     const auto imgui_end = scope_guard(&ImGui::End);
     if (!ImGui::Begin(gui_labels::control_panel.data()))
     {
          // m_mouse_positions.mouse_enabled = handle_mouse_cursor();
          return;
     }
     // ImGui::PushFont(icons_font());
     // ImGui::Button(ICON_FA_MAGNIFYING_GLASS " Search");
     // ImGui::PopFont();
     //    if (m_first)
     //    {
     //      ImGui::SetWindowPos({ 0U, 0U });
     //    }
     background_color_picker();
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
     frame_rate();

     m_map_sprite->const_visit_working_tiles([&](const auto &tiles) {
          if (!ImGui::CollapsingHeader("Conflicting Tiles"))
          {
               return;
          }

          const auto &conflicts          = m_map_sprite->working_conflicts();
          auto        range_of_conflicts = conflicts.range_of_conflicts();
          for (const auto &conflict_group : range_of_conflicts)
          {
               const auto location = ff_8::source_tile_conflicts::reverse_index(conflict_group.front());
               format_imgui_text("X {}, Y {}, T {}: ", location.x, location.y, location.t);
               for (const auto index : conflict_group)
               {
                    //assert(cmp_less(index, std::ranges::size(tiles)) && "index out of range!");
                    const auto &tile = [&]() {
                         auto begin = std::ranges::cbegin(tiles);
                         std::ranges::advance(begin, index);
                         return *begin;
                    }();
                    const auto push_pop_id = PushPopID();
                    (void)create_tile_button(*m_map_sprite, tile, { 32.F, 32.F });
                    // Ensure subsequent buttons are on the same row
                    std::string strtooltip = fmt::format("Index {}\n{}", index, tile);
                    tool_tip(strtooltip);
                    ImGui::SameLine();
               }
               // Break the line after finishing a conflict group
               ImGui::NewLine();
          }
     });

     m_mouse_positions.mouse_enabled = handle_mouse_cursor();
     text_mouse_position();

     if (std::ranges::empty(m_clicked_tile_indices))
     {
          return;
     }

     m_map_sprite->const_visit_working_tiles([&](const auto &tiles) {
          for (const auto &i : m_clicked_tile_indices)
          {
               if (i < std::ranges::size(tiles))
               {
                    const auto &tile = tiles[i];
                    collapsing_tile_info(*m_map_sprite, tile, i);
               }
          }
     });
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
               format_imgui_text("{} == {}", gui_labels::width, gui_labels::max_tiles);
          }
     }
     // if (m_changed)
     // {
     //      scale_window();
     // }
}
void gui::control_panel_window_map()
{
     combo_upscale_path();
     combo_deswizzle_path();
     checkbox_map_swizzle();
     m_changed = m_import.checkbox_render_imported_image() || m_changed;
     checkbox_map_disable_blending();
     compact_flatten_buttons();
     collapsing_header_filters();
     // if (m_changed)
     // {
     //      scale_window();
     // }
}
void gui::frame_rate()
{
     const auto framerate = ImGui::GetIO().Framerate;
     const auto fps       = fmt::format("{:>3.2f} fps", framerate);
     format_imgui_text("{}", fps);
     tool_tip(fps);
}
void gui::compact_flatten_buttons()
{
     if (!ImGui::CollapsingHeader(gui_labels::compact_flatten.data()))
     {
          return;
     }
     format_imgui_wrapped_text("{}: ", gui_labels::compact_flatten_warning);
     const ImGuiStyle &style       = ImGui::GetStyle();
     const float       spacing     = style.ItemInnerSpacing.x;
     const ImVec2      button_size = { ImGui::GetFrameHeight() * 3.75F, ImGui::GetFrameHeight() };
     format_imgui_text("{}: ", gui_labels::compact);
     tool_tip(gui_labels::compact_tooltip);
     ImGui::SameLine(0, spacing);
     if (ImGui::Button(gui_labels::rows.data(), button_size))
     {
          m_map_sprite->compact_rows();
     }
     tool_tip(gui_labels::compact_rows_tooltip);
     ImGui::SameLine(0, spacing);
     if (ImGui::Button(gui_labels::all.data(), button_size))
     {
          m_map_sprite->compact_all();
     }
     tool_tip(gui_labels::compact_all_tooltip);
     ImGui::SameLine(0, spacing);
     if (ImGui::Button(gui_labels::map_order.data(), button_size))
     {
          m_map_sprite->compact_map_order();
     }
     tool_tip(gui_labels::compact_map_order_tooltip);
     format_imgui_text("{}: ", gui_labels::flatten);
     tool_tip(gui_labels::flatten_tooltip);
     ImGui::SameLine(0, spacing);
     if (ImGui::Button(gui_labels::bpp.data(), button_size))
     {
          m_map_sprite->flatten_bpp();
     }
     tool_tip(gui_labels::flatten_bpp_tooltip);
     ImGui::SameLine(0, spacing);
     if (ImGui::Button(gui_labels::palette.data(), button_size))
     {
          m_map_sprite->flatten_palette();
     }
     tool_tip(gui_labels::flatten_palette_tooltip);
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
     // Begin non imgui drawing.
     m_window.clear(sf::Color::Black);
     directory_browser_display();
     file_browser_save_texture();
     render_dockspace();
     menu_bar();
     //     popup_batch_deswizzle();
     //     popup_batch_reswizzle();
     if (m_selections->display_batch_window)
     {
          m_batch.draw_window();
     }

     if (toggle_imgui_demo_window)
     {
          ImGui::ShowDemoWindow();
     }
     control_panel_window();
     //     batch_ops_ask_menu();
     // begin_batch_embed_map_warning_window();
     //     popup_batch_embed();
     m_import.render();
     m_history_window.render();

     draw_window();
     //  m_mouse_positions.cover.setColor(clear_color);
     //  m_window.draw(m_mouse_positions.cover);
     ImGui::SFML::Render(m_window);
     m_window.display();
     consume_one_future();
}
void gui::draw_window()
{
     if (!m_selections->display_draw_window)
     {
          return;
     }
     static constexpr ImGuiWindowFlags window_flags =
       ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar;
     if (mim_test())
     {
          // m_window.draw(m_mim_sprite.toggle_grids(m_selections->draw_grid, m_selections->draw_texture_page_grid));

          const auto pop_style0 = scope_guard([]() { ImGui::PopStyleVar(); });
          ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.F, 0.F));
          const auto pop_id0    = PushPopID();
          const auto pop_end    = scope_guard(&ImGui::End);
          const auto pop_style1 = scope_guard([]() { ImGui::PopStyleColor(); });
          ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ clear_color.r / 256.F, clear_color.g / 256.F, clear_color.b / 256.F, 0.9F });
          if (!ImGui::Begin(gui_labels::draw_window_title.data(), nullptr, window_flags))
          {
               return;
          }


          const auto         wsize      = ImGui::GetContentRegionAvail();
          const auto         img_size   = m_mim_sprite.get_texture()->getSize();

          const auto         screen_pos = ImGui::GetCursorScreenPos();
          const float        scale      = std::max(wsize.x / img_size.x, wsize.y / img_size.y);
          const sf::Vector2f scaled_size(img_size.x * scale, img_size.y * scale);


          const auto         pop_id1 = PushPopID();

          ImGui::Image(*m_mim_sprite.get_texture(), scaled_size);

          draw_mim_grid_lines_for_tiles(screen_pos, scaled_size, scale);

          draw_mim_grid_lines_for_texture_page(screen_pos, scaled_size, scale);
     }
     else if (map_test())
     {
          const auto pop_style0 = scope_guard([]() { ImGui::PopStyleVar(); });
          ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.F, 0.F));
          const auto pop_id0    = PushPopID();
          const auto pop_end    = scope_guard(&ImGui::End);
          const auto pop_style1 = scope_guard([]() { ImGui::PopStyleColor(); });
          ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ clear_color.r / 256.F, clear_color.g / 256.F, clear_color.b / 256.F, 0.9F });
          if (!ImGui::Begin(gui_labels::draw_window_title.data(), nullptr, window_flags))
          {
               return;
          }


          const auto         wsize      = ImGui::GetContentRegionAvail();
          const auto         img_size   = m_map_sprite->get_render_texture()->getSize();

          const auto         screen_pos = ImGui::GetCursorScreenPos();
          const float        scale      = std::max(wsize.x / img_size.x, wsize.y / img_size.y);
          const sf::Vector2f scaled_size(img_size.x * scale, img_size.y * scale);


          const auto         pop_id1 = PushPopID();

          ImGui::Image(*m_map_sprite->get_render_texture(), scaled_size);

          update_hover_and_mouse_button_status_for_map(screen_pos, scale);

          draw_map_grid_lines_for_tiles(screen_pos, scaled_size, scale);

          draw_map_grid_lines_for_texture_page(screen_pos, scaled_size, scale);

          draw_mouse_positions_sprite(scale, screen_pos);
     }
     on_click_not_imgui();
}
void gui::update_hover_and_mouse_button_status_for_map(const ImVec2 &img_start, const float scale)
{
     // Check if the mouse is over the image
     const ImVec2 mouse_pos = ImGui::GetMousePos();
     if (ImGui::IsItemHovered())
     {
          // Calculate the mouse position relative to the image
          sf::Vector2f relative_pos(mouse_pos.x - img_start.x, mouse_pos.y - img_start.y);

          // Map it back to the texture coordinates
          m_mouse_positions.pixel = sf::Vector2i(
            static_cast<int>(relative_pos.x / scale / static_cast<float>(m_map_sprite->get_map_scale())),
            static_cast<int>(relative_pos.y / scale / static_cast<float>(m_map_sprite->get_map_scale())));

          if (m_selections->draw_swizzle)
          {
               m_mouse_positions.pixel /= 16;
               m_mouse_positions.pixel *= 16;
          }
          m_mouse_positions.mouse_enabled = true;
          m_mouse_positions.texture_page  = static_cast<uint8_t>(m_mouse_positions.pixel.x / 256);
          if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
          {
               ImGui::BeginTooltip();
               format_imgui_text("({}, {})", m_mouse_positions.pixel.x, m_mouse_positions.pixel.y);
               ImGui::EndTooltip();
          }
     }
     else
     {
          m_mouse_positions.mouse_enabled = false;
     }
     if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
     {
          m_mouse_positions.left = true;
          m_map_sprite->const_visit_working_tiles([&](const auto &tiles) {
               m_clicked_tile_indices =
                 m_map_sprite->find_intersecting(tiles, m_mouse_positions.pixel, m_mouse_positions.texture_page, false, true);
          });
     }
     if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
     {
          m_mouse_positions.left = false;
     }
}


void gui::draw_mim_grid_lines_for_tiles(const ImVec2 &screen_pos, const sf::Vector2f &scaled_size, const float scale)
{
     // Drawing grid lines within the window if m_selections->draw_grid is true
     if (m_selections->draw_grid)
     {
          // Get the starting position and size of the image
          const ImVec2 img_end      = { screen_pos.x + scaled_size.x, screen_pos.y + scaled_size.y };

          // Calculate grid spacing
          const float  grid_spacing = (m_selections->draw_palette ? 1.F : 16.0f) * scale;

          // Iterate over horizontal and vertical lines
          for (float x = screen_pos.x; x < img_end.x; x += grid_spacing)
          {
               // Draw vertical lines
               ImGui::GetWindowDrawList()->AddLine(ImVec2(x, screen_pos.y), ImVec2(x, img_end.y), IM_COL32(255, 255, 255, 255));
          }

          for (float y = screen_pos.y; y < img_end.y; y += grid_spacing)
          {
               // Draw horizontal lines
               ImGui::GetWindowDrawList()->AddLine(ImVec2(screen_pos.x, y), ImVec2(img_end.x, y), IM_COL32(255, 255, 255, 255));
          }
     }
}
void gui::draw_mim_grid_lines_for_texture_page(const ImVec2 &screen_pos, const sf::Vector2f &scaled_size, const float scale)
{
     // Drawing grid lines within the window if m_selections->draw_grid is true
     if (m_selections->draw_texture_page_grid && !m_selections->draw_palette)
     {
          // Get the starting position and size of the image
          const ImVec2 img_end      = { screen_pos.x + scaled_size.x, screen_pos.y + scaled_size.y };

          // Calculate grid spacing

          const float  grid_spacing = [&]() {
               switch (m_selections->bpp)
               {
                    default:
                    case 0:
                         return 256.f;
                    case 1:
                         return 128.f;
                    case 2:
                         return 64.F;
               }
          }() * scale;

          //  m_selections->bpp

          // Iterate over horizontal and vertical lines
          for (float x = screen_pos.x; x < img_end.x; x += grid_spacing)
          {
               // Draw vertical lines
               ImGui::GetWindowDrawList()->AddLine(ImVec2(x, screen_pos.y), ImVec2(x, img_end.y), IM_COL32(255, 255, 0, 255));
          }
     }
}

void gui::draw_map_grid_lines_for_tiles(const ImVec2 &screen_pos, const sf::Vector2f &scaled_size, const float scale)
{
     // Drawing grid lines within the window if m_selections->draw_grid is true
     if (m_selections->draw_grid)
     {
          // Get the starting position and size of the image
          const ImVec2 img_end      = { screen_pos.x + scaled_size.x, screen_pos.y + scaled_size.y };

          // Calculate grid spacing
          const float  grid_spacing = 16.0f * scale * static_cast<float>(m_map_sprite->get_map_scale());

          // Iterate over horizontal and vertical lines
          for (float x = screen_pos.x; x < img_end.x; x += grid_spacing)
          {
               // Draw vertical lines
               ImGui::GetWindowDrawList()->AddLine(ImVec2(x, screen_pos.y), ImVec2(x, img_end.y), IM_COL32(255, 255, 255, 255));
          }

          for (float y = screen_pos.y; y < img_end.y; y += grid_spacing)
          {
               // Draw horizontal lines
               ImGui::GetWindowDrawList()->AddLine(ImVec2(screen_pos.x, y), ImVec2(img_end.x, y), IM_COL32(255, 255, 255, 255));
          }
     }
}
void gui::draw_map_grid_lines_for_texture_page(const ImVec2 &screen_pos, const sf::Vector2f &scaled_size, const float scale)
{
     // Drawing grid lines within the window if m_selections->draw_grid is true
     if (m_selections->draw_texture_page_grid && m_selections->draw_swizzle)
     {
          // Get the starting position and size of the image
          const ImVec2 img_end      = { screen_pos.x + scaled_size.x, screen_pos.y + scaled_size.y };

          // Calculate grid spacing
          const float  grid_spacing = 256.0f * scale * static_cast<float>(m_map_sprite->get_map_scale());

          // Iterate over horizontal and vertical lines
          for (float x = screen_pos.x; x < img_end.x; x += grid_spacing)
          {
               // Draw vertical lines
               ImGui::GetWindowDrawList()->AddLine(ImVec2(x, screen_pos.y), ImVec2(x, img_end.y), IM_COL32(255, 255, 0, 255));
          }
     }
}
void gui::draw_mouse_positions_sprite(const float scale, const ImVec2 &screen_pos)
{
     if (m_mouse_positions.sprite.getTexture() != nullptr)
     {

          sf::RenderStates states = {};
          if (m_drag_sprite_shader)
          {
               m_drag_sprite_shader->setUniform("texture", *m_mouse_positions.sprite.getTexture());
               static constexpr float border_width = 2.F;
               m_drag_sprite_shader->setUniform("borderWidth", border_width);
               states.shader = m_drag_sprite_shader.get();
          }

          // Prepare a render texture to draw the sprite with the shader
          m_shader_renderTexture.create(
            static_cast<std::uint32_t>(m_mouse_positions.sprite.getGlobalBounds().width),
            static_cast<std::uint32_t>(m_mouse_positions.sprite.getGlobalBounds().height));

          // Clear and draw the sprite with the shader
          m_shader_renderTexture.clear(sf::Color::Transparent);
          m_mouse_positions.sprite.setPosition(sf::Vector2f{});
          m_shader_renderTexture.draw(m_mouse_positions.sprite, states);
          m_shader_renderTexture.display();

          // int offset_y = -32 + m_mouse_positions.pixel.y % 16;
          ImGui::SetCursorScreenPos(ImVec2(
            (m_mouse_positions.pixel.x - 24) * scale * static_cast<float>(m_map_sprite->get_map_scale()) + screen_pos.x,
            (m_mouse_positions.pixel.y - 24) * scale * static_cast<float>(m_map_sprite->get_map_scale()) + screen_pos.y));
          ImGui::Image(
            std::bit_cast<ImTextureID>(static_cast<std::uintptr_t>(m_shader_renderTexture.getTexture().getNativeHandle())),
            ImVec2(
              m_mouse_positions.sprite.getGlobalBounds().width * scale * static_cast<float>(m_map_sprite->get_map_scale()),
              m_mouse_positions.sprite.getGlobalBounds().height * scale * static_cast<float>(m_map_sprite->get_map_scale())),
            ImVec2(0, 1),
            ImVec2(1, 0));
     }
}
void gui::consume_one_future()
{
     static constexpr int interval           = 50;// the interval in milliseconds
     static int           total_elapsed_time = 0;// keep track of the elapsed time using a static variable

     total_elapsed_time += m_elapsed_time.asMilliseconds();// add the elapsed time since last update

     if (total_elapsed_time < interval)
     {
          return;
     }
     // perform your operation here
     total_elapsed_time = 0;// reset the elapsed time
     if (!m_future_of_future_consumer.done())
     {
          ++m_future_of_future_consumer;
     }
     else if (!m_future_of_future_consumer.output_empty())
     {
          m_future_consumer = m_future_of_future_consumer.get_consumer();
     }
     else if (!m_future_consumer.done())
     {
          ++m_future_consumer;
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

void gui::on_click_not_imgui()
{
     if (m_mouse_positions.mouse_enabled)
     {
          m_mouse_positions.update_sprite_pos(m_selections->draw_swizzle);
          if (m_mouse_positions.left_changed())
          {
               if (map_test())
               {
                    if (m_mouse_positions.left)
                    {
                         // left mouse down
                         m_mouse_positions.sprite =
                           m_map_sprite->save_intersecting(m_mouse_positions.pixel, m_mouse_positions.texture_page);
                         m_mouse_positions.down_pixel = m_mouse_positions.pixel;
                         m_mouse_positions.max_tile_x = m_map_sprite->max_x_for_saved();
                    }
                    else
                    {
                         // left mouse up
                         m_map_sprite->update_position(
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
               m_mouse_positions.sprite = {};
               // mouse up off-screen ?
          }
     }
}
void gui::text_mouse_position() const
{
     if (!map_test())
     {
          return;
     }
     // Display texture coordinates if they are set
     if (!m_mouse_positions.mouse_enabled)
     {
          format_imgui_text("{}", gui_labels::mouse_not_over);
     }
     else
     {
          format_imgui_text("{}: ({:4}, {:3})", gui_labels::mouse_pos, m_mouse_positions.pixel.x, m_mouse_positions.pixel.y);
          ImGui::SameLine();
          const int tile_size = 16;
          format_imgui_text(
            "{}: ({:2}, {:2})", gui_labels::tile_pos, m_mouse_positions.pixel.x / tile_size, m_mouse_positions.pixel.y / tile_size);
          if (m_selections->draw_swizzle)
          {
               format_imgui_text("{}: {:2}", gui_labels::page, m_mouse_positions.texture_page);
          }
     }
     if (!ImGui::CollapsingHeader(gui_labels::hovered_tiles.data()))
     {
          return;
     }
     if (!m_mouse_positions.mouse_enabled)
     {
          return;
     }
     m_map_sprite->const_visit_tiles_both([&](const auto &tiles, const auto &front_tiles) {
          static std::vector<std::size_t> indices = {};
          if (m_mouse_positions.mouse_moved)
          {
               indices = m_map_sprite->find_intersecting(tiles, m_mouse_positions.pixel, m_mouse_positions.texture_page, false, true);
          }
          if (std::ranges::empty(indices))
          {
               return;
          }
          format_imgui_text("{} {:4}", gui_labels::number_of_tiles, std::ranges::size(indices));
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

               (void)create_tile_button(*m_map_sprite, front_tiles[index]);
          }
          ImGui::EndTable();
     });
}
bool gui::handle_mouse_cursor()
{
     return m_mouse_positions.mouse_enabled;
}
void gui::combo_coo()
{
     constexpr static auto values = open_viii::LangCommon::to_array();
     const auto            gcc    = GenericComboClass(
       gui_labels::language, []() { return values; }, []() { return values | std::views::transform(AsString{}); }, m_selections->coo);
     if (gcc.render())
     {
          update_field();
     }
     tool_tip(gui_labels::language_dropdown_tool_tip);
}
const open_viii::LangT &gui::get_coo() const
{
     static constexpr auto coos = open_viii::LangCommon::to_array();
     return coos.at(static_cast<size_t>(m_selections->coo));
}
void gui::combo_field()
{
     const auto gcc = GenericComboClass(
       gui_labels::field,
       [this]() { return std::views::iota(0, static_cast<int>(std::ranges::ssize(m_archives_group->mapdata()))); },
       [this]() { return m_archives_group->mapdata(); },
       m_selections->field);
     if (gcc.render())
     {
          Configuration config{};
          const auto   &maps = m_archives_group->mapdata();
          config->insert_or_assign("starter_field", *std::next(maps.begin(), m_selections->field));
          config.save();
          update_field();
     }
}

void gui::update_field()
{
     m_field = m_archives_group->field(m_selections->field);
     switch (m_selections->draw)
     {
          case draw_mode::draw_mim:
               m_mim_sprite = m_mim_sprite.with_field(m_field);
               break;
          case draw_mode::draw_map:
               m_map_sprite = std::make_shared<map_sprite>(m_map_sprite->with_field(m_field, get_coo()));
               m_import.update(m_map_sprite);
               m_history_window.update(m_map_sprite);
               break;
     }
     m_loaded_swizzle_texture_path   = std::filesystem::path{};
     m_loaded_deswizzle_texture_path = std::filesystem::path{};

     m_changed                       = true;
     if (m_field)
     {
          generate_upscale_paths(std::string{ m_field->get_base_name() }, get_coo());
     }

     m_clicked_tile_indices.clear();
}

void gui::checkbox_map_swizzle()
{
     if (ImGui::Checkbox(gui_labels::swizzle.data(), &m_selections->draw_swizzle))
     {
          Configuration config{};
          config->insert_or_assign("selections_draw_swizzle", m_selections->draw_swizzle);
          config.save();
          if (m_selections->draw_swizzle)
          {
               m_map_sprite->enable_disable_blends();
               m_map_sprite->enable_draw_swizzle();
          }
          else
          {
               m_map_sprite->disable_draw_swizzle();
               if (!m_selections->draw_disable_blending)
               {
                    m_map_sprite->disable_disable_blends();
               }
          }
          m_changed = true;
     }
     tool_tip(gui_labels::swizzle_tooltip);
}
void gui::checkbox_map_disable_blending()
{
     if (!m_selections->draw_swizzle && ImGui::Checkbox(gui_labels::disable_blending.data(), &m_selections->draw_disable_blending))
     {
          Configuration config{};
          config->insert_or_assign("selections_draw_disable_blending", m_selections->draw_disable_blending);
          config.save();
          if (m_selections->draw_disable_blending)
          {
               m_map_sprite->enable_disable_blends();
          }
          else
          {
               m_map_sprite->disable_disable_blends();
          }
          m_changed = true;
     }
     tool_tip(gui_labels::disable_blending_tooltip);
}
void gui::checkbox_mim_palette_texture()
{
     if (ImGui::Checkbox(gui_labels::draw_palette_texture.data(), &m_selections->draw_palette))
     {
          Configuration config{};
          config->insert_or_assign("selections_draw_palette", m_selections->draw_palette);
          config.save();
          m_mim_sprite = m_mim_sprite.with_draw_palette(m_selections->draw_palette);
          m_changed    = true;
     }
     tool_tip(gui_labels::draw_palette_texture_tooltip);
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
          static constexpr auto bpp_strings = Mim::bpp_selections_c_str();
          static const auto     bpp_values  = std::ranges::views::iota(int{ 0 }, static_cast<int>(std::ranges::size(bpp_strings)));
          const auto            gcc         = GenericComboClass(
            gui_labels::bpp,
            [&]() { return bpp_values; },
            [&]() { return bpp_strings | std::ranges::views::transform([](std::string_view sv) { return sv; }); },
            m_selections->bpp);

          if (gcc.render())
          {
               Configuration config{};
               config->insert_or_assign("selections_bpp", m_selections->bpp);
               config.save();
               if (mim_test())
               {
                    update_bpp(m_mim_sprite, bpp());
               }
               if (map_test())
               {
                    update_bpp(*m_map_sprite, bpp());
               }
               m_changed = true;
          }
     }
}
std::uint8_t gui::palette() const
{
     return static_cast<uint8_t>(Mim::palette_selections().at(static_cast<size_t>(m_selections->palette)));
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
     if (m_selections->bpp != 2)
     {
          {
               static constexpr auto palette_values  = Mim::palette_selections();
               static constexpr auto palette_strings = Mim::palette_selections_c_str();
               const auto            gcc             = GenericComboClass(
                 gui_labels::palette,
                 []() { return palette_values; },
                 []() { return palette_strings | std::ranges::views::transform([](std::string_view sv) { return sv; }); },
                 m_selections->palette);
               if (gcc.render())
               {
                    if (mim_test())
                    {
                         update_palette(m_mim_sprite, palette());
                    }
                    if (map_test())
                    {
                         update_palette(*m_map_sprite, palette());
                    }
                    Configuration config{};
                    config->insert_or_assign("selections_palette", m_selections->palette);
                    config.save();
                    m_changed = true;
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

     file_menu();

     edit_menu();

     windows_menu();
}
void gui::windows_menu()
{
     if (!ImGui::BeginMenu(gui_labels::windows.data()))
     {
          return;
     }
     const auto end_menu = scope_guard(&ImGui::EndMenu);
     if (ImGui::MenuItem(gui_labels::batch_operation_window.data(), "Control + B", &m_selections->display_batch_window))
     {
          Configuration config{};
          config->insert_or_assign("selections_display_batch_window", m_selections->display_batch_window);
          config.save();
     }
     if (ImGui::MenuItem(gui_labels::import_page.data(), "Control + I", &m_selections->display_import_image))
     {
          Configuration config{};
          config->insert_or_assign("selections_display_import_image", m_selections->display_import_image);
          config.save();
     }
     ImGui::Separator();
     if (ImGui::MenuItem(gui_labels::display_history.data(), "Control + H", &m_selections->display_history_window))
     {
          Configuration config{};
          config->insert_or_assign("selections_display_history_window", m_selections->display_history_window);
          config.save();
     }
     ImGui::Separator();
     if (ImGui::MenuItem(gui_labels::display_draw_window.data(), "Control + D", &m_selections->display_draw_window))
     {
          Configuration config{};
          config->insert_or_assign("selections_display_draw_window", m_selections->display_draw_window);
          config.save();
     }
     if (ImGui::MenuItem(gui_labels::display_control_panel_window.data(), "Control + P", &m_selections->display_control_panel_window))
     {
          Configuration config{};
          config->insert_or_assign("selections_display_control_panel_window", m_selections->display_control_panel_window);
          config.save();
     }
     if (ImGui::MenuItem("ImGui Demo Window", std::nullptr_t{}, &toggle_imgui_demo_window))
     {
     }
}
void gui::edit_menu()
{
     if (!ImGui::BeginMenu(gui_labels::edit.data()))
     {
          return;
     }
     const auto end_menu = scope_guard(&ImGui::EndMenu);
     if (ImGui::MenuItem(gui_labels::undo.data(), "Control + Z", false, m_map_sprite->undo_enabled()))
     {
          m_map_sprite->undo();
     }
     else if (m_map_sprite->undo_enabled())
     {

          const auto description = m_map_sprite->current_undo_description();
          tool_tip(description);
     }
     if (ImGui::MenuItem(gui_labels::redo.data(), "Control + Y", false, m_map_sprite->redo_enabled()))
     {
          m_map_sprite->redo();
     }
     else if (m_map_sprite->redo_enabled())
     {
          const auto description = m_map_sprite->current_redo_description();
          tool_tip(description);
     }
     ImGui::Separator();
     if (ImGui::MenuItem(gui_labels::undo_all.data(), "Shift + Control + Z", false, m_map_sprite->undo_enabled()))
     {
          m_map_sprite->undo_all();
     }
     if (ImGui::MenuItem(gui_labels::redo_all.data(), "Shift + Control + Y", false, m_map_sprite->redo_enabled()))
     {
          m_map_sprite->redo_all();
     }
     ImGui::Separator();
     if (ImGui::MenuItem(gui_labels::display_history.data(), "Control + H", &m_selections->display_history_window))
     {
          Configuration config{};
          config->insert_or_assign("selections_display_history_window", m_selections->display_history_window);
          config.save();
     }
     ImGui::Separator();
     if (ImGui::MenuItem(gui_labels::draw_tile_grid.data(), nullptr, &m_selections->draw_grid))
     {
          Configuration config{};
          config->insert_or_assign("selections_draw_grid", m_selections->draw_grid);
          config.save();
     }
     if ((map_test() && m_selections->draw_swizzle) || (mim_test() && !m_selections->draw_palette))
     {
          if (ImGui::MenuItem(gui_labels::draw_texture_page_grid.data(), nullptr, &m_selections->draw_texture_page_grid))
          {
               Configuration config{};
               config->insert_or_assign("selections_draw_texture_page_grid", m_selections->draw_texture_page_grid);
               config.save();
          }
     }
}
void gui::file_menu()
{
     if (!ImGui::BeginMenu(gui_labels::file.data()))
     {
          return;
     }
     const auto end_menu = scope_guard(&ImGui::EndMenu);
     menuitem_locate_ff8();
     if (map_test())
     {
          menuitem_locate_custom_upscale();
     }
     ImGui::Separator();
     menuitem_save_texture(mim_test() || map_test());
     if (mim_test())
     {
          ImGui::Separator();
          menuitem_save_mim_file();
     }
     if (map_test())
     {
          ImGui::Separator();
          menuitem_save_map_file();
          menuitem_save_map_file_modified();
          menuitem_load_map_file();
          ImGui::Separator();
          menuitem_save_swizzle_textures();
          menuitem_load_swizzle_textures();
          ImGui::Separator();
          menuitem_save_deswizzle_textures();
          menuitem_load_deswizzle_textures();
     }
}
bool gui::map_test() const
{
     return m_map_sprite && !m_map_sprite->fail() && m_selections && m_selections->draw == draw_mode::draw_map;
}
bool gui::mim_test() const
{
     return !m_mim_sprite.fail() && m_selections && m_selections->draw == draw_mode::draw_mim;
}
std::string gui::save_texture_path() const
{
     if (m_archives_group->mapdata().empty())
     {
          return {};
     }
     const std::string &field_name = m_archives_group->mapdata().at(static_cast<size_t>(m_selections->field));
     spdlog::info("field_name = {}", field_name);
     if (mim_test())// MIM
     {
          if (m_mim_sprite.draw_palette())
          {
               return fmt::format("{}_mim_palettes.png", field_name);
          }
          const int bpp = static_cast<int>(Mim::bpp_selections().at(static_cast<size_t>(m_selections->bpp)));
          return fmt::format("{}_mim_{}bpp_{}.png", field_name, bpp, m_selections->palette);
     }
     else if (map_test())
     {
          return fmt::format("{}_map.png", field_name);
     }
     return {};
}
void gui::directory_browser_display()
{
     m_directory_browser.Display();
     if (!m_directory_browser.HasSelected())
     {
          return;
     }
     const auto pop_directory = scope_guard([this]() { m_directory_browser.ClearSelected(); });
     auto       selected_path = m_directory_browser.GetSelected();
     switch (m_modified_directory_map)
     {
          case map_directory_mode::ff8_install_directory: {
               Configuration config{};
               config->insert_or_assign("ff8_install_navigation_path", selected_path.string());
               config.save();
               m_paths.push_back(selected_path.string());
               sort_paths();
               m_selections->path = selected_path.string();
               update_path();
          }
          break;
          case map_directory_mode::save_swizzle_textures: {
               Configuration config{};
               config->insert_or_assign("swizzle_path", selected_path.string());
               selected_path = path_with_prefix_and_base_name(std::move(selected_path));
               config->insert_or_assign("single_swizzle_path", selected_path.string());
               config.save();
               std::error_code error_code{};
               std::filesystem::create_directories(selected_path, error_code);
               if (error_code)
               {
                    spdlog::error(
                      "{}:{} - {}: {} - path: {}", __FILE__, __LINE__, error_code.value(), error_code.message(), selected_path.string());
                    error_code.clear();
               }
               // todo modify these two functions :P to use the imported image.
               m_future_of_future_consumer = m_map_sprite->save_swizzle_textures(selected_path);// done.
               m_map_sprite->save_modified_map(selected_path / m_map_sprite->map_filename());// done.
               open_directory(selected_path);
          }
          break;
          case map_directory_mode::save_deswizzle_textures: {
               Configuration config{};
               config->insert_or_assign("deswizzle_path", selected_path.string());
               selected_path = path_with_prefix_and_base_name(std::move(selected_path));
               config->insert_or_assign("single_deswizzle_path", selected_path.string());
               config.save();
               std::error_code error_code{};
               std::filesystem::create_directories(selected_path, error_code);
               if (error_code)
               {
                    spdlog::error(
                      "{}:{} - {}: {} - path: {}", __FILE__, __LINE__, error_code.value(), error_code.message(), selected_path.string());
                    error_code.clear();
               }
               m_future_of_future_consumer = m_map_sprite->save_pupu_textures(selected_path);
               m_map_sprite->save_modified_map(selected_path / m_map_sprite->map_filename());
               open_directory(selected_path);
          }
          break;
          case map_directory_mode::load_swizzle_textures: {
               Configuration config{};
               config->insert_or_assign("single_swizzle_path", selected_path.string());
               config.save();
               m_loaded_swizzle_texture_path = selected_path;
               if (m_field)
               {
                    generate_upscale_paths(std::string{ m_field->get_base_name() }, get_coo());
               }
               m_map_sprite->filter().deswizzle.disable();
               m_map_sprite->filter().upscale.update(m_loaded_swizzle_texture_path).enable();
               auto          map_path      = m_loaded_swizzle_texture_path / m_map_sprite->map_filename();
               safedir const safe_map_path = map_path;
               if (safe_map_path.is_exists())
               {
                    m_map_sprite->load_map(map_path);
               }
               m_map_sprite->update_render_texture(true);
          }
          break;
          case map_directory_mode::load_deswizzle_textures: {
               Configuration config{};
               config->insert_or_assign("single_deswizzle_path", selected_path.string());
               config.save();
               m_loaded_deswizzle_texture_path = selected_path;
               m_map_sprite->filter().upscale.disable();
               m_map_sprite->filter().deswizzle.update(m_loaded_deswizzle_texture_path).enable();
               auto          map_path      = m_loaded_deswizzle_texture_path / m_map_sprite->map_filename();
               safedir const safe_map_path = map_path;
               if (safe_map_path.is_exists())
               {
                    m_map_sprite->load_map(map_path);
               }
               m_map_sprite->update_render_texture(true);
          }
          break;
          case map_directory_mode::custom_upscale_directory: {
               m_custom_upscale_paths.push_back(selected_path.string());
               // todo remove paths that don't exist.
               Configuration config{};
               config->insert_or_assign("custom_upscale_paths_vector", m_custom_upscale_paths);
               config.save();
               if (m_field)
               {
                    generate_upscale_paths(std::string{ m_field->get_base_name() }, get_coo());
               }
               // todo toggle filter enabled?
          }
          break;
     }
}
std::filesystem::path gui::path_with_prefix_and_base_name(std::filesystem::path selected_path) const
{
     std::string const      base_name = m_map_sprite->get_base_name();
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
               switch (m_file_dialog_mode)
               {
                    case file_dialog_mode::save_mim_file: {
                         m_mim_sprite.mim_save(selected_path);
                         Configuration config{};
                         config->insert_or_assign("mim_path", m_save_file_browser.GetPwd().string());
                         config.save();
                    }
                    break;
                    case file_dialog_mode::save_image_file: {
                         m_mim_sprite.save(selected_path);
                         Configuration config{};
                         config->insert_or_assign("save_image_path", m_save_file_browser.GetPwd().string());
                         config.save();
                    }
                    break;
                    case file_dialog_mode::save_unmodified_map_file:
                    case file_dialog_mode::save_modified_map_file:
                    case file_dialog_mode::load_map_file:
                         break;
               }
          }
          else if (map_test())
          {
               switch (m_file_dialog_mode)
               {
                    case file_dialog_mode::save_modified_map_file: {
                         m_map_sprite->save_modified_map(selected_path);
                         Configuration config{};
                         config->insert_or_assign("map_path", m_save_file_browser.GetPwd().string());
                         config.save();
                         open_file_explorer(selected_path);
                    }
                    break;
                    case file_dialog_mode::save_unmodified_map_file: {
                         m_map_sprite->map_save(selected_path);
                         Configuration config{};
                         config->insert_or_assign("map_path", m_save_file_browser.GetPwd().string());
                         config.save();
                         open_file_explorer(selected_path);
                    }
                    break;
                    case file_dialog_mode::load_map_file: {
                         m_map_sprite->load_map(selected_path);
                         Configuration config{};
                         config->insert_or_assign("map_path", m_save_file_browser.GetPwd().string());
                         config.save();
                         m_changed = true;
                    }
                    break;
                    case file_dialog_mode::save_image_file: {
                         m_map_sprite->save(selected_path);
                         Configuration config{};
                         config->insert_or_assign("save_image_path", m_save_file_browser.GetPwd().string());
                         config.save();
                         open_file_explorer(selected_path);
                    }
                    break;
                    case file_dialog_mode::save_mim_file:
                         break;
               }
          }
          m_save_file_browser.ClearSelected();
     }
}
void gui::menuitem_locate_ff8()
{
     if (!ImGui::MenuItem(gui_labels::locate_a_ff8_install.data()))
     {
          return;
     }
     open_locate_ff8_filebrowser();
}
void gui::open_locate_ff8_filebrowser()
{
     m_directory_browser.Open();
     m_directory_browser.SetTitle(gui_labels::choose_a_ff8_install.data());
     m_directory_browser.SetPwd(Configuration{}["ff8_install_navigation_path"].value_or(std::filesystem::current_path().string()));
     m_directory_browser.SetTypeFilters({ ".exe" });
     m_modified_directory_map = map_directory_mode::ff8_install_directory;
}
void gui::menuitem_locate_custom_upscale()
{
     if (!ImGui::MenuItem(gui_labels::locate_a_custom_upscale_directory.data()))
     {
          return;
     }

     m_directory_browser.Open();
     m_directory_browser.SetTitle(gui_labels::choose_a_custom_upscale_directory.data());
     m_modified_directory_map = map_directory_mode::custom_upscale_directory;
}
void gui::menuitem_save_swizzle_textures()
{
     if (!ImGui::MenuItem(gui_labels::save_swizzled_textures.data(), nullptr, false, true))
     {
          return;
     }
     save_swizzle_textures();
}
void gui::save_swizzle_textures()
{
     m_directory_browser.Open();
     m_directory_browser.SetTitle(m_map_sprite->appends_prefix_base_name(gui_labels::choose_directory_to_save_textures_to));
     m_directory_browser.SetPwd(Configuration{}["swizzle_path"].value_or(std::filesystem::current_path().string()));
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_modified_directory_map = map_directory_mode::save_swizzle_textures;
}
void gui::menuitem_save_deswizzle_textures()
{
     if (ImGui::MenuItem(gui_labels::save_deswizzled_textures.data(), nullptr, false, true))
     {
          m_directory_browser.Open();
          m_directory_browser.SetTitle(m_map_sprite->appends_prefix_base_name(gui_labels::choose_directory_to_save_textures_to));
          m_directory_browser.SetPwd(Configuration{}["deswizzle_path"].value_or(std::filesystem::current_path().string()));
          m_directory_browser.SetTypeFilters({ ".map", ".png" });
          m_modified_directory_map = map_directory_mode::save_deswizzle_textures;
     }
}
void gui::menuitem_load_swizzle_textures()
{
     if (!ImGui::MenuItem("Load Swizzled Textures", nullptr, false, true))
     {
          return;
     }
     m_directory_browser.Open();
     m_directory_browser.SetTitle(gui_labels::choose_directory_to_load_textures_from.data());
     m_directory_browser.SetPwd(Configuration{}["single_swizzle_path"].value_or(std::filesystem::current_path().string()));
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_modified_directory_map = map_directory_mode::load_swizzle_textures;
}
void gui::menuitem_load_deswizzle_textures()
{
     if (!ImGui::MenuItem("Load Deswizzled Textures", nullptr, false, true))
          return;
     m_directory_browser.Open();
     m_directory_browser.SetTitle(gui_labels::choose_directory_to_load_textures_from.data());
     m_directory_browser.SetPwd(Configuration{}["single_deswizzle_path"].value_or(std::filesystem::current_path().string()));
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_modified_directory_map = map_directory_mode::load_deswizzle_textures;
}
void gui::menuitem_save_texture(bool enabled)
{
     if (!ImGui::MenuItem(gui_labels::save_displayed_texture.data(), nullptr, false, enabled))
     {
          return;
     }
     const std::string &path = save_texture_path();
     m_save_file_browser.Open();
     m_save_file_browser.SetTitle(gui_labels::save_texture_as.data());
     m_save_file_browser.SetPwd(Configuration{}["save_image_path"].value_or(std::filesystem::current_path().string()));
     m_save_file_browser.SetTypeFilters({ ".png", ".ppm" });
     m_save_file_browser.SetInputName(path.c_str());
     m_file_dialog_mode = file_dialog_mode::save_image_file;
}
void gui::menuitem_save_mim_file(bool enabled)
{
     if (!ImGui::MenuItem(gui_labels::save_mim_unmodified.data(), nullptr, false, enabled))
     {
          return;
     }
     const std::string &path = m_mim_sprite.mim_filename();
     m_save_file_browser.Open();
     m_save_file_browser.SetTitle(gui_labels::save_mim_as.data());
     m_save_file_browser.SetPwd(Configuration{}["mim_path"].value_or(std::filesystem::current_path().string()));
     m_save_file_browser.SetTypeFilters({ Mim::EXT.data() });
     m_save_file_browser.SetInputName(path);
     m_file_dialog_mode = file_dialog_mode::save_mim_file;
}
void gui::menuitem_save_map_file(bool enabled)
{
     if (!ImGui::MenuItem(gui_labels::save_map_unmodified.data(), nullptr, false, enabled))
     {
          return;
     }
     const std::string &path = m_map_sprite->map_filename();
     m_save_file_browser.Open();
     m_save_file_browser.SetTitle(gui_labels::save_map_as.data());
     m_save_file_browser.SetPwd(Configuration{}["map_path"].value_or(std::filesystem::current_path().string()));
     m_save_file_browser.SetTypeFilters({ Map::EXT.data() });
     m_save_file_browser.SetInputName(path);
     m_file_dialog_mode = file_dialog_mode::save_unmodified_map_file;
}
void gui::menuitem_save_map_file_modified(bool enabled)
{
     if (!ImGui::MenuItem(gui_labels::save_map_modified.data(), nullptr, false, enabled))
     {
          return;
     }
     const std::string &path = m_map_sprite->map_filename();
     m_save_file_browser.Open();
     m_save_file_browser.SetTitle(gui_labels::save_map_as.data());
     m_save_file_browser.SetPwd(Configuration{}["map_path"].value_or(std::filesystem::current_path().string()));
     m_save_file_browser.SetTypeFilters({ Map::EXT.data() });
     m_save_file_browser.SetInputName(path);
     m_file_dialog_mode = file_dialog_mode::save_modified_map_file;
}
void gui::menuitem_load_map_file(bool enabled)
{
     if (!ImGui::MenuItem(gui_labels::load_map_file.data(), nullptr, false, enabled))
     {
          return;
     }
     const std::string &path = m_map_sprite->map_filename();
     m_load_file_browser.Open();
     m_load_file_browser.SetTitle(gui_labels::load_map.data());
     m_load_file_browser.SetPwd(Configuration{}["map_path"].value_or(std::filesystem::current_path().string()));
     m_load_file_browser.SetTypeFilters({ Map::EXT.data() });
     m_load_file_browser.SetInputName(path);
     m_file_dialog_mode = file_dialog_mode::load_map_file;
}
void gui::combo_draw()
{
     static const constinit auto iota_draw_mode =
       std::views::iota(0, 2) | std::views::transform([](const int mode) { return static_cast<draw_mode>(mode); });
     static const auto str_draw_mode =
       iota_draw_mode | std::views::transform([](draw_mode in_draw_mode) { return fmt::format("{}", in_draw_mode); });

     const auto gcc =
       GenericComboClass(gui_labels::draw, [=]() { return iota_draw_mode; }, [=]() { return str_draw_mode; }, m_selections->draw);

     if (!gcc.render())
     {
          return;
     }
     Configuration config{};
     config->insert_or_assign("selections_draw", static_cast<std::underlying_type_t<draw_mode>>(m_selections->draw));
     config.save();
     switch (m_selections->draw)
     {
          case draw_mode::draw_mim:
               m_mim_sprite = get_mim_sprite();
               break;
          case draw_mode::draw_map:
               m_map_sprite =
                 std::make_shared<map_sprite>(m_map_sprite->update(ff_8::map_group(m_field, get_coo()), m_selections->draw_swizzle));
               m_import.update(m_map_sprite);
               m_history_window.update(m_map_sprite);
               break;
     }
     m_changed = true;
}
bool gui::combo_path()
{
     const auto transformed_paths =
       m_paths | std::ranges::views::transform([](toml::node &item) -> std::string { return item.value_or<std::string>({}); });
     const auto browse_button = scope_guard([&]() {
          if (ImGui::Button(gui_labels::browse.data()))
          {
               open_locate_ff8_filebrowser();
          }
          tool_tip(gui_labels::locate_a_ff8_install);
          ImGui::SameLine();
          if (ImGui::Button(gui_labels::explore.data()))
          {
               open_directory(m_selections->path);
          }
          tool_tip(gui_labels::explore_tooltip);
          ImGui::SameLine();
          ImGui::BeginDisabled(std::ranges::empty(transformed_paths));
          if (ImGui::Button(gui_labels::remove.data()))
          {
               // Find and remove the selected path from m_paths
               auto it =
                 std::ranges::find_if(m_paths, [&](toml::node &item) { return item.value_or<std::string>({}) == m_selections->path; });
               if (it != m_paths.end())
               {
                    m_paths.erase(it);
                    Configuration config{};
                    config->insert_or_assign("paths_vector", m_paths);
                    config.save();
               }
          }
          ImGui::EndDisabled();
          tool_tip(gui_labels::remove_the_selected_ff8_path);
     });
     const auto gcc =
       GenericComboClass(gui_labels::path, [&]() { return transformed_paths; }, [&]() { return transformed_paths; }, m_selections->path, 1);
     if (gcc.render())
     {
          Configuration config{};
          config->insert_or_assign("selections_path", m_selections->path);
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
}
void gui::loop_events()
{
     m_mouse_positions.update();
     while (m_window.pollEvent(m_event))
     {
          ImGui::SFML::ProcessEvent(m_window, m_event);
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
                   m_changed                     = true;
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
                        // m_mouse_positions.left = false;
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
               // m_mouse_positions.left = false;
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
               // m_mouse_positions.left = true;
               spdlog::trace("{}", "Left Mouse Button Down");
          }
          break;
          default:
               break;
     }
}
void gui::event_type_key_pressed([[maybe_unused]] const sf::Event::KeyEvent &key)
{
     // if (key.code == sf::Keyboard::Up)
     // {
     //      m_scrolling.up = true;
     // }
     // else if (key.code == sf::Keyboard::Down)
     // {
     //      m_scrolling.down = true;
     // }
     // else if (key.code == sf::Keyboard::Left)
     // {
     //      m_scrolling.left = true;
     // }
     // else if (key.code == sf::Keyboard::Right)
     // {
     //      m_scrolling.right = true;
     // }
}
void gui::event_type_key_released(const sf::Event::KeyEvent &key)
{
     if (key.shift && key.control && key.code == sf::Keyboard::Z)
     {
          m_map_sprite->undo_all();
     }
     else if (key.shift && key.control && key.code == sf::Keyboard::Y)
     {
          m_map_sprite->redo_all();
     }
     else if (key.control && key.code == sf::Keyboard::Z)
     {
          m_map_sprite->undo();
     }
     else if (key.control && key.code == sf::Keyboard::Y)
     {
          m_map_sprite->redo();
     }
     else if (key.control && key.code == sf::Keyboard::H)
     {
          m_selections->display_history_window = !m_selections->display_history_window;
          Configuration config{};
          config->insert_or_assign("selections_display_history_window", m_selections->display_history_window);
          config.save();
     }
     else if (key.control && key.code == sf::Keyboard::I)
     {
          m_selections->display_import_image = !m_selections->display_import_image;
          Configuration config{};
          config->insert_or_assign("selections_display_import_image", m_selections->display_import_image);
          config.save();
     }
     else if (key.control && key.code == sf::Keyboard::B)
     {
          m_selections->display_batch_window = !m_selections->display_batch_window;
          Configuration config{};
          config->insert_or_assign("selections_display_batch_window", m_selections->display_batch_window);
          config.save();
     }
     else if (key.control && key.code == sf::Keyboard::D)
     {
          m_selections->display_draw_window = !m_selections->display_draw_window;
          Configuration config{};
          config->insert_or_assign("selections_display_draw_window", m_selections->display_draw_window);
          config.save();
     }
     else if (key.control && key.code == sf::Keyboard::P)
     {
          m_selections->display_control_panel_window = !m_selections->display_control_panel_window;
          Configuration config{};
          config->insert_or_assign("selections_display_control_panel_window", m_selections->display_control_panel_window);
          config.save();
     }
     // else if (key.code == sf::Keyboard::Up)
     // {
     //      m_scrolling.up = false;
     // }
     // else if (key.code == sf::Keyboard::Down)
     // {
     //      m_scrolling.down = false;
     // }
     // else if (key.code == sf::Keyboard::Left)
     // {
     //      m_scrolling.left = false;
     // }
     // else if (key.code == sf::Keyboard::Right)
     // {
     //      m_scrolling.right = false;
     // }
}
std::uint32_t gui::image_height() const
{
     if (map_test())
     {
          return m_map_sprite->height();
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
     if (!std::ranges::empty(m_selections->path))
     {
          return { open_viii::LangCommon::to_array().front(), m_selections->path };
     }
     return {};
}
sf::RenderWindow gui::get_render_window() const
{
     return sf::RenderWindow{ sf::VideoMode(m_selections->window_width, m_selections->window_height),
                              sf::String{ gui_labels::window_title.data() } };
}
void gui::update_path()
{
     m_archives_group = std::make_shared<archives_group>(m_archives_group->with_path(m_selections->path));
     m_batch          = m_archives_group;
     update_field();
}
mim_sprite gui::get_mim_sprite() const
{
     return { m_field,
              Mim::bpp_selections().at(static_cast<std::size_t>(m_selections->bpp)),
              static_cast<std::uint8_t>(Mim::palette_selections().at(static_cast<std::size_t>(m_selections->palette))),
              get_coo(),
              m_selections->draw_palette };
}
void gui::init_and_get_style()
{
     m_window.setVerticalSyncEnabled(true);
     (void)ImGui::SFML::Init(m_window, false);
     (void)icons_font();
     ImGui::SFML::UpdateFontTexture();
     ImGuiIO &imgui_io    = ImGui::GetIO();
     imgui_io.ConfigFlags = bitwise_or(imgui_io.ConfigFlags, ImGuiConfigFlags_DockingEnable);
     if (m_field)
     {
          generate_upscale_paths(std::string{ m_field->get_base_name() }, get_coo());
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
  , m_archives_group(std::make_shared<archives_group>(get_archives_group()))
  , m_field(init_field())
  , m_mim_sprite(get_mim_sprite())
  , m_map_sprite(get_map_sprite())

{
     m_import.update(m_selections);
     m_history_window.update(m_selections);
     m_import.update(m_map_sprite);
     m_history_window.update(m_map_sprite);
     m_window.setVerticalSyncEnabled(false);
     GLenum const err = glewInit();
     if (std::cmp_not_equal(GLEW_OK, err))
     {
          // GLEW initialization failed
          const GLubyte *error_msg = glewGetErrorString(err);
          spdlog::error("{}", reinterpret_cast<const char *>(error_msg));
          std::terminate();
     }
     // Enable debug output
     glEnable(GL_DEBUG_OUTPUT);
     glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

     // Set the debug callback function
     glDebugMessageCallback(DebugCallback, nullptr);
     sort_paths();
     init_and_get_style();
}
std::shared_ptr<open_viii::archive::FIFLFS<false>> gui::init_field()
{
     m_selections->field = get_selected_field();
     return m_archives_group->field(m_selections->field);
}
std::shared_ptr<map_sprite> gui::get_map_sprite() const
{
     //     map_sprite(ff_8::map_group map_group, bool draw_swizzle, ff_8::filters in_filters, bool force_disable_blends, bool
     //     require_coo);
     return std::make_shared<map_sprite>(
       ff_8::map_group{ m_field, get_coo() }, m_selections->draw_swizzle, ff_8::filters{}, m_selections->draw_disable_blending, false);
}

int gui::get_selected_field()
{
     if (const int field = m_archives_group->find_field(starter_field()); field != -1)
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
     const auto gcc = GenericComboClassWithFilter(
       gui_labels::pupu_id,
       [&]() { return m_map_sprite->working_unique_pupu(); },
       [&]() { return m_map_sprite->working_unique_pupu() | std::views::transform(AsString{}); },
       [&]() {
            return m_map_sprite->working_unique_pupu()
                   | std::views::transform([](const ff_8::PupuID &pupu_id) -> decltype(auto) { return pupu_id.create_summary(); });
       },
       [this]() -> auto & { return m_map_sprite->filter().pupu; });

     if (!gcc.render())
     {
          return;
     }
     m_map_sprite->update_render_texture();
     m_changed = true;
}

void gui::combo_draw_bit()
{
     using namespace std::string_view_literals;
     static constexpr auto values = std::array{ ff_8::draw_bitT::all, ff_8::draw_bitT::enabled, ff_8::draw_bitT::disabled };
     const auto            gcc    = fme::GenericComboClassWithFilter(
       gui_labels::draw_bit,
       []() { return values; },
       []() { return values | std::views::transform(AsString{}); },
       []() {
            return std::array{ gui_labels::draw_bit_all_tooltip,
                               gui_labels::draw_bit_enabled_tooltip,
                               gui_labels::draw_bit_disabled_tooltip };
       },
       [this]() -> auto               &{ return m_map_sprite->filter().draw_bit; });
     if (!gcc.render())
          return;
     m_map_sprite->update_render_texture();
     m_changed = true;
}
void gui::combo_filtered_palettes()
{
     const auto &map = m_map_sprite->uniques().palette();
     const auto &key = m_map_sprite->filter().bpp.value();
     if (!map.contains(key))
     {
          return;
     }
     const auto &pair = map.at(key);
     const auto  gcc  = fme::GenericComboClassWithFilter(
       gui_labels::palette,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       EmptyStringView{},
       [this]() -> auto  &{ return m_map_sprite->filter().palette; });
     if (!gcc.render())
     {
          return;
     }
     m_map_sprite->update_render_texture();
     m_selections->palette = m_map_sprite->filter().palette.value();
     Configuration config{};
     config->insert_or_assign("selections_palette", m_selections->palette);
     config.save();
     m_changed = true;
}

void gui::combo_filtered_bpps()
{
     const auto &pair = m_map_sprite->uniques().bpp();
     const auto  gcc  = fme::GenericComboClassWithFilter(
       gui_labels::bpp,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       EmptyStringView{},
       [this]() -> auto  &{ return m_map_sprite->filter().bpp; });
     if (!gcc.render())
     {
          return;
     }
     m_map_sprite->update_render_texture();
     m_selections->bpp = static_cast<int>(m_map_sprite->filter().bpp.value().raw() & 3U);
     Configuration config{};
     config->insert_or_assign("selections_bpp", m_selections->bpp);
     config.save();
     m_changed = true;
}

void gui::combo_blend_modes()
{
     const auto &pair = m_map_sprite->uniques().blend_mode();
     const auto  gcc  = fme::GenericComboClassWithFilter(
       gui_labels::blend_mode,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       EmptyStringView{},
       [this]() -> auto  &{ return m_map_sprite->filter().blend_mode; });
     if (!gcc.render())
     {
          return;
     }
     m_map_sprite->update_render_texture();
     m_changed = true;
}

void gui::combo_layers()
{
     const auto &pair = m_map_sprite->uniques().layer_id();
     const auto  gcc  = fme::GenericComboClassWithFilter(
       gui_labels::layer_id,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       EmptyStringView{},
       [this]() -> auto  &{ return m_map_sprite->filter().layer_id; });
     if (!gcc.render())
     {
          return;
     }
     m_map_sprite->update_render_texture();
     m_changed = true;
}
void gui::combo_texture_pages()
{
     const auto &pair = m_map_sprite->uniques().texture_page_id();
     const auto  gcc  = fme::GenericComboClassWithFilter(
       gui_labels::texture_page,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       EmptyStringView{},
       [this]() -> auto  &{ return m_map_sprite->filter().texture_page_id; });
     if (!gcc.render())
     {
          return;
     }
     m_map_sprite->update_render_texture();
     m_changed = true;
}
void gui::combo_animation_ids()
{
     const auto &pair = m_map_sprite->uniques().animation_id();
     const auto  gcc  = fme::GenericComboClassWithFilter(
       gui_labels::animation_id,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       EmptyStringView{},
       [this]() -> auto  &{ return m_map_sprite->filter().animation_id; });
     if (!gcc.render())
     {
          return;
     }
     m_map_sprite->update_render_texture();
     m_changed = true;
}
void gui::combo_blend_other()
{
     const auto &pair = m_map_sprite->uniques().blend_other();
     const auto  gcc  = fme::GenericComboClassWithFilter(
       gui_labels::blend_other,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       EmptyStringView{},
       [this]() -> auto  &{ return m_map_sprite->filter().blend_other; });
     if (!gcc.render())
     {
          return;
     }
     m_map_sprite->update_render_texture();
     m_changed = true;
}


void gui::combo_z()
{
     const auto &pair = m_map_sprite->uniques().z();
     const auto  gcc  = fme::GenericComboClassWithFilter(
       gui_labels::z,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       EmptyStringView{},
       [this]() -> auto  &{ return m_map_sprite->filter().z; });
     if (!gcc.render())
     {
          return;
     }
     m_map_sprite->update_render_texture();
     m_changed = true;
}

void gui::combo_animation_frames()
{
     const auto &map = m_map_sprite->uniques().animation_frame();
     const auto &key = m_map_sprite->filter().animation_id.value();
     if (!map.contains(key))
     {
          return;
     }
     const auto &pair = map.at(key);
     const auto  gcc  = fme::GenericComboClassWithFilter(
       gui_labels::animation_frame,
       [&pair]() { return pair.values(); },
       [&pair]() { return pair.strings(); },
       EmptyStringView{},
       [this]() -> auto  &{ return m_map_sprite->filter().animation_frame; });
     if (!gcc.render())
     {
          return;
     }
     m_map_sprite->update_render_texture();
     m_changed = true;
}
BPPT gui::bpp() const
{
     return Mim::bpp_selections().at(static_cast<size_t>(m_selections->bpp));
}
void gui::combo_deswizzle_path()
{
     if (const safedir deswizzle_texture_path = m_loaded_deswizzle_texture_path; !deswizzle_texture_path.is_exists() || !m_field)
     {
          return;
     }
     std::vector<std::string> strings = { m_loaded_deswizzle_texture_path.string() };
     const auto               gcc     = fme::GenericComboClassWithFilter(
       gui_labels::deswizzle_path,
       //[&values]() { return values; },
       [&strings]() { return strings; },
       [&strings]() { return strings; },
       EmptyStringView{},
       [this]() -> auto                   &{ return m_map_sprite->filter().deswizzle; });
     if (gcc.render())
     {
          if (m_map_sprite->filter().deswizzle.enabled())
          {
               m_map_sprite->filter().upscale.disable();
          }
          m_map_sprite->update_render_texture(true);
          m_changed = true;
     }
}
void gui::combo_upscale_path()
{
     if (!m_field)
     {
          return;
     }
     if (!combo_upscale_path(m_map_sprite->filter().upscale))
     {
          return;
     }
     if (m_map_sprite->filter().upscale.enabled())
     {
          m_map_sprite->filter().deswizzle.disable();
     }
     m_map_sprite->update_render_texture(true);
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
     const auto gcc = fme::GenericComboClassWithFilter(
       gui_labels::upscale_path,
       [this]() { return m_upscale_paths; },
       [this]() { return m_upscale_paths; },
       [this]() { return m_upscale_paths; },
       [&filter]() -> auto & { return filter; },
       1);
     return m_field && gcc.render();
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

          const auto gcc = GenericComboClass(gui_labels::upscale_path, [&paths]() { return paths; }, [&paths]() { return paths; }, path);

          return gcc.render();
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
template<bool Nested>
std::vector<std::filesystem::path>
  gui::replace_entries(const open_viii::archive::FIFLFS<Nested> &field, const std::vector<std::filesystem::path> &paths) const
{
     auto tmp = open_viii::archive::replace_files<Nested>(field, paths);
     return tmp;
}


}// namespace fme