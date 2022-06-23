//
// Created by pcvii on 6/15/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPTILEADJUSTMENTS_HPP
#define FIELD_MAP_EDITOR_MAPTILEADJUSTMENTS_HPP
#include "MapDims.hpp"
#include "MapFilters.hpp"
#include "tile_operations.hpp"
#include <GenericCombo.hpp>
#include <ImGuiDisabled.hpp>
#include <ImGuiPushItemWidth.hpp>
#include <SubTexture.hpp>
namespace ff_8
{
template<typename TileFunctions>
class MapTileAdjustments
{
  using MapT  = open_viii::graphics::background::Map;
  using DimsT = MapDims<TileFunctions>;

public:
  MapTileAdjustments(MapT &map, MapFilters &filters, const DimsT &dims)
    : m_map(map)
    , m_filters(filters)
    , m_map_dims(dims)
  {
  }
  template<typename TileT>
  void operator()(
    TileT                                     &tile,
    bool                                      &changed,
    std::size_t                                i,
    const std::optional<glengine::SubTexture> &sub_texture) const
  {
    check_box_draw(tile, changed);
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
  }

private:
  MapT        &m_map;
  MapFilters  &m_filters;
  const DimsT &m_map_dims;
  static auto  generate_inner_width(int components) -> std::pair<float, float>
  {
    components = std::clamp(components, 1, std::numeric_limits<int>::max());
    const float f_count    = static_cast<float>(components);
    const auto &style      = ImGui::GetStyle();
    const float w_full     = ImGui::CalcItemWidth();
    const float w_item_one = (std::max)(
      1.0f,
      std::floor(
        (w_full
         - (style.ItemInnerSpacing.x) * static_cast<float>(components - 1))
        / f_count));
    const float w_item_last = (std::max)(
      1.0f,
      std::floor(
        w_full
        - (w_item_one + style.ItemInnerSpacing.x)
            * static_cast<float>(components - 1)));
    return { w_item_one, w_item_last };
  }
  template<typename TileT>
  void check_box_draw(TileT &tile, bool &changed) const
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
      tile = tile.with_draw(static_cast<DrawT<TileT>>(draw));
    }
  }
  void inputs_read_only(const auto &tile, const int index, const int id) const
  {
    // ImGui doesn't seem to have const pointer versions of their functions,
    // ReadOnly flag is set on the functions. It's ment to display the data in a
    // way that can be copied into the clipboard. That is why I'm using
    // const_cast
    {
      const std::array<int, 2> tile_dims = { static_cast<int>(tile.width()),
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
      const std::string hex = [&]() -> std::string {
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
  int combo_bpp(auto &tile, bool &changed) const
  {
    std::array<const char *, 3> bpp_options           = { "4", "8", "16" };
    int                         current_bpp_selection = [&]() -> int {
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
    if (ImGui::Combo("BPP", &current_bpp_selection, bpp_options.data(), 3))
    {
      using namespace open_viii::graphics::literals;
      switch (current_bpp_selection)
      {
        case 0:
        default:
          tile = tile.with_depth(4_bpp);
          break;
        case 1:
          tile = tile.with_depth(8_bpp);
          break;
        case 2:
          tile = tile.with_depth(16_bpp);
          break;
      }
      m_filters.unique_tile_values().refresh_bpp(m_map);
      changed = true;
    }
    return current_bpp_selection;
  }
  template<typename TileT>
  void slider_int_2_source_xy(
    TileT    &tile,
    bool     &changed,
    const int current_bpp_selection) const
  {
    using namespace tile_operations;
    std::array<int, 2> source_xy = {
      static_cast<int>(tile.source_x() / tile.width()),
      static_cast<int>(tile.source_y() / tile.height())
    };
    const std::pair<float, float> item_width = generate_inner_width(2);
    {
      const auto pop_width = glengine::ImGuiPushItemWidth(item_width.first);
      if (ImGui::SliderInt(
            "##Source (X)",
            &source_xy[0],
            0,
            (static_cast<int>(
              std::pow(2, (2 - current_bpp_selection) + 2) - 1))))
      {
        source_xy[0] *= tile.width();
        changed = true;
        tile = tile.with_source_x(static_cast<SourceXT<TileT>>(source_xy[0]));
      }
    }
    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
    {
      const auto pop_width = glengine::ImGuiPushItemWidth(item_width.second);
      if (ImGui::SliderInt(
            "##Source (Y)",
            &source_xy[1],
            std::numeric_limits<SourceYT<TileT>>::min() / tile.height(),
            std::numeric_limits<SourceYT<TileT>>::max() / tile.height()))
      {
        changed = true;
        source_xy[1] *= tile.height();
        tile = tile.with_source_y(static_cast<SourceYT<TileT>>(source_xy[1]));
      }
    }
    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text(
      "%s",
      fmt::format("Source Pos: ({}, {})", tile.source_x(), tile.source_y())
        .c_str());
    // todo add second source for moving a tile to a new location in
    // the mim / swizzled map. Without changing the image.
  }
  template<typename TileT>
  void slider_int_3_xyz(TileT &tile, bool &changed) const
  {
    using namespace tile_operations;
    std::array<int, 3> xyz = { static_cast<int>(tile.x() / tile.width()),
                               static_cast<int>(tile.y() / tile.height()),
                               static_cast<int>(tile.z()) };

    const std::pair<float, float> item_width = generate_inner_width(3);
    {
      const auto pop_width = glengine::ImGuiPushItemWidth(item_width.first);
      if (ImGui::SliderInt(
            "##Destination (X)",
            &xyz[0],
            static_cast<int>(m_map_dims.true_min.x / tile.width()),
            static_cast<int>(m_map_dims.true_max.x / tile.width())))
      {
        changed = true;
        xyz[0] *= tile.width();
        tile = tile.with_x(static_cast<XT<TileT>>(xyz[0]));
      }
    }
    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
    {
      const auto pop_width = glengine::ImGuiPushItemWidth(item_width.first);
      if (ImGui::SliderInt(
            "##Destination (Y)",
            &xyz[1],
            static_cast<int>(m_map_dims.true_min.y / tile.height()),
            static_cast<int>(m_map_dims.true_max.y / tile.height())))
      {
        changed = true;
        xyz[1] *= tile.height();
        tile = tile.with_y(static_cast<YT<TileT>>(xyz[1]));
      }
    }
    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
    {
      const auto pop_width = glengine::ImGuiPushItemWidth(item_width.second);
      if (ImGui::SliderInt("##Destination (Z)", &xyz[2], 0, 0xFFF))
      {
        changed = true;
        tile    = tile.with_z(static_cast<ZT<TileT>>(xyz[2]));
        m_filters.unique_tile_values().refresh_z(m_map);
      }
    }
    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text(
      "%s",
      fmt::format("Destination Pos: ({}, {}, {})", tile.x(), tile.y(), tile.z())
        .c_str());
  }
  template<typename TileT>
  void combo_blend_modes(TileT &tile, bool &changed) const
  {
    using namespace tile_operations;
    const auto disabled = glengine::ImGuiDisabled(!has_with_blend_mode<TileT>);
    const auto blend_mode                   = tile.blend_mode();
    int        current_blend_mode_selection = static_cast<int>(blend_mode);
    const std::array<std::string_view, 5> blend_mode_str = {
      "half_add", "add", "subtract", "quarter_add", "none",
    };
    if (glengine::GenericCombo(
          "Blend Mode", current_blend_mode_selection, blend_mode_str))
    {
      if constexpr (has_with_blend_mode<TileT>)
      {
        changed = true;
        tile    = tile.with_blend_mode(
          static_cast<BlendModeT<TileT>>(current_blend_mode_selection));
        m_filters.unique_tile_values().refresh_blend_mode(m_map);
      }
    }
  }
  template<typename TileT>
  void slider_int_layer_id(TileT &tile, bool &changed) const
  {
    using namespace tile_operations;
    using namespace open_viii::graphics::background;
    int        layer_id = tile.layer_id();
    const auto disabled = glengine::ImGuiDisabled(!has_with_layer_id<TileT>);
    if (ImGui::SliderInt(
          "Layer ID",
          &layer_id,
          std::numeric_limits<LayerIdT<TileT>>::min(),
          std::numeric_limits<LayerIdT<TileT>>::max()))
    {
      if constexpr (has_with_layer_id<TileT>)
      {
        changed = true;
        tile    = tile.with_layer_id(static_cast<LayerIdT<TileT>>(layer_id));
        m_filters.unique_tile_values().refresh_layer_id(m_map);
      }
    }
  }
  template<typename TileT>
  void slider_int_texture_page_id(TileT &tile, bool &changed) const
  {
    using namespace tile_operations;
    int texture_page_id = static_cast<int>(tile.texture_id());
    if (ImGui::SliderInt("Texture Page ID", &texture_page_id, 0, 13))
    {
      changed = true;
      tile =
        tile.with_texture_id(static_cast<TextureIdT<TileT>>(texture_page_id));
      m_filters.unique_tile_values().refresh_texture_page_id(m_map);
    }
  }
  template<typename TileT>
  void slider_int_palette_id(TileT &tile, bool &changed) const
  {
    using namespace tile_operations;
    int palette_id = static_cast<int>(tile.palette_id());
    if (ImGui::SliderInt("Palette ID", &palette_id, 0, 16))
    {
      changed = true;
      tile = tile.with_palette_id(static_cast<PaletteIdT<TileT>>(palette_id));
      m_filters.unique_tile_values().refresh_palette_id(m_map);
    }
  }
  template<typename TileT>
  void slider_int_blend_other(TileT &tile, bool &changed) const
  {
    using namespace tile_operations;
    int blend = tile.blend();
    if (ImGui::SliderInt(
          "Blend Other",
          &blend,
          std::numeric_limits<BlendT<TileT>>::min(),
          std::numeric_limits<BlendT<TileT>>::max()))
    {
      changed = true;
      tile    = tile.with_blend(static_cast<BlendT<TileT>>(blend));
      m_filters.unique_tile_values().refresh_blend_other(m_map);
    }
  }
  template<typename TileT>
  void slider_int_2_animation(TileT &tile, bool &changed) const
  {
    using namespace tile_operations;
    using namespace open_viii::graphics::background;
    int                           animation_id    = tile.animation_id();
    int                           animation_state = tile.animation_state();
    const std::pair<float, float> item_width      = generate_inner_width(2);
    {
      const auto disabled =
        glengine::ImGuiDisabled(!has_with_animation_id<TileT>);
      const auto pop_width = glengine::ImGuiPushItemWidth(item_width.first);
      if (ImGui::SliderInt(
            "##Animation ID",
            &animation_id,
            std::numeric_limits<AnimationIdT<TileT>>::min(),
            std::numeric_limits<AnimationIdT<TileT>>::max()))
      {
        if constexpr (has_with_animation_id<TileT>)
        {
          changed = true;
          tile    = tile.with_animation_id(
            static_cast<AnimationIdT<TileT>>(animation_id));
          m_filters.unique_tile_values().refresh_animation_id(m_map);
        }
      }
    }
    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
    {
      const auto disabled =
        glengine::ImGuiDisabled(!has_with_animation_state<TileT>);
      const auto pop_width = glengine::ImGuiPushItemWidth(item_width.second);
      if (ImGui::SliderInt(
            "##Animation State",
            &animation_state,
            std::numeric_limits<AnimationStateT<TileT>>::min(),
            std::numeric_limits<AnimationStateT<TileT>>::max()))
      {
        if constexpr (has_with_animation_state<TileT>)
        {
          changed = true;
          tile    = tile.with_animation_state(
            static_cast<AnimationStateT<TileT>>(animation_state));
          m_filters.unique_tile_values().refresh_animation_frame(m_map);
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
