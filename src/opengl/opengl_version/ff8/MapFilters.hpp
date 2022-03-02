//
// Created by pcvii on 3/2/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPFILTERS_HPP
#define FIELD_MAP_EDITOR_MAPFILTERS_HPP
#include "UniqueTileValues.hpp"
class MapFilters
{

public:
  MapFilters() = default;
  MapFilters(const open_viii::graphics::background::Map &map)
    : m_unique_tile_values(map)
    , m_disabled(map.visit_tiles(
        [](auto &&tiles) { return std::ranges::empty(tiles); }))
  {
  }
  [[nodiscard]] bool OnImGuiUpdate() const
  {
    bool       changed        = false;
    const auto filter_disable = glengine::ImGuiDisabled(m_disabled);
    if (ImGui::CollapsingHeader("Filters"))
    {
      const auto        un_indent0 = glengine::ImGuiIndent();
      static const auto common =
        [&changed](
          const char                             *label,
          std::ranges::random_access_range auto  &bool_range,
          std::ranges::random_access_range auto &&possible_value_range,
          std::ranges::random_access_range auto &&possible_value_string_range,
          std::ranges::random_access_range auto &&used_value_range,
          std::uint32_t                           line_count) {
          assert(
            std::ranges::size(possible_value_range)
            == std::ranges::size(possible_value_string_range));
          assert(
            std::ranges::size(bool_range)
            == std::ranges::size(possible_value_range));
          assert(
            std::ranges::size(possible_value_range)
            >= std::ranges::size(used_value_range));
          if (ImGui::CollapsingHeader(label))
          {
            const auto un_indent1 = glengine::ImGuiIndent();

            auto       boolptr    = std::ranges::begin(bool_range);
            const auto boolsent   = std::ranges::end(bool_range);
            auto current_value    = std::ranges::cbegin(possible_value_range);
            auto current_string =
              std::ranges::cbegin(possible_value_string_range);

            for (std::uint32_t i = 0; boolptr != boolsent; ++i,
                               (void)++boolptr,
                               (void)++current_value,
                               (void)++current_string)
            {
              auto found = std::ranges::find(used_value_range, *current_value);

              const auto disabled = glengine::ImGuiDisabled(
                found == std::ranges::end(used_value_range));

              const auto pop       = glengine::ImGuiPushID();
              const auto string    = fmt::format("{:>4}", *current_string);
              auto       size      = ImGui::CalcTextSize(string.c_str());
              bool       same_line = line_count > 0 && i % line_count != 0;
              if (same_line)
                ImGui::SameLine();
              ImGui::Dummy(ImVec2(2.F, 2.F));
              if (same_line)
                ImGui::SameLine();
              if (ImGui::Selectable(
                    string.c_str(), static_cast<bool>(*boolptr), 0, size))
              {
                *boolptr = !static_cast<bool>(*boolptr);
                changed  = true;
              }
            }
            ImGui::Dummy(ImVec2(2.F, 2.F));
            {
              const auto pop = glengine::ImGuiPushID();
              if (ImGui::Button("All"))
              {
                std::ranges::fill(bool_range, true);
                changed = true;
              }
            }
            ImGui::SameLine();
            ImGui::Dummy(ImVec2(2.F, 2.F));
            ImGui::SameLine();
            {
              const auto pop = glengine::ImGuiPushID();
              if (ImGui::Button("None"))
              {
                std::ranges::fill(bool_range, false);
                changed = true;
              }
            }
            ImGui::Dummy(ImVec2(2.F, 2.F));
          }
        };
      static constexpr auto common_unique =
        [](const char *label, auto &&unique, std::uint32_t row_size) {
          common(
            label,
            unique.enable(),
            unique.values(),
            unique.strings(),
            unique.values(),
            row_size);
        };

      common(
        "BPP",
        m_possible_tile_values.bpp.enable(),
        m_possible_tile_values.bpp.values(),
        m_possible_tile_values.bpp.strings(),
        m_unique_tile_values.bpp.values(),
        4);

      common(
        "Palettes",
        m_possible_tile_values.palette_id.enable(),
        m_possible_tile_values.palette_id.values(),
        m_possible_tile_values.palette_id.strings(),
        m_unique_tile_values.palette_id.values(),
        8);

      common(
        "Blend Mode",
        m_possible_tile_values.blend_mode.enable(),
        m_possible_tile_values.blend_mode.values(),
        m_possible_tile_values.blend_mode.strings(),
        m_unique_tile_values.blend_mode.values(),
        3);

      common_unique("Blend Other", m_unique_tile_values.blend_other, 8);

      common_unique("Z", m_unique_tile_values.z, 4);

      common_unique("Layer ID", m_unique_tile_values.layer_id, 8);

      common_unique("Texture Page ID", m_unique_tile_values.texture_page_id, 8);

      common_unique("Animation ID", m_unique_tile_values.animation_id, 8);

      common_unique("Animation Frame", m_unique_tile_values.animation_frame, 8);
    }
    return changed;
  }
  auto TestTile() const noexcept
  {
    return [&](const auto &tile) -> bool {
      return filter(
               tile.layer_id(),
               m_unique_tile_values.layer_id.enable(),
               m_unique_tile_values.layer_id.values())
             && filter(
               tile.z(),
               m_unique_tile_values.z.enable(),
               m_unique_tile_values.z.values())
             && filter(
               tile.texture_id(),
               m_unique_tile_values.texture_page_id.enable(),
               m_unique_tile_values.texture_page_id.values())
             && filter(
               tile.blend(),
               m_unique_tile_values.blend_other.enable(),
               m_unique_tile_values.blend_other.values())
             && filter(
               tile.animation_id(),
               m_unique_tile_values.animation_id.enable(),
               m_unique_tile_values.animation_id.values())
             && filter(
               tile.animation_state(),
               m_unique_tile_values.animation_frame.enable(),
               m_unique_tile_values.animation_frame.values())
             && filter(
               tile.layer_id(),
               m_unique_tile_values.layer_id.enable(),
               m_unique_tile_values.layer_id.values())
             && filter(
               tile.blend_mode(),
               m_possible_tile_values.blend_mode.enable(),
               m_possible_tile_values.blend_mode.values())
             && filter(
               tile.depth(),
               m_possible_tile_values.bpp.enable(),
               m_possible_tile_values.bpp.values())
             && filter(
               tile.palette_id(),
               m_possible_tile_values.palette_id.enable(),
               m_possible_tile_values.palette_id.values());
    };
  }

private:
  bool filter(
    auto                    &&value,
    std::ranges::range auto &&bool_range,
    std::ranges::range auto &&value_range)
    const noexcept requires std::equality_comparable_with<
      decltype(value),
      std::ranges::range_value_t<decltype(value_range)>>
  {
    assert(std::ranges::size(bool_range) == std::ranges::size(value_range));
    auto       current_value  = std::ranges::cbegin(value_range);
    const auto value_sentinal = std::ranges::cend(value_range);
    auto       current_bool   = std::ranges::cbegin(bool_range);
    for (; current_value != value_sentinal;
         (void)++current_value, ++current_bool)
    {
      if (*current_value == value)
      {
        if (static_cast<bool>(*current_bool))
        {
          return true;
        }
        break;
      }
    }
    return false;
  }
  UniqueTileValues   m_unique_tile_values   = {};
  TilePossibleValues m_possible_tile_values = {};
  bool               m_disabled             = true;
};
#endif// FIELD_MAP_EDITOR_MAPFILTERS_HPP
