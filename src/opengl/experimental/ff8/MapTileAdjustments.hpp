//
// Created by pcvii on 6/15/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPTILEADJUSTMENTS_HPP
#define FIELD_MAP_EDITOR_MAPTILEADJUSTMENTS_HPP
#include "MapDims.hpp"
#include "MapFilters.hpp"
#include "SimilarAdjustments.hpp"
#include "tile_operations.hpp"
#include "VisitState.hpp"
#include <glengine/GenericCombo.hpp>
#include <glengine/SubTexture.hpp>
#include <ImGuiDisabled.hpp>
#include <ImGuiPushItemWidth.hpp>
namespace ff_8
{
template<typename TileFunctions>
class MapTileAdjustments
{
     using DimsT = MapDims<TileFunctions>;

   public:
     MapTileAdjustments(
       const MapHistory   &map,
       MapFilters         &filters,
       SimilarAdjustments &similar_adjustments)
       : m_map_history(map)
       , m_filters(filters)
       , m_matching(similar_adjustments)
     {
     }
     template<typename TileT>
     [[nodiscard]] VisitState operator()(
       const TileT                               &tile,
       bool                                      &changed,
       std::size_t                                i,
       const std::optional<glengine::SubTexture> &sub_texture) const
     {
          VisitState visit_state = {};
          check_box_draw(tile, changed);
          ImGui::SameLine();
          {
               const auto disabled
                 = glengine::ImGuiDisabled(!m_map_history.undo_enabled());
               if (ImGui::Button("Undo"))
               {
                    visit_state = VisitState::Undo;
               }
               ImGui::SameLine();
               if (ImGui::Button("Undo All"))
               {
                    visit_state = VisitState::UndoAll;
               }
               const auto count = m_map_history.count();
               if (count != 0U)
               {
                    ImGui::SameLine();
                    ImGui::Text("%s", fmt::format("{} states.", count).c_str());
               }
          }
          int current_bpp_selection = combo_bpp(tile, changed);
          slider_int_2_source_xy(tile, changed, current_bpp_selection);
          slider_int_3_xyz(tile, changed);
          slider_int_layer_id(tile, changed);
          slider_int_blend_other(tile, changed);
          slider_int_palette_id(tile, changed);
          slider_int_texture_page_id(tile, changed);
          slider_int_2_animation(tile, changed);
          combo_blend_modes(tile, changed);
          inputs_read_only(
            tile,
            static_cast<int>(i),
            static_cast<int>(static_cast<uint32_t>(sub_texture->id())));
          return visit_state;
     }

   private:
     const MapHistory   &m_map_history;
     MapFilters         &m_filters;
     SimilarAdjustments &m_matching;
     static auto         generate_inner_width(
               int   components,
               float neg_width = {})
       -> std::pair<
         float,
         float>
     {
          components
            = std::clamp(components, 1, std::numeric_limits<int>::max());
          const float f_count = static_cast<float>(components);
          const auto &style   = ImGui::GetStyle();
          const float w_full  = ImGui::CalcItemWidth();
          const float w_item_one
            = (std::max)(1.0f,
                         std::floor(
                           (w_full - neg_width
                            - (style.ItemInnerSpacing.x)
                                * static_cast<float>(components - 1))
                           / f_count));
          const float w_item_last
            = (std::max)(1.0f,
                         std::floor(
                           w_full - neg_width
                           - (w_item_one + style.ItemInnerSpacing.x)
                               * static_cast<float>(components - 1)));
          return { w_item_one, w_item_last };
     }
     template<typename TileT>
     void check_box_draw(
       const TileT &tile,
       bool        &changed) const
     {
          using namespace tile_operations;
          bool draw = tile.draw();
          if (ImGui::Checkbox("draw?", &draw))
          {
               if constexpr (typename TileFunctions::UseBlending{})
               {
                    // this won't display change on swizzle because if we skip
                    // those tiles they won't output to the image file.
                    changed = true;
               }
               m_map_history.copy_working_perform_operation(
                 tile,
                 m_matching,
                 [&](TileT &new_tile)
                 {
                      new_tile
                        = new_tile.with_draw(static_cast<DrawT<TileT>>(draw));
                 });
          }
     }
     void inputs_read_only(
       const auto &tile,
       const int   index,
       const int   id) const
     {
          // ImGui doesn't seem to have const pointer versions of their
          // functions, ReadOnly flag is set on the functions. It's ment to
          // display the data in a way that can be copied into the clipboard.
          // That is why I'm using const_cast
          {
               const std::array<int, 2> tile_dims
                 = { static_cast<int>(tile.width()),
                     static_cast<int>(tile.height()) };
               ImGui::InputInt2(
                 "Tile Dimensions",
                 const_cast<int *>(tile_dims.data()),
                 ImGuiInputTextFlags_ReadOnly);
          }
          {
               const std::string index_str = fmt::format("{}", index);
               ImGui::InputText(
                 "Index",
                 const_cast<char *>(index_str.data()),
                 index_str.size(),
                 ImGuiInputTextFlags_ReadOnly);
          }
          {
               const std::string id_str = fmt::format("{}", id);
               ImGui::InputText(
                 "OpenGL Texture ID",
                 const_cast<char *>(id_str.data()),
                 id_str.size(),
                 ImGuiInputTextFlags_ReadOnly);
          }
          {
               const std::string hex = [&]() -> std::string
               {
                    std::stringstream ss = {};
                    tile.to_hex(ss);
                    return ss.str();
               }();
               ImGui::InputText(
                 "Raw Hex",
                 const_cast<char *>(hex.data()),
                 hex.size(),
                 ImGuiInputTextFlags_ReadOnly);
          }
     }
     template<typename TileT>
     int combo_bpp(
       const TileT &tile,
       bool        &changed) const
     {
          std::array<const char *, 3> bpp_options = { "4", "8", "16" };
          int                         current_bpp_selection = [&]() -> int
          {
               switch (static_cast<int>(tile.depth()))
               {
                    case 4:
                    default:
                         return 0;
                    case 8:
                         return 1;
                    case 16:
                         return 2;
               }
          }();
          const float checkbox_width = get_checkbox_width(1U);
          checkbox_tool_tip("##matching bpp", "Matching BPP", m_matching.depth);
          const float width = ImGui::CalcItemWidth();
          const auto  pop_width
            = glengine::ImGuiPushItemWidth(width - checkbox_width);
          if (ImGui::Combo(
                "BPP", &current_bpp_selection, bpp_options.data(), 3))
          {
               m_map_history.copy_working_perform_operation(
                 tile,
                 m_matching,
                 [&](TileT &new_tile)
                 {
                      using namespace open_viii::graphics::literals;
                      switch (current_bpp_selection)
                      {
                           case 0:
                           default:
                                new_tile = new_tile.with_depth(4_bpp);
                                break;
                           case 1:
                                new_tile = new_tile.with_depth(8_bpp);
                                break;
                           case 2:
                                new_tile = new_tile.with_depth(16_bpp);
                                break;
                      }
                 });
               m_filters.unique_tile_values().refresh_bpp(m_map_history);
               changed = true;
          }
          return current_bpp_selection;
     }
     void tool_tip(const char *tooltip) const
     {
          if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
          {
               ImGui::SetTooltip("%s", tooltip);
          }
     }

     bool checkbox_tool_tip(
       const char *label,
       const char *tooltip,
       bool       &toggle) const
     {
          bool ret = ImGui::Checkbox(label, &toggle);
          tool_tip(tooltip);
          ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
          return ret;
     }

     template<typename GroupT>
     bool generic_slider_int(
       GroupT            &group,
       float              width        = {},
       int                step         = 1,
       std::optional<int> override_max = std::nullopt) const
     {
          const auto pop_width = glengine::ImGuiPushItemWidth(width);
          const auto pop_id    = glengine::ImGuiPushId();
          if constexpr (!GroupT::read_only)
          {
               assert(step > 0);
               int current_int = static_cast<int>(group.current) / step;
               if (ImGui::SliderInt(
                     "##",
                     &current_int,
                     static_cast<int>(group.min_value) / step,
                     override_max ? *override_max
                                  : static_cast<int>(group.max_value) / step,
                     {},
                     ImGuiSliderFlags_AlwaysClamp))
               {
                    group.current = static_cast<typename GroupT::value_type>(
                      current_int * step);
                    return true;
               }
          }
          else
          {
               (void)ImGui::InputText(
                 fmt::format("{}", group.current),
                 ImGuiInputTextFlags_ReadOnly);
          }
          return false;
     }
     template<typename TileT>
     void slider_int_2_source_xy(
       const TileT &tile,
       bool        &changed,
       const int    current_bpp_selection) const
     {
          using namespace tile_operations;
          const float                   checkbox_width = get_checkbox_width(3U);
          const std::pair<float, float> item_width
            = generate_inner_width(2, checkbox_width);
          checkbox_tool_tip(
            "##matching source_xy",
            "Matching Source X and Source Y",
            m_matching.source_xy);
          checkbox_tool_tip(
            "##matching source_x", "Matching Source X", m_matching.source_x);
          auto x = SourceXGroup(tile);
          if (generic_slider_int(
                x,
                item_width.first,
                tile.width(),
                (static_cast<int>(
                  std::pow(2, (2 - current_bpp_selection) + 2) - 1))))
          {
               changed       = true;
               const auto op = [&](TileT &new_tile)
               { new_tile = new_tile.with_source_x(x.current); };
               m_map_history.copy_working_perform_operation(
                 tile, m_matching, op);
          }
          ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
          checkbox_tool_tip(
            "##matching source_y", "Matching Source Y", m_matching.source_y);
          auto y = SourceYGroup(tile);
          if (generic_slider_int(y, item_width.second, tile.height()))
          {
               changed       = true;
               const auto op = [&](TileT &new_tile)
               { new_tile = new_tile.with_source_y(y.current); };
               m_map_history.copy_working_perform_operation(
                 tile, m_matching, op);
          }
          ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
          ImGui::Text(
            "%s",
            fmt::format("Source Pos: ({}, {})", x.current, y.current).c_str());
          // todo add second source for moving a tile to a new location in
          // the mim / swizzled map. Without changing the image.
     }
     float get_checkbox_width(const uint8_t count) const
     {
          return (ImGui::GetStyle().ItemInnerSpacing.x
                  + ImGui::GetFrameHeight())
                 * static_cast<float>(count);
     }
     template<typename TileT>
     void slider_int_3_xyz(
       const TileT &tile,
       bool        &changed) const
     {
          using namespace tile_operations;
          const float                   checkbox_width = get_checkbox_width(5U);
          const std::pair<float, float> item_width
            = generate_inner_width(3, checkbox_width);
          checkbox_tool_tip(
            "##matching xyz",
            "Matching Destination X, Destination Y and Destination Z",
            m_matching.xyz);
          checkbox_tool_tip(
            "##matching xy",
            "Matching Destination X and Destination Y",
            m_matching.xy);
          checkbox_tool_tip(
            "##matching x", "Matching Destination X", m_matching.x);
          auto x = XGroup{ tile };
          if (generic_slider_int(x, item_width.first, tile.width()))
          {
               changed = true;
               m_map_history.copy_working_perform_operation(
                 tile,
                 m_matching,
                 [&](TileT &new_tile)
                 { new_tile = new_tile.with_x(x.current); });
          }
          ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
          checkbox_tool_tip(
            "##matching y", "Matching Destination Y", m_matching.y);
          auto y = YGroup{ tile };
          if (generic_slider_int(y, item_width.first, tile.width()))
          {
               changed = true;
               m_map_history.copy_working_perform_operation(
                 tile,
                 m_matching,
                 [&](TileT &new_tile)
                 { new_tile = new_tile.with_y(y.current); });
          }
          ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
          checkbox_tool_tip(
            "##matching z", "Matching Destination Z", m_matching.z);
          auto z = ZGroup{ tile };
          if (generic_slider_int(z, item_width.second, tile.width()))
          {
               changed = true;
               m_map_history.copy_working_perform_operation(
                 tile,
                 m_matching,
                 [&](TileT &new_tile)
                 { new_tile = new_tile.with_z(z.current); });
               m_filters.unique_tile_values().refresh_z(m_map_history);
          }
          ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
          ImGui::Text(
            "%s",
            fmt::format(
              "Destination Pos: ({}, {}, {})", x.current, y.current, z.current)
              .c_str());
     }
     template<typename TileT>
     void combo_blend_modes(
       const TileT &tile,
       bool        &changed) const
     {
          using namespace tile_operations;
          const auto disabled
            = glengine::ImGuiDisabled(!has_with_blend_mode<TileT>);
          const auto blend_mode            = tile.blend_mode();
          int current_blend_mode_selection = static_cast<int>(blend_mode);
          const std::array<std::string_view, 5> blend_mode_str = {
               "half_add", "add", "subtract", "quarter_add", "none",
          };
          const float checkbox_width = get_checkbox_width(1U);
          checkbox_tool_tip(
            "##matching blend mode",
            "Matching Blend Mode",
            m_matching.blend_mode);
          if (glengine::GenericCombo(
                "Blend Mode",
                current_blend_mode_selection,
                blend_mode_str,
                checkbox_width))
          {
               if constexpr (has_with_blend_mode<TileT>)
               {
                    changed = true;
                    m_map_history.copy_working_perform_operation(
                      tile,
                      m_matching,
                      [&](TileT &new_tile)
                      {
                           new_tile = new_tile.with_blend_mode(
                             static_cast<BlendModeT<TileT>>(
                               current_blend_mode_selection));
                      });
                    m_filters.unique_tile_values().refresh_blend_mode(
                      m_map_history);
               }
          }
     }
     template<typename TileT>
     void slider_int_layer_id(
       const TileT &tile,
       bool        &changed) const
     {
          using namespace tile_operations;
          using namespace open_viii::graphics::background;
          int        layer_id = tile.layer_id();
          const auto disabled
            = glengine::ImGuiDisabled(!has_with_layer_id<TileT>);
          const float checkbox_width = get_checkbox_width(1U);
          const float width          = ImGui::CalcItemWidth();
          const auto  pop_width
            = glengine::ImGuiPushItemWidth(width - checkbox_width);
          checkbox_tool_tip(
            "##matching layer_id", "Matching Layer ID", m_matching.layer_id);
          if (ImGui::SliderInt(
                "Layer ID",
                &layer_id,
                LayerIdT<TileT>{},
                LayerIdT<TileT>{ 0b0111'1111U },
                {},
                ImGuiSliderFlags_AlwaysClamp))
          {
               if constexpr (has_with_layer_id<TileT>)
               {
                    changed = true;
                    m_map_history.copy_working_perform_operation(
                      tile,
                      m_matching,
                      [&](TileT &new_tile)
                      {
                           new_tile = new_tile.with_layer_id(
                             static_cast<LayerIdT<TileT>>(layer_id));
                      });
                    m_filters.unique_tile_values().refresh_layer_id(
                      m_map_history);
               }
          }
     }
     template<typename TileT>
     void slider_int_texture_page_id(
       const TileT &tile,
       bool        &changed) const
     {
          using namespace tile_operations;
          int         texture_page_id = static_cast<int>(tile.texture_id());
          const float checkbox_width  = get_checkbox_width(1U);
          const float width           = ImGui::CalcItemWidth();
          const auto  pop_width
            = glengine::ImGuiPushItemWidth(width - checkbox_width);
          checkbox_tool_tip(
            "##matching texture_page_id",
            "Matching Texture Page ID",
            m_matching.texture_id);
          if (ImGui::SliderInt(
                "Texture Page ID",
                &texture_page_id,
                TextureIdT<TileT>{},
                TextureIdT<TileT>{
                  12U },// 15 is max val but 12 or 11 is max used
                {},
                ImGuiSliderFlags_AlwaysClamp))
          {
               changed = true;
               m_map_history.copy_working_perform_operation(
                 tile,
                 m_matching,
                 [&](TileT &new_tile)
                 {
                      new_tile = new_tile.with_texture_id(
                        static_cast<TextureIdT<TileT>>(texture_page_id));
                 });
               m_filters.unique_tile_values().refresh_texture_page_id(
                 m_map_history);
          }
     }
     template<typename TileT>
     void slider_int_palette_id(
       const TileT &tile,
       bool        &changed) const
     {
          using namespace tile_operations;
          const float checkbox_width = get_checkbox_width(1U);
          const float width          = ImGui::CalcItemWidth();
          const auto  pop_width
            = glengine::ImGuiPushItemWidth(width - checkbox_width);
          checkbox_tool_tip(
            "##matching palette_id",
            "Matching Palette ID",
            m_matching.palette_id);
          int palette_id = static_cast<int>(tile.palette_id());
          if (ImGui::SliderInt(
                "Palette ID",
                &palette_id,
                PaletteIdT<TileT>{},
                PaletteIdT<TileT>{ 0b1111U },
                {},
                ImGuiSliderFlags_AlwaysClamp))

          {
               changed = true;
               m_map_history.copy_working_perform_operation(
                 tile,
                 m_matching,
                 [&](TileT &new_tile)
                 {
                      new_tile = new_tile.with_palette_id(
                        static_cast<PaletteIdT<TileT>>(palette_id));
                 });
               m_filters.unique_tile_values().refresh_palette_id(m_map_history);
          }
     }
     template<typename TileT>
     void slider_int_blend_other(
       const TileT &tile,
       bool        &changed) const
     {
          using namespace tile_operations;
          int         blend          = tile.blend();
          const float checkbox_width = get_checkbox_width(1U);
          const float width          = ImGui::CalcItemWidth();
          checkbox_tool_tip(
            "##matching blend other", "Matching Blend Other", m_matching.blend);
          const auto pop_width
            = glengine::ImGuiPushItemWidth(width - checkbox_width);
          if (ImGui::SliderInt(
                "Blend Other",
                &blend,
                BlendT<TileT>{},
                BlendT<TileT>{ 0b0011U },
                {},
                ImGuiSliderFlags_AlwaysClamp))
          {
               changed = true;
               m_map_history.copy_working_perform_operation(
                 tile,
                 m_matching,
                 [&](TileT &new_tile)
                 {
                      new_tile = new_tile.with_blend(
                        static_cast<BlendT<TileT>>(blend));
                 });
               m_filters.unique_tile_values().refresh_blend_other(
                 m_map_history);
          }
     }
     template<typename TileT>
     void slider_int_2_animation(
       const TileT &tile,
       bool        &changed) const
     {
          using namespace tile_operations;
          using namespace open_viii::graphics::background;
          int         animation_id    = tile.animation_id();
          int         animation_state = tile.animation_state();
          const float checkbox_width  = get_checkbox_width(2U);
          checkbox_tool_tip(
            "##matching animation_id",
            "Matching Animation ID",
            m_matching.animation_id);
          const std::pair<float, float> item_width
            = generate_inner_width(2, checkbox_width);
          {
               const auto disabled
                 = glengine::ImGuiDisabled(!has_with_animation_id<TileT>);
               const auto pop_width
                 = glengine::ImGuiPushItemWidth(item_width.first);
               if (ImGui::SliderInt(
                     "##Animation ID",
                     &animation_id,
                     std::numeric_limits<AnimationIdT<TileT>>::min(),
                     std::numeric_limits<AnimationIdT<TileT>>::max(),
                     {},
                     ImGuiSliderFlags_AlwaysClamp))
               {
                    if constexpr (has_with_animation_id<TileT>)
                    {
                         changed = true;
                         m_map_history.copy_working_perform_operation(
                           tile,
                           m_matching,
                           [&](TileT &new_tile)
                           {
                                new_tile = new_tile.with_animation_id(
                                  static_cast<AnimationIdT<TileT>>(
                                    animation_id));
                           });
                         m_filters.unique_tile_values().refresh_animation_id(
                           m_map_history);
                    }
               }
          }
          ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
          checkbox_tool_tip(
            "##matching animation_state",
            "Matching Animation State",
            m_matching.animation_state);
          {
               const auto disabled
                 = glengine::ImGuiDisabled(!has_with_animation_state<TileT>);
               const auto pop_width
                 = glengine::ImGuiPushItemWidth(item_width.second);
               if (ImGui::SliderInt(
                     "##Animation State",
                     &animation_state,
                     std::numeric_limits<AnimationStateT<TileT>>::min(),
                     std::numeric_limits<AnimationStateT<TileT>>::max(),
                     {},
                     ImGuiSliderFlags_AlwaysClamp))
               {
                    if constexpr (has_with_animation_state<TileT>)
                    {
                         changed = true;
                         m_map_history.copy_working_perform_operation(
                           tile,
                           m_matching,
                           [&](TileT &new_tile)
                           {
                                new_tile = new_tile.with_animation_state(
                                  static_cast<AnimationStateT<TileT>>(
                                    animation_state));
                           });
                         m_filters.unique_tile_values().refresh_animation_frame(
                           m_map_history);
                    }
               }
          }
          ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
          ImGui::Text(
            "%s",
            fmt::format("Animation: ({}, {})", animation_id, animation_state)
              .c_str());
     }
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAPTILEADJUSTMENTS_HPP
