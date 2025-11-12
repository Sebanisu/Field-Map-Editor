//
// Created by pcvii on 1/11/2022.
//

#ifndef FIELD_MAP_EDITOR_MAP_HPP
#define FIELD_MAP_EDITOR_MAP_HPP
#include "Application.hpp"
#include "Changed.hpp"
#include "FF8LoadTextures.hpp"
#include "Fields.hpp"
#include "ImGuiIndent.hpp"
#include "ImGuiTileDisplayWindow.hpp"
#include "ImGuiViewPortWindow.hpp"
#include "MapBlends.hpp"
#include "MapDims.hpp"
#include "MapFilters.hpp"
#include "MapHistory.hpp"
#include "MapTileAdjustments.hpp"
#include "MouseToTilePos.h"
#include "OrthographicCameraController.hpp"
#include "SimilarAdjustments.hpp"
#include "TransformedSortedUniqueCopy.hpp"
#include "UniqueTileValues.hpp"
#include "Window.hpp"
#include <ff_8/TileOperations.hpp>
#include <glengine/BatchRenderer.hpp>
#include <glengine/BlendModeEquations.hpp>
#include <glengine/BlendModeParameters.hpp>
#include <glengine/BlendModeSettings.hpp>
#include <glengine/Counter.hpp>
#include <glengine/DelayedTextures.hpp>
#include <glengine/Event/EventDispatcher.hpp>
#include <glengine/FrameBuffer.hpp>
#include <glengine/FrameBufferBackup.hpp>
#include <glengine/OrthographicCamera.hpp>
#include <glengine/PixelBuffer.hpp>
#include <imgui_utils/GenericCombo.hpp>
#include <imgui_utils/ImGuiDisabled.hpp>
#include <imgui_utils/ImGuiPushID.hpp>
#include <source_location>
#include <type_traits>
namespace ff_8
{
template<typename TileFunctions>
class [[nodiscard]] MoveTiles
{
     const std::vector<std::intmax_t> &m_indexes;
     glm::ivec3                        m_pressed  = {};
     glm::ivec3                        m_released = {};

   public:
     MoveTiles(
       const std::vector<std::intmax_t> &indexes,
       glm::ivec3                        pressed,
       glm::ivec3                        released)
       : m_indexes(indexes)
       , m_pressed(pressed)
       , m_released(released)
     {
     }
     void operator()() const
     {
          GetMapHistory()->back().visit_tiles(*this);
     }
     void operator()(const auto &tiles) const
     {
          using TileT
            = std::ranges::range_value_t<std::remove_cvref_t<decltype(tiles)>>;
          std::vector<std::function<TileT(const TileT &)>> operations{};
          if constexpr (std::is_same_v<
                          typename TileFunctions::X,
                          TileOperations::X>)
          {
               operations.push_back(
                 TileOperations::X::TranslateWith{ m_released.x, m_pressed.x });
          }
          if constexpr (std::is_same_v<
                          typename TileFunctions::Y,
                          TileOperations::Y>)
          {
               operations.push_back(
                 TileOperations::Y::TranslateWith{ m_released.y, m_pressed.y });
          }
          if constexpr (std::is_same_v<
                          typename TileFunctions::X,
                          TileOperations::SourceX>)
          {
               operations.push_back(
                 TileOperations::SourceX::TranslateWith{ m_released.x,
                                                         m_pressed.x });
          }
          if constexpr (std::is_same_v<
                          typename TileFunctions::Y,
                          TileOperations::SourceY>)
          {
               operations.push_back(
                 TileOperations::SourceY::TranslateWith{ m_released.y,
                                                         m_pressed.y });
          }
          //@todo I donno what this if constexpr is doing.
          if constexpr (std::is_same_v<
                          typename TileFunctions::TexturePage,
                          TileOperations::TextureId>)
          {
               operations.push_back(
                 TileOperations::TextureId::With{ m_released.z });
          }
          GetMapHistory()->copy_working_perform_operation<TileT>(
            m_indexes,
            [&](TileT &new_tile)
            {
                 int i = {};
                 for (const auto &op : operations)
                 {
                      new_tile = op(new_tile);
                      spdlog::debug("Performed operation {}", i++);
                 }
            });
          GetWindow().trigger_refresh_image();
     }
     [[nodiscard]] operator bool() const noexcept
     {
          return m_released != m_pressed;
     }
};
template<typename TileFunctions>
class Map
{
   public:
     Map() = default;
     Map(const Fields &fields)
       : Map(
           fields,
           {})
     {
     }
     Map(
       const Fields &,
       std::filesystem::path swizzle_path)
       : m_swizzle_path(std::move(swizzle_path))
     {
          if (std::empty(GetMim().path) || std::empty(GetMapHistory().path))
          {
               return;
          }
          if (!std::ranges::empty(m_swizzle_path))
          {
               const auto stem = std::filesystem::path(GetMapHistory().path)
                                   .parent_path()
                                   .stem();
               m_swizzle_path = (std::filesystem::path(m_swizzle_path)
                                 / stem.string().substr(0, 2) / stem)
                                  .string();
               spdlog::debug(
                 "Swizzle Location: \"{}\"", m_swizzle_path.string());
          }
          spdlog::debug("Loaded Map: \"{}\"", GetMapHistory().path);
          spdlog::debug("Begin Loading Textures from Mim.");
          m_swizzle_delayed_textures = LoadTextures(m_swizzle_path);
          visit_unsorted_unfiltered_tiles();
          const auto count            = visit_unsorted_unfiltered_tiles_count();
          m_tile_button_state         = std::vector<bool>(count, false);
          m_tile_button_state_hover   = std::vector<bool>(count, false);
          m_tile_button_state_pressed = std::vector<bool>(count, false);
     }
     void on_update(float ts) const
     {
          if (GetMim().on_update() || m_swizzle_delayed_textures.on_update())
          {
               if (!std::ranges::empty(m_swizzle_path))
               {
                    const auto current_max
                      = (std::ranges::
                           max_element)(*m_swizzle_delayed_textures.textures,
                                        {},
                                        [](const glengine::Texture &texture)
                                        { return texture.height(); });
                    if (
                      static_cast<float>(GetMim()->get_height())
                        * m_map_dims.tile_scale
                      < static_cast<float>(current_max->height()))
                    {
                         m_map_dims.tile_scale
                           = static_cast<float>(current_max->height())
                             / static_cast<float>(GetMim()->get_height());
                         visit_unsorted_unfiltered_tiles();
                    }
               }
               m_changed();
          }
          m_imgui_viewport_window.on_update(ts);
          m_imgui_viewport_window.fit(s_fit_width, s_fit_height);
          m_batch_renderer.on_update(ts);
     }
     void on_render() const
     {
          if (
            std::ranges::empty(GetMapHistory().path)
            || std::ranges::empty(GetMim().path))
          {
               return;
          }
          {
               const auto not_changed = m_changed.unset();
               if (m_changed)
               {
                    m_offscreen_drawing              = true;
                    const auto not_offscreen_drawing = glengine::ScopeGuard(
                      [&]() { m_offscreen_drawing = false; });
                    const auto fbb = glengine::FrameBufferBackup{};
                    m_frame_buffer.bind();

                    glengine::GlCall{}(
                      glViewport,
                      0,
                      0,
                      m_frame_buffer.specification().width,
                      m_frame_buffer.specification().height);
                    glengine::Renderer::Clear();
                    m_frame_buffer.clear_non_standard_color_attachments();
                    render_tiles();
                    if (!m_saving)
                    {
                         render_frame_buffer_grid();
                    }
               }
          }
          // RestoreViewPortToFrameBuffer();
          m_imgui_viewport_window.on_render([this]()
                                            { render_frame_buffer(); });
          m_has_hover = m_imgui_viewport_window.has_hover();
          GetViewPortPreview().on_render(
            m_imgui_viewport_window,
            [this]()
            {
                 m_preview = true;
                 const auto pop_preview
                   = glengine::ScopeGuard([&]() { m_preview = false; });
                 render_frame_buffer();
            });
          ff_8::ImGuiTileDisplayWindow::take_control(m_has_hover, m_id);
          draw_to_tiles_window();
     }
     void draw_to_tiles_window() const
     {

          ff_8::ImGuiTileDisplayWindow::on_render_forward(
            m_id,
            [this]()
            {
                 //      ImGui::Text(
                 //        "%s", fmt::format("Map {}",
                 //        static_cast<uint32_t>(m_id)).c_str());
                 float      text_width = 0.F;
                 ImVec2     last_pos   = {};
                 const auto render_sub_texture
                   = [&text_width, &last_pos](
                       const glengine::SubTexture &sub_texture) -> bool
                 {
                      const auto imgui_texture_id_ref
                        = ConvertGliDtoImTextureId<std::uint64_t>(
                          sub_texture.id());
                      const auto uv     = sub_texture.im_gui_uv<ImVec2>();
                      const auto id_pop = imgui_utils::ImGuiPushId();
                      const auto color  = ImVec4(0.F, 0.F, 0.F, 0.F);
                      last_pos          = ImGui::GetCursorPos();
                      text_width        = ImGui::GetItemRectMax().x;
                      ImGui::PushStyleColor(ImGuiCol_Button, color);
                      const auto pop_color = glengine::ScopeGuard(
                        []() { ImGui::PopStyleColor(1); });
                      // ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
                      // ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
                      bool value = ImGui::ImageButton(
                        "##tile_display_window",
                        imgui_texture_id_ref,
                        ImVec2(32, 32),
                        uv[0],
                        uv[1]);
                      text_width = ImGui::GetStyle().ItemSpacing.x
                                   + ImGui::GetItemRectMax().x - text_width;
                      return value;
                 };
                 const auto  dims = ImGui::GetContentRegionAvail();
                 std::size_t i    = {};

                 const auto  mta  = MapTileAdjustments<TileFunctions>(
                   GetMapHistory(), GetMapHistory().filters, m_similar);
                 auto      *tile_button_state = &m_tile_button_state;
                 const auto common_operation
                   = [&](auto &tile, VisitState &visit_state) -> bool
                 {
                      auto &local_tile_button_state = *tile_button_state;
                      using namespace open_viii::graphics::background;
                      const auto id_pop_2    = imgui_utils::ImGuiPushId();
                      const auto sub_texture = tile_to_sub_texture(tile);
                      const auto increment
                        = glengine::ScopeGuard([&]() { ++i; });
                      if (!sub_texture)
                      {
                           return false;
                      }
                      if (render_sub_texture(*sub_texture))
                      {
                           local_tile_button_state.at(i).flip();
                      }
                      bool changed = false;
                      if (local_tile_button_state.at(i))
                      {
                           ImGui::SameLine();
                           visit_state = mta(tile, changed, i, sub_texture);
                      }
                      else if (
                        dims.x
                          - (last_pos.x + text_width - ImGui::GetCursorPos().x)
                        > text_width)
                      {
                           if (
                             local_tile_button_state.size() != i + 1
                             && local_tile_button_state.at(i + 1))
                           {
                           }
                           else
                           {
                                ImGui::SameLine();
                           }
                      }
                      return changed;
                 };
                 if (m_has_hover)
                 {
                      const auto tp = MouseToTilePos(
                        m_imgui_viewport_window.offset_mouse_pos(), m_map_dims);

                      ImGui::Text(
                        "%s",
                        fmt::format(
                          "Hovered Texture Page :{}\n"
                          "Hovered X,Y: ({},{})",
                          tp.texture_page,
                          tp.x,
                          tp.y)
                          .c_str());

                      tile_button_state = &m_tile_button_state_hover;
                      if (visit_unsorted_unfiltered_tiles(
                            common_operation,
                            MouseTileOverlap<
                              TileFunctions,
                              decltype(m_filters)>(tp, m_filters)))
                      {
                           GetWindow().trigger_refresh_image();
                           // m_changed();
                      }
                      ImGui::Text("%s", " ");
                 }
                 if (m_map_dims.pressed_mouse_location)
                 {
                      tile_button_state = &m_tile_button_state_pressed;
                      i                 = {};
                      last_pos          = ImGui::GetCursorPos();
                      text_width        = ImGui::GetItemRectMax().x;
                      ImGui::Text(
                        "%s",
                        fmt::format(
                          "Clicked Texture Page :{}\n"
                          "Clicked X,Y: ({},{})",
                          m_map_dims.pressed_mouse_location->z,
                          m_map_dims.pressed_mouse_location->x,
                          m_map_dims.pressed_mouse_location->y)
                          .c_str());
                      if (
                        visit_unsorted_unfiltered_tiles(
                          common_operation,// todo fix this to use the stored
                                           // tile indexes
                          //              MouseTileOverlap<TileFunctions,
                          //              MapFilters>(
                          //                MouseToTilePos{
                          //                *(m_map_dims.pressed_mouse_location)
                          //                },
                          //
                          //                GetMapHistory().filters)
                          [this](auto &&tile) -> bool
                          {
                               return std::ranges::any_of(
                                 m_clicked_indexes,
                                 [&](auto &&j) -> bool
                                 {
                                      return std::cmp_equal(
                                        j,
                                        GetMapHistory()
                                          ->get_offset_from_working(tile));
                                 });
                          }))
                      {
                           GetWindow().trigger_refresh_image();
                           // m_changed();
                      }
                      ImGui::Text("%s", " ");
                 }
                 if (ImGui::CollapsingHeader("All Tiles"))
                 {
                      i                 = {};
                      tile_button_state = &m_tile_button_state;
                      last_pos          = ImGui::GetCursorPos();
                      text_width        = ImGui::GetItemRectMax().x;
                      if (visit_unsorted_unfiltered_tiles(
                            common_operation, m_filters))
                      {
                           GetWindow().trigger_refresh_image();
                           // m_changed();
                      }
                 }
            });
     }
     void on_im_gui_update() const
     {
          const auto pop_id = imgui_utils::ImGuiPushId();
          {
               const auto disable = imgui_utils::ImGuiDisabled(
                 std::ranges::empty(GetMapHistory().path)
                 || std::ranges::empty(GetMim().path));

               (void)ImGui::Checkbox("fit Height", &s_fit_height);
               (void)ImGui::Checkbox("fit Width", &s_fit_width);
               m_changed.set_if_true(
                 std::ranges::any_of(
                   std::array{
                     ImGui::Checkbox("draw Grid", &s_draw_grid),
                     [&]() -> bool
                     {
                          if constexpr (!typename TileFunctions::UseBlending{})
                          {
                               return false;
                          }
                          else
                          {
                               const bool checkbox_changed
                                 = ImGui::Checkbox("Blending", &s_blending);
                               const bool blend_options_changed
                                 = s_blends.on_im_gui_update();
                               return checkbox_changed || blend_options_changed;
                          }
                     }() },
                   std::identity{}));


               if (ImGui::Button("Save"))
               {
                    save();
               }
          }
          if (ImGui::CollapsingHeader("Viewport"))
          {
               m_imgui_viewport_window.on_im_gui_update();
          }
          if (ImGui::CollapsingHeader("Map Dims"))
          {
               ImGui::Text(
                 "%s",
                 fmt::format(
                   "DrawPos ({}, {}, {}), Width {}, Height {}"
                   "\n\tOffset ({}, {}),\n\tMin ({}, {}), Max ({},{})\n",
                   m_map_dims.position.x,
                   m_map_dims.position.y,
                   m_map_dims.position.z,
                   m_frame_buffer.specification().width,
                   m_frame_buffer.specification().height,
                   m_map_dims.offset.x,
                   m_map_dims.offset.y,
                   m_map_dims.min.x,
                   m_map_dims.min.y,
                   m_map_dims.max.x,
                   m_map_dims.max.y)
                   .c_str());
          }
          if (ImGui::CollapsingHeader("Batch Renderer"))
          {
               m_batch_renderer.on_im_gui_update();
               m_batch_renderer_red_integer.on_im_gui_update();
          }
          // if (ImGui::CollapsingHeader("Fixed Prerender Camera"))
          // {
          //      m_fixed_render_camera.on_im_gui_update();
          // }
     }
     void on_event(const glengine::event::Item &event) const
     {
          using namespace glengine::event;
          using glengine::Mouse;
          using glengine::Mods;
          m_imgui_viewport_window.on_event(event);
          m_batch_renderer.on_event(event);
          Dispatcher dispatcher(event);
          dispatcher.Dispatch<RefreshImage>(
            [this](const RefreshImage &refresh_image) -> bool
            {
                 spdlog::debug("Refresh Image:{}", refresh_image.changed());
                 if (refresh_image)
                 {
                      m_changed();
                 }
                 return false;
            });
          dispatcher.Dispatch<MouseButtonPressed>(
            [this](const MouseButtonPressed &pressed) -> bool
            {
                 if (!m_has_hover)
                 {
                      return true;
                 }
                 if (pressed.button() == Mouse::ButtonLeft)
                 {
                      m_map_dims.pressed_mouse_location = MouseToTilePos(
                        m_imgui_viewport_window.offset_mouse_pos(), m_map_dims);
                      if ((+pressed.mods() & (+Mods::Shift | +Mods::Alt)) == 0)
                      {
                           m_clicked_indexes.clear();
                      }
                      const MouseTileOverlap overlap
                        = MouseTileOverlap<TileFunctions, decltype(m_filters)>(
                          MouseToTilePos{
                            *(m_map_dims.pressed_mouse_location) },
                          m_filters);
                      if (pressed.mods() == Mods::Alt)
                      {
                           decltype(m_clicked_indexes) tmp{};
                           visit_unsorted_unfiltered_tiles(
                             [&](const auto &tile, VisitState &) -> bool
                             {
                                  tmp.push_back(
                                    static_cast<std::intmax_t>(
                                      GetMapHistory()->get_offset_from_working(
                                        tile)));
                                  return false;
                             },
                             overlap);
                           const auto removal = std::ranges::remove_if(
                             m_clicked_indexes,
                             [&](auto item) -> bool
                             {
                                  return std::ranges::any_of(
                                    tmp,
                                    [=](auto other) -> bool
                                    { return item == other; });
                             });
                           m_clicked_indexes.erase(
                             removal.begin(), removal.end());
                      }
                      else
                      {
                           visit_unsorted_unfiltered_tiles(
                             [this](const auto &tile, VisitState &) -> bool
                             {
                                  m_clicked_indexes.push_back(
                                    static_cast<std::intmax_t>(
                                      GetMapHistory()->get_offset_from_working(
                                        tile)));
                                  return false;
                             },
                             overlap);
                      }
                      {// sort and remove duplicates
                           std::ranges::sort(m_clicked_indexes);
                           const auto not_unique
                             = std::ranges::unique(m_clicked_indexes);
                           m_clicked_indexes.erase(
                             not_unique.begin(), not_unique.end());
                      }
                      spdlog::debug(
                        "Mouse Pressed: x:{}, y:{}, texture_page:{}, tile "
                        "count:{}",
                        m_map_dims.pressed_mouse_location->x,
                        m_map_dims.pressed_mouse_location->y,
                        m_map_dims.pressed_mouse_location->z,
                        m_clicked_indexes.size());

                      std::fill(
                        m_tile_button_state_pressed.begin(),
                        m_tile_button_state_pressed.end(),
                        false);
                      if (!m_clicked_indexes.empty())
                      {
                           m_dragging = true;
                           (void)GetMapHistory()->copy_back_preemptive();
                      }
                 }
                 return true;
            });

          dispatcher.Dispatch<MouseMoved>(
            [this](const MouseMoved &) -> bool
            {
                 if (
                   !m_dragging || !m_has_hover
                   || m_clicked_indexes.empty())// dragging
                                                // set by
                                                // another
                                                // event
                 {
                      return false;
                 }
                 glm::ivec3 temp = MouseToTilePos(
                   m_imgui_viewport_window.offset_mouse_pos(), m_map_dims);
                 const auto move_tiles = MoveTiles<TileFunctions>(
                   m_clicked_indexes,
                   (m_map_dims.dragging_mouse_location.has_value()
                      ? *m_map_dims.dragging_mouse_location
                      : *m_map_dims.pressed_mouse_location),
                   temp);
                 const bool has_moved               = move_tiles;
                 m_map_dims.dragging_mouse_location = temp;
                 if (!has_moved)
                 {
                      return true;
                 }
                 spdlog::debug(
                   "Mouse Dragging: x:{}, y:{}, texture_page:{}, hovered:{}, "
                   "dragging:{}, moved:{}",
                   m_map_dims.dragging_mouse_location->x,
                   m_map_dims.dragging_mouse_location->y,
                   m_map_dims.dragging_mouse_location->z,
                   m_has_hover,
                   m_dragging,
                   has_moved);
                 move_tiles();
                 return true;
            });

          dispatcher.Dispatch<MouseButtonReleased>(
            [this](const MouseButtonReleased &released) -> bool
            {
                 if (!m_dragging || !m_has_hover || m_clicked_indexes.empty())
                 {
                      return true;
                 }
                 if (released.button() == Mouse::ButtonLeft)
                 {
                      const auto unset_dragging = glengine::ScopeGuard(
                        [this]()
                        {
                             m_dragging                         = false;
                             m_map_dims.dragging_mouse_location = std::nullopt;
                             GetMapHistory()->end_preemptive_copy_mode();
                             // todo check to see if change occurred.
                        });
                      m_map_dims.released_mouse_location = MouseToTilePos(
                        m_imgui_viewport_window.offset_mouse_pos(), m_map_dims);
                      const auto move_tiles = MoveTiles<TileFunctions>(
                        m_clicked_indexes,
                        (m_map_dims.dragging_mouse_location.has_value()
                           ? *m_map_dims.dragging_mouse_location
                           : *m_map_dims.pressed_mouse_location),
                        *m_map_dims.released_mouse_location);
                      const bool moved = move_tiles;
                      spdlog::debug(
                        "Mouse Released: x:{}, y:{}, texture_page:{}, "
                        "hovered:{}, "
                        "dragging:{}, moved:{}",
                        m_map_dims.released_mouse_location->x,
                        m_map_dims.released_mouse_location->y,
                        m_map_dims.released_mouse_location->z,
                        m_has_hover,
                        m_dragging,
                        moved);
                      //          if (!moved)
                      //          {
                      //            return true;
                      //          }
                      // move_tiles(); //seems to move the tiles extra here for
                      // some reason.
                      (void)GetMapHistory()
                        ->remove_duplicate();// checks most recent for
                                             // duplicate
                 }
                 return true;
            });
     }

   private:
     // set uniforms
     void set_uniforms(const glengine::Shader &shader) const
     {
          if (m_offscreen_drawing || m_saving)
          {
               shader.set_uniform(
                 "u_MVP", m_fixed_render_camera.view_projection_matrix());
          }
          else if (m_preview)
          {
               shader.set_uniform(
                 "u_MVP",
                 m_imgui_viewport_window.preview_view_projection_matrix());
          }
          else
          {
               shader.set_uniform(
                 "u_MVP", m_imgui_viewport_window.view_projection_matrix());
          }
          shader.set_uniform("u_Grid", 0.F, 0.F);
          //    if (!s_draw_grid || m_offscreen_drawing || m_saving)
          //    {
          //      shader.set_uniform("u_Grid", 0.F, 0.F);
          //    }
          //    else
          //    {
          //      shader.set_uniform(
          //        "u_Grid", m_map_dims.scaled_tile_size());
          //    }
          shader.set_uniform("u_Tint", m_uniform_color);
     }
     std::optional<glengine::SubTexture>
       tile_to_sub_texture(const auto &tile) const
     {
          const auto &mim     = GetMim();
          const auto  bpp     = tile.depth();
          const auto  palette = tile.palette_id();
          const auto  texture_page_id
            = GetMapHistory()->get_original_version_of_working_tile(
              tile,
              [&](const auto &front_tile) { return front_tile.texture_id(); });
          const auto [texture_index, texture_page_width] = [&]()
          {
               if (std::ranges::empty(m_swizzle_path))
               {
                    return index_and_page_width(bpp, palette);
               }
               return index_and_page_width(palette, texture_page_id);
          }();
          const auto texture_page_offset
            = [&, texture_page_width_copy = texture_page_width]()
          {
               if (std::ranges::empty(m_swizzle_path))
               {
                    return texture_page_id * texture_page_width_copy;
               }
               return 0;
          }();
          const auto &texture
            = [&, texture_index = texture_index]() -> decltype(auto)
          {
               if (std::ranges::empty(m_swizzle_path))
               {
                    return mim.delayed_textures.textures->at(texture_index);
               }
               return m_swizzle_delayed_textures.textures->at(texture_index);
          }();
          if (texture.width() == 0 || texture.height() == 0)
               return std::nullopt;
          const auto texture_dims
            = glm::vec2{ texture.width(), texture.height() };
          const float tile_scale = static_cast<float>(texture.height())
                                   / static_cast<float>(GetMim()->get_height());
          const float tile_size = tile_scale * map_dims_statics::TileSize;
          // glm::vec2(m_mim.get_width(tile.depth()), m_mim.get_height());
          return GetMapHistory()->get_original_version_of_working_tile(
            tile,
            [&](const auto &front_tile)
            {
                 // todo maybe should have a toggle to force back tile.
                 return std::optional<glengine::SubTexture>{
                      std::in_place_t{}, texture,
                      glm::vec2{
                        front_tile.source_x() * tile_scale
                          + static_cast<float>(texture_page_offset),
                        texture_dims.y
                          - (front_tile.source_y() * tile_scale + tile_size) }
                        / texture_dims,
                      glm::vec2{
                        front_tile.source_x() * tile_scale
                          + static_cast<float>(texture_page_offset) + tile_size,
                        texture_dims.y - front_tile.source_y() * tile_scale }
                        / texture_dims
                 };
            });
     }
     glm::vec3 tile_to_draw_pos(const auto &tile) const
     {
          static constexpr typename TileFunctions::X           x{};
          static constexpr typename TileFunctions::Y           y{};
          static constexpr typename TileFunctions::TexturePage texture_page{};
          return { (static_cast<float>(
                      x(tile)
                      + texture_page(tile) * map_dims_statics::TexturePageWidth)
                    - m_map_dims.offset.x)
                     * m_map_dims.tile_scale,
                   (m_map_dims.offset.y - static_cast<float>(y(tile)))
                     * m_map_dims.tile_scale,
                   0.F };
     }
     auto visit_tiles(auto &&lambda) const
     {
          return GetMapHistory()->back().visit_tiles(
            [&](const auto &tiles)
            {
                 auto f_tiles
                   = tiles
                     | std::views::filter(TileOperations::NotInvalidTile{})
                     | std::views::filter(
                       []([[maybe_unused]] const auto &tile) -> bool
                       {
                            if constexpr (typename TileFunctions::UseBlending{})
                            {
                                 return tile.draw();
                            }
                            else
                            {
                                 return true;
                            }
                       })
                     | std::views::filter(m_filters);
                 std::vector<std::uint16_t> unique_z{};
                 {
                      // unique_z.reserve(std::ranges::size(tiles));
                      std::ranges::transform(
                        f_tiles,
                        std::back_inserter(unique_z),
                        TileOperations::Z{});
                      std::ranges::sort(unique_z);
                      auto [begin, end] = std::ranges::unique(unique_z);
                      unique_z.erase(begin, end);
                 }

                 auto unique_z_reverse = unique_z | std::views::reverse;
                 for (const auto z : unique_z_reverse)
                 {
                      auto f_tiles_reverse_filter_z
                        = f_tiles | std::views::reverse
                          | std::views::filter(TileOperations::Z::Match{ z });
                      for (const auto &tile : f_tiles_reverse_filter_z)
                      {
                           if (!lambda(tile))
                                return;
                      }
                 }
            });
     }
     // draws tiles
     void render_tiles() const
     {
          using open_viii::graphics::background::BlendModeT;
          BlendModeT last_blend_mode{ BlendModeT::none };
          m_uniform_color = s_default_color;
          glengine::BlendModeSettings::default_blend();
          m_imgui_viewport_window.on_render();
          m_batch_renderer.bind();
          set_uniforms(m_batch_renderer.shader());
          m_batch_renderer.clear();
          visit_tiles(
            [this, &last_blend_mode](const auto &tile) -> bool
            {
                 auto sub_texture = tile_to_sub_texture(tile);
                 if (!sub_texture)
                 {
                      return true;
                 }
                 update_blend_mode(tile, last_blend_mode);
                 m_batch_renderer.draw_quad(
                   *sub_texture,
                   tile_to_draw_pos(tile),
                   m_map_dims.scaled_tile_size(),
                   static_cast<int>(
                     GetMapHistory()->get_offset_from_working(tile)),
                   GetMapHistory()
                     ->get_pupu_from_working(tile)
                     .raw());// change to index to unique pupu
                 return true;
            });
          m_batch_renderer.draw();
          m_batch_renderer.on_render();
          glengine::BlendModeSettings::default_blend();
          m_uniform_color = s_default_color;
     }
     void update_blend_mode(
       [[maybe_unused]] const auto &tile,
       [[maybe_unused]] open_viii::graphics::background::BlendModeT
         &last_blend_mode) const
     {
          if constexpr (typename TileFunctions::UseBlending{})
          {
               if (!s_blending)
               {
                    return;
               }
               auto blend_mode = tile.blend_mode();
               if (blend_mode != last_blend_mode)
               {
                    m_batch_renderer.draw();// flush buffer.
                    last_blend_mode = blend_mode;
                    if (s_blends.percent_blend_enabled())
                    {
                         switch (blend_mode)
                         {
                              case open_viii::graphics::background::BlendModeT::
                                half_add:
                                   m_uniform_color = s_half_color;
                                   break;
                              case open_viii::graphics::background::BlendModeT::
                                quarter_add:
                                   m_uniform_color = s_quarter_color;
                                   break;
                              default:
                                   m_uniform_color = s_default_color;
                                   break;
                         }
                         m_batch_renderer.shader().set_uniform(
                           "u_Tint", m_uniform_color);
                    }
                    switch (blend_mode)
                    {
                         case open_viii::graphics::background::BlendModeT::
                           half_add:
                         case open_viii::graphics::background::BlendModeT::
                           quarter_add:
                         case open_viii::graphics::background::BlendModeT::add:
                         {
                              s_blends.set_add_blend();
                         }
                         break;
                         case open_viii::graphics::background::BlendModeT ::
                           subtract:
                         {
                              s_blends.set_subtract_blend();
                         }
                         break;
                         default:
                              glengine::BlendModeSettings::default_blend();
                    }
               }
          }
     }
     struct [[nodiscard]] IndexAndPageWidthReturn
     {
          std::size_t  texture_index = {};
          std::int16_t texture_page_width
            = { map_dims_statics::TexturePageWidth };
     };

     [[nodiscard]] static auto index_and_page_width(
       open_viii::graphics::BPPT bpp,
       std::uint8_t              palette)
     {
          IndexAndPageWidthReturn r = { .texture_index = palette };
          if (bpp.bpp8())
          {
               r.texture_index      = 16 + palette;
               r.texture_page_width = map_dims_statics::TexturePageWidth / 2;
          }
          else if (bpp.bpp16())
          {
               r.texture_index      = 16 * 2;
               r.texture_page_width = map_dims_statics::TexturePageWidth / 4;
          }
          return r;
     }
     [[maybe_unused]] [[nodiscard]] auto index_and_page_width(
       std::uint8_t palette,
       std::uint8_t texture_page) const
     {
          IndexAndPageWidthReturn r
            = { .texture_index
                = static_cast<size_t>(texture_page + 13U * (palette + 1U)) };
          if (!m_swizzle_delayed_textures.textures->at(r.texture_index))
          {
               // no palette with texture page combo was found. So attempt to
               // load texture page without palette.
               r.texture_index = texture_page;
          }
          return r;
     }

     void render_frame_buffer() const
     {
          glengine::BlendModeSettings::default_blend();
          m_imgui_viewport_window.on_render();
          const auto draw_batch_render =
            [this](
              const glengine::BatchRenderer &batch_renderer, uint32_t index = 0)
          {
               batch_renderer.clear();
               batch_renderer.bind();
               set_uniforms(batch_renderer.shader());
               batch_renderer.draw_quad(
                 m_frame_buffer.bind_color_attachment(index),
                 m_map_dims.scaled_position(),
                 glm::vec2(
                   m_frame_buffer.specification().width,
                   m_frame_buffer.specification().height));
               batch_renderer.draw();
               batch_renderer.on_render();
          };
          draw_batch_render(m_batch_renderer);
          draw_batch_render(m_batch_renderer_red_integer, 1);
     }

     void render_frame_buffer_grid() const
     {
          // todo remove unneeded code
          // todo maybe use transparent texture to render grid
          if (!s_draw_grid)
          {
               return;
          }
          const auto fbb = glengine::FrameBufferBackup{};
          const auto offscreen_pop
            = glengine::ScopeGuard([&]() { m_offscreen_drawing = false; });
          m_offscreen_drawing = true;
          glengine::BlendModeSettings::default_blend();
          m_imgui_viewport_window.on_render();
          m_batch_renderer.bind();
          set_uniforms(m_batch_renderer.shader());
          m_frame_buffer.bind(true);
          glengine::GlCall{}(
            glViewport,
            0,
            0,
            m_frame_buffer.specification().width,
            m_frame_buffer.specification().height);
          m_batch_renderer.shader().set_uniform(
            "u_Grid", m_map_dims.scaled_tile_size());
          m_batch_renderer.clear();
          m_batch_renderer.draw_quad(
            m_frame_buffer.bind_color_attachment(),
            m_map_dims.scaled_position(),
            glm::vec2(
              m_frame_buffer.specification().width,
              m_frame_buffer.specification().height));
          m_batch_renderer.draw();
          m_batch_renderer.on_render();
     }
     void save() const
     {
          m_saving = true;
          const auto not_saving
            = glengine::ScopeGuard([&]() { m_saving = false; });
          if (s_draw_grid)
          {
               m_changed();
          }
          const auto changed = glengine::ScopeGuard(
            [&]()
            {
                 if (s_draw_grid)
                 {
                      m_changed();
                 }
            });
          on_render();
          const auto path   = std::filesystem::path(GetMapHistory().path);
          auto       string = fmt::format(
            "{}_map.png", (path.parent_path() / path.stem()).string());
          glengine::PixelBuffer pixel_buffer{ m_frame_buffer.specification() };
          pixel_buffer.operator()(m_frame_buffer, string);
          while (pixel_buffer.operator()(&glengine::Texture::save))
               ;
     }
     auto visit_unsorted_unfiltered_tiles_count() const
     {
          return GetMapHistory()->back().visit_tiles(
            [&](const auto &tiles) -> std::size_t
            {
                 auto f_tiles
                   = tiles
                     | std::views::filter(TileOperations::NotInvalidTile{});
                 return static_cast<std::size_t>(std::ranges::count_if(
                   f_tiles, [](auto &&) { return true; }));
            });
     }
     bool visit_unsorted_unfiltered_tiles(
       auto &&lambda,
       auto &&filter) const
     {
          return GetMapHistory()->back().visit_tiles(
            [&](auto &&tiles) -> bool
            {
                 auto f_tiles
                   = tiles
                     | std::views::filter(TileOperations::NotInvalidTile{})
                     | std::views::filter(filter);
                 bool       changed     = false;
                 VisitState visit_state = {};
                 for (auto &tile : f_tiles)
                 {
                      changed = lambda(tile, visit_state) || changed;
                      if (visit_state == VisitState::ShortCircuit)
                      {
                           break;
                      }
                 }
                 if (visit_state == VisitState::Undo)
                 {
                      changed = GetMapHistory()->undo();
                      //        m_changed.enable_undo();
                 }
                 if (visit_state == VisitState::UndoAll)
                 {
                      changed = true;
                      GetMapHistory()->undo_all();
                 }
                 return changed;
            });
     }
     void visit_unsorted_unfiltered_tiles() const
     {
          // s_camera.RefreshAspectRatio(m_imgui_viewport_window.ViewPortAspectRatio());
          const glm::vec2 size = m_map_dims.scaled_size();
          m_imgui_viewport_window.set_image_bounds(size);
          m_fixed_render_camera.set_projection(size);
          m_frame_buffer
            = glengine::FrameBuffer(m_frame_buffer.specification().resize(
              static_cast<int>(abs(size.x)), static_cast<int>(abs(size.y))));
     }
     mutable glengine::OrthographicCamera m_fixed_render_camera = {};
     inline constinit static auto         s_fit_height          = bool{ true };
     inline constinit static auto         s_fit_width           = bool{ true };
     inline constinit static auto         s_draw_grid           = bool{ false };
     inline constinit static auto         s_blending            = bool{ true };

     static constexpr auto                s_default_color = glm::vec4{ 1.F };
     static constexpr auto                s_half_color = s_default_color / 2.F;
     static constexpr auto                s_quarter_color = s_half_color / 2.F;
     mutable glm::vec4                    m_uniform_color = s_default_color;

     std::filesystem::path                m_swizzle_path  = {};
     // dimensions of map
     MapDims<TileFunctions> m_map_dims = { GetMapHistory()->back() };
     // loads the textures overtime instead of forcing them to load at start.
     glengine::DelayedTextures<17U * 13U> m_swizzle_delayed_textures
       = {};// 20 is detected max 16(+1)*13 is
            // possible max. 0 being no palette and
            // 1-17 being with palettes
     // takes quads and draws them to the frame buffer or screen.
     glengine::BatchRenderer m_batch_renderer = { 1000 };
     glengine::BatchRenderer m_batch_renderer_red_integer
       = { 1,
           { std::filesystem::current_path() / "res" / "shader"
             / "red_integer.shader" } };
     // holds rendered image at 1:1 scale to prevent gaps when scaling.
     mutable glengine::FrameBuffer      m_frame_buffer              = {};
     mutable bool                       m_offscreen_drawing         = { false };
     mutable bool                       m_saving                    = { false };
     mutable bool                       m_preview                   = { false };
     inline constinit static MapBlends  s_blends                    = {};
     Changed                            m_changed                   = {};
     glengine::Counter                  m_id                        = {};
     mutable std::vector<bool>          m_tile_button_state         = {};
     mutable std::vector<bool>          m_tile_button_state_hover   = {};
     mutable std::vector<bool>          m_tile_button_state_pressed = {};
     mutable std::vector<std::intmax_t> m_clicked_indexes           = {};
     glengine::ImGuiViewPortWindow      m_imgui_viewport_window
       = { TileFunctions::label };
     mutable bool               m_has_hover = { false };
     mutable bool               m_dragging  = { false };
     mutable SimilarAdjustments m_similar   = {};
     static inline const auto   m_filters   = [](const auto &tile) -> bool
     {
          return GetMapHistory().filters(tile)
                 && GetMapHistory().filters(
                   GetMapHistory()->get_pupu_from_working(tile));
     };
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAP_HPP