#include "draw_window.hpp"
#include "gui_labels.hpp"
#include "push_pop_id.hpp"
#include "tool_tip.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>// for glm::translate, glm::ortho, etc.
#include <glm/gtc/type_ptr.hpp>// for glm::value_ptr
#include <IconsFontAwesome6.h>
#include <ImGuizmo.h>
static ImVec2 operator+(const ImVec2 &a, const ImVec2 &b)
{
     return ImVec2(a.x + b.x, a.y + b.y);
}

void fme::draw_window::update(std::weak_ptr<fme::Selections> in_selections)
{
     m_selections = std::move(in_selections);
}

void fme::draw_window::update(std::weak_ptr<const mim_sprite> in_mim_sprite)
{
     m_mim_sprite = std::move(in_mim_sprite);
}

void fme::draw_window::update(std::weak_ptr<fme::map_sprite> in_map_sprite)
{
     m_map_sprite = std::move(in_map_sprite);
}
void fme::draw_window::render() const
{

     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock selections: shared_ptr is expired.");
          return;
     }
     if (!selections->display_draw_window)
     {
          return;
     }

     const auto t_mim_sprite = m_mim_sprite.lock();
     if (!t_mim_sprite)
     {
          spdlog::error("Failed to lock mim_sprite: shared_ptr is expired.");
          return;
     }

     const auto t_map_sprite = m_map_sprite.lock();
     if (!t_map_sprite)
     {
          spdlog::error("Failed to lock map_sprite: shared_ptr is expired.");
          return;
     }

     static constexpr ImGuiWindowFlags window_flags =
       ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar;
     const auto        is_valid_float  = [](const float f) -> bool { return !std::isnan(f) && !std::isinf(f); };
     const auto        is_valid_ImVec2 = [&is_valid_float](const ImVec2 v) -> bool { return is_valid_float(v.x) && is_valid_float(v.y); };
     static const auto DrawCheckerboardBackground =
       [&](const ImVec2 &window_pos, const ImVec2 &window_size, float tile_size, color color1, color color2) {
            if (
              window_size.x < 1.f || window_size.y < 1.f || !is_valid_ImVec2(window_pos) || !is_valid_ImVec2(window_size)
              || !is_valid_float(tile_size))
            {
                 return;
            }
            //   spdlog::info(
            //     "window_pos: ({:.2f}, {:.2f}), window_size: ({:.2f}, {:.2f}), tile_size: {:.2f}, color1: ({}, {}, {}, {}), color2: ({},
            //     {}, "
            //     "{}, {})",
            //     window_pos.x,
            //     window_pos.y,
            //     window_size.x,
            //     window_size.y,
            //     tile_size,
            //     color1.r,
            //     color1.g,
            //     color1.b,
            //     color1.a,
            //     color2.r,
            //     color2.g,
            //     color2.b,
            //     color2.a);
            const auto fbb  = m_checkerboard_framebuffer.backup();
            const auto fbrb = m_checkerboard_batchrenderer.backup();
            if (
              m_checkerboard_framebuffer.width() != static_cast<int>(window_size.x)
              || m_checkerboard_framebuffer.height() != static_cast<int>(window_size.y))
            {
                 glengine::FrameBufferSpecification spec = { .width  = static_cast<int>(window_size.x),
                                                             .height = static_cast<int>(window_size.y) };
                 m_checkerboard_framebuffer              = glengine::FrameBuffer{ spec };
            }

            m_checkerboard_framebuffer.bind();
            glengine::GlCall{}(glViewport, 0, 0, (GLsizei)m_checkerboard_framebuffer.width(), (GLsizei)m_checkerboard_framebuffer.height());
            glengine::Renderer::Clear();
            m_checkerboard_batchrenderer.bind();
            m_fixed_render_camera.set_projection(
              0.f, static_cast<float>(m_checkerboard_framebuffer.width()), 0.f, static_cast<float>(m_checkerboard_framebuffer.height()));

            m_checkerboard_batchrenderer.shader().set_uniform("tile_size", tile_size);
            //   m_checkerboard_batchrenderer.shader().set_uniform(
            //     "resolution", glm::vec2{ m_checkerboard_framebuffer.width(), m_checkerboard_framebuffer.height() });
            m_checkerboard_batchrenderer.shader().set_uniform("color1", glm::vec4{ color1 });
            m_checkerboard_batchrenderer.shader().set_uniform("color2", glm::vec4{ color2 });
            m_checkerboard_batchrenderer.shader().set_uniform("u_MVP", m_fixed_render_camera.view_projection_matrix());
            m_checkerboard_batchrenderer.clear();
            m_checkerboard_batchrenderer.draw_quad(
              glm::vec3{}, fme::colors::White, glm::vec2{ m_checkerboard_framebuffer.width(), m_checkerboard_framebuffer.height() });
            m_checkerboard_batchrenderer.draw();
            m_checkerboard_batchrenderer.on_render();
            m_checkerboard_framebuffer.bind_color_attachment();
            if (!ImGuizmo::IsOver())
            {
                 ImGui::InvisibleButton("##DrawWindowViewport", window_size, ImGuiButtonFlags_None);
            }
            else
            {
                 ImGui::Dummy(window_size);// Doesn't generate a hoverable item
            }
            ImGui::GetWindowDrawList()->AddImage(
              glengine::ConvertGliDtoImTextureId<ImTextureID>(m_checkerboard_framebuffer.color_attachment_id()),
              window_pos,
              ImVec2{ window_pos.x + window_size.x, window_pos.y + window_size.y });
            ImGui::SetCursorScreenPos(window_pos);
       };

     bool      &visible     = selections->display_draw_window;
     const auto pop_visible = glengine::ScopeGuard{ [&selections, &visible, was_visable = visible] {
          if (was_visable != visible)
          {
               selections->update_configuration_key(ConfigKey::DisplayDrawWindow);
          }
     } };

     const auto map_test = [&]() { return !t_map_sprite->fail() && selections->draw == draw_mode::draw_map; };
     const auto mim_test = [&]() { return !t_mim_sprite->fail() && selections->draw == draw_mode::draw_mim; };

     if (mim_test())
     {
          const auto pop_style0 = glengine::ScopeGuard([]() { ImGui::PopStyleVar(); });
          ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.F, 0.F));
          const auto pop_id0 = PushPopID();
          const auto pop_end = glengine::ScopeGuard(&ImGui::End);
          // const auto pop_style1 = glengine::ScopeGuard([]() { ImGui::PopStyleColor(); });
          //  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ clear_color.r / 256.F, clear_color.g / 256.F, clear_color.b / 256.F,
          //  0.9F });
          if (!ImGui::Begin(gui_labels::draw_window_title.data(), &visible, window_flags))
          {
               return;
          }

          const auto   wsize      = ImGui::GetContentRegionAvail();
          const auto   img_size   = t_mim_sprite->get_texture()->get_size();

          const auto   screen_pos = ImGui::GetCursorScreenPos();
          const float  scale      = std::max(wsize.x / img_size.x, wsize.y / img_size.y);
          const ImVec2 scaled_size(img_size.x * scale, img_size.y * scale);

          DrawCheckerboardBackground(
            screen_pos,
            scaled_size,
            (selections->draw_palette ? 0.25F * scale : 4.F * scale),
            selections->background_color.fade(-0.2F),
            selections->background_color.fade(0.2F));

          const auto pop_id1 = PushPopID();

          ImGui::GetWindowDrawList()->AddImage(
            glengine::ConvertGliDtoImTextureId<ImTextureID>(t_mim_sprite->get_texture()->id()),
            screen_pos,
            ImVec2{ screen_pos.x + scaled_size.x, screen_pos.y + scaled_size.y });

          draw_mim_grid_lines_for_tiles(screen_pos, scaled_size, scale);

          draw_mim_grid_lines_for_texture_page(screen_pos, scaled_size, scale);
     }
     else if (map_test())
     {
          const auto pop_style0 = glengine::ScopeGuard([]() { ImGui::PopStyleVar(); });
          ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.F, 0.F));
          const auto pop_id0 = PushPopID();
          const auto pop_end = glengine::ScopeGuard(&ImGui::End);
          // const auto pop_style1 = glengine::ScopeGuard([]() { ImGui::PopStyleColor(); });
          //  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ clear_color.r / 256.F, clear_color.g / 256.F, clear_color.b / 256.F,
          //  0.9F });
          if (!ImGui::Begin(gui_labels::draw_window_title.data(), &visible, window_flags))
          {
               return;
          }


          const auto   wsize      = ImGui::GetContentRegionAvail();
          const auto   img_size   = t_map_sprite->get_render_texture().get_size();

          const auto   screen_pos = ImGui::GetCursorScreenPos();
          const float  scale      = (std::max)(wsize.x / img_size.x, wsize.y / img_size.y);
          const ImVec2 scaled_size(img_size.x * scale, img_size.y * scale);

          DrawCheckerboardBackground(
            screen_pos,
            scaled_size,
            (static_cast<float>(t_map_sprite->get_map_scale() * 4.F)) * scale,
            selections->background_color.fade(-0.2F),
            selections->background_color.fade(0.2F));

          const auto pop_id1 = PushPopID();
          ImGui::GetWindowDrawList()->AddImage(
            glengine::ConvertGliDtoImTextureId<ImTextureID>(t_map_sprite->get_render_texture().color_attachment_id()),
            screen_pos,
            ImVec2{ screen_pos.x + scaled_size.x, screen_pos.y + scaled_size.y });


          draw_map_grid_lines_for_tiles(screen_pos, scaled_size, scale);

          draw_map_grid_lines_for_texture_page(screen_pos, scaled_size, scale);

          draw_map_grid_for_conflict_tiles(screen_pos, scale);

          UseImGuizmo(scale, screen_pos);

          if (!ImGuizmo::IsUsing() && !ImGuizmo::IsOver())
          {
               update_hover_and_mouse_button_status_for_map(screen_pos, scale);
          }
     }
}


void fme::draw_window::hovered_index(std::ptrdiff_t index)
{
     m_hovered_index = index;
}
const std::vector<std::size_t> &fme::draw_window::hovered_tiles_indices() const
{
     return m_hovered_tiles_indices;
}
const fme::MousePositions &fme::draw_window::mouse_positions() const
{
     return m_mouse_positions;
}
void fme::draw_window::update_mouse_positions()
{
     m_mouse_positions.update();
}
const std::vector<std::size_t> &fme::draw_window::clicked_tile_indices() const
{
     return m_clicked_tile_indices;
}
void fme::draw_window::clear_clicked_tile_indices() const
{
     m_clicked_tile_indices.clear();
}

void fme::draw_window::remove_clicked_index(std::size_t in_index) const
{
     const auto remove_result = std::ranges::remove_if(m_clicked_tile_indices, [&](std::size_t index) { return in_index == index; });
     m_clicked_tile_indices.erase(remove_result.begin(), remove_result.end());
}

void fme::draw_window::update_hover_and_mouse_button_status_for_map(const ImVec2 &img_start, const float scale) const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock selections: shared_ptr is expired.");
          return;
     }
     const auto t_map_sprite = m_map_sprite.lock();
     if (!t_map_sprite)
     {
          spdlog::error("Failed to lock map_sprite: shared_ptr is expired.");
          return;
     }

     // Check if the mouse is over the image
     const ImVec2 mouse_pos = ImGui::GetMousePos();
     if (ImGui::IsItemHovered())
     {
          // Calculate the mouse position relative to the image
          glm::vec2  relative_pos(mouse_pos.x - img_start.x, mouse_pos.y - img_start.y);

          const auto old_pixel    = m_mouse_positions.pixel;
          // Map it back to the texture coordinates
          m_mouse_positions.pixel = glm::ivec2(
            static_cast<int>(relative_pos.x / scale / static_cast<float>(t_map_sprite->get_map_scale())),
            static_cast<int>(relative_pos.y / scale / static_cast<float>(t_map_sprite->get_map_scale())));

          if (selections->draw_swizzle)
          {
               m_mouse_positions.pixel /= 16;
               m_mouse_positions.pixel *= 16;
               m_mouse_positions.texture_page = static_cast<uint8_t>(m_mouse_positions.pixel.x / 256);
          }
          else
          {
               m_mouse_positions.texture_page = (std::numeric_limits<std::uint8_t>::max)();
          }

          m_mouse_positions.mouse_enabled = true;
          m_mouse_positions.mouse_moved   = (m_mouse_positions.pixel != old_pixel);


          if (m_mouse_positions.mouse_moved)
          {
               t_map_sprite->const_visit_working_tiles([&](const auto &tiles) {
                    m_hovered_tiles_indices =
                      t_map_sprite->find_intersecting(tiles, m_mouse_positions.pixel, m_mouse_positions.texture_page, false, true);
               });
          }

          if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
          {
               m_mouse_positions.left = true;
               if (m_mouse_positions.mouse_moved)
               {
                    m_clicked_tile_indices = m_hovered_tiles_indices;
               }
               else
               {
                    t_map_sprite->const_visit_working_tiles([&](const auto &tiles) {
                         m_clicked_tile_indices =
                           t_map_sprite->find_intersecting(tiles, m_mouse_positions.pixel, m_mouse_positions.texture_page, false, true);
                    });

                    m_mouse_positions.down_pixel = m_mouse_positions.pixel;
               }
          }
     }
     else
     {
          m_mouse_positions.mouse_enabled = false;
          m_mouse_positions.mouse_moved   = false;
          m_hovered_tiles_indices.clear();
     }
     if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
     {
          m_mouse_positions.left = false;
     }
}
void fme::draw_window::draw_map_grid_lines_for_tiles(const ImVec2 &screen_pos, const ImVec2 &scaled_size, const float scale) const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock selections: shared_ptr is expired.");
          return;
     }
     // Drawing grid lines within the window if selections->draw_grid is true
     if (!selections->draw_grid)
     {
          return;
     }
     const auto t_map_sprite = m_map_sprite.lock();
     if (!t_map_sprite)
     {
          spdlog::error("Failed to lock map_sprite: shared_ptr is expired.");
          return;
     }
     // Get the starting position and size of the image
     const ImVec2 img_end      = { screen_pos.x + scaled_size.x, screen_pos.y + scaled_size.y };

     // Calculate grid spacing
     const float  grid_spacing = 16.0f * scale * static_cast<float>(t_map_sprite->get_map_scale());

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
void fme::draw_window::draw_map_grid_for_conflict_tiles(const ImVec2 &screen_pos, const float scale) const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock selections: shared_ptr is expired.");
          return;
     }
     if (!selections->draw_tile_conflict_rects)
     {
          return;
     }
     const auto t_map_sprite = m_map_sprite.lock();
     if (!t_map_sprite)
     {
          spdlog::error("Failed to lock map_sprite: shared_ptr is expired.");
          return;
     }
     t_map_sprite->const_visit_working_tiles([&](const auto &working_tiles) {
          const auto &similar_counts   = t_map_sprite->working_similar_counts();
          const auto &animation_counts = t_map_sprite->working_animation_counts();

          for (const auto indices : t_map_sprite->working_conflicts().range_of_conflicts())
          {
               const auto action = [&](const auto index) {
                    assert(std::cmp_less(index, std::ranges::size(working_tiles)) && "Index out of Range...");
                    const auto index_to_working_tile = [&working_tiles](const auto i) {
                         auto begin = std::ranges::cbegin(working_tiles);
                         std::ranges::advance(begin, i);
                         return *begin;
                    };
                    const auto        &working_tile  = index_to_working_tile(index);
                    const std::uint8_t similar_count = [&] -> std::uint8_t {
                         if (auto it = similar_counts.find(working_tile); it != similar_counts.end())
                         {
                              return it->second;
                         }
                         return {};
                    }();
                    const bool         similar_over_1  = std::cmp_greater(similar_count, 1);
                    const std::uint8_t animation_count = [&] -> std::uint8_t {
                         if (auto it = animation_counts.find(working_tile); it != animation_counts.end())
                         {
                              return it->second;
                         }
                         return {};
                    }();
                    const bool  animation_over_1 = std::cmp_greater(animation_count, 1);

                    const float x                = [&]() {
                         if (selections->draw_swizzle)
                         {
                              return screen_pos.x
                            + ((static_cast<float>(working_tile.source_x()) + (static_cast<float>(working_tile.texture_id()) * 256.F)) * scale * static_cast<float>(t_map_sprite->get_map_scale()));
                         }
                         return screen_pos.x
                                + (static_cast<float>(working_tile.x()) * scale * static_cast<float>(t_map_sprite->get_map_scale()));
                    }();
                    const float y = [&]() {
                         if (selections->draw_swizzle)
                         {
                              return screen_pos.y
                                     + (static_cast<float>(working_tile.source_y()) * scale * static_cast<float>(t_map_sprite->get_map_scale()));
                         }
                         return screen_pos.y
                                + (static_cast<float>(working_tile.y()) * scale * static_cast<float>(t_map_sprite->get_map_scale()));
                    }();
                    const float tile_size     = 16.0f * scale * static_cast<float>(t_map_sprite->get_map_scale());
                    const auto [c, thickness] = [&]() -> std::pair<color, float> {
                         const auto default_thickness = 3.F;
                         const auto hover_thickeness  = 4.5F;
                         if (
                           (!selections->draw_swizzle && !animation_over_1 && std::cmp_not_equal(m_hovered_index, index))
                           || ((selections->draw_swizzle || animation_over_1) && std::ranges::find(indices, m_hovered_index) == std::ranges::end(indices)))
                         {
                              if (
                                std::ranges::empty(m_hovered_tiles_indices)
                                || std::ranges::find(m_hovered_tiles_indices, static_cast<std::size_t>(index))
                                     == std::ranges::end(m_hovered_tiles_indices)
                                || !m_mouse_positions.mouse_enabled)
                              {
                                   if (similar_over_1)
                                   {
                                        return { colors::ButtonGreen.opaque().fade(-.2F), default_thickness };
                                   }
                                   if (animation_over_1)
                                   {
                                        return { colors::ButtonPink.opaque().fade(-.2F), default_thickness };
                                   }
                                   return { colors::Button.opaque().fade(-.2F), default_thickness };
                              }
                              if (m_mouse_positions.left)
                              {
                                   if (similar_over_1)
                                   {
                                        return { colors::ButtonGreenActive.opaque().fade(-.2F), default_thickness };
                                   }
                                   if (animation_over_1)
                                   {
                                        return { colors::ButtonPinkActive.opaque().fade(-.2F), default_thickness };
                                   }
                                   return { colors::ButtonActive.opaque().fade(-.2F), default_thickness };
                              }

                              if (selections->draw_swizzle)
                              {
                                   std::string strtooltip = fmt::format(
                                     "Indicies: {}\n{}", indices, indices | std::ranges::views::transform(index_to_working_tile));
                                   tool_tip(strtooltip, true);
                              }
                              else
                              {
                                   std::string strtooltip = fmt::format(
                                     "Index {}\n{}\nSimilar Count: {}\nAnimation Count: {}",
                                     index,
                                     working_tile,
                                     similar_over_1 ? similar_count : 0,
                                     animation_over_1 ? animation_count : 0);

                                   tool_tip(strtooltip, true);
                              }
                         }
                         if (similar_over_1)
                         {
                              return { colors::ButtonGreenHovered.opaque(), hover_thickeness };
                         }

                         if (animation_over_1)
                         {
                              return { colors::ButtonPinkHovered.opaque(), hover_thickeness };
                         }
                         return { colors::ButtonHovered.opaque(), hover_thickeness };
                    }();

                    ImGui::GetWindowDrawList()->AddRect(ImVec2(x, y), ImVec2(x + tile_size, y + tile_size), ImU32{ c }, {}, {}, thickness);

                    // todo add hover action using the hovered_indices and change color for if similar counts >1
               };
               if (selections->draw_swizzle)
               {// all are drawn in the same spot so we only need to draw one.
                    if (std::ranges::find(indices, m_hovered_index) == std::ranges::end(indices))
                    {
                         std::ranges::for_each(indices | std::ranges::views::take(1), action);
                    }
                    else
                    {
                         action(m_hovered_index);
                    }


                    // there might be different kinds of conflicts in the same location. but here we're assuming your either one or
                    // another. because we can't quite draw all the colors in the same place.
               }
               else
               {
                    std::ranges::for_each(indices, action);
                    if (std::ranges::find(indices, m_hovered_index) != std::ranges::end(indices))
                    {
                         action(m_hovered_index);
                    }
               }
          }
     });
}
void fme::draw_window::draw_map_grid_lines_for_texture_page(const ImVec2 &screen_pos, const ImVec2 &scaled_size, const float scale) const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock selections: shared_ptr is expired.");
          return;
     }
     // Drawing grid lines within the window if selections->draw_grid is true
     if (!selections->draw_texture_page_grid || !selections->draw_swizzle)
     {
          return;
     }
     const auto t_map_sprite = m_map_sprite.lock();
     if (!t_map_sprite)
     {
          spdlog::error("Failed to lock map_sprite: shared_ptr is expired.");
          return;
     }
     // Get the starting position and size of the image
     const ImVec2 img_end      = { screen_pos.x + scaled_size.x, screen_pos.y + scaled_size.y };

     // Calculate grid spacing
     const float  grid_spacing = 256.0f * scale * static_cast<float>(t_map_sprite->get_map_scale());

     // Iterate over horizontal and vertical lines
     for (float x = screen_pos.x; x < img_end.x; x += grid_spacing)
     {
          // Draw vertical lines
          ImGui::GetWindowDrawList()->AddLine(ImVec2(x, screen_pos.y), ImVec2(x, img_end.y), IM_COL32(255, 255, 0, 255));
     }
}
void fme::draw_window::draw_mim_grid_lines_for_tiles(const ImVec2 &screen_pos, const ImVec2 &scaled_size, const float scale) const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock selections: shared_ptr is expired.");
          return;
     }
     // Drawing grid lines within the window if selections->draw_grid is true
     if (!selections->draw_grid)
     {
          return;
     }

     const auto t_map_sprite = m_map_sprite.lock();
     if (!t_map_sprite)
     {
          spdlog::error("Failed to lock map_sprite: shared_ptr is expired.");
          return;
     }
     // Get the starting position and size of the image
     const ImVec2 img_end      = { screen_pos.x + scaled_size.x, screen_pos.y + scaled_size.y };

     // Calculate grid spacing
     const float  grid_spacing = (selections->draw_palette ? 1.F : 16.0f) * scale;

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
void fme::draw_window::draw_mim_grid_lines_for_texture_page(const ImVec2 &screen_pos, const ImVec2 &scaled_size, const float scale) const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock selections: shared_ptr is expired.");
          return;
     }
     // Drawing grid lines within the window if selections->draw_grid is true
     if (!selections->draw_texture_page_grid || selections->draw_palette)
     {
          return;
     }

     // Get the starting position and size of the image
     const ImVec2 img_end      = { screen_pos.x + scaled_size.x, screen_pos.y + scaled_size.y };

     // Calculate grid spacing

     const float  grid_spacing = [&]() {
          using namespace open_viii::graphics;
          if (selections->bpp.bpp4())
          {
               return 256.f;
          }
          else if (selections->bpp.bpp8())
          {
               return 128.f;
          }
          else if (selections->bpp.bpp16())
          {
               return 64.F;
          }
          else
          {
               spdlog::error("selections->bpp.raw() = {}", selections->bpp.raw());
               throw;
          }
     }() * scale;

     //  selections->bpp

     // Iterate over horizontal and vertical lines
     for (float x = screen_pos.x; x < img_end.x; x += grid_spacing)
     {
          // Draw vertical lines
          ImGui::GetWindowDrawList()->AddLine(ImVec2(x, screen_pos.y), ImVec2(x, img_end.y), IM_COL32(255, 255, 0, 255));
     }
}
void fme::draw_window::UseImGuizmo([[maybe_unused]] const float scale, [[maybe_unused]] const ImVec2 &screen_pos) const
{
     if (std::ranges::empty(m_clicked_tile_indices))
     {
          return;
     }
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock selections: shared_ptr is expired.");
          return;
     }
     const auto t_map_sprite = m_map_sprite.lock();
     if (!t_map_sprite)
     {
          spdlog::error("Failed to lock map_sprite: shared_ptr is expired.");
          return;
     }

     // attempt to make gizmo stay on screen
     ImVec2    window_pos  = ImGui::GetWindowPos();// top-left of window (includes title bar)
     ImVec2    content_min = ImGui::GetWindowContentRegionMin();// top-left of content (relative to window)
     ImVec2    content_max = ImGui::GetWindowContentRegionMax();// bottom-right of content (relative to window)

     ImVec2    clip_min    = window_pos + content_min;// top-left of visible drawable area
     ImVec2    clip_max    = window_pos + content_max;// bottom-right of visible drawable area

     // Y flipped projection matrix for visible window area
     glm::mat4 projection  = glm::ortho(
       clip_min.x - screen_pos.x,
       clip_max.x - screen_pos.x,
       clip_max.y - screen_pos.y,// flipped Y
       clip_min.y - screen_pos.y,
       -1.0f,
       1.0f);

     glm::mat4 view         = m_fixed_render_camera.view_matrix();// identity for 2D or your actual camera view if available

     auto      tilePosition = glm::vec3(m_mouse_positions.down_pixel, 0.f) * scale * static_cast<float>(t_map_sprite->get_map_scale());
     // Your object transform matrix, e.g. tile transform
     glm::mat4 objectMatrix = glm::translate(glm::mat4(1.0f), tilePosition);

     // Apply the adjusted size
     // ImGuizmo::SetGizmoSizeClipSpace(selections->draw_swizzle ? 0.021f : 0.15f);
     ImGuizmo::SetOrthographic(true);
     ImGuizmo::SetDrawlist();
     // Set gizmo draw region to the visible ImGui window area
     ImGuizmo::SetRect(clip_min.x, clip_min.y, clip_max.x - clip_min.x, clip_max.y - clip_min.y);

     ImVec2 mouse = ImGui::GetMousePos();
     if (
       !ImGuizmo::IsUsing() && ImGuizmo::IsOver()
       && (mouse.x < clip_min.x || mouse.x > clip_max.x || mouse.y < clip_min.y || mouse.y > clip_max.y))
     {
          return;// Or skip ImGuizmo interaction for this frame
     }

     if (ImGuizmo::Manipulate(
           glm::value_ptr(view),
           glm::value_ptr(projection),
           ImGuizmo::TRANSLATE_X | ImGuizmo::TRANSLATE_Y,
           ImGuizmo::LOCAL,
           glm::value_ptr(objectMatrix)))
     {
          tilePosition            = objectMatrix[3];
          const auto relative_pos = glm::vec2(tilePosition.x, tilePosition.y);
          const auto old_pixel    = m_mouse_positions.down_pixel;
          // Map it back to the texture coordinates
          m_mouse_positions.pixel = glm::ivec2(
            static_cast<int>(relative_pos.x / scale / static_cast<float>(t_map_sprite->get_map_scale())),
            static_cast<int>(relative_pos.y / scale / static_cast<float>(t_map_sprite->get_map_scale())));

          if (selections->draw_swizzle)
          {
               m_mouse_positions.pixel /= 16;
               m_mouse_positions.pixel *= 16;
               m_mouse_positions.texture_page = static_cast<uint8_t>(m_mouse_positions.pixel.x / 256);
          }
          else
          {
               m_mouse_positions.texture_page = (std::numeric_limits<std::uint8_t>::max)();
          }

          m_mouse_positions.mouse_moved = (m_mouse_positions.pixel != old_pixel);


          if (m_mouse_positions.mouse_moved)
          {
               if (!m_translation_in_progress)
               {
                    m_location_backup = m_mouse_positions.down_pixel;
               }
               m_translation_in_progress = true;
               t_map_sprite->begin_multi_frame_working(fmt::format("ImGuizmo {}, {}", ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT, ICON_FA_SPINNER));

               t_map_sprite->const_visit_working_tiles([&](const auto &tiles) {
                    m_hovered_tiles_indices =
                      t_map_sprite->find_intersecting(tiles, m_mouse_positions.pixel, m_mouse_positions.texture_page, false, true);
               });

               t_map_sprite->update_position(m_mouse_positions.pixel, m_mouse_positions.down_pixel, m_clicked_tile_indices);
          }
          m_mouse_positions.down_pixel = m_mouse_positions.pixel;
     }
     else if (m_translation_in_progress && !ImGuizmo::IsUsing())
     {
          m_translation_in_progress      = false;
          const glm::ivec2 delta         = m_mouse_positions.down_pixel - m_location_backup;
          std::string      history_entry = fmt::format("ImGuizmo {} ({}, {})", ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT, delta.x, delta.y);
          t_map_sprite->end_multi_frame_working(std::move(history_entry));
     }
}
