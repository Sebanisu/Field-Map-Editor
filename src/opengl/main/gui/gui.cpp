//
// Created by pcvii on 9/7/2021.
//
#include "gui.hpp"
#include "collapsing_tile_info.hpp"
#include "colors.hpp"
#include "EmptyStringIterator.hpp"
#include "gui_labels.hpp"
#include "imgui_color.hpp"
#include "main_menu_paths.hpp"
#include "open_file_explorer.hpp"
#include "path_search.hpp"
#include "push_pop_id.hpp"
#include "safedir.hpp"
#include "tool_tip.hpp"
#include "utilities.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <fmt/ranges.h>
#include <ImGuizmo.h>
#include <open_viii/paths/Paths.hpp>
#include <ranges>
#include <stacktrace>
#include <utility>

using namespace open_viii::graphics::background;
using namespace open_viii::graphics;
using namespace open_viii::graphics::literals;
using namespace std::string_literals;

struct mim_palette
{
     auto values() const
     {
          return Mim::palette_selections() | std::ranges::views::transform([](auto i) { return static_cast<std::uint8_t>(i); });
     }
     auto strings() const
     {
          return Mim::palette_selections_c_str() | std::ranges::views::transform([](std::string_view sv) { return sv; });
     }
     auto zip() const
     {
          return std::ranges::views::zip(values(), strings());
     }
};
struct mim_bpp
{
     auto values() const
     {
          return Mim::bpp_selections();
     }
     auto strings() const
     {
          return Mim::bpp_selections_c_str() | std::ranges::views::transform([](std::string_view sv) { return sv; });
     }
     auto zip() const
     {
          return std::ranges::views::zip(values(), strings());
     }
};

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
/**
 * @brief Checks if any index in the first range is also present in the second range.
 *
 * @tparam Range1 A range type satisfying std::ranges::range.
 * @tparam Range2 A range type satisfying std::ranges::range.
 * @param hovered_tiles_indices The first range of indices to check.
 * @param conflicts_range The second range of indices to compare against.
 * @return true If any index from `hovered_tiles_indices` is found in `conflicts_range`.
 * @return false Otherwise.
 */
static constexpr auto are_indices_in_both_ranges =
  [](std::ranges::range auto &&hovered_tiles_indices, std::ranges::range auto &&conflicts_range) -> bool {
     return std::ranges::any_of(
       std::forward<decltype(hovered_tiles_indices)>(hovered_tiles_indices), [&](const std::integral auto &hovered_tile) -> bool {
            return std::ranges::any_of(
              std::forward<decltype(conflicts_range)>(conflicts_range),
              [&](const std::integral auto &conflict_tile) -> bool { return std::cmp_equal(hovered_tile, conflict_tile); });
       });
};

// /**
//  * @brief Converts an ImVec4 color to an sf::Color.
//  *
//  * @param color The input color as ImVec4 (ImGui format, with floating-point components in the range [0, 1]).
//  * @return sf::Color The converted color, with each component scaled to the range [0, 255].
//  */
// static constexpr auto ImVec4ToSFColor = [](const ImVec4 &color) -> sf::Color {
//      return { static_cast<std::uint8_t>(color.x * 255.F),
//               static_cast<std::uint8_t>(color.y * 255.F),
//               static_cast<std::uint8_t>(color.z * 255.F),
//               static_cast<std::uint8_t>(color.w * 255.F) };
// };

/**
 * @brief Calculates the maximum number of buttons that can fit per row in the available content region.
 *
 * Ensures that the result is an even number and defaults to a minimum of 2 buttons per row.
 *
 * @param buttonWidth The width of each button.
 * @param buttonSpacing The spacing between buttons.
 * @return std::uint16_t The number of buttons that can fit per row, ensuring it's even and at least 2.
 */
static constexpr auto get_count_per_row = [](float buttonWidth, float buttonSpacing) -> std::uint16_t {
     const auto count_per_row_in =
       (std::max)(static_cast<std::uint16_t>((std::floor)(ImGui::GetContentRegionAvail().x / (buttonWidth + buttonSpacing))),
                  std::uint16_t{ 2 });
     return count_per_row_in % 2 != 0 ? static_cast<std::uint16_t>((std::max)(count_per_row_in - 1, 2)) : count_per_row_in;
};

/**
 * @brief Callback function for OpenGL debug messages.
 *
 * This function handles OpenGL debug output messages based on their severity level.
 * High-severity messages are logged as errors and trigger an exception.
 * Medium- and low-severity messages are logged for debugging purposes,
 * while notifications are logged with minimal impact.
 *
 * @param source The source of the debug message (e.g., API, shader compiler). May be unused.
 * @param type The type of the debug message (e.g., error, performance issue). May be unused.
 * @param id A unique identifier for the debug message. May be unused.
 * @param severity The severity of the debug message (e.g., high, medium, low, or notification).
 * @param length The length of the debug message string. May be unused.
 * @param message The actual debug message as a null-terminated string.
 * @param userParam A user-defined parameter passed to the callback. May be unused.
 */
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
          case GL_DEBUG_SEVERITY_HIGH: {
               spdlog::error("OpenGL high: {}", message);
               std::stacktrace st = std::stacktrace::current();
               std::cerr << st << std::endl;
               // throw;
          }
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
void gui::start(GLFWwindow *const window)
{
     bool stop_loop = false;
     // scale_window(static_cast<float>(m_selections->window_width), static_cast<float>(m_selections->window_height));
     (void)icons_font();
     do
     {
          m_changed      = false;
          get_imgui_id() = {};// reset id counter
          m_elapsed_time = m_delta_clock;
          consume_one_future();
          glfwPollEvents();// Input

          m_draw_window.update_mouse_positions();

          ImGui_ImplGlfw_NewFrame();
          ImGui_ImplOpenGL3_NewFrame();
          ImGui::NewFrame();
          ImGuizmo::BeginFrame();

          if (m_selections->force_reloading_of_textures)
          {
               refresh_render_texture(true);
          }
          else if (m_selections->force_rendering_of_map)
          {
               refresh_render_texture();
          }
          m_batch.update(m_elapsed_time);

          // Begin non imgui drawing.
          directory_browser_display();
          render_dockspace();
          menu_bar();
          //     popup_batch_deswizzle();
          //     popup_batch_reswizzle();
          if (m_selections->display_batch_window)
          {
               m_batch.draw_window();
          }
          m_custom_paths_window.render();
          m_field_file_window.render();

          if (toggle_imgui_demo_window)
          {
               ImGui::ShowDemoWindow();
          }
          control_panel_window();
          m_import.render();
          m_history_window.render();

          m_draw_window.render();
          //  m_mouse_positions.cover.setColor(clear_color);
          //  window.draw(m_mouse_positions.cover);
          ImGui::Render();

          int display_w, display_h;
          glfwGetFramebufferSize(window, &display_w, &display_h);
          glViewport(0, 0, display_w, display_h);

          // Clear and draw main viewport
          glengine::Renderer::Clear();
          ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

          ImGuiIO &io = ImGui::GetIO();
          if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
          {
               GLFWwindow *backup_current_context = glfwGetCurrentContext();

               // This must be called AFTER rendering the main draw data
               ImGui::UpdatePlatformWindows();
               ImGui::RenderPlatformWindowsDefault();

               glfwMakeContextCurrent(backup_current_context);
          }

          // Swap only after ALL ImGui draw calls
          glfwSwapBuffers(window);

     } while (!glfwWindowShouldClose(window) && !stop_loop);
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


     bind_shortcuts();
     const auto imgui_end = glengine::ScopeGuard(&ImGui::End);
     if (!ImGui::Begin("##FieldMapEditor", nullptr, window_flags))
     {
          return;
     }

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

     bool      &visible     = m_selections->display_control_panel_window;
     const auto pop_visible = glengine::ScopeGuard{ [this, &visible, was_visable = visible] {
          if (was_visable != visible)
          {
               m_selections->update_configuration_key(ConfigKey::DisplayControlPanelWindow);
          }
     } };
     const auto imgui_end = glengine::ScopeGuard(&ImGui::End);
     if (!ImGui::Begin(gui_labels::control_panel.data(), &visible))
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
     if (mim_test() || map_test())
     {
          background_color_picker();
          combo_draw();
     }
     combo_path();

     if (m_selections->paths_vector.empty())
     {
          return;
     }
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


     text_mouse_position();
     hovered_tiles_panel();
     selected_tiles_panel();
     tile_conflicts_panel();
}
void gui::tile_conflicts_panel()
{
     m_draw_window.hovered_index(-1);// reset to -1 and below we set to number if we're hovering.
     if (!map_test())
     {
          return;
     }
     m_map_sprite->const_visit_tiles_both([&](const auto &working_tiles, const auto &original_tiles) {
          if (!ImGui::CollapsingHeader("Conflicting Tiles"))
          {
               return;
          }
          static constexpr float buttonWidth        = 32.F;
          static constexpr float buttonSpacing      = 12.F;
          const auto             count_per_row      = get_count_per_row(buttonWidth, buttonSpacing);

          const auto            &conflicts          = m_map_sprite->working_conflicts();
          auto                   range_of_conflicts = conflicts.range_of_conflicts();
          const auto            &similar_counts     = m_map_sprite->working_similar_counts();
          const auto            &animation_counts   = m_map_sprite->working_animation_counts();

          const auto             pop_table_id       = PushPopID();
          // Default hover options for ImGuiCol_ButtonHovered
          const auto options_hover   = tile_button_options{ .size = { buttonWidth, buttonWidth }, .button_color = colors::ButtonHovered };

          // Default regular options for ImGuiCol_Button
          const auto options_regular = tile_button_options{ .size = { buttonWidth, buttonWidth } };

          // New options for when the similar count is greater than 1 (green tint)
          const auto options_similar = tile_button_options{ .size                = { buttonWidth, buttonWidth },
                                                            .button_color        = colors::ButtonGreen,
                                                            .button_hover_color  = colors::ButtonGreenHovered,
                                                            .button_active_color = colors::ButtonGreenActive };

          // New hover options for similar tiles (green hover)
          const auto options_similar_hover = tile_button_options{
               .size = { buttonWidth, buttonWidth }, .button_color = colors::ButtonGreenHovered// Green hover tint
          };

          // New options for when the animation count is greater than 1 (pink tint)
          const auto options_animation       = tile_button_options{ .size                = { buttonWidth, buttonWidth },
                                                                    .button_color        = colors::ButtonPink,
                                                                    .button_hover_color  = colors::ButtonPinkHovered,
                                                                    .button_active_color = colors::ButtonPinkActive };

          // New hover options for animation tiles (pink hover)
          const auto options_animation_hover = tile_button_options{
               .size = { buttonWidth, buttonWidth }, .button_color = colors::ButtonPinkHovered// pink hover tint
          };

          format_imgui_text("{}", "Legend: ");
          ImGui::SameLine();
          blue_color_button();
          const bool hovered_conflicts = ImGui::IsItemHovered();
          tool_tip("Button Color - Conflicts with different tiles.");
          ImGui::SameLine();
          green_color_button();
          const bool hovered_similar = ImGui::IsItemHovered();
          tool_tip("Button Color - Conflicts with similar tiles, or duplicate tiles.");
          ImGui::SameLine();
          pink_color_button();
          const bool hovered_animation = ImGui::IsItemHovered();
          tool_tip("Button Color - Conflicts with similar tiles with different animation frame or blend modes.");


          for (const auto &conflict_group : range_of_conflicts)
          {
               {
                    const auto  first_index = conflict_group.front();
                    const auto &first_tile  = [&]() {
                         auto begin = std::ranges::cbegin(working_tiles);
                         std::ranges::advance(begin, first_index);
                         return *begin;
                    }();

                    format_imgui_text(
                      "X = {}, (X + T * 256 = {}), Y = {}, T = {}: ",
                      first_tile.source_x(),
                      first_tile.source_x() + first_tile.texture_id() * ff_8::source_tile_conflicts::GRID_SIZE,
                      first_tile.source_y(),
                      first_tile.texture_id());
               }
               format_imgui_text("index count: {}", std::ranges::size(conflict_group));
               if (ImGui::BeginTable(
                     "##table_overlaps",
                     count_per_row,
                     ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX))
               {

                    for (const auto index : conflict_group)
                    {
                         assert(std::cmp_less(index, std::ranges::size(working_tiles)) && "index out of range!");
                         assert(std::cmp_less(index, std::ranges::size(original_tiles)) && "index out of range!");


                         // ImGui::TableNextColumn();
                         // format_imgui_text("{:4}", index);

                         ImGui::TableNextColumn();
                         const auto get_tile = [&](const auto &tiles) -> decltype(auto) {
                              auto begin = std::ranges::cbegin(tiles);
                              std::ranges::advance(begin, index);
                              return *begin;
                         };

                         const auto &working_tile  = get_tile(working_tiles);
                         const auto &original_tile = get_tile(original_tiles);
                         assert(similar_counts.contains(working_tile) && "Tile wasn't in the map");
                         const auto similar_count  = similar_counts.at(working_tile);
                         const bool similar_over_1 = std::cmp_greater(similar_count, 1);

                         assert(animation_counts.contains(working_tile) && "Tile wasn't in the map");
                         const auto animation_count  = animation_counts.at(working_tile);
                         const bool animation_over_1 = std::cmp_greater(animation_count, 1);

                         const auto options          = [&]() {
                              if (animation_over_1 && !similar_over_1 && hovered_animation)
                              {
                                   return options_animation_hover;
                              }
                              if (similar_over_1 && hovered_similar)
                              {
                                   return options_similar_hover;
                              }
                              if (!similar_over_1 && !animation_over_1 && hovered_conflicts)
                              {
                                   return options_hover;
                              }
                              if (
                                std::ranges::empty(m_draw_window.hovered_tiles_indices())
                                || std::ranges::find(m_draw_window.hovered_tiles_indices(), static_cast<std::size_t>(index))
                                     == std::ranges::end(m_draw_window.hovered_tiles_indices())
                                || !m_draw_window.mouse_positions().mouse_enabled)
                              {
                                   if (similar_over_1)
                                   {
                                        return options_similar;
                                   }
                                   if (animation_over_1)
                                   {
                                        return options_animation;
                                   }
                                   return options_regular;
                              }
                              if (similar_over_1)
                              {
                                   return options_similar_hover;
                              }
                              if (animation_over_1)
                              {
                                   return options_animation_hover;
                              }
                              return options_hover;
                         }();

                         (void)create_tile_button(m_map_sprite, original_tile, options);
                         if (ImGui::IsItemHovered())
                         {
                              m_draw_window.hovered_index(index);
                         }
                         // Ensure subsequent buttons are on the same row
                         std::string strtooltip = fmt::format(
                           "Index {}\n{}\nSimilar Count: {}\nAnimation Count: {}",
                           index,
                           working_tile,
                           similar_over_1 ? similar_count : 0,
                           animation_over_1 ? animation_count : 0);
                         tool_tip(strtooltip);
                    }
                    // Break the line after finishing a conflict group

                    ImGui::EndTable();
               }
          }
     });
}

void gui::selected_tiles_panel()
{


     if (!map_test())
     {
          return;
     }

     if (std::ranges::empty(m_draw_window.clicked_tile_indices()))
     {
          return;
     }

     std::optional<std::size_t> remove_index = {};
     m_map_sprite->const_visit_original_tiles([&](const auto &original_tiles) {
          m_map_sprite->const_visit_working_tiles([&](const auto &working_tiles) {
               for (const auto &i : m_draw_window.clicked_tile_indices())
               {
                    if (i < std::ranges::size(original_tiles) && i < std::ranges::size(working_tiles))
                    {
                         const auto &original_tile = original_tiles[i];
                         const auto &working_tile  = working_tiles[i];
                         if (collapsing_tile_info(m_map_sprite, original_tile, working_tile, {}, i))
                         {
                              remove_index = i;
                         }
                    }
               }
          });
     });
     if (remove_index.has_value())
     {
          m_draw_window.remove_clicked_index(remove_index.value());
     }
}
void gui::control_panel_window_mim()
{
     checkbox_mim_palette_texture();
     if (ImGui::CollapsingHeader(gui_labels::filters.data()))
     {
          if (!m_mim_sprite->draw_palette())
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
     combo_swizzle_path();
     combo_deswizzle_path();
     combo_swizzle_as_one_image_path();
     combo_swizzle_map_path();
     combo_deswizzle_map_path();
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
     const ImVec2 button_size = { ImGui::GetFrameHeight() * 5.50F, ImGui::GetFrameHeight() };
     if (!ImGui::BeginTable("##compact buttons", 4, ImGuiTableFlags_SizingFixedSame))
     {
          return;
     }
     ImGui::TableNextRow();
     ImGui::TableNextColumn();
     format_imgui_text("{}: ", gui_labels::compact);
     tool_tip(gui_labels::compact_tooltip);
     ImGui::TableNextColumn();
     if (ImGui::Button(gui_labels::rows.data(), button_size))
     {
          m_map_sprite->compact_rows();
     }
     tool_tip(gui_labels::compact_rows_tooltip);
     ImGui::TableNextColumn();
     if (ImGui::Button(gui_labels::all.data(), button_size))
     {
          m_map_sprite->compact_all();
     }
     tool_tip(gui_labels::compact_all_tooltip);
     ImGui::TableNextRow();
     ImGui::TableNextColumn();
     ImGui::TableNextColumn();
     if (ImGui::Button(gui_labels::move_conflicts.data(), button_size))
     {
          m_map_sprite->compact_move_conflicts_only();
     }
     tool_tip(gui_labels::move_conflicts_only_tooltip);
     ImGui::TableNextRow();
     ImGui::TableNextColumn();
     ImGui::TableNextColumn();
     if (ImGui::Button(gui_labels::map_order.data(), button_size))
     {
          m_map_sprite->compact_map_order();
     }
     tool_tip(gui_labels::compact_map_order_tooltip);
     ImGui::TableNextColumn();
     if (ImGui::Button(gui_labels::compact_map_order_ffnx.data(), button_size))
     {
          m_map_sprite->compact_map_order_ffnx();
     }
     tool_tip(gui_labels::compact_map_order_ffnx_tooltip);
     ImGui::TableNextRow();
     ImGui::TableNextColumn();
     format_imgui_text("{}: ", gui_labels::flatten);
     tool_tip(gui_labels::flatten_tooltip);
     ImGui::TableNextColumn();
     if (ImGui::Button(gui_labels::bpp.data(), button_size))
     {
          m_map_sprite->flatten_bpp();
     }
     tool_tip(gui_labels::flatten_bpp_tooltip);
     ImGui::TableNextColumn();
     if (ImGui::Button(gui_labels::palette.data(), button_size))
     {
          m_map_sprite->flatten_palette();
     }
     tool_tip(gui_labels::flatten_palette_tooltip);

     ImGui::EndTable();
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
bool gui::change_background_color(const fme::color &in_color)
{
     if (m_selections->background_color == in_color)
     {
          return false;
     }
     m_selections->background_color = in_color;
     return true;
}
void gui::save_background_color()
{
     spdlog::info("selections_background_color: {}", m_selections->background_color);
     m_selections->update_configuration_key(ConfigKey::BackgroundColor);
}
void gui::background_color_picker()
{
     clear_color_f = { static_cast<float>(m_selections->background_color.r) / 255.F,
                       static_cast<float>(m_selections->background_color.g) / 255.F,
                       static_cast<float>(m_selections->background_color.b) / 255.F };
     if (ImGui::ColorEdit3(gui_labels::background.data(), clear_color_f.data(), ImGuiColorEditFlags_DisplayRGB))
     {
          change_background_color({ clear_color_f[0], clear_color_f[1], clear_color_f[2] });
     }
     if (ImGui::IsItemDeactivatedAfterEdit())
     {
          save_background_color();
     }
}

void gui::consume_one_future()
{
     // static constexpr float interval           = 0.013f;// the interval in seconds
     // static float           total_elapsed_time = 0.0f;// keep track of the elapsed time using a static variable

     // total_elapsed_time += m_elapsed_time;// add the elapsed time since last update

     // if (total_elapsed_time < interval)
     // {
     //      return;
     // }
     // // perform your operation here
     // total_elapsed_time = 0.f;// reset the elapsed time

     m_map_sprite->consume_one_future();
     if (!m_future_of_future_paths_consumer.done())
     {
          ++m_future_of_future_paths_consumer;
     }
     else if (m_future_of_future_paths_consumer.consumer_ready())
     {
          m_future_paths_consumer += m_future_of_future_paths_consumer.get_consumer();
     }
     m_future_paths_consumer.consume_one_with_callback([&](PathsAndEnabled pande) {
          auto to_string = [](ConfigKey key) -> std::string_view {
               using namespace std::string_view_literals;
               switch (key)
               {
                    case ConfigKey::CacheUpscalePaths:
                         return "CacheUpscalePaths"sv;
                    case ConfigKey::CacheDeswizzlePaths:
                         return "CacheDeswizzlePaths"sv;
                    case ConfigKey::CacheUpscaleMapPaths:
                         return "CacheUpscaleMapPaths"sv;
                    case ConfigKey::CacheDeswizzleMapPaths:
                         return "CacheDeswizzleMapPaths"sv;
                    case ConfigKey::CacheUpscalePathsEnabled:
                         return "CacheUpscalePathsEnabled"sv;
                    case ConfigKey::CacheDeswizzlePathsEnabled:
                         return "CacheDeswizzlePathsEnabled"sv;
                    case ConfigKey::CacheUpscaleMapPathsEnabled:
                         return "CacheUpscaleMapPathsEnabled"sv;
                    case ConfigKey::CacheDeswizzleMapPathsEnabled:
                         return "CacheDeswizzleMapPathsEnabled"sv;
                    default: {
                         spdlog::error("unhandled key type");
                    }
               }
               return ""sv;
          };

          spdlog::info("Updating path config [{}] with {} entries", to_string(pande.path_key), pande.path.size());
          switch (pande.path_key)
          {
               case ConfigKey::CacheUpscalePaths:
                    m_selections->cache_swizzle_paths = std::move(pande.path);
                    break;
               case ConfigKey::CacheDeswizzlePaths:
                    m_selections->cache_deswizzle_paths = std::move(pande.path);
                    break;
               case ConfigKey::CacheUpscaleMapPaths:
                    m_selections->cache_swizzle_map_paths = std::move(pande.path);
                    break;
               case ConfigKey::CacheDeswizzleMapPaths:
                    m_selections->cache_deswizzle_map_paths = std::move(pande.path);
                    break;
               default: {
                    spdlog::error("unhandled key type");
               }
          }
          m_selections->update_configuration_key(pande.path_key);

          spdlog::info("Updating enabled config [{}] with {} entries", to_string(pande.enabled_key), pande.enabled.size());
          switch (pande.enabled_key)
          {
               case ConfigKey::CacheUpscalePathsEnabled:
                    m_selections->cache_swizzle_paths_enabled = std::move(pande.enabled);
                    break;
               case ConfigKey::CacheDeswizzlePathsEnabled:
                    m_selections->cache_deswizzle_paths_enabled = std::move(pande.enabled);
                    break;
               case ConfigKey::CacheUpscaleMapPathsEnabled:
                    m_selections->cache_swizzle_map_paths_enabled = std::move(pande.enabled);
                    break;
               case ConfigKey::CacheDeswizzleMapPathsEnabled:
                    m_selections->cache_deswizzle_map_paths_enabled = std::move(pande.enabled);
                    break;
               default: {
                    spdlog::error("unhandled key type");
               }
          }
          m_selections->update_configuration_key(pande.enabled_key);
     });

     if (!m_future_of_future_consumer.done())
     {
          ++m_future_of_future_consumer;
     }
     else if (m_future_of_future_consumer.consumer_ready())
     {
          m_future_consumer += m_future_of_future_consumer.get_consumer();
     }
     ++m_future_consumer;
}

void gui::text_mouse_position() const
{

     // Display texture coordinates if they are set
     if (!m_draw_window.mouse_positions().mouse_enabled)
     {
          format_imgui_text("{}", gui_labels::mouse_not_over);
          return;
     }
     else
     {
          format_imgui_text(
            "{}: ({:4}, {:3})", gui_labels::mouse_pos, m_draw_window.mouse_positions().pixel.x, m_draw_window.mouse_positions().pixel.y);
          ImGui::SameLine();
          const int tile_size = 16;
          format_imgui_text(
            "{}: ({:2}, {:2})",
            gui_labels::tile_pos,
            m_draw_window.mouse_positions().pixel.x / tile_size,
            m_draw_window.mouse_positions().pixel.y / tile_size);
          if (m_selections->draw_swizzle)
          {
               format_imgui_text("{}: {:2}", gui_labels::page, m_draw_window.mouse_positions().texture_page);
          }
     }
}
void gui::hovered_tiles_panel()
{
     if (!map_test())
     {
          return;
     }


     m_map_sprite->const_visit_tiles_both([&](const auto &working_tiles, const auto &original_tiles) {
          const auto &hovered_tiles_indices = m_draw_window.hovered_tiles_indices();
          format_imgui_text("{} {:4}", gui_labels::number_of_tiles, std::ranges::size(hovered_tiles_indices));
          if (!ImGui::CollapsingHeader(gui_labels::hovered_tiles.data()))
          {
               return;
          }
          if (!m_draw_window.mouse_positions().mouse_enabled)
          {
               return;
          }
          if (std::ranges::empty(hovered_tiles_indices))
          {
               return;
          }
          static constexpr float buttonWidth   = 32.F;
          static constexpr float buttonSpacing = 12.F;
          const auto             columns       = get_count_per_row(buttonWidth, buttonSpacing);

          if (!ImGui::BeginTable(
                "##table", columns, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX))
          {
               return;
          }
          const auto &conflicts_obj          = m_map_sprite->working_conflicts();
          auto        conflict_range         = conflicts_obj.range_of_conflicts_flattened();
          const bool  hovering_over_conflict = are_indices_in_both_ranges(hovered_tiles_indices, conflict_range);


          const auto &similar_counts         = m_map_sprite->working_similar_counts();
          const auto &animation_counts       = m_map_sprite->working_animation_counts();
          for (const auto index : hovered_tiles_indices)
          {
               assert(std::cmp_less(index, std::ranges::size(original_tiles)) && "index out of range!");
               const auto c = [&]() {
                    const auto &working_tile = working_tiles[index];
                    if (hovering_over_conflict)
                    {
                         if (similar_counts.contains(working_tile) && std::cmp_greater(similar_counts.at(working_tile), 1))
                         {
                              return colors::ButtonGreenHovered;
                         }
                         if (animation_counts.contains(working_tile) && std::cmp_greater(animation_counts.at(working_tile), 1))
                         {
                              return colors::ButtonPinkHovered;
                         }
                         return colors::ButtonHovered;
                    }

                    if (similar_counts.contains(working_tile) && std::cmp_greater(similar_counts.at(working_tile), 1))
                    {
                         return colors::ButtonGreen;
                    }
                    if (animation_counts.contains(working_tile) && std::cmp_greater(animation_counts.at(working_tile), 1))
                    {
                         return colors::ButtonPink;
                    }
                    return colors::Button;
               }();
               const auto options = tile_button_options{ .size = { buttonWidth, buttonWidth }, .button_color = c };


               ImGui::TableNextColumn();
               format_imgui_text("{:4}", index);
               ImGui::TableNextColumn();

               (void)create_tile_button(m_map_sprite, original_tiles[index], options);
          }
          ImGui::EndTable();
     });
}
void gui::combo_coo()
{
     constexpr static auto values = open_viii::LangCommon::to_array();
     const auto            gcc    = GenericComboClass(
       gui_labels::language, []() { return values; }, []() { return values | std::views::transform(AsString{}); }, m_selections->coo);
     if (gcc.render())
     {
          refresh_coo();
     }
     else
     {
          tool_tip(gui_labels::language_dropdown_tool_tip);
     }
}

void gui::refresh_coo()
{
     update_field();
     spdlog::info("selections_coo: {}", m_selections->coo);
     m_selections->update_configuration_key(ConfigKey::Coo);
}
const open_viii::LangT &gui::get_coo() const
{
     static constexpr auto coos = open_viii::LangCommon::to_array();
     return coos.at(static_cast<size_t>(m_selections->coo));
}

/**
 * @brief Updates the configuration with the current field selection and refreshes the field.
 *
 * Retrieves the field name from the field index, updates the configuration with this field name,
 * saves the configuration, and then triggers a field update.
 */
void gui::refresh_field()
{
     // Get a reference to the map data (field name list)
     const auto &maps            = m_archives_group->mapdata();

     // Update the starter_field name based on the current field index
     m_selections->starter_field = *std::next(maps.begin(), m_field_index);

     // Save the selected field name to the configuration
     m_selections->update_configuration_key(ConfigKey::StarterField);

     // Apply the updated field selection
     update_field();
}


void gui::combo_field()
{
     const auto gcc = GenericComboClass(
       gui_labels::field,
       [this]() { return std::views::iota(0, static_cast<int>(std::ranges::ssize(m_archives_group->mapdata()))); },
       [this]() {
            return m_archives_group->mapdata() | std::ranges::views::transform([](const std::string &str) -> std::string_view {
                        using namespace std::string_view_literals;
                        return std::string_view(str).starts_with("ma"sv) ? ""sv : str;
                   });
       },
       m_field_index);

     if (gcc.render())
     {
          refresh_field();
     }
}

/**
 * @brief Updates the currently loaded field and refreshes all dependent components.
 *
 * Loads the selected field archive based on the current selection index,
 * refreshes the UI elements tied to field content, and updates any stateful
 * drawing components depending on the draw mode.
 *
 * Also resets cached texture paths and clears clicked tile selections.
 */
void gui::update_field()
{
     // Load the selected field archive
     m_field = m_archives_group->field(m_field_index);

     // Refresh the file window with the new field
     m_field_file_window.refresh(m_field);

     // Update UI/rendering components depending on the selected draw mode
     switch (m_selections->draw)
     {
          case draw_mode::draw_mim:
               // Update MIM sprite with the new field
               *m_mim_sprite = m_mim_sprite->with_field(m_field);
               // m_draw_window.update(m_mim_sprite);
               break;

          case draw_mode::draw_map:
               // Update map sprite and associated UI elements
               *m_map_sprite = m_map_sprite->with_field(m_field, get_coo());
               // m_draw_window.update(m_map_sprite);
               // m_import.update(m_map_sprite);
               // m_history_window.update(m_map_sprite);
               break;
     }

     // Reset cached texture paths
     // m_loaded_swizzle_texture_path   = std::filesystem::path{};
     m_loaded_deswizzle_texture_path = std::filesystem::path{};

     // Mark field as changed
     m_changed                       = true;

     // Okay it's possible this doesn't need to be called everytime because if the paths are generic enough and nothing major changed.
     // Generate swizzle texture paths if a field is loaded
     if (m_field)
     {
          m_future_of_future_paths_consumer += generate_swizzle_paths();
          m_future_of_future_paths_consumer += generate_deswizzle_paths();
          m_future_of_future_paths_consumer += generate_swizzle_map_paths();
          m_future_of_future_paths_consumer += generate_deswizzle_map_paths();
          sort_paths();
     }

     // Clear clicked tile indices used for selection logic
     m_draw_window.clear_clicked_tile_indices();
}

void gui::refresh_map_swizzle()
{
     m_selections->update_configuration_key(ConfigKey::DrawSwizzle);
     if (m_selections->draw_swizzle)
     {
          spdlog::info("selections_draw_swizzle: enabled");
          m_map_sprite->enable_disable_blends();
          m_map_sprite->enable_draw_swizzle();
     }
     else
     {
          spdlog::info("selections_draw_swizzle: disabled");
          m_map_sprite->disable_draw_swizzle();
          if (!m_selections->draw_disable_blending)
          {
               m_map_sprite->disable_disable_blends();
          }
     }
     m_changed = true;
}
void gui::checkbox_map_swizzle()
{
     if (ImGui::Checkbox(gui_labels::swizzle.data(), &m_selections->draw_swizzle))
     {
          refresh_map_swizzle();
     }
     tool_tip(gui_labels::swizzle_tooltip);
}
void gui::refresh_map_disable_blending()
{
     m_selections->update_configuration_key(ConfigKey::DrawDisableBlending);
     if (m_selections->draw_disable_blending)
     {
          spdlog::info("selections_draw_disable_blending: enabled");
          m_map_sprite->enable_disable_blends();
     }
     else
     {
          spdlog::info("selections_draw_disable_blending: disabled");
          m_map_sprite->disable_disable_blends();
     }
     m_changed = true;
}
void gui::checkbox_map_disable_blending()
{
     if (!m_selections->draw_swizzle)
     {
          if (ImGui::Checkbox(gui_labels::disable_blending.data(), &m_selections->draw_disable_blending))
          {
               refresh_map_disable_blending();
          }
          else
          {
               tool_tip(gui_labels::disable_blending_tooltip);
          }
     }
     else
     {
          static const bool checked = true;
          ImGui::BeginDisabled();
          ImGui::Checkbox(gui_labels::disable_blending.data(), const_cast<bool *>(&checked));
          tool_tip(gui_labels::disable_blending_tooltip);
          tool_tip(gui_labels::forced_on_while_swizzled);
          ImGui::EndDisabled();
     }
     if (ImGui::Checkbox(gui_labels::force_reloading_of_textures.data(), &m_selections->force_reloading_of_textures))
     {
          m_selections->update_configuration_key(ConfigKey::ForceReloadingOfTextures);
     }
     else
     {
          tool_tip(gui_labels::force_reloading_of_textures_tooltip);
     }

     if (!m_selections->force_reloading_of_textures)
     {
          if (ImGui::Checkbox(gui_labels::force_rendering_of_map.data(), &m_selections->force_rendering_of_map))
          {
               m_selections->update_configuration_key(ConfigKey::ForceRenderingOfMap);
          }
          else
          {
               tool_tip(gui_labels::force_rendering_of_map_tooltip);
          }
     }
     else
     {
          ImGui::BeginDisabled();
          static const bool checked = true;
          ImGui::Checkbox(gui_labels::force_rendering_of_map.data(), const_cast<bool *>(&checked));
          tool_tip(gui_labels::force_rendering_of_map_tooltip);
          ImGui::EndDisabled();
     }
}
void gui::refresh_mim_palette_texture()
{
     spdlog::info("selections_draw_palette: {}", m_selections->draw_palette ? "enabled" : "disabled");
     m_selections->update_configuration_key(ConfigKey::DrawPalette);
     *m_mim_sprite = m_mim_sprite->with_draw_palette(m_selections->draw_palette);
     // m_draw_window.update(m_mim_sprite);
     m_changed     = true;
}
void gui::checkbox_mim_palette_texture()
{
     if (ImGui::Checkbox(gui_labels::draw_palette_texture.data(), &m_selections->draw_palette))
     {
          refresh_mim_palette_texture();
     }
     tool_tip(gui_labels::draw_palette_texture_tooltip);
}
static void update_bpp(mim_sprite &sprite, BPPT bpp)
{
     sprite = sprite.with_bpp(bpp);
}
static void update_bpp(map_sprite &sprite, [[maybe_unused]] BPPT bpp)
{
     sprite.update_render_texture();
}
void gui::refresh_bpp(BPPT in_bpp)
{
     m_selections->bpp = in_bpp;
     if (m_map_sprite)
     {
          m_map_sprite->filter().bpp.update(in_bpp);
     }
     else
     {
          m_selections->update_configuration_key(ConfigKey::Bpp);
     }
     if (mim_test())
     {
          update_bpp(*m_mim_sprite, bpp());
     }
     if (map_test())
     {
          update_bpp(*m_map_sprite, bpp());
     }
     m_changed = true;
}
void gui::combo_bpp()
{
     {
          const auto gcc = GenericComboClass(
            gui_labels::bpp,
            [&]() { return Mim::bpp_selections(); },
            [&]() { return Mim::bpp_selections_c_str() | std::ranges::views::transform([](std::string_view sv) { return sv; }); },
            m_selections->bpp);

          if (!gcc.render())
          {
               return;
          }
          gui::refresh_bpp(m_selections->bpp);
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
static void update_palette(map_sprite &sprite, [[maybe_unused]] uint8_t palette)
{
     sprite.update_render_texture();
}
void gui::combo_palette()
{
     if (m_selections->bpp != BPPT::BPP16_CONST())
     {
          {
               static constexpr auto palette_values  = Mim::palette_selections();
               static constexpr auto palette_strings = Mim::palette_selections_c_str();
               const auto            gcc             = GenericComboClass(
                 gui_labels::palette,
                 []() { return palette_values | std::ranges::views::transform([](auto i) { return static_cast<uint8_t>(i); }); },
                 []() { return palette_strings | std::ranges::views::transform([](std::string_view sv) { return sv; }); },
                 m_selections->palette);
               if (gcc.render())
               {
                    refresh_palette(m_selections->palette);
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
     const auto end_menu_bar = glengine::ScopeGuard(&ImGui::EndMainMenuBar);
     //  if (!ImGui::BeginMenuBar())
     //    return;
     //  const auto end_menu_bar = glengine::ScopeGuard(&ImGui::EndMenuBar);

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
     const auto end_menu = glengine::ScopeGuard(&ImGui::EndMenu);
     if (ImGui::MenuItem(gui_labels::display_control_panel_window.data(), "Control + P", &m_selections->display_control_panel_window))
     {
          m_selections->update_configuration_key(ConfigKey::DisplayControlPanelWindow);
     }
     if (ImGui::MenuItem("ImGui Demo Window", std::nullptr_t{}, &toggle_imgui_demo_window))
     {
     }
     if (!map_test() && !mim_test())
     {
          return;
     }
     if (ImGui::MenuItem(gui_labels::batch_operation_window.data(), "Control + B", &m_selections->display_batch_window))
     {
          m_selections->update_configuration_key(ConfigKey::DisplayBatchWindow);
     }
     // todo fix imports
     //  if (ImGui::MenuItem(gui_labels::import_page.data(), "Control + I", &m_selections->display_import_image_window))
     //  {
     //       m_selections->update_configuration_key(ConfigKey::DisplayImportImageWindow);
     //  }
     ImGui::Separator();
     if (ImGui::MenuItem(gui_labels::display_history.data(), "Control + H", &m_selections->display_history_window))
     {
          m_selections->update_configuration_key(ConfigKey::DisplayHistoryWindow);
     }
     ImGui::Separator();
     if (ImGui::MenuItem(gui_labels::display_draw_window.data(), "Control + D", &m_selections->display_draw_window))
     {
          m_selections->update_configuration_key(ConfigKey::DisplayDrawWindow);
     }
     ImGui::Separator();
     if (ImGui::MenuItem(gui_labels::display_custom_paths_window.data(), "Control + U", &m_selections->display_custom_paths_window))
     {
          m_selections->update_configuration_key(ConfigKey::DisplayCustomPathsWindow);
     }
     ImGui::Separator();
     if (ImGui::MenuItem(gui_labels::display_field_file_window.data(), "Control + F", &m_selections->display_field_file_window))
     {
          m_selections->update_configuration_key(ConfigKey::DisplayFieldFileWindow);
     }
}
void gui::edit_menu()
{
     if (!map_test() && !mim_test())
     {
          return;
     }
     if (!ImGui::BeginMenu(gui_labels::edit.data()))
     {
          return;
     }
     const auto end_menu = glengine::ScopeGuard(&ImGui::EndMenu);
     if (map_test())
     {
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
               m_selections->update_configuration_key(ConfigKey::DisplayHistoryWindow);
          }
          ImGui::Separator();
     }

     if (map_test() || mim_test())
     {
          if (ImGui::BeginMenu(gui_labels::draw.data()))
          {
               const auto pop_menu = glengine::ScopeGuard(&ImGui::EndMenu);
               {
                    static const constinit auto iota_draw_mode =
                      std::views::iota(0, 2) | std::views::transform([](const int mode) { return static_cast<draw_mode>(mode); });
                    static const auto str_draw_mode =
                      iota_draw_mode | std::views::transform([](draw_mode in_draw_mode) { return fmt::format("{}", in_draw_mode); });
                    auto zip_modes = std::ranges::views::zip(iota_draw_mode, str_draw_mode);
                    for (auto &&[mode, str] : zip_modes)
                    {
                         bool care_not = m_selections->draw == mode;
                         if (ImGui::MenuItem(str.data(), nullptr, &care_not, !care_not))
                         {
                              if (m_selections->draw != mode)
                              {
                                   m_selections->draw = mode;
                                   refresh_draw_mode();
                              }
                         }
                    }
               }


               ImGui::Separator();

               if (map_test())
               {

                    static const constinit std::array<bool, 2>             swizzle_value  = { true, false };
                    static const constinit std::array<std::string_view, 2> swizzle_string = { gui_labels::swizzle, gui_labels::deswizzle };
                    static const constinit std::array<std::string_view, 2> swizzle_tooltips = { gui_labels::swizzle_tooltip,
                                                                                                gui_labels::deswizzle_tooltip };
                    static auto constinit zip_modes = std::ranges::views::zip(swizzle_value, swizzle_string, swizzle_tooltips);
                    for (auto &&[mode, str, tool_tip_str] : zip_modes)
                    {
                         bool care_not = m_selections->draw_swizzle == mode;
                         if (ImGui::MenuItem(str.data(), nullptr, &care_not, !care_not))
                         {
                              if (m_selections->draw_swizzle != mode)
                              {
                                   m_selections->draw_swizzle = mode;
                                   refresh_map_swizzle();
                              }
                         }
                         else
                         {
                              tool_tip(tool_tip_str);
                         }
                    }
               }

               if (map_test())
               {


                    if (!m_selections->draw_swizzle)
                    {
                         if (

                           ImGui::MenuItem(gui_labels::disable_blending.data(), nullptr, &m_selections->draw_disable_blending))
                         {
                              refresh_map_disable_blending();
                         }
                         else
                         {
                              tool_tip(gui_labels::disable_blending_tooltip);
                         }
                    }
                    else
                    {
                         static const bool checked = true;
                         ImGui::BeginDisabled();
                         ImGui::MenuItem(gui_labels::disable_blending.data(), nullptr, const_cast<bool *>(&checked));
                         tool_tip(gui_labels::disable_blending_tooltip);
                         tool_tip(gui_labels::forced_on_while_swizzled);
                         ImGui::EndDisabled();
                    }

                    if (ImGui::MenuItem(
                          gui_labels::force_reloading_of_textures.data(), nullptr, &m_selections->force_reloading_of_textures))
                    {
                         m_selections->update_configuration_key(ConfigKey::ForceReloadingOfTextures);
                    }
                    else
                    {
                         tool_tip(gui_labels::force_reloading_of_textures_tooltip);
                    }

                    if (!m_selections->force_reloading_of_textures)
                    {
                         if (ImGui::MenuItem(gui_labels::force_rendering_of_map.data(), nullptr, &m_selections->force_rendering_of_map))
                         {
                              m_selections->update_configuration_key(ConfigKey::ForceRenderingOfMap);
                         }
                         else
                         {
                              tool_tip(gui_labels::force_rendering_of_map_tooltip);
                         }
                    }
                    else
                    {
                         ImGui::BeginDisabled();
                         static const bool checked = true;
                         ImGui::MenuItem(gui_labels::force_rendering_of_map.data(), nullptr, const_cast<bool *>(&checked));
                         tool_tip(gui_labels::force_rendering_of_map_tooltip);
                         ImGui::EndDisabled();
                    }
               }

               if (mim_test())
               {
                    if (ImGui::MenuItem(gui_labels::draw_palette_texture.data(), nullptr, &m_selections->draw_palette))
                    {
                         refresh_mim_palette_texture();
                    }
                    else
                    {
                         tool_tip(gui_labels::draw_palette_texture_tooltip);
                    }
               }

               ImGui::Separator();

               if (ImGui::MenuItem(gui_labels::draw_tile_grid.data(), nullptr, &m_selections->draw_grid))
               {
                    spdlog::info("selections_draw_grid: {}", m_selections->draw_grid ? "enabled" : "disabled");
                    m_selections->update_configuration_key(ConfigKey::DrawGrid);
               }

               if ((map_test() && m_selections->draw_swizzle) || (mim_test() && !m_selections->draw_palette))
               {
                    if (ImGui::MenuItem(gui_labels::draw_texture_page_grid.data(), nullptr, &m_selections->draw_texture_page_grid))
                    {
                         spdlog::info(
                           "selections_draw_texture_page_grid: {}", m_selections->draw_texture_page_grid ? "enabled" : "disabled");
                         m_selections->update_configuration_key(ConfigKey::DrawTexturePageGrid);
                    }
               }

               if (map_test())
               {
                    if (ImGui::MenuItem(gui_labels::draw_tile_conflict_rects.data(), nullptr, &m_selections->draw_tile_conflict_rects))
                    {
                         spdlog::info(
                           "selections_draw_tile_conflict_rects: {}", m_selections->draw_tile_conflict_rects ? "enabled" : "disabled");
                         m_selections->update_configuration_key(ConfigKey::DrawTileConflictRects);
                    }
               }
          }

          if (ImGui::BeginMenu("Background Color"))
          {
               float         sz       = ImGui::GetTextLineHeight();
               auto          zip_view = std::ranges::views::zip(fme::colors::ColorValues, fme::colors::ColorNames);
               constexpr int columns  = 2;
               if (ImGui::BeginTable("##ColorTable", columns, ImGuiTableFlags_SizingStretchSame))
               {
                    for (auto &&[color_value, color_name] : zip_view)
                    {
                         if (std::cmp_less(color_value.a, 255))
                         {
                              continue;
                         }
                         ImGui::TableNextColumn();
                         ImVec2 p = ImGui::GetCursorScreenPos();
                         ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImU32{ color_value });
                         ImGui::Dummy(ImVec2(sz, sz));
                         ImGui::SameLine();
                         if (ImGui::MenuItem(color_name.data()))
                         {
                              if (change_background_color(color_value))
                              {
                                   save_background_color();
                              }
                         }
                    }
                    ImGui::EndTable();
               }
               if (ImGui::ColorPicker3("##Choose Background Color", clear_color_f.data(), ImGuiColorEditFlags_DisplayRGB))
               {
                    change_background_color({ clear_color_f[0], clear_color_f[1], clear_color_f[2] });
               }
               if (ImGui::IsItemDeactivatedAfterEdit())
               {
                    save_background_color();
               }
               ImGui::EndMenu();
          }

          if (ImGui::BeginMenu(gui_labels::filters.data()))
          {
               const auto pop_menu            = glengine::ScopeGuard(&ImGui::EndMenu);


               const auto generic_filter_menu = [&](
                                                  std::string_view                       label,
                                                  ff_8::HasValuesAndStringsAndZip auto &&pair,
                                                  ff_8::IsFilterOld auto                &filter,
                                                  std::invocable auto                  &&lambda) {
                    if (ImGui::BeginMenu(label.data()))
                    {
                         const auto pop_menu1       = glengine::ScopeGuard(&ImGui::EndMenu);
                         const auto process_element = [&](auto &value, auto &str) {
                              const bool selected = filter.value() == value;
                              bool       checked  = selected && filter.enabled();
                              if (ImGui::MenuItem(str.data(), nullptr, &checked))
                              {
                                   if (selected)
                                   {
                                        if (filter.enabled())
                                        {
                                             filter.disable();
                                        }
                                        else
                                        {
                                             filter.enable();
                                        }
                                   }
                                   else
                                   {
                                        filter.update(value);
                                        filter.enable();
                                   }
                                   std::invoke(std::forward<decltype(lambda)>(lambda));
                              }
                         };
                         const std::uint32_t cols = { 3U };
                         std::uint32_t       i    = {};
                         ImGui::BeginTable("##filter_menu_items", cols);
                         for (auto &&t : pair.zip())
                         {
                              if (i % cols == 0U)
                              {
                                   ImGui::TableNextRow();
                              }
                              ImGui::TableNextColumn();
                              if constexpr (std::tuple_size_v<std::decay_t<decltype(t)>> == 2)
                              {
                                   auto &&[value, str] = t;
                                   // Handle the case where tuple has 2 elements
                                   process_element(value, str);
                              }
                              else if constexpr (std::tuple_size_v<std::decay_t<decltype(t)>> == 3)
                              {
                                   auto &&[value, str, tooltip_str] = t;
                                   // Handle the case where tuple has 3 elements
                                   process_element(value, str);
                                   tool_tip(tooltip_str);
                              }
                              ++i;
                         }
                         ImGui::EndTable();
                    }
               };
               if (map_test())
               {
                    generic_filter_menu(
                      gui_labels::pupu_id, map_pupu_id{ m_map_sprite }, m_map_sprite->filter().pupu, [&]() { refresh_render_texture(); });
               }
               if (mim_test())
               {
                    auto tmp_bpp = std::remove_cvref_t<decltype(m_map_sprite->filter().bpp)>{
                         m_selections->bpp, ff_8::WithFlag(ff_8::FilterSettings::Default, ff_8::FilterSettings::Toggle_Enabled, true)
                    };
                    auto tmp_palette = std::remove_cvref_t<decltype(m_map_sprite->filter().palette)>{
                         static_cast<std::uint8_t>(m_selections->palette),
                         ff_8::WithFlag(ff_8::FilterSettings::Default, ff_8::FilterSettings::Toggle_Enabled, true)
                    };
                    generic_filter_menu(gui_labels::bpp.data(), mim_bpp{}, tmp_bpp, [&]() { refresh_bpp(tmp_bpp.value()); });
                    generic_filter_menu(
                      gui_labels::palette.data(), mim_palette{}, tmp_palette, [&]() { refresh_palette(tmp_palette.value()); });
               }
               else if (map_test())
               {
                    generic_filter_menu(gui_labels::bpp.data(), m_map_sprite->uniques().bpp(), m_map_sprite->filter().bpp, [&]() {
                         refresh_bpp(m_map_sprite->filter().bpp.value());
                    });
                    {
                         const auto &map = m_map_sprite->uniques().palette();
                         const auto &key = m_map_sprite->filter().bpp.value();
                         if (map.contains(key))
                         {
                              generic_filter_menu(gui_labels::palette.data(), map.at(key), m_map_sprite->filter().palette, [&]() {
                                   refresh_palette(m_map_sprite->filter().palette.value());
                              });
                         }
                         else
                         {
                              m_map_sprite->filter().bpp.update(m_map_sprite->uniques().bpp().values().front());
                         }
                    }

                    generic_filter_menu(
                      gui_labels::blend_mode.data(), m_map_sprite->uniques().blend_mode(), m_map_sprite->filter().blend_mode, [&]() {
                           refresh_render_texture();
                      });


                    generic_filter_menu(
                      gui_labels::blend_other.data(), m_map_sprite->uniques().blend_other(), m_map_sprite->filter().blend_other, [&]() {
                           refresh_render_texture();
                      });

                    generic_filter_menu(
                      gui_labels::layer_id.data(), m_map_sprite->uniques().layer_id(), m_map_sprite->filter().layer_id, [&]() {
                           refresh_render_texture();
                      });

                    generic_filter_menu(
                      gui_labels::texture_page.data(),
                      m_map_sprite->uniques().texture_page_id(),
                      m_map_sprite->filter().texture_page_id,
                      [&]() { refresh_render_texture(); });

                    generic_filter_menu(
                      gui_labels::animation_id.data(), m_map_sprite->uniques().animation_id(), m_map_sprite->filter().animation_id, [&]() {
                           refresh_render_texture();
                      });

                    {
                         const auto &map = m_map_sprite->uniques().animation_frame();
                         const auto &key = m_map_sprite->filter().animation_id.value();
                         if (map.contains(key))
                         {
                              generic_filter_menu(
                                gui_labels::animation_frame.data(), map.at(key), m_map_sprite->filter().animation_frame, [&]() {
                                     refresh_render_texture();
                                });
                         }
                         else
                         {
                              m_map_sprite->filter().animation_id.update(m_map_sprite->uniques().animation_id().values().front());
                         }
                    }

                    generic_filter_menu(
                      gui_labels::z.data(), m_map_sprite->uniques().z(), m_map_sprite->filter().z, [&]() { refresh_render_texture(); });

                    generic_filter_menu(
                      gui_labels::draw_bit.data(), map_draw_bit{}, m_map_sprite->filter().draw_bit, [&]() { refresh_render_texture(); });
               }
          }
     }
}
void gui::browse_buttons()
{

     if (ImGui::Button(gui_labels::browse.data()))
     {
          open_locate_ff8_filebrowser();
     }
     tool_tip(gui_labels::locate_a_ff8_install);
     ImGui::SameLine();
     ImGui::BeginDisabled(std::ranges::empty(m_selections->paths_vector));
     if (ImGui::Button(gui_labels::explore.data()))
     {
          open_directory(m_selections->path);
     }
     tool_tip(gui_labels::explore_tooltip);
     ImGui::SameLine();
     if (ImGui::Button(gui_labels::remove.data()))
     {
          // Find and remove the selected path from m_selections->paths
          auto it = std::ranges::find(m_selections->paths_vector, m_selections->path);
          if (it != m_selections->paths_vector.end())
          {

               bool selected = *it == m_selections->path;
               m_selections->paths_vector.erase(it);
               m_selections->update_configuration_key(ConfigKey::PathsVector);
               if (selected)
               {
                    if (std::ranges::empty(m_selections->paths_vector))
                    {
                         m_selections->path = "";
                    }
                    else
                    {
                         m_selections->path = m_selections->paths_vector.front();
                    }
                    refresh_path();
               }
          }
     }
     ImGui::EndDisabled();
     tool_tip(gui_labels::remove_the_selected_ff8_path);
}
void gui::file_menu()
{
     if (!ImGui::BeginMenu(gui_labels::file.data()))
     {
          return;
     }
     const auto end_menu = glengine::ScopeGuard(&ImGui::EndMenu);
     if (ImGui::BeginMenu(gui_labels::path.data()))
     {
          const auto end_menu1 = glengine::ScopeGuard(&ImGui::EndMenu);
          menuitem_locate_ff8();
          if (ImGui::MenuItem(gui_labels::explore.data(), nullptr, nullptr, !std::ranges::empty(m_selections->paths_vector)))
          {
               open_directory(m_selections->path);
          }
          else
          {
               tool_tip(gui_labels::explore_tooltip);
          }
          if (std::ranges::empty(m_selections->paths_vector))
          {
               return;
          }
          ImGui::Separator();

          std::ptrdiff_t delete_me = -1;
          if (ImGui::BeginTable("##path_table", 2))
          {
               const auto end_table = glengine::ScopeGuard(&ImGui::EndTable);
               for (const auto &[index, path] : m_selections->paths_vector | std::ranges::views::enumerate)
               {
                    bool is_checked = path == m_selections->path;
                    ImGui::TableNextColumn();
                    ImGui::SetNextItemAllowOverlap();
                    if (ImGui::MenuItem(path.data(), nullptr, &is_checked, !is_checked))
                    {
                         m_selections->path = path;
                         refresh_path();
                    }
                    ImGui::TableNextColumn();
                    const auto  pop_id      = PushPopID();

                    const float button_size = ImGui::GetFrameHeight();
                    const auto  _           = PushPopID();
                    if (ImGui::Button(ICON_FA_FOLDER_OPEN, ImVec2{ button_size, button_size }))
                    {
                         open_directory(path);
                    }
                    else
                    {
                         tool_tip(gui_labels::explore_tooltip);
                    }

                    ImGui::SameLine();
                    if (ImGui::Button(ICON_FA_TRASH, ImVec2{ button_size, button_size }))
                    {
                         delete_me = index;
                         // ImGui::CloseCurrentPopup();
                         break;
                    }
                    else
                    {
                         tool_tip("delete me");
                    }
               }
          }
          if (std::cmp_greater(delete_me, -1))
          {
               auto it = std::ranges::begin(m_selections->paths_vector);
               std::ranges::advance(it, delete_me);
               if (it != std::ranges::end(m_selections->paths_vector))
               {

                    bool selected = *it == m_selections->path;
                    m_selections->paths_vector.erase(it);
                    m_selections->update_configuration_key(ConfigKey::PathsVector);
                    if (selected)
                    {
                         if (std::ranges::empty(m_selections->paths_vector))
                         {
                              m_selections->path = "";
                         }
                         else
                         {
                              m_selections->path = m_selections->paths_vector.front();
                         }
                         refresh_path();
                    }
               }
          }
     }

     if (std::ranges::empty(m_archives_group->mapdata()))
     {
          return;
     }
     menu_swizzle_paths();
     menu_deswizzle_paths();
     menu_swizzle_map_paths();
     menu_deswizzle_map_paths();

     if (ImGui::BeginMenu(gui_labels::language.data()))
     {
          const auto            end_menu1 = glengine::ScopeGuard(&ImGui::EndMenu);
          constexpr static auto values    = open_viii::LangCommon::to_array();
          const static auto     strings   = values | std::views::transform(AsString{});
          static auto           zip_items = std::ranges::views::zip(values, strings);
          for (auto &&[value, string] : zip_items)
          {

               bool is_checked = value == m_selections->coo;
               if (ImGui::MenuItem(string.data(), nullptr, &is_checked, !is_checked))
               {
                    m_selections->coo = value;
                    refresh_coo();
               }
               else
               {
                    tool_tip(gui_labels::language_dropdown_tool_tip);
                    if (value == open_viii::LangT::generic)
                         tool_tip(gui_labels::language_generic_tool_tip);
               }
          }
     }

     if (ImGui::BeginMenu(gui_labels::field.data()))
     {
          static std::array<char, 128> filter_buf = {};

          ImGui::InputText("Filter", filter_buf.data(), filter_buf.size());

          const auto        end_menu1 = glengine::ScopeGuard(&ImGui::EndMenu);
          static const auto cols      = 5;
          if (ImGui::BeginTable("##field_table", cols))
          {
               const auto               end_table1    = glengine::ScopeGuard(&ImGui::EndTable);
               auto                     numbered_maps = m_archives_group->mapdata() | std::ranges::views::enumerate;
               static const std::string dummy         = {};
               std::string_view         start         = dummy;
               bool                     row_toggle    = false;
               std::uint8_t             i             = 0;
               for (const auto &[index, str] : numbered_maps)
               {
                    if (filter_buf[0] && str.find(filter_buf.data()) == std::string::npos)
                    {
                         continue;
                    }
                    const auto temp = std::string_view(str).substr(0, 2);
                    if (temp == std::string_view("ma"))
                    {
                         continue;
                    }
                    if (start != temp || (i % cols == 0))
                    {
                         start = temp;
                         ImGui::TableNextRow();
                         row_toggle = !row_toggle;
                         i          = 0;
                    }
                    ++i;
                    if (row_toggle)
                    {
                         ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableDarkGray.fade(-.6F).fade_alpha(-.4F) });
                    }
                    else
                    {
                         ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableLightDarkGray.fade(-.6F).fade_alpha(-.4F) });
                    }
                    ImGui::TableNextColumn();
                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colors::ButtonHovered);
                    ImGui::PushStyleColor(ImGuiCol_HeaderActive, colors::ButtonActive);
                    const bool checked = std::cmp_equal(m_field_index, index);
                    if (checked)
                    {
                         ImGui::TableSetBgColor(
                           ImGuiTableBgTarget_CellBg, ImU32{ colors::Button });// Make the selected field stand out more.
                    }
                    if (ImGui::MenuItem(str.c_str(), nullptr, const_cast<bool *>(&checked), !checked))
                    {
                         m_field_index = static_cast<int>(index);
                         refresh_field();
                    }
                    ImGui::PopStyleColor(2);
               }
          }
     }
     if (!map_test() && !mim_test())
     {
          return;
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


// Helper function to handle path deletion
std::optional<std::string> gui::handle_path_deletion(std::vector<std::string> &paths_vector, std::ptrdiff_t offset)
{
     if (std::cmp_less(offset, 0))
     {
          return std::nullopt;
     }
     auto it = std::ranges::begin(paths_vector);
     std::ranges::advance(it, offset);
     if (it != std::ranges::end(paths_vector))
     {
          auto return_value = std::optional<std::string>(std::move(*it));
          paths_vector.erase(it);
          return return_value;
     }
     return std::nullopt;
}

std::string gui::find_replacement_path_value(const std::vector<std::string> &paths, const std::vector<bool> &paths_enabled)
{
     if (std::ranges::empty(paths))
     {
          return "";
     }
     if (std::ranges::size(paths) == std::ranges::size(paths_enabled))
     {
          return "";
     }

     auto zip_paths = std::ranges::views::zip(paths, paths_enabled);
     auto it        = std::ranges::find_if(zip_paths, [](const auto &pair) { return std::get<1>(pair); });
     if (it != std::ranges::end(zip_paths))
     {
          return std::get<0>(*it);
     }
     return "";
}

void gui::menu_swizzle_paths()
{
     if (!map_test())
     {
          return;
     }
     const main_menu_paths_settings mmps = { .user_paths              = m_selections->paths_vector_swizzle,
                                             .generated_paths         = m_selections->cache_swizzle_paths,
                                             .generated_paths_enabled = m_selections->cache_swizzle_paths_enabled,
                                             .main_label              = gui_labels::swizzle_path,
                                             .browse_tooltip          = "Browse for a directory containing swizzle textures." };
     const main_menu_paths          mmp  = { m_map_sprite->filter().swizzle, m_map_sprite->filter().swizzle, mmps };
     mmp.render(
       [&]() {
            m_selections->update_configuration_key(ConfigKey::PathsVectorUpscale);
            m_future_of_future_paths_consumer += generate_swizzle_map_paths();
       },
       [&]() { refresh_render_texture(true); },
       [&]() {
            m_directory_browser.Open();
            m_directory_browser.SetTitle(gui_labels::choose_directory_to_load_textures_from.data());
            m_directory_browser.SetDirectory(m_selections->swizzle_path);
            m_directory_browser.SetTypeFilters({ ".png" });
            m_modified_directory_map = map_directory_mode::load_swizzle_textures;
       });
}

void gui::menu_deswizzle_paths()
{
     if (!map_test())
     {
          return;
     }
     const main_menu_paths_settings mmps = { .user_paths              = m_selections->paths_vector_deswizzle,
                                             .generated_paths         = m_selections->cache_deswizzle_paths,
                                             .generated_paths_enabled = m_selections->cache_deswizzle_paths_enabled,
                                             .main_label              = gui_labels::deswizzle_path,
                                             .browse_tooltip          = "Browse for a directory containing deswizzle textures." };
     const main_menu_paths          mmp  = { m_map_sprite->filter().deswizzle, m_map_sprite->filter().deswizzle, mmps };
     mmp.render(
       [&]() {
            m_selections->update_configuration_key(ConfigKey::PathsVectorDeswizzle);
            m_future_of_future_paths_consumer += generate_swizzle_map_paths();
       },
       [&]() { refresh_render_texture(true); },
       [&]() {
            m_directory_browser.Open();
            m_directory_browser.SetTitle(gui_labels::choose_directory_to_load_textures_from.data());
            m_directory_browser.SetDirectory(m_selections->deswizzle_path);
            m_directory_browser.SetTypeFilters({ ".png" });
            m_modified_directory_map = map_directory_mode::load_deswizzle_textures;
       });
}


void gui::menu_swizzle_map_paths()
{
     if (!map_test())
     {
          return;
     }
     const main_menu_paths_settings mmps = { .user_paths              = m_selections->paths_vector_swizzle_map,
                                             .generated_paths         = m_selections->cache_swizzle_map_paths,
                                             .generated_paths_enabled = m_selections->cache_swizzle_map_paths_enabled,
                                             .main_label              = gui_labels::swizzle_map_path,
                                             .browse_tooltip          = "Browse for a directory containing swizzled .map files." };
     const main_menu_paths          mmp  = { m_map_sprite->filter().swizzle_map, m_map_sprite->filter().swizzle_map, mmps };
     mmp.render(
       [&]() {
            m_selections->update_configuration_key(ConfigKey::PathsVectorUpscaleMap);
            m_future_of_future_paths_consumer += generate_swizzle_map_paths();
       },
       [&]() {
            if (m_map_sprite->filter().swizzle_map.enabled())
            {
                 if (const auto paths = fme::generate_swizzle_map_paths(m_selections, *m_map_sprite)(); !std::ranges::empty(paths))
                 {
                      m_map_sprite->filter().deswizzle_map.disable();
                      m_map_sprite->load_map(paths.front());// grab the first match.
                 }
                 else
                 {
                      m_map_sprite->filter().swizzle_map.disable();
                 }
            }
            else
            {
                 // load map from .map here.
                 m_map_sprite->first_to_working_and_original();
            }
            refresh_render_texture();
       },
       [&]() {
            m_directory_browser.Open();
            m_directory_browser.SetTitle(gui_labels::load_map_file.data());
            m_directory_browser.SetDirectory(m_selections->swizzle_path);
            m_directory_browser.SetTypeFilters({ ".map" });
            m_modified_directory_map = map_directory_mode::load_swizzle_map;
       });
}

void gui::menu_deswizzle_map_paths()
{
     if (!map_test())
     {
          return;
     }
     const main_menu_paths_settings mmps = { .user_paths              = m_selections->paths_vector_deswizzle_map,
                                             .generated_paths         = m_selections->cache_deswizzle_map_paths,
                                             .generated_paths_enabled = m_selections->cache_deswizzle_map_paths_enabled,
                                             .main_label              = gui_labels::deswizzle_map_path,
                                             .browse_tooltip          = "Browse for a directory containing deswizzled .map files." };
     const main_menu_paths          mmp  = { m_map_sprite->filter().deswizzle_map, m_map_sprite->filter().swizzle_map, mmps };
     mmp.render(
       [&]() {
            m_selections->update_configuration_key(ConfigKey::PathsVectorDeswizzleMap);
            m_future_of_future_paths_consumer += generate_deswizzle_map_paths();
       },
       [&]() {
            if (m_map_sprite->filter().deswizzle_map.enabled())
            {
                 if (const auto paths = fme::generate_deswizzle_map_paths(m_selections, *m_map_sprite)(); !std::ranges::empty(paths))
                 {
                      m_map_sprite->filter().swizzle_map.disable();
                      m_map_sprite->load_map(paths.front());// grab the first match.
                 }
                 else
                 {
                      m_map_sprite->filter().deswizzle_map.disable();
                 }
            }
            else
            {
                 // load map from .map here.
                 m_map_sprite->first_to_working_and_original();
            }
            refresh_render_texture();
       },
       [&]() {
            m_directory_browser.Open();
            m_directory_browser.SetTitle(gui_labels::load_map_file.data());
            m_directory_browser.SetDirectory(m_selections->deswizzle_path);
            m_directory_browser.SetTypeFilters({ ".map" });
            m_modified_directory_map = map_directory_mode::load_deswizzle_map;
       });
}

std::ptrdiff_t gui::add_delete_button(const std::ptrdiff_t index)
{
     const auto pop_id = PushPopID();
     if (ImGui::Button(ICON_FA_TRASH))
     {
          ImGui::CloseCurrentPopup();
          return index;
     }
     else
     {
          tool_tip("delete me");
     }
     return -1;
}

std::ptrdiff_t gui::add_delete_button(const std::string &path, const std::vector<std::string> &paths)
{
     auto       transformed_paths = paths | std::ranges::views::enumerate;
     const auto it = std::ranges::find_if(transformed_paths, [&path](const auto &pair) { return path.starts_with(std::get<1>(pair)); });
     if (it != std::ranges::end(transformed_paths))
     {
          const auto &index  = std::get<0>(*it);
          const auto  pop_id = PushPopID();
          if (ImGui::Button(ICON_FA_TRASH))
          {
               ImGui::CloseCurrentPopup();
               return static_cast<std::ptrdiff_t>(index);
          }
          tool_tip("delete me");
     }
     return -1;
}


bool gui::map_test() const
{
     return m_map_sprite && !m_map_sprite->fail() && m_selections && m_selections->draw == draw_mode::draw_map;
}
bool gui::mim_test() const
{
     return m_mim_sprite && !m_mim_sprite->fail() && m_selections && m_selections->draw == draw_mode::draw_mim;
}
std::string gui::save_texture_path() const
{
     if (m_archives_group->mapdata().empty())
     {
          return {};
     }
     const std::string &field_name = m_archives_group->mapdata().at(static_cast<size_t>(m_field_index));
     spdlog::info("field_name = {}", field_name);
     if (mim_test())// MIM
     {
          if (m_mim_sprite->draw_palette())
          {
               return fmt::format("{}_mim_palettes.png", field_name);
          }
          return fmt::format("{}_mim_{}bpp_{}.png", field_name, static_cast<int>(m_selections->bpp), m_selections->palette);
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
     const auto            pop_directory         = glengine::ScopeGuard([this]() { m_directory_browser.ClearSelected(); });
     [[maybe_unused]] bool changed               = false;
     const auto            selected_path         = m_directory_browser.GetSelected();
     static constexpr auto noop                  = []([[maybe_unused]] const std::filesystem::path &path) {};
     const auto            process_texture_paths = [&](
                                          std::string                                                       &target_path,
                                          ConfigKey                                                          path_key,
                                          std::vector<std::string>                                          &paths_vector,
                                          ConfigKey                                                          vector_key,
                                          [[maybe_unused]] std::vector<std::string>                         &target_paths,
                                          [[maybe_unused]] std::vector<bool>                                &target_paths_enabled,
                                          [[maybe_unused]] auto                                             &main_filter,
                                          [[maybe_unused]] auto                                             &other_filter,
                                          [[maybe_unused]] const std::invocable<std::filesystem::path> auto &has_path_funct,
                                          [[maybe_unused]] const std::invocable<std::filesystem::path> auto &action) {
          // Remember the last grabbed path
          target_path = selected_path.string();
          // Save the setting to TOML
          m_selections->update_configuration_key(path_key);
          // Store a copy of the directory
          paths_vector.push_back(target_path);
          sort_and_remove_duplicates(paths_vector);
          m_selections->update_configuration_key(vector_key);
          // Append the path to the various search patterns
          const auto temp_paths = ff_8::path_search::get_paths(m_selections, get_coo(), target_path);

          // If we have found matches
          if (!temp_paths.empty())
          {
               for (const auto &path : temp_paths)
               {
                    // Add matches to the drop-down
                    target_paths.emplace_back(path.string());
                    bool path_enabled = std::invoke(has_path_funct, path);
                    target_paths_enabled.emplace_back(path_enabled);
                    if (path_enabled && !changed)
                    {
                         other_filter.disable();
                         main_filter.update(path).enable();
                         std::invoke(action, path);
                         changed = true;
                    }
               }
               sort_and_remove_duplicates(target_paths, target_paths_enabled);
               if (changed)
               {
                    refresh_render_texture(true);
               }
          }
     };

     const ff_8::path_search ps           = static_cast<ff_8::path_search>(*m_map_sprite);
     const auto              has_map_path = [&](const std::filesystem::path &path) -> bool { return ps.has_map_path(path, ".map"); };
     switch (m_modified_directory_map)
     {
          case map_directory_mode::ff8_install_directory: {
               m_selections->path = selected_path.string();
               m_selections->update_configuration_key(ConfigKey::SelectionsPath);
               m_selections->paths_vector.push_back(selected_path.string());
               m_selections->update_configuration_key(ConfigKey::PathsVector);
               sort_paths();
               update_path();
          }
          break;
          case map_directory_mode::save_swizzle_textures: {
               m_selections->swizzle_path = selected_path.string();
               m_selections->update_configuration_key(ConfigKey::SwizzlePath);
               std::error_code error_code{};
               std::filesystem::create_directories(selected_path, error_code);
               if (error_code)
               {
                    spdlog::error(
                      "{}:{} - {}: {} - path: {}", __FILE__, __LINE__, error_code.value(), error_code.message(), selected_path.string());
                    error_code.clear();
               }
               // todo modify these two functions :P to use the imported image.
               m_future_consumer +=
                 m_map_sprite->save_swizzle_textures(m_selections->output_swizzle_pattern, selected_path.string());// done.
               const key_value_data cpm = {
                    .field_name    = m_map_sprite->get_base_name(),
                    .ext           = ".map",
                    .language_code = m_selections && m_selections->coo != open_viii::LangT::generic && m_map_sprite->using_coo()
                                       ? std::optional{ m_selections->coo }
                                       : std::nullopt,
               };// todo coo might not be done correctly. if the map sprite isn't using a coo we shouldn't either. m_selections->coo is
                 // what coo we're asking for but the it will default to generic when it can't find it. so we need to know what the map
                 // sprite is using for it's coo.

               m_map_sprite->save_modified_map(
                 cpm.replace_tags(m_selections->output_map_pattern_for_swizzle, m_selections, selected_path.string()));
               open_directory(selected_path);
          }
          break;
          case map_directory_mode::save_deswizzle_textures: {
               m_selections->deswizzle_path = selected_path.string();
               m_selections->update_configuration_key(ConfigKey::DeswizzlePath);
               std::error_code error_code{};
               std::filesystem::create_directories(selected_path, error_code);
               if (error_code)
               {
                    spdlog::error(
                      "{}:{} - {}: {} - path: {}", __FILE__, __LINE__, error_code.value(), error_code.message(), selected_path.string());
                    error_code.clear();
               }
               m_future_consumer += m_map_sprite->save_pupu_textures(m_selections->output_deswizzle_pattern, selected_path.string());
               const key_value_data cpm = {
                    .field_name    = m_map_sprite->get_base_name(),
                    .ext           = ".map",
                    .language_code = m_selections && m_selections->coo != open_viii::LangT::generic && m_map_sprite->using_coo()
                                       ? std::optional{ m_selections->coo }
                                       : std::nullopt,
               };
               m_map_sprite->save_modified_map(
                 cpm.replace_tags(m_selections->output_map_pattern_for_deswizzle, m_selections, selected_path.string()));
               open_directory(selected_path);
          }
          break;
          case map_directory_mode::load_swizzle_textures: {
               process_texture_paths(
                 m_selections->swizzle_path,
                 ConfigKey::SwizzlePath,
                 m_selections->paths_vector_swizzle,
                 ConfigKey::PathsVectorUpscale,
                 m_selections->cache_swizzle_paths,
                 m_selections->cache_swizzle_paths_enabled,
                 m_map_sprite->filter().swizzle,
                 m_map_sprite->filter().deswizzle,
                 [&](const std::filesystem::path &path) -> bool {
                      // we add the paths to map search for ease of use.
                      m_selections->cache_swizzle_map_paths.push_back(path.string());
                      m_selections->cache_swizzle_map_paths_enabled.push_back(has_map_path(path));
                      return ps.has_swizzle_path(path, ".png");
                      return false;
                 },
                 noop);
               sort_and_remove_duplicates(m_selections->cache_swizzle_map_paths, m_selections->cache_swizzle_map_paths_enabled);
          }
          break;
          case map_directory_mode::load_deswizzle_textures: {
               process_texture_paths(
                 m_selections->deswizzle_path,
                 ConfigKey::DeswizzlePath,
                 m_selections->paths_vector_deswizzle,
                 ConfigKey::PathsVectorDeswizzle,
                 m_selections->cache_deswizzle_paths,
                 m_selections->cache_deswizzle_paths_enabled,
                 m_map_sprite->filter().deswizzle,
                 m_map_sprite->filter().swizzle,
                 [&](const std::filesystem::path &path) -> bool {
                      // we add the paths to map search for ease of use.
                      m_selections->cache_deswizzle_map_paths.push_back(path.string());
                      m_selections->cache_deswizzle_map_paths_enabled.push_back(has_map_path(path));
                      return ps.has_deswizzle_path(path, ".png");
                      return false;
                 },
                 noop);
               sort_and_remove_duplicates(m_selections->cache_deswizzle_map_paths, m_selections->cache_deswizzle_map_paths_enabled);
          }
          break;
          case map_directory_mode::load_swizzle_map: {
               process_texture_paths(
                 m_selections->swizzle_path,
                 ConfigKey::SwizzlePath,
                 m_selections->paths_vector_swizzle,
                 ConfigKey::PathsVectorUpscaleMap,
                 m_selections->cache_swizzle_map_paths,
                 m_selections->cache_swizzle_map_paths_enabled,
                 m_map_sprite->filter().swizzle_map,
                 m_map_sprite->filter().deswizzle_map,
                 has_map_path,
                 [&]([[maybe_unused]] const std::filesystem::path &path) {
                      if (const auto paths = ps.generate_swizzle_map_paths(path, ".map"); !std::ranges::empty(paths))
                      {
                           m_map_sprite->load_map(paths.front());// grab the first match.
                      }
                 });
          }
          break;
          case map_directory_mode::load_deswizzle_map: {
               process_texture_paths(
                 m_selections->deswizzle_path,
                 ConfigKey::DeswizzlePath,
                 m_selections->paths_vector_deswizzle,
                 ConfigKey::PathsVectorDeswizzleMap,
                 m_selections->cache_deswizzle_map_paths,
                 m_selections->cache_deswizzle_map_paths_enabled,
                 m_map_sprite->filter().deswizzle_map,
                 m_map_sprite->filter().swizzle_map,
                 has_map_path,
                 [&]([[maybe_unused]] const std::filesystem::path &path) {
                      if (const auto paths = ps.generate_deswizzle_map_paths(path, ".map"); !std::ranges::empty(paths))
                      {
                           m_map_sprite->load_map(paths.front());// grab the first match.
                      }
                 });
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

/**
 * @brief Sorts and deduplicates the stored paths.
 *
 * This function reads the current paths from `m_selections->paths`, sorts them alphabetically,
 * and removes duplicates. If the paths are already sorted and unique, no action is taken.
 * After modification, the updated paths are saved back into the configuration.
 *
 * @note Paths that do not exist on the filesystem are TODO: not yet removed.
 */
void gui::sort_paths()
{
     if (sort_and_remove_duplicates(m_selections->paths_vector))
          m_selections->update_configuration_key(ConfigKey::PathsVector);
     if (sort_and_remove_duplicates(m_selections->paths_vector_swizzle))
          m_selections->update_configuration_key(ConfigKey::PathsVectorUpscale);
     if (sort_and_remove_duplicates(m_selections->paths_vector_deswizzle))
          m_selections->update_configuration_key(ConfigKey::PathsVectorDeswizzle);
     if (sort_and_remove_duplicates(m_selections->paths_vector_swizzle_map))
          m_selections->update_configuration_key(ConfigKey::PathsVectorUpscaleMap);
     if (sort_and_remove_duplicates(m_selections->paths_vector_deswizzle_map))
          m_selections->update_configuration_key(ConfigKey::PathsVectorDeswizzleMap);


     sort_and_remove_duplicates(m_selections->cache_swizzle_paths, m_selections->cache_swizzle_paths_enabled);
     sort_and_remove_duplicates(m_selections->cache_swizzle_map_paths, m_selections->cache_swizzle_map_paths_enabled);
     sort_and_remove_duplicates(m_selections->cache_deswizzle_paths, m_selections->cache_deswizzle_paths_enabled);
     sort_and_remove_duplicates(m_selections->cache_deswizzle_map_paths, m_selections->cache_deswizzle_map_paths_enabled);
}

void gui::file_browser_save_texture()
{
     m_save_file_browser.Display();
     if (m_save_file_browser.HasSelected())
     {
          [[maybe_unused]] const auto &selected_path      = m_save_file_browser.GetSelected();
          [[maybe_unused]] const auto &selected_directory = m_save_file_browser.GetDirectory();
          if (mim_test())
          {
               switch (m_file_dialog_mode)
               {
                    case file_dialog_mode::save_mim_file: {
                         m_mim_sprite->mim_save(selected_path);
                         m_selections->output_mim_path = selected_directory.string();
                         m_selections->update_configuration_key(ConfigKey::OutputMimPath);
                         open_file_explorer(selected_path);
                    }
                    break;
                    case file_dialog_mode::save_image_file: {
                         m_mim_sprite->save(selected_path);
                         m_selections->output_image_path = selected_directory.string();
                         m_selections->update_configuration_key(ConfigKey::OutputImagePath);
                         open_file_explorer(selected_path);
                    }
                    break;
                    default:
                         spdlog::warn(
                           "{}:{} m_file_dialog_mode ({}) unused by mim", __FILE__, __LINE__, std::to_underlying(m_file_dialog_mode));
                         break;
               }
          }
          else if (map_test())
          {
               switch (m_file_dialog_mode)
               {
                    case file_dialog_mode::save_modified_map_file: {
                         m_map_sprite->save_modified_map(selected_path);
                         m_selections->output_map_path = selected_directory.string();
                         m_selections->update_configuration_key(ConfigKey::OutputMapPath);
                         open_file_explorer(selected_path);
                    }
                    break;
                    case file_dialog_mode::save_unmodified_map_file: {
                         m_map_sprite->map_save(selected_path);
                         m_selections->output_map_path = selected_directory.string();
                         m_selections->update_configuration_key(ConfigKey::OutputMapPath);
                         open_file_explorer(selected_path);
                    }
                    break;
                    case file_dialog_mode::load_map_file: {
                         /// TODO load in as a filter? or leave as is... For now we'll disable the filter and just load the selected
                         /// file.
                         m_map_sprite->filter().swizzle_map.disable();
                         m_map_sprite->filter().deswizzle_map.disable();
                         m_map_sprite->load_map(selected_path);
                         m_selections->output_map_path = selected_directory.string();
                         m_selections->update_configuration_key(ConfigKey::OutputMapPath);
                         m_changed = true;
                    }
                    break;
                    case file_dialog_mode::save_image_file: {
                         m_map_sprite->save(selected_path);
                         m_selections->output_image_path = selected_directory.string();
                         m_selections->update_configuration_key(ConfigKey::OutputImagePath);
                         open_file_explorer(selected_path);
                    }
                    break;
                    default:
                         spdlog::warn(
                           "{}:{} m_file_dialog_mode ({}) unused by map", __FILE__, __LINE__, std::to_underlying(m_file_dialog_mode));
                         break;
               }
          }
          m_save_file_browser.ClearSelected();
     }
}
void gui::menuitem_locate_ff8()
{
     if (ImGui::MenuItem(gui_labels::browse.data()))
     {
          open_locate_ff8_filebrowser();
     }
     else
     {
          tool_tip(gui_labels::locate_a_ff8_install);
     }
}
void gui::open_locate_ff8_filebrowser()
{
     m_directory_browser.Open();
     m_directory_browser.SetTitle(gui_labels::choose_a_ff8_install.data());
     m_directory_browser.SetDirectory(m_selections->path);
     m_directory_browser.SetTypeFilters({ ".exe" });
     m_modified_directory_map = map_directory_mode::ff8_install_directory;
}
void gui::menuitem_save_swizzle_textures()
{
     if (!ImGui::MenuItem(gui_labels::save_swizzled_textures.data(), nullptr, false, true))
     {
          tool_tip("Browse for a directory to save swizzled textures.");
          return;
     }
     m_directory_browser.Open();
     m_directory_browser.SetTitle(m_map_sprite->appends_prefix_base_name(gui_labels::choose_directory_to_save_textures_to));
     m_directory_browser.SetDirectory(m_selections->swizzle_path);
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_modified_directory_map = map_directory_mode::save_swizzle_textures;
}

void gui::menuitem_save_deswizzle_textures()
{
     if (!ImGui::MenuItem(gui_labels::save_deswizzled_textures.data(), nullptr, false, true))
     {
          tool_tip("Browse for a directory to save deswizzled textures.");
          return;
     }
     m_directory_browser.Open();
     m_directory_browser.SetTitle(m_map_sprite->appends_prefix_base_name(gui_labels::choose_directory_to_save_textures_to));
     m_directory_browser.SetDirectory(m_selections->deswizzle_path);
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_modified_directory_map = map_directory_mode::save_deswizzle_textures;
}
void gui::menuitem_load_swizzle_textures()
{
     if (!ImGui::MenuItem("Load Swizzled Textures", nullptr, false, true))
     {
          tool_tip("Browse for a directory containing swizzled textures.");
          return;
     }
     m_directory_browser.Open();
     m_directory_browser.SetTitle(gui_labels::choose_directory_to_load_textures_from.data());
     m_directory_browser.SetDirectory(m_selections->swizzle_path);
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_modified_directory_map = map_directory_mode::load_swizzle_textures;
}

void gui::menuitem_load_swizzle_textures2()
{
     if (!ImGui::MenuItem(gui_labels::browse.data(), nullptr, false, true))
     {
          tool_tip(gui_labels::locate_a_custom_swizzle_directory);
          return;
     }
     m_directory_browser.Open();
     m_directory_browser.SetTitle(gui_labels::choose_directory_to_load_textures_from.data());
     m_directory_browser.SetDirectory(m_selections->swizzle_path);
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_modified_directory_map = map_directory_mode::load_swizzle_textures;
}
void gui::menuitem_load_deswizzle_textures()
{
     if (!ImGui::MenuItem("Load Deswizzled Textures", nullptr, false, true))
     {
          tool_tip("Browse for a directory containing deswizzled textures.");
          return;
     }
     m_directory_browser.Open();
     m_directory_browser.SetTitle(gui_labels::choose_directory_to_load_textures_from.data());
     m_directory_browser.SetDirectory(m_selections->deswizzle_path);
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_modified_directory_map = map_directory_mode::load_deswizzle_textures;
}

void gui::menuitem_load_deswizzle_textures2()
{
     if (!ImGui::MenuItem(gui_labels::browse.data(), nullptr, false, true))
     {
          tool_tip("Browse for a directory containing deswizzled textures.");
          return;
     }
     m_directory_browser.Open();
     m_directory_browser.SetTitle(gui_labels::choose_directory_to_load_textures_from.data());
     m_directory_browser.SetDirectory(m_selections->deswizzle_path);
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_modified_directory_map = map_directory_mode::load_deswizzle_textures;
}
void gui::menuitem_save_texture(bool enabled)
{
     if (!map_test() && !mim_test())
     {
          return;
     }
     if (!ImGui::MenuItem(gui_labels::save_displayed_texture.data(), nullptr, false, enabled))
     {
          return;
     }
     const std::string &path = save_texture_path();
     m_save_file_browser.Open();
     m_save_file_browser.SetTitle(gui_labels::save_texture_as.data());
     m_save_file_browser.SetDirectory(m_selections->output_image_path);
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
     const std::string &path = m_mim_sprite->mim_filename();
     m_save_file_browser.Open();
     m_save_file_browser.SetTitle(gui_labels::save_mim_as.data());
     m_save_file_browser.SetDirectory(m_selections->output_mim_path);
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
     m_save_file_browser.SetDirectory(m_selections->output_map_path);
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
     m_save_file_browser.SetDirectory(m_selections->output_map_path);
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
     m_load_file_browser.SetDirectory(m_selections->output_map_path);
     m_load_file_browser.SetTypeFilters({ Map::EXT.data() });
     m_load_file_browser.SetInputName(path);
     m_file_dialog_mode = file_dialog_mode::load_map_file;
}
void gui::refresh_draw_mode()
{
     m_selections->update_configuration_key(ConfigKey::Draw);
     switch (m_selections->draw)
     {
          case draw_mode::draw_mim:
               *m_mim_sprite = *get_mim_sprite();
               // m_draw_window.update(m_mim_sprite);
               break;
          case draw_mode::draw_map:
               *m_map_sprite = m_map_sprite->update(ff_8::map_group(m_field, get_coo()), m_selections->draw_swizzle);
               // m_draw_window.update(m_map_sprite);
               // m_import.update(m_map_sprite);
               // m_history_window.update(m_map_sprite);
               break;
     }
     m_changed = true;
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
     refresh_draw_mode();
}
bool gui::combo_path()
{
     const auto pop_buttons = glengine::ScopeGuard([&]() { browse_buttons(); });
     const auto gcc         = GenericComboClass(
       gui_labels::path,
       [&]() { return m_selections->paths_vector; },
       [&]() { return m_selections->paths_vector; },
       m_selections->path,
       generic_combo_settings{ .num_columns = 1, .show_explore_button = true });
     if (!m_selections->paths_vector.empty() && gcc.render())
     {
          refresh_path();
          return true;
     }
     return false;
}

void gui::refresh_path()
{
     m_selections->update_configuration_key(ConfigKey::SelectionsPath);
     m_selections->refresh_ffnx_paths();
     update_path();
}
// todo fix events.
//  void gui::event_type_mouse_button_released(const sf::Mouse::Button &button)
//  {
//       switch (button)
//       {
//            case sf::Mouse::Left: {
//                 // m_mouse_positions.left = false;
//                 spdlog::trace("{}", "Left Mouse Button Up");
//            }
//            break;
//            default:
//                 break;
//       }
//  }
//  void gui::event_type_mouse_button_pressed(const sf::Mouse::Button &button)
//  {
//       switch (button)
//       {
//            case sf::Mouse::Left: {
//                 // m_mouse_positions.left = true;
//                 spdlog::trace("{}", "Left Mouse Button Down");
//            }
//            break;
//            default:
//                 break;
//       }
//  }
//  void gui::event_type_key_pressed([[maybe_unused]] const sf::Event::KeyEvent &key) {}
void gui::bind_shortcuts() const
{

     constexpr auto flags = ImGuiInputFlags_RouteGlobal | ImGuiInputFlags_RouteOverFocused;
     if (ImGui::Shortcut(ImGuiKey_Escape, flags))
     {
          m_draw_window.clear_clicked_tile_indices();
     }
     // Undo All: Ctrl+Shift+Z
     else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Z, flags))
     {
          m_map_sprite->undo_all();
     }
     // Redo All: Ctrl+Shift+Y
     else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Y, flags))
     {
          m_map_sprite->redo_all();
     }
     // Undo: Ctrl+Z
     else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Z, flags))
     {
          m_map_sprite->undo();
     }
     // Redo: Ctrl+Y
     else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Y, flags))
     {
          m_map_sprite->redo();
     }
     // Toggle windows
     else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_H, flags))
     {
          m_selections->display_history_window ^= true;
          m_selections->update_configuration_key(ConfigKey::DisplayHistoryWindow);
     }
     else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_I, flags))
     {
          // todo fix imports in new branch.
          // m_selections->display_import_image_window ^= true;
          // m_selections->update_configuration_key(ConfigKey::DisplayImportImageWindow);
     }
     else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_B, flags))
     {
          m_selections->display_batch_window ^= true;
          m_selections->update_configuration_key(ConfigKey::DisplayBatchWindow);
     }
     else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_D, flags))
     {
          m_selections->display_draw_window ^= true;
          m_selections->update_configuration_key(ConfigKey::DisplayDrawWindow);
     }
     else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_U, flags))
     {
          m_selections->display_custom_paths_window ^= true;
          m_selections->update_configuration_key(ConfigKey::DisplayCustomPathsWindow);
     }
     else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_F, flags))
     {
          m_selections->display_field_file_window ^= true;
          m_selections->update_configuration_key(ConfigKey::DisplayFieldFileWindow);
     }
     else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_P, flags))
     {
          m_selections->display_control_panel_window ^= true;
          m_selections->update_configuration_key(ConfigKey::DisplayControlPanelWindow);
     }
}
std::uint32_t gui::image_height() const
{
     if (map_test())
     {
          return m_map_sprite->height();
     }
     if (mim_test())
     {
          return m_mim_sprite->height();
     }
     return {};
}
float gui::scaled_menubar_gap() const
{
     if (saved_window_height < std::numeric_limits<float>::epsilon())
     {
          return {};
     }
     return ImGui::GetFrameHeight() * static_cast<float>(image_height()) / saved_window_height;
}
// void gui::scale_window(float width, float height)
// {
//      //const auto            img_height = static_cast<float>(image_height());
//      static constexpr auto load       = [](auto &saved, auto &not_saved) {
//           if (not_saved < std::numeric_limits<float>::epsilon())
//           {
//                not_saved = saved;
//           }
//           else
//           {
//                saved = not_saved;
//           }
//      };
//      load(saved_window_width, width);
//      load(saved_window_height, height);
//      //const float adjusted_height = height - ImGui::GetFrameHeight();
//      // this scales up the elements without losing the horizontal space. so
//      // going from 4:3 to 16:9 will end up with wide screen.
//      //m_scale_width               = std::round(width / adjusted_height * img_height);
//      //float const scaled_gap      = scaled_menubar_gap();
//      // m_window.setView(
//      //   sf::View(
//      //     sf::FloatRect(std::round(m_cam_pos.x), std::round(m_cam_pos.y - scaled_gap), m_scale_width, std::round(img_height +
//      //     scaled_gap))));
// }
archives_group gui::get_archives_group() const
{
     if (!std::ranges::empty(m_selections->path))
     {
          return { open_viii::LangCommon::to_array().front(), m_selections->path };
     }
     return {};
}
void gui::update_path()
{
     m_archives_group = std::make_shared<archives_group>(m_archives_group->with_path(m_selections->path));
     m_batch          = m_archives_group;
     m_custom_paths_window.refresh();
     update_field();
}
std::shared_ptr<mim_sprite> gui::get_mim_sprite() const
{
     return std::make_shared<mim_sprite>(
       m_field,
       m_selections->bpp,
       static_cast<std::uint8_t>(Mim::palette_selections().at(static_cast<std::size_t>(m_selections->palette))),
       get_coo(),
       m_selections->draw_palette);
}
gui::gui(GLFWwindow *const window)
{
     // 0. Set the debug callback function
     glDebugMessageCallback(DebugCallback, nullptr);

     // 1. Create ImGui context (MUST be first!)
     IMGUI_CHECKVERSION();
     ImGui::CreateContext();
     IM_ASSERT(ImGui::GetCurrentContext() != nullptr);

     // 2. Configure ImGui (optional but common)
     ImGuiIO          &imgui_io   = ImGui::GetIO();
     std::error_code   error_code = {};
     static const auto path       = (std::filesystem::current_path(error_code) / "res" / "field-map-editor_imgui.ini").string();
     imgui_io.ConfigFlags         = bitwise_or(imgui_io.ConfigFlags, ImGuiConfigFlags_DockingEnable);
     imgui_io.ConfigFlags         = bitwise_or(imgui_io.ConfigFlags, ImGuiConfigFlags_ViewportsEnable);
     imgui_io.IniFilename         = path.c_str();
     if (error_code)
     {
          spdlog::warn("{}:{} - {}: {} path: \"{}\"", __FILE__, __LINE__, error_code.value(), error_code.message(), path);
          error_code.clear();
     }
     ImGui::LoadIniSettingsFromDisk(path.c_str());

     // 3. Set ImGui style (optional)
     ImGui::StyleColorsDark();// or Light(), Classic()

     // 4. Init backends
     static constexpr bool use_imgui_callbacks = true;
     const char           *glsl_version        = "#version 450 core";

     ImGui_ImplGlfw_InitForOpenGL(window, use_imgui_callbacks);
     ImGui_ImplOpenGL3_Init(glsl_version);
     // 5. Load Fonts
     (void)icons_font();
     ImGui_ImplOpenGL3_DestroyFontsTexture();// safely clears existing texture
     ImGui_ImplOpenGL3_CreateFontsTexture();// creates new one from ImGuiIO::Fonts

     // 6. MISC
     m_archives_group = std::make_shared<archives_group>(get_archives_group());
     m_batch          = fme::batch{ m_selections, m_archives_group };
     m_field          = init_field();
     m_field_file_window.refresh(m_field);
     m_mim_sprite = get_mim_sprite();
     m_map_sprite = get_map_sprite();
     m_draw_window.update(m_mim_sprite);
     m_draw_window.update(m_map_sprite);
     m_import.update(m_selections);
     m_history_window.update(m_selections);
     m_import.update(m_map_sprite);
     m_history_window.update(m_map_sprite);

     if (m_field)
     {
          m_future_of_future_paths_consumer += generate_swizzle_paths();
          m_future_of_future_paths_consumer += generate_deswizzle_paths();
          m_future_of_future_paths_consumer += generate_swizzle_map_paths();
          m_future_of_future_paths_consumer += generate_deswizzle_map_paths();
     }
     // todo queue up sort_paths so it runs after generate is done.
     sort_paths();
     //      if (!m_drag_sprite_shader)
     //      {
     //           m_drag_sprite_shader                       = std::make_shared<sf::Shader>();
     //           static const std::string border_shader_raw = R"(#version 130
     // uniform sampler2D texture;
     // uniform float borderWidth;

     // void main()
     // {
     //     vec4 pixel = texture2D(texture, gl_TexCoord[0].st);
     //     vec2 texelSize = vec2(1.0/textureSize(texture,0).x, 1.0/textureSize(texture,0).y);
     //     float alpha = 0;
     //     int space = int(borderWidth);
     //     int threshold = (2*space) * (2*space);
     //     int count = 0;
     //     for(int y=-space;y<=space;++y)
     //     {
     //       for(int x=-space;x<=space;++x)
     //       {
     //         if(texture2D(texture, gl_TexCoord[0].st + vec2(x*texelSize.x, y*texelSize.y)).a > 0.5)
     //         {
     //             count++;
     //         }
     //       }
     //     }
     //     if(pixel.a > 0.5)
     //     {
     //       gl_FragColor = pixel;
     //     }
     //     else
     //     {
     //       gl_FragColor = vec4(1, 0, 0, float(count)/float(threshold));
     //     }
     // }
     // )";
     //           m_drag_sprite_shader->loadFromMemory(border_shader_raw, sf::Shader::Fragment);
     //      }
}

gui::~gui()
{
     if (m_selections)
     {
          m_selections->update_configuration();
     }
     ImGui_ImplOpenGL3_Shutdown();
     ImGui_ImplGlfw_Shutdown();
     ImGui::DestroyContext();
}

/**
 * @brief Initializes the field data using the selected field name.
 *
 * Uses the string stored in the configuration to find the corresponding field index.
 * If the field name is not found, defaults to index 0.
 *
 * @return A shared pointer to the field archive.
 */
std::shared_ptr<open_viii::archive::FIFLFS<false>> gui::init_field()
{
     const int field = m_archives_group->find_field(m_selections->starter_field);

     // If the field was not found (returns < 0), fall back to 0
     m_field_index   = field >= 0 ? field : 0;

     return m_archives_group->field(m_field_index);
}

std::shared_ptr<map_sprite> gui::get_map_sprite() const
{
     //     map_sprite(ff_8::map_group map_group, bool draw_swizzle, ff_8::filters in_filters, bool force_disable_blends, bool
     //     require_coo);
     return std::make_shared<map_sprite>(
       ff_8::map_group{ m_field, get_coo() },
       m_selections->draw_swizzle,
       ff_8::filters{ true },
       m_selections->draw_disable_blending,
       false,
       m_selections);
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

void gui::refresh_palette(std::uint8_t palette)
{
     palette               = static_cast<std::uint8_t>(palette & 0xFU);
     m_selections->palette = palette;
     if (m_map_sprite)
     {
          m_map_sprite->filter().palette.update(palette);
     }
     else
     {
          // filter saves config now but mim doens't use filter.
          m_selections->update_configuration_key(ConfigKey::Palette);
     }
     if (mim_test())
     {
          update_palette(*m_mim_sprite, palette);
     }
     if (map_test())
     {
          update_palette(*m_map_sprite, palette);
     }
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
     refresh_palette(m_map_sprite->filter().palette.value());
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
     refresh_bpp(m_map_sprite->filter().bpp.value());
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


void gui::refresh_render_texture(bool reload_textures)
{
     m_map_sprite->update_render_texture(reload_textures);
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
     refresh_render_texture();
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
     refresh_render_texture();
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
     refresh_render_texture();
}

BPPT gui::bpp() const
{
     return m_selections->bpp;
}

void gui::combo_swizzle_path()
{
     if (!m_field)
     {
          return;
     }
     if (!combo_swizzle_path(m_map_sprite->filter().swizzle))
     {
          return;
     }
     if (m_map_sprite->filter().swizzle.enabled())
     {
          m_map_sprite->filter().deswizzle.disable();
     }

     refresh_render_texture(true);
}

void gui::combo_deswizzle_path()
{
     if (!m_field)
     {
          return;
     }
     if (!combo_deswizzle_path(m_map_sprite->filter().deswizzle))
     {
          return;
     }
     if (m_map_sprite->filter().deswizzle.enabled())
     {
          m_map_sprite->filter().swizzle.disable();
     }

     refresh_render_texture(true);
}

void gui::combo_swizzle_as_one_image_path()
{
     if (!m_field)
     {
          return;
     }
     if (!combo_swizzle_as_one_image_path(m_map_sprite->filter().swizzle_as_one_image))
     {
          return;
     }
     if (m_map_sprite->filter().swizzle_as_one_image.enabled())
     {
          m_map_sprite->filter().swizzle_as_one_image.disable();
     }

     refresh_render_texture(true);
}

void gui::combo_swizzle_map_path()
{
     if (!m_field)
     {
          return;
     }
     if (!combo_swizzle_map_path(m_map_sprite->filter().swizzle_map))
     {
          return;
     }
     // below if changed
     if (m_map_sprite->filter().swizzle_map.enabled())
     {
          if (const auto paths = fme::generate_swizzle_map_paths(m_selections, *m_map_sprite)(); !std::ranges::empty(paths))
          {
               m_map_sprite->filter().deswizzle_map.disable();
               m_map_sprite->load_map(paths.front());// grab the first match.
          }
          else
          {
               //.map was not found.
               m_map_sprite->filter().swizzle_map.disable();
          }
     }
     else
     {
          // load map from .map here.
          m_map_sprite->first_to_working_and_original();
     }

     refresh_render_texture();
}


void gui::combo_deswizzle_map_path()
{
     if (!m_field)
     {
          return;
     }
     if (!combo_deswizzle_map_path(m_map_sprite->filter().deswizzle_map))
     {
          return;
     }
     if (m_map_sprite->filter().deswizzle_map.enabled())
     {

          if (const auto paths = fme::generate_deswizzle_map_paths(m_selections, *m_map_sprite)(); !std::ranges::empty(paths))
          {

               m_map_sprite->filter().swizzle_map.disable();
               m_map_sprite->load_map(paths.front());// grab the first match.
          }
          else
          {
               m_map_sprite->filter().deswizzle_map.disable();
          }
     }
     else
     {
          // load map from .map here.
          m_map_sprite->first_to_working_and_original();
     }

     refresh_render_texture();
}

std::future<std::future<gui::PathsAndEnabled>> gui::generate_swizzle_paths()
{
     return std::async(
       std::launch::async,
       [ps = ff_8::path_search{ .selections                   = m_selections,
                                .opt_coo                      = get_coo(),
                                .field_name                   = m_map_sprite->get_base_name(),
                                .filters_swizzle_value_string = m_map_sprite->filter().swizzle.value().string(),
                                .bpp_palette                  = m_map_sprite->uniques().palette(),
                                .texture_page_id = m_map_sprite->uniques().texture_page_id() }]() -> std::future<PathsAndEnabled> {
            gui::PathsAndEnabled pande{ .path_key = ConfigKey::CacheUpscalePaths, .enabled_key = ConfigKey::CacheUpscalePathsEnabled };

            const auto           get_map_paths_joined = [&](const auto &container) {
                 return container | std::views::transform([&](const std::string &path) {
                             return ff_8::path_search::get_paths(ps.selections, ps.opt_coo, path);
                        })
                        | std::views::join;
            };

            const auto process = [&](const auto &...ranges) {
                 (
                   [&](const auto &range) {
                        for (const auto &path : get_map_paths_joined(range))
                        {
                             pande.path.emplace_back(path.string());
                        }
                   }(ranges),
                   ...);
            };

            process(ps.selections->paths_vector, ps.selections->paths_vector_swizzle);

            for (const auto &path : pande.path)
            {
                 pande.enabled.push_back(ps.has_swizzle_path(std::filesystem::path{ path }));
            }
            return std::async(std::launch::deferred, [moved_pande = std::move(pande)]() -> PathsAndEnabled { return moved_pande; });
       });
}


std::future<std::future<gui::PathsAndEnabled>> gui::generate_deswizzle_paths()
{
     return std::async(
       std::launch::async,
       [ps = ff_8::path_search{ .selections                     = m_selections,
                                .opt_coo                        = get_coo(),
                                .field_name                     = m_map_sprite->get_base_name(),
                                .filters_deswizzle_value_string = m_map_sprite->filter().deswizzle.value().string(),
                                .working_unique_pupu            = m_map_sprite->working_unique_pupu() }]() -> std::future<PathsAndEnabled> {
            gui::PathsAndEnabled pande{ .path_key = ConfigKey::CacheDeswizzlePaths, .enabled_key = ConfigKey::CacheDeswizzlePathsEnabled };

            const auto           get_map_paths_joined = [&](const auto &container) {
                 return container
                        | std::views::transform([&](const std::string &path) { return ps.get_paths(ps.selections, ps.opt_coo, path); })
                        | std::views::join;
            };

            const auto process = [&](const auto &...ranges) {
                 (
                   [&](const auto &range) {
                        for (const auto &path : get_map_paths_joined(range))
                        {
                             pande.path.emplace_back(path.string());
                        }
                   }(ranges),
                   ...);
            };

            process(ps.selections->paths_vector, ps.selections->paths_vector_deswizzle);

            for (const auto &path : pande.path)
            {
                 pande.enabled.push_back(ps.has_deswizzle_path(std::filesystem::path{ path }));
            }
            return std::async(std::launch::deferred, [moved_pande = std::move(pande)]() -> PathsAndEnabled { return moved_pande; });
       });
}

std::future<std::future<gui::PathsAndEnabled>> gui::generate_swizzle_map_paths()
{
     return std::async(
       std::launch::async,
       [ps = ff_8::path_search{ .selections                   = m_selections,
                                .opt_coo                      = get_coo(),
                                .field_name                   = m_map_sprite->get_base_name(),
                                .filters_swizzle_value_string = m_map_sprite->filter().swizzle.value().string(),
                                .filters_swizzle_map_value_string =
                                  m_map_sprite->filter().swizzle_map.value().string() }]() -> std::future<PathsAndEnabled> {
            gui::PathsAndEnabled pande{ .path_key    = ConfigKey::CacheUpscaleMapPaths,
                                        .enabled_key = ConfigKey::CacheUpscaleMapPathsEnabled };

            const auto           get_map_paths_joined = [&](const auto &container) {
                 return container
                        | std::views::transform([&](const std::string &path) { return ps.get_paths(ps.selections, ps.opt_coo, path); })
                        | std::views::join;
            };

            const auto process = [&](const auto &...ranges) {
                 (
                   [&](const auto &range) {
                        for (const auto &path : get_map_paths_joined(range))
                        {
                             pande.path.emplace_back(path.string());
                        }
                   }(ranges),
                   ...);
            };

            process(ps.selections->paths_vector, ps.selections->paths_vector_swizzle, ps.selections->paths_vector_swizzle_map);

            for (const auto &path : pande.path)
            {
                 pande.enabled.push_back(
                   ps.has_map_path(std::filesystem::path{ path }, ".map", ps.selections->output_map_pattern_for_swizzle));
            }
            return std::async(std::launch::deferred, [moved_pande = std::move(pande)]() -> PathsAndEnabled { return moved_pande; });
       });
}


std::future<std::future<gui::PathsAndEnabled>> gui::generate_deswizzle_map_paths()
{
     return std::async(
       std::launch::async,
       [ps = ff_8::path_search{ .selections                     = m_selections,
                                .opt_coo                        = get_coo(),
                                .field_name                     = m_map_sprite->get_base_name(),
                                .filters_deswizzle_value_string = m_map_sprite->filter().deswizzle.value().string(),
                                .filters_deswizzle_map_value_string =
                                  m_map_sprite->filter().deswizzle_map.value().string() }]() -> std::future<PathsAndEnabled> {
            gui::PathsAndEnabled pande{ .path_key    = ConfigKey::CacheDeswizzleMapPaths,
                                        .enabled_key = ConfigKey::CacheDeswizzleMapPathsEnabled };

            const auto           get_map_paths_joined = [&](const auto &container) {
                 return container
                        | std::views::transform([&](const std::string &path) { return ps.get_paths(ps.selections, ps.opt_coo, path); })
                        | std::views::join;
            };

            const auto process = [&](const auto &...ranges) {
                 (
                   [&](const auto &range) {
                        for (const auto &path : get_map_paths_joined(range))
                        {
                             pande.path.emplace_back(path.string());
                        }
                   }(ranges),
                   ...);
            };

            process(ps.selections->paths_vector, ps.selections->paths_vector_deswizzle, ps.selections->paths_vector_deswizzle_map);

            for (const auto &path : pande.path)
            {
                 pande.enabled.emplace_back(
                   ps.has_map_path(std::filesystem::path{ path }, ".map", ps.selections->output_map_pattern_for_deswizzle));
            }
            return std::async(std::launch::deferred, [moved_pande = std::move(pande)]() -> PathsAndEnabled { return moved_pande; });
       });
}

bool gui::combo_swizzle_path(ff_8::filter_old<std::filesystem::path, ff_8::FilterTag::Swizzle> &filter) const
{
     const auto gcc = fme::GenericComboClassWithFilterAndFixedToggles(
       gui_labels::swizzle_path,
       [this]() { return m_selections->cache_swizzle_paths; },
       [this]() { return m_selections->cache_swizzle_paths_enabled; },
       [this]() { return m_selections->cache_swizzle_paths; },
       [this]() { return m_selections->cache_swizzle_paths; },
       [&filter]() -> auto & { return filter; },
       generic_combo_settings{ .num_columns = 1, .show_explore_button = true });
     return m_field && gcc.render();
}


bool gui::combo_swizzle_as_one_image_path(ff_8::filter_old<std::filesystem::path, ff_8::FilterTag::SwizzleAsOneImage> &filter) const
{
     const auto gcc = fme::GenericComboClassWithFilterAndFixedToggles(
       gui_labels::swizzle_as_one_image_path,
       [this]() { return m_selections->cache_swizzle_paths; },
       [this]() { return m_selections->cache_swizzle_paths_enabled; },
       [this]() { return m_selections->cache_swizzle_paths; },
       [this]() { return m_selections->cache_swizzle_paths; },
       [&filter]() -> auto & { return filter; },
       generic_combo_settings{ .num_columns = 1, .show_explore_button = true });
     return m_field && gcc.render();
}

bool gui::combo_deswizzle_path(ff_8::filter_old<std::filesystem::path, ff_8::FilterTag::Deswizzle> &filter) const
{
     const auto gcc = fme::GenericComboClassWithFilterAndFixedToggles(
       gui_labels::deswizzle_path,
       [this]() { return m_selections->cache_deswizzle_paths; },
       [this]() { return m_selections->cache_deswizzle_paths_enabled; },
       [this]() { return m_selections->cache_deswizzle_paths; },
       [this]() { return m_selections->cache_deswizzle_paths; },
       [&filter]() -> auto & { return filter; },
       generic_combo_settings{ .num_columns = 1, .show_explore_button = true });
     return m_field && gcc.render();
}

bool gui::combo_swizzle_map_path(ff_8::filter_old<std::filesystem::path, ff_8::FilterTag::SwizzleMap> &filter) const
{
     const auto gcc = fme::GenericComboClassWithFilterAndFixedToggles(
       gui_labels::swizzle_map_path,
       [this]() { return m_selections->cache_swizzle_map_paths; },
       [this]() { return m_selections->cache_swizzle_map_paths_enabled; },
       [this]() { return m_selections->cache_swizzle_map_paths; },
       [this]() { return m_selections->cache_swizzle_map_paths; },
       [&filter]() -> auto & { return filter; },
       generic_combo_settings{ .num_columns = 1, .show_explore_button = true });
     return m_field && gcc.render();
}


bool gui::combo_deswizzle_map_path(ff_8::filter_old<std::filesystem::path, ff_8::FilterTag::DeswizzleMap> &filter) const
{
     const auto gcc = fme::GenericComboClassWithFilterAndFixedToggles(
       gui_labels::deswizzle_map_path,
       [this]() { return m_selections->cache_deswizzle_map_paths; },
       [this]() { return m_selections->cache_deswizzle_map_paths_enabled; },
       [this]() { return m_selections->cache_deswizzle_map_paths; },
       [this]() { return m_selections->cache_deswizzle_map_paths; },
       [&filter]() -> auto & { return filter; },
       generic_combo_settings{ .num_columns = 1, .show_explore_button = true });
     return m_field && gcc.render();
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