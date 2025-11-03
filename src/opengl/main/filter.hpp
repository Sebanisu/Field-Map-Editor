//
// Created by pcvii on 10/4/2021.
//

#ifndef FIELD_MAP_EDITOR_FILTER_HPP
#define FIELD_MAP_EDITOR_FILTER_HPP
#include "Configuration.hpp"
#include "draw_bit_t.hpp"
#include "formatters.hpp"
#include "gui/colors.hpp"
#include "gui/compact_type.hpp"
#include "open_viii/graphics/background/BlendModeT.hpp"
#include "open_viii/graphics/BPPT.hpp"
#include "tile_operations.hpp"
#include "utilities.hpp"
#include <cstdint>
#include <ff_8/PupuID.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <glengine/concepts.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>
namespace ff_8
{
enum class FilterSettings : std::uint8_t
{
     All_Disabled   = 0U,
     Toggle_Enabled = 0x1U << 0,
     Config_Enabled = 0x1U << 1,

     Default        = Config_Enabled,
};

[[nodiscard]] inline FilterSettings operator|(
  FilterSettings lhs,
  FilterSettings rhs)
{
     return static_cast<FilterSettings>(
       static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
}

[[nodiscard]] inline FilterSettings operator&(
  FilterSettings lhs,
  FilterSettings rhs)
{
     return static_cast<FilterSettings>(
       static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
}

inline FilterSettings &operator|=(
  FilterSettings &lhs,
  FilterSettings  rhs)
{
     lhs = lhs | rhs;
     return lhs;
}

inline FilterSettings &operator&=(
  FilterSettings &lhs,
  FilterSettings  rhs)
{
     lhs = lhs & rhs;
     return lhs;
}

[[nodiscard]] inline FilterSettings operator~(FilterSettings val)
{
     return static_cast<FilterSettings>(~static_cast<std::uint8_t>(val));
}

[[nodiscard]] inline bool HasFlag(
  FilterSettings settings,
  FilterSettings flag)
{
     return (settings & flag) != FilterSettings::All_Disabled;
}

inline void SetFlag(
  FilterSettings &settings,
  FilterSettings  flag,
  bool            enabled)
{
     if (enabled)
          settings |= flag;
     else
          settings &= ~flag;
}

[[nodiscard]] inline FilterSettings WithFlag(
  const FilterSettings settings,
  FilterSettings       flag,
  bool                 enabled)
{
     if (enabled)
          return settings | flag;
     else
          return settings & ~flag;
}


enum class FilterTag : std::uint8_t
{
     Pupu,
     MultiPupu,
     Swizzle,
     Deswizzle,
     SwizzleAsOneImage,
     FullFileName,
     Map,
     DrawBit,
     Z,
     MultiZ,
     Palette,
     MultiPalette,
     AnimationId,
     MultiAnimationId,
     AnimationFrame,
     MultiAnimationState,
     LayerId,
     MultiLayerId,
     TexturePageId,
     MultiTexturePageId,
     BlendMode,
     MultiBlendMode,
     BlendOther,
     MultiBlendOther,
     Bpp,
     MultiBpp,
     Compact,
     Flatten,
     CompactOnLoadOriginal,
     FlattenOnLoadOriginal,
     All,
     None,
};

template<FilterTag Tag>
struct ConfigKeys;

template<>
struct ConfigKeys<FilterTag::Pupu>
{
     using value_type                                   = PupuID;
     static constexpr std::string_view key_name         = "filter_pupu";
     static constexpr std::string_view enabled_key_name = "filter_pupu_enabled";
};

template<>
struct ConfigKeys<FilterTag::MultiPupu>
{
     using value_type                           = std::vector<PupuID>;
     static constexpr std::string_view key_name = "filter_multi_pupu";
     static constexpr std::string_view enabled_key_name
       = "filter_multi_pupu_enabled";
};

template<>
struct ConfigKeys<FilterTag::Swizzle>
{
     using value_type                           = std::filesystem::path;
     static constexpr std::string_view key_name = "filter_swizzle";
     static constexpr std::string_view enabled_key_name
       = "filter_swizzle_enabled";
};

template<>
struct ConfigKeys<FilterTag::Deswizzle>
{
     using value_type                           = std::filesystem::path;
     static constexpr std::string_view key_name = "filter_deswizzle";
     static constexpr std::string_view enabled_key_name
       = "filter_deswizzle_enabled";
};

template<>
struct ConfigKeys<FilterTag::SwizzleAsOneImage>
{
     using value_type                           = std::filesystem::path;
     static constexpr std::string_view key_name = "filter_swizzle_as_one_image";
     static constexpr std::string_view enabled_key_name
       = "filter_swizzle_as_one_image_enabled";
};

template<>
struct ConfigKeys<FilterTag::FullFileName>
{
     using value_type                           = std::filesystem::path;
     static constexpr std::string_view key_name = "filter_full_filename";
     static constexpr std::string_view enabled_key_name
       = "filter_full_filename_enabled";
};


template<>
struct ConfigKeys<FilterTag::Map>
{
     using value_type                                   = std::filesystem::path;
     static constexpr std::string_view key_name         = "filter_map";
     static constexpr std::string_view enabled_key_name = "filter_map_enabled";
};
template<>
struct ConfigKeys<FilterTag::DrawBit>
{
     using value_type = draw_bitT;
     static constexpr draw_bitT to_draw_bitT(bool in)
     {
          return in ? draw_bitT::enabled : draw_bitT::disabled;
     }
     using operation_type
       = decltype([](auto &&...args) { return to_draw_bitT(ff_8::tile_operations::Draw{}(std::forward<decltype(args)>(args)...)); });
     static constexpr std::string_view key_name         = "filter_draw";
     static constexpr std::string_view enabled_key_name = "filter_draw_enabled";
};

template<>
struct ConfigKeys<FilterTag::Z>
{
     using TileT          = open_viii::graphics::background::Tile1;
     using value_type     = ff_8::tile_operations::ZT<TileT>;
     using operation_type = ff_8::tile_operations::Z;
     static constexpr std::string_view key_name         = "filter_z";
     static constexpr std::string_view enabled_key_name = "filter_z_enabled";
};


template<>
struct ConfigKeys<FilterTag::MultiZ>
{
     using TileT          = open_viii::graphics::background::Tile1;
     using value_type     = std::vector<ff_8::tile_operations::ZT<TileT>>;
     using operation_type = ff_8::tile_operations::Z;
     static constexpr std::string_view key_name = "filter_multi_z";
     static constexpr std::string_view enabled_key_name
       = "filter_multi_z_enabled";
};

template<>
struct ConfigKeys<FilterTag::Palette>
{
     using TileT          = open_viii::graphics::background::Tile1;
     using value_type     = ff_8::tile_operations::PaletteIdT<TileT>;
     using operation_type = ff_8::tile_operations::PaletteId;
     static constexpr std::string_view key_name = "filter_palette";
     static constexpr std::string_view enabled_key_name
       = "filter_palette_enabled";
};

template<>
struct ConfigKeys<FilterTag::MultiPalette>
{
     using TileT      = open_viii::graphics::background::Tile1;
     using value_type = std::vector<ff_8::tile_operations::PaletteIdT<TileT>>;
     using operation_type = ff_8::tile_operations::PaletteId;
     static constexpr std::string_view key_name = "filter_multi_palette";
     static constexpr std::string_view enabled_key_name
       = "filter_multi_palette_enabled";
};

template<>
struct ConfigKeys<FilterTag::AnimationId>
{
     using TileT          = open_viii::graphics::background::Tile1;
     using value_type     = ff_8::tile_operations::AnimationIdT<TileT>;
     using operation_type = ff_8::tile_operations::AnimationId;
     static constexpr std::string_view key_name = "filter_animation_id";
     static constexpr std::string_view enabled_key_name
       = "filter_animation_id_enabled";
};

template<>
struct ConfigKeys<FilterTag::MultiAnimationId>
{
     using TileT      = open_viii::graphics::background::Tile1;
     using value_type = std::vector<ff_8::tile_operations::AnimationIdT<TileT>>;
     using operation_type = ff_8::tile_operations::AnimationId;
     static constexpr std::string_view key_name = "filter_multi_animation_id";
     static constexpr std::string_view enabled_key_name
       = "filter_multi_animation_id_enabled";
};

template<>
struct ConfigKeys<FilterTag::AnimationFrame>
{
     using TileT          = open_viii::graphics::background::Tile1;
     using value_type     = ff_8::tile_operations::AnimationStateT<TileT>;
     using operation_type = ff_8::tile_operations::AnimationState;
     static constexpr std::string_view key_name = "filter_animation_frame";
     static constexpr std::string_view enabled_key_name
       = "filter_animation_frame_enabled";
};


template<>
struct ConfigKeys<FilterTag::MultiAnimationState>
{
     using TileT = open_viii::graphics::background::Tile1;
     using value_type
       = std::vector<ff_8::tile_operations::AnimationStateT<TileT>>;
     using operation_type = ff_8::tile_operations::AnimationState;
     static constexpr std::string_view key_name
       = "filter_multi_animation_frame";
     static constexpr std::string_view enabled_key_name
       = "filter_multi_animation_frame_enabled";
};

template<>
struct ConfigKeys<FilterTag::LayerId>
{
     using TileT          = open_viii::graphics::background::Tile1;
     using value_type     = ff_8::tile_operations::LayerIdT<TileT>;
     using operation_type = ff_8::tile_operations::LayerId;
     static constexpr std::string_view key_name = "filter_layer_id";
     static constexpr std::string_view enabled_key_name
       = "filter_layer_id_enabled";
};


template<>
struct ConfigKeys<FilterTag::MultiLayerId>
{
     using TileT          = open_viii::graphics::background::Tile1;
     using value_type     = std::vector<ff_8::tile_operations::LayerIdT<TileT>>;
     using operation_type = ff_8::tile_operations::LayerId;
     static constexpr std::string_view key_name = "filter_multi_layer_id";
     static constexpr std::string_view enabled_key_name
       = "filter_multi_layer_id_enabled";
};

template<>
struct ConfigKeys<FilterTag::TexturePageId>
{
     using TileT          = open_viii::graphics::background::Tile1;
     using value_type     = ff_8::tile_operations::TextureIdT<TileT>;
     using operation_type = ff_8::tile_operations::TextureId;
     static constexpr std::string_view key_name = "filter_texture_page_id";
     static constexpr std::string_view enabled_key_name
       = "filter_texture_page_id_enabled";
};

template<>
struct ConfigKeys<FilterTag::MultiTexturePageId>
{
     using TileT      = open_viii::graphics::background::Tile1;
     using value_type = std::vector<ff_8::tile_operations::TextureIdT<TileT>>;
     using operation_type = ff_8::tile_operations::TextureId;
     static constexpr std::string_view key_name
       = "filter_multi_texture_page_id";
     static constexpr std::string_view enabled_key_name
       = "filter_multi_texture_page_id_enabled";
};

template<>
struct ConfigKeys<FilterTag::BlendMode>
{
     using TileT          = open_viii::graphics::background::Tile1;
     using value_type     = ff_8::tile_operations::BlendModeT<TileT>;
     using operation_type = ff_8::tile_operations::BlendMode;
     static constexpr std::string_view key_name = "filter_blend_mode";
     static constexpr std::string_view enabled_key_name
       = "filter_blend_mode_enabled";
};

template<>
struct ConfigKeys<FilterTag::MultiBlendMode>
{
     using TileT      = open_viii::graphics::background::Tile1;
     using value_type = std::vector<ff_8::tile_operations::BlendModeT<TileT>>;
     using operation_type = ff_8::tile_operations::BlendMode;
     static constexpr std::string_view enabled_key_name
       = "filter_multi_blend_mode_enabled";
     static constexpr std::string_view key_name = "filter_multi_blend_mode";
};

template<>
struct ConfigKeys<FilterTag::BlendOther>
{
     using TileT          = open_viii::graphics::background::Tile1;
     using value_type     = ff_8::tile_operations::BlendT<TileT>;
     using operation_type = ff_8::tile_operations::Blend;
     static constexpr std::string_view key_name = "filter_blend_other";
     static constexpr std::string_view enabled_key_name
       = "filter_blend_other_enabled";
};


template<>
struct ConfigKeys<FilterTag::MultiBlendOther>
{
     using TileT          = open_viii::graphics::background::Tile1;
     using value_type     = std::vector<ff_8::tile_operations::BlendT<TileT>>;
     using operation_type = ff_8::tile_operations::Blend;
     static constexpr std::string_view key_name = "filter_multi_blend_other";
     static constexpr std::string_view enabled_key_name
       = "filter_multi_blend_other_enabled";
};

template<>
struct ConfigKeys<FilterTag::Bpp>
{
     using TileT          = open_viii::graphics::background::Tile1;
     using value_type     = ff_8::tile_operations::DepthT<TileT>;
     using operation_type = ff_8::tile_operations::Depth;
     static constexpr std::string_view key_name         = "filter_bpp";
     static constexpr std::string_view enabled_key_name = "filter_bpp_enabled";
};

template<>
struct ConfigKeys<FilterTag::MultiBpp>
{
     using TileT          = open_viii::graphics::background::Tile1;
     using value_type     = std::vector<ff_8::tile_operations::DepthT<TileT>>;
     using operation_type = ff_8::tile_operations::Depth;
     static constexpr std::string_view key_name = "filter_multi_bpp";
     static constexpr std::string_view enabled_key_name
       = "filter_multi_bpp_enabled";
};

template<>
struct ConfigKeys<FilterTag::Compact>
{
     using value_type                           = fme::compact_type;
     static constexpr std::string_view key_name = "batch_compact_type";
     static constexpr std::string_view enabled_key_name
       = "batch_compact_enabled";
};

template<>
struct ConfigKeys<FilterTag::Flatten>
{
     using value_type                           = fme::flatten_type;
     static constexpr std::string_view key_name = "batch_flatten_type";
     static constexpr std::string_view enabled_key_name
       = "batch_flatten_enabled";
};

template<>
struct ConfigKeys<FilterTag::CompactOnLoadOriginal>
{
     using value_type                           = fme::compact_type;
     static constexpr std::string_view key_name = "CompactOnLoad";
     static constexpr std::string_view enabled_key_name
       = "CompactOnLoadEnabled";
};

template<>
struct ConfigKeys<FilterTag::FlattenOnLoadOriginal>
{
     using value_type                           = fme::flatten_type;
     static constexpr std::string_view key_name = "FlattenOnLoad";
     static constexpr std::string_view enabled_key_name
       = "FlattenOnLoadEnabled";
};


template<std::default_initializable ValueT>
struct FilterLoadStrategy
{
     static ValueT load_value(
       const toml::table &config,
       std::string_view   id)
     {
          return load_value(true, config, id);
     }
     static ValueT load_value(
       bool               load_config,
       const toml::table &config,
       std::string_view   id)
     {
          ValueT value = {};
          if (!config.contains(id) || !load_config)
          {
               return value;
          }
          if constexpr (std::same_as<ValueT, std::filesystem::path>)
          {
               value = config[id].value_or(value.u8string());
          }
          else if constexpr (std::convertible_to<ValueT, fme::color>)
          {
               value = std::bit_cast<fme::color>(
                 config[id].value_or(std::bit_cast<std::uint32_t>(value)));
          }
          else if constexpr (requires { std::declval<ValueT>().raw(); })
          {
               value = ValueT{ config[id].value_or(value.raw()) };
          }
          else if constexpr (std::is_enum_v<ValueT>)
          {
               value = static_cast<ValueT>(
                 config[id].value_or(std::to_underlying(value)));
          }
          else if constexpr (glengine::is_std_vector<ValueT>)
          {
               if constexpr (std::is_enum_v<
                               glengine::vector_elem_type_t<ValueT>>)
               {
                    (void)fme::Configuration::load_array<
                      glengine::vector_elem_type_t<ValueT>,
                      std::underlying_type_t<
                        glengine::vector_elem_type_t<ValueT>>>(
                      config, id, value);
               }
               else if constexpr (std::same_as<
                                    glengine::vector_elem_type_t<ValueT>,
                                    open_viii::graphics::BPPT>)
               {
                    (void)fme::Configuration::load_array<
                      glengine::vector_elem_type_t<ValueT>,
                      std::uint8_t>(config, id, value);
               }
               else if constexpr (std::same_as<
                                    glengine::vector_elem_type_t<ValueT>,
                                    PupuID>)
               {
                    (void)fme::Configuration::load_array<
                      glengine::vector_elem_type_t<ValueT>,
                      std::uint32_t>(config, id, value);
               }
               else
               {
                    (void)fme::Configuration::load_array<
                      glengine::vector_elem_type_t<ValueT>>(config, id, value);
               }
          }
          else
          {
               value = config[id].value_or(value);
          }
          return value;
     }

     static constexpr FilterSettings load_settings(
       const toml::table &config,
       std::string_view   enabled_key_name)
     {
          return load_settings(true, config, enabled_key_name);
     }

     static constexpr FilterSettings load_settings(
       bool               load_config,
       const toml::table &config,
       std::string_view   enabled_key_name)
     {
          if (load_config)
          {
               return WithFlag(
                 FilterSettings::Default,
                 FilterSettings::Toggle_Enabled,
                 config[enabled_key_name].value_or(false));
          }
          return FilterSettings::All_Disabled;
     }
};

template<typename ValueT>
struct FilterUpdateStrategy
{

     static void update_value(
       toml::table     &config,
       std::string_view id,
       const ValueT    &value)
     {
          if constexpr (std::same_as<ValueT, std::filesystem::path>)
          {
               std::u8string str_val = value.u8string();
               std::ranges::replace(
                 str_val, u8'\\', u8'/');// normalize to forward slashes
               spdlog::debug(
                 "selection<{}>: \"{}\"",
                 id,
                 std::filesystem::path(str_val).string());
               config.insert_or_assign(id, str_val);
          }
          else if constexpr (std::convertible_to<ValueT, fme::color>)
          {
               spdlog::debug("selection<{}>: {}", id, value);
               config.insert_or_assign(id, std::bit_cast<std::uint32_t>(value));
          }
          else if constexpr (requires { std::declval<ValueT>().raw(); })
          {
               spdlog::debug("selection<{}>: {}", id, value);
               config.insert_or_assign(id, value.raw());
          }
          else if constexpr (std::is_enum_v<ValueT>)
          {
               spdlog::debug("selection<{}>: {}", id, value);
               config.insert_or_assign(id, std::to_underlying(value));
          }
          else if constexpr (glengine::is_std_vector<ValueT>)
          {
               if constexpr (std::is_enum_v<
                               glengine::vector_elem_type_t<ValueT>>)
               {
                    fme::Configuration::update_array<
                      glengine::vector_elem_type_t<ValueT>,
                      std::underlying_type_t<
                        glengine::vector_elem_type_t<ValueT>>>(
                      config, id, value);
               }
               else if constexpr (std::same_as<
                                    glengine::vector_elem_type_t<ValueT>,
                                    open_viii::graphics::BPPT>)
               {
                    fme::Configuration::update_array<
                      glengine::vector_elem_type_t<ValueT>,
                      std::uint8_t>(config, id, value);
               }
               else if constexpr (std::same_as<
                                    glengine::vector_elem_type_t<ValueT>,
                                    PupuID>)
               {
                    fme::Configuration::update_array<
                      glengine::vector_elem_type_t<ValueT>,
                      std::uint32_t>(config, id, value);
               }
               else
               {
                    fme::Configuration::update_array<
                      glengine::vector_elem_type_t<ValueT>>(config, id, value);
               }
               spdlog::trace("selection<{}>", id);
          }
          else
          {
               spdlog::debug("selection<{}>: {}", id, value);
               config.insert_or_assign(id, value);
          }
     }

     static void update_settings(
       toml::table     &config,
       std::string_view enabled_key_name,
       FilterSettings   settings)
     {
          // Check if the Toggle_Enabled flag is set, write it to config
          const bool enabled
            = HasFlag(settings, FilterSettings::Toggle_Enabled);
          config.insert_or_assign(
            std::string{ enabled_key_name }, toml::value{ enabled });
     }
};

template<FilterTag tag>
concept HasOperationType
  = requires { typename ConfigKeys<tag>::operation_type; };


template<FilterTag Tag>
struct filter
{
   public:
     using value_type                            = ConfigKeys<Tag>::value_type;
     static constexpr inline FilterTag tag_value = Tag;

   private:
     value_type     m_value    = {};
     FilterSettings m_settings = {};

   public:
     filter(
       value_type     value,
       FilterSettings settings)// FilterSettings::Default
       : m_value(std::move(value))
       , m_settings(settings)
     {
     }
     filter(
       bool                      load_config,
       const fme::Configuration &config)
       : filter(
           FilterLoadStrategy<value_type>::load_value(
             load_config,
             config,
             ConfigKeys<Tag>::key_name),
           FilterLoadStrategy<value_type>::load_settings(
             load_config,
             config,
             ConfigKeys<Tag>::enabled_key_name))
     {
     }

     filter(FilterSettings settings)
       : filter(
           HasFlag(
             settings,
             FilterSettings::Config_Enabled),
           fme::Configuration{})
     {
     }

     filter &reload(const toml::table &table)
     {
          m_value = FilterLoadStrategy<value_type>::load_value(
            table, ConfigKeys<Tag>::key_name);
          m_settings = FilterLoadStrategy<value_type>::load_settings(
            table, ConfigKeys<Tag>::enabled_key_name);
          return *this;
     }

     filter &reload()
     {
          if (HasFlag(m_settings, FilterSettings::Config_Enabled))
          {
               fme::Configuration config{};
               return reload(config);
          }
          return *this;
     }

     filter &combine(const toml::table &table)
     {
          // Load temporary values
          const auto tmp_settings
            = FilterLoadStrategy<value_type>::load_settings(
              table, ConfigKeys<Tag>::enabled_key_name);

          // Only merge if enabled
          if (HasFlag(tmp_settings, FilterSettings::Config_Enabled))
          {
               auto tmp_value = FilterLoadStrategy<value_type>::load_value(
                 table, ConfigKeys<Tag>::key_name);
               if constexpr (glengine::is_std_vector<value_type>)
               {
                    m_value.insert(
                      m_value.end(),
                      std::make_move_iterator(tmp_value.begin()),
                      std::make_move_iterator(tmp_value.end()));
                    std::ranges::sort(m_value);
                    const auto not_unique = std::ranges::unique(m_value);
                    m_value.erase(not_unique.begin(), not_unique.end());
               }
               else
               {
                    // Fallback for non-vector types: overwrite
                    m_value = std::move(tmp_value);
               }

               m_settings |= tmp_settings;
          }

          return *this;
     }


     template<typename U>
     const filter &update([[maybe_unused]] U &&value) const
          requires(std::same_as<
                   std::remove_cvref_t<U>,
                   toml::table>)
     {
          if (enabled())
          {
               FilterUpdateStrategy<value_type>::update_value(
                 value, ConfigKeys<Tag>::key_name, m_value);
               FilterUpdateStrategy<value_type>::update_settings(
                 value, ConfigKeys<Tag>::enabled_key_name, m_settings);
          }
          else
          {
               value.erase(ConfigKeys<Tag>::key_name);
               value.erase(ConfigKeys<Tag>::enabled_key_name);
          }
          return *this;
     }

     template<typename U>
          requires std::same_as<
            std::remove_cvref_t<U>,
            toml::table>
     filter &update(U &&value)
     {
          if (enabled())
          {
               FilterUpdateStrategy<value_type>::update_value(
                 value, ConfigKeys<Tag>::key_name, m_value);
               FilterUpdateStrategy<value_type>::update_settings(
                 value, ConfigKeys<Tag>::enabled_key_name, m_settings);
          }
          else
          {
               value.erase(ConfigKeys<Tag>::key_name);
               value.erase(ConfigKeys<Tag>::enabled_key_name);
          }
          return *this;
     }

     template<typename U>
          requires(
            !std::same_as<
              std::remove_cvref_t<U>,
              value_type>
            && std::ranges::range<std::remove_cvref_t<U>>
            && std::ranges::range<value_type>
            && std::indirectly_movable<
              std::ranges::iterator_t<std::remove_cvref_t<U>>,
              std::back_insert_iterator<value_type>>)
     filter &update(U &&value)
     {
          if (!std::ranges::equal(m_value, value))
          {
               if constexpr (requires(value_type v) { v.clear(); })
                    m_value.clear();

               std::ranges::move(value, std::back_inserter(m_value));

               if (HasFlag(m_settings, FilterSettings::Config_Enabled))
               {
                    fme::Configuration config{};
                    FilterUpdateStrategy<value_type>::update_value(
                      *config, ConfigKeys<Tag>::key_name, m_value);
                    config.save();
               }
          }
          return *this;
     }

     template<typename U>
          requires(
            std::equality_comparable_with<
              std::remove_cvref_t<U>,
              value_type>
            && std::assignable_from<
              value_type &,
              U>)

     filter &update(U &&value)
     {
          const bool not_same = [&]()
          {
               if constexpr (
                 std::is_enum_v<std::remove_cvref_t<U>>
                 && std::is_enum_v<std::remove_cvref_t<value_type>>)
               {
                    return std::to_underlying(m_value)
                           != std::to_underlying(value);
               }
               else
               {
                    return m_value != value;
               }
          }();
          if (not_same)
          {
               m_value = std::forward<U>(value);

               if (HasFlag(m_settings, FilterSettings::Config_Enabled))
               {
                    fme::Configuration config{};
                    FilterUpdateStrategy<value_type>::update_value(
                      config, ConfigKeys<Tag>::key_name, m_value);
                    config.save();
               }
          }
          return *this;
     }
     [[nodiscard]] const value_type &value() const
     {
          return m_value;
     }
     [[nodiscard]] value_type &value()
     {
          return m_value;
     }
     [[nodiscard]] bool enabled() const
     {
          return HasFlag(m_settings, FilterSettings::Toggle_Enabled);
     }
     filter &enable()
     {
          if (HasFlag(m_settings, FilterSettings::Toggle_Enabled))
          {
               return *this;
          }
          SetFlag(m_settings, FilterSettings::Toggle_Enabled, true);
          if constexpr (std::same_as<
                          std::remove_cvref_t<
                            decltype(ConfigKeys<Tag>::enabled_key_name)>,
                          std::string_view>)
          {
               if (HasFlag(m_settings, FilterSettings::Config_Enabled))
               {
                    fme::Configuration config{};
                    FilterUpdateStrategy<value_type>::update_settings(
                      config, ConfigKeys<Tag>::enabled_key_name, m_settings);
                    config.save();
               }
          }
          return *this;
     }
     filter &disable()
     {
          if (!HasFlag(m_settings, FilterSettings::Toggle_Enabled))
          {
               return *this;
          }
          SetFlag(m_settings, FilterSettings::Toggle_Enabled, false);
          if constexpr (std::same_as<
                          std::remove_cvref_t<
                            decltype(ConfigKeys<Tag>::enabled_key_name)>,
                          std::string_view>)
          {
               if (HasFlag(m_settings, FilterSettings::Config_Enabled))
               {
                    fme::Configuration config{};
                    FilterUpdateStrategy<value_type>::update_settings(
                      config, ConfigKeys<Tag>::enabled_key_name, m_settings);
                    config.save();
               }
          }
          return *this;
     }
     template<typename T>
     [[nodiscard]] bool contains(const T &cmp) const
     {
          if constexpr (std::ranges::range<value_type>)
          {
               return std::ranges::any_of(
                 m_value, [&](const auto &current) { return current == cmp; });
          }
          else
          {
               return m_value == cmp;
          }
     }
     [[nodiscard]] bool operator==(const value_type &cmp) const
     {
          return m_value == cmp;
     }
     [[nodiscard]] bool operator!=(const value_type &cmp) const
     {
          return m_value != cmp;
     }
     [[nodiscard]] explicit operator bool() const
     {
          return enabled();
     }
     explicit operator value_type() const
     {
          return m_value;
     }
};
template<FilterTag Tag>
     requires(HasOperationType<Tag>)
struct filter<Tag>
{
   public:
     using value_type     = ConfigKeys<Tag>::value_type;
     using operation_type = ConfigKeys<Tag>::operation_type;
     static constexpr inline FilterTag tag_value = Tag;

   private:
     value_type                         m_value     = {};
     FilterSettings                     m_settings  = {};
     static const inline operation_type s_operation = {};

   public:
     filter(
       value_type     value,
       FilterSettings settings)// FilterSettings::Default
       : m_value(std::move(value))
       , m_settings(settings)
     {
     }
     filter(
       bool                      load_config,
       const fme::Configuration &config)
       : filter(
           FilterLoadStrategy<value_type>::load_value(
             load_config,
             config,
             ConfigKeys<Tag>::key_name),
           FilterLoadStrategy<value_type>::load_settings(
             load_config,
             config,
             ConfigKeys<Tag>::enabled_key_name))
     {
     }
     filter(FilterSettings settings)
       : filter(
           HasFlag(
             settings,
             FilterSettings::Config_Enabled),
           fme::Configuration{})
     {
     }

     filter &reload(const toml::table &table)
     {
          m_value = FilterLoadStrategy<value_type>::load_value(
            table, ConfigKeys<Tag>::key_name);
          m_settings = FilterLoadStrategy<value_type>::load_settings(
            table, ConfigKeys<Tag>::enabled_key_name);
          return *this;
     }


     filter &combine(const toml::table &table)
     {
          // Load temporary values
          const auto tmp_settings
            = FilterLoadStrategy<value_type>::load_settings(
              table, ConfigKeys<Tag>::enabled_key_name);

          // Only merge if enabled
          if (HasFlag(tmp_settings, FilterSettings::Config_Enabled))
          {
               auto tmp_value = FilterLoadStrategy<value_type>::load_value(
                 table, ConfigKeys<Tag>::key_name);
               if constexpr (glengine::is_std_vector<value_type>)
               {
                    m_value.insert(
                      m_value.end(),
                      std::make_move_iterator(tmp_value.begin()),
                      std::make_move_iterator(tmp_value.end()));
                    std::ranges::sort(m_value);
                    const auto not_unique = std::ranges::unique(m_value);
                    m_value.erase(not_unique.begin(), not_unique.end());
               }
               else
               {
                    // Fallback for non-vector types: overwrite
                    m_value = std::move(tmp_value);
               }

               m_settings |= tmp_settings;
          }

          return *this;
     }

     filter &reload()
     {
          if (HasFlag(m_settings, FilterSettings::Config_Enabled))
          {
               fme::Configuration config{};
               return reload(config);
          }
          return *this;
     }
     template<typename U>
     const filter &update([[maybe_unused]] U &&value) const
          requires(std::same_as<
                   std::remove_cvref_t<U>,
                   toml::table>)
     {
          if (enabled())
          {
               FilterUpdateStrategy<value_type>::update_value(
                 value, ConfigKeys<Tag>::key_name, m_value);
               FilterUpdateStrategy<value_type>::update_settings(
                 value, ConfigKeys<Tag>::enabled_key_name, m_settings);
          }
          else
          {
               value.erase(ConfigKeys<Tag>::key_name);
               value.erase(ConfigKeys<Tag>::enabled_key_name);
          }
          return *this;
     }
     template<typename U>
     filter &update(U &&value)
          requires open_viii::graphics::background::is_tile<
            std::remove_cvref_t<U>>
     {
          return update(std::invoke(s_operation, value));
     }

     template<typename U>
          requires std::same_as<
            std::remove_cvref_t<U>,
            toml::table>
     filter &update(U &&value)
     {
          if (enabled())
          {
               FilterUpdateStrategy<value_type>::update_value(
                 value, ConfigKeys<Tag>::key_name, m_value);
               FilterUpdateStrategy<value_type>::update_settings(
                 value, ConfigKeys<Tag>::enabled_key_name, m_settings);
          }
          else
          {
               value.erase(ConfigKeys<Tag>::key_name);
               value.erase(ConfigKeys<Tag>::enabled_key_name);
          }
          return *this;
     }

     template<typename U>
          requires(
            !std::same_as<
              std::remove_cvref_t<U>,
              value_type>
            && std::ranges::range<std::remove_cvref_t<U>>
            && std::ranges::range<value_type>
            && std::indirectly_movable<
              std::ranges::iterator_t<std::remove_cvref_t<U>>,
              std::back_insert_iterator<value_type>>)
     filter &update(U &&value)
     {
          if (!std::ranges::equal(m_value, value))
          {
               if constexpr (requires(value_type v) { v.clear(); })
                    m_value.clear();

               std::ranges::move(value, std::back_inserter(m_value));

               if (HasFlag(m_settings, FilterSettings::Config_Enabled))
               {
                    fme::Configuration config{};
                    FilterUpdateStrategy<value_type>::update_value(
                      *config, ConfigKeys<Tag>::key_name, m_value);
                    config.save();
               }
          }
          return *this;
     }

     template<typename U>
          requires(
            std::equality_comparable_with<
              std::remove_cvref_t<U>,
              value_type>
            && std::assignable_from<
              value_type &,
              U>)

     filter &update(U &&value)
     {
          const bool not_same = [&]()
          {
               if constexpr (
                 std::is_enum_v<std::remove_cvref_t<U>>
                 && std::is_enum_v<std::remove_cvref_t<value_type>>)
               {
                    return std::to_underlying(m_value)
                           != std::to_underlying(value);
               }
               else
               {
                    return m_value != value;
               }
          }();
          if (not_same)
          {
               m_value = std::forward<U>(value);

               if (HasFlag(m_settings, FilterSettings::Config_Enabled))
               {
                    fme::Configuration config{};
                    FilterUpdateStrategy<value_type>::update_value(
                      config, ConfigKeys<Tag>::key_name, m_value);
                    config.save();
               }
          }
          return *this;
     }

     [[nodiscard]] const value_type &value() const
     {
          return m_value;
     }
     [[nodiscard]] value_type &value()
     {
          return m_value;
     }
     [[nodiscard]] bool enabled() const
     {
          return HasFlag(m_settings, FilterSettings::Toggle_Enabled);
     }
     filter &enable()
     {
          SetFlag(m_settings, FilterSettings::Toggle_Enabled, true);
          if constexpr (std::same_as<
                          std::remove_cvref_t<
                            decltype(ConfigKeys<Tag>::enabled_key_name)>,
                          std::string_view>)
          {
               if (HasFlag(m_settings, FilterSettings::Config_Enabled))
               {
                    fme::Configuration config{};
                    FilterUpdateStrategy<value_type>::update_settings(
                      config, ConfigKeys<Tag>::enabled_key_name, m_settings);
                    config.save();
               }
          }
          return *this;
     }
     filter &disable()
     {
          SetFlag(m_settings, FilterSettings::Toggle_Enabled, false);
          if constexpr (std::same_as<
                          std::remove_cvref_t<
                            decltype(ConfigKeys<Tag>::enabled_key_name)>,
                          std::string_view>)
          {
               if (HasFlag(m_settings, FilterSettings::Config_Enabled))
               {
                    fme::Configuration config{};
                    FilterUpdateStrategy<value_type>::update_settings(
                      config, ConfigKeys<Tag>::enabled_key_name, m_settings);
                    config.save();
               }
          }
          return *this;
     }

     template<typename T>
     [[nodiscard]] bool contains(const T &cmp) const
     {
          if constexpr (std::ranges::range<value_type>)
          {
               return std::ranges::any_of(
                 m_value, [&](const auto &current) { return current == cmp; });
          }
          else
          {
               return m_value == cmp;
          }
     }
     [[nodiscard]] bool operator==(const value_type &cmp) const
     {
          return m_value == cmp;
     }
     [[nodiscard]] bool operator!=(const value_type &cmp) const
     {
          return m_value != cmp;
     }
     [[nodiscard]] explicit operator bool() const
     {
          return enabled();
     }
     explicit operator value_type() const
     {
          return m_value;
     }
     template<open_viii::graphics::background::is_tile TileT>
     bool operator()(const TileT &tile) const
     {
          return !enabled() || contains(std::invoke(s_operation, tile));
     }
};

template<typename FilterT>
using FilterLoadStrategyFromFilter
  = FilterLoadStrategy<typename ConfigKeys<FilterT::tag_value>::value_type>;

template<typename FilterT>
using FilterUpdateStrategyFromFilter
  = FilterUpdateStrategy<typename ConfigKeys<FilterT::tag_value>::value_type>;

template<typename T>
concept IsFilterOld = requires(T obj) {
     {
          obj.update(typename std::remove_cvref_t<T>::value_type{})
     } -> std::same_as<T &>;
     {
          obj.value()
     } -> std::convertible_to<
       typename std::remove_cvref_t<T>::value_type const &>;
     { obj.enabled() } -> std::convertible_to<const bool &>;
     { obj.enable() } -> std::same_as<T &>;
     { obj.disable() } -> std::same_as<T &>;
     { static_cast<bool>(obj) } -> std::convertible_to<bool>;
     {
          static_cast<typename std::remove_cvref_t<T>::value_type>(obj)
     } -> std::convertible_to<typename std::remove_cvref_t<T>::value_type>;
};


template<typename T, typename TileT>
concept IsFilter
  = open_viii::graphics::background::is_tile<TileT> && IsFilterOld<T>
    && requires(T obj, TileT tile) {
            {
                 obj(tile)
            } -> std::convertible_to<bool>;// Ensure it works as a predicate on
                                           // tiles
       };

template<typename T, typename TileT = void>
concept IsEitherFilter = IsFilterOld<T> || IsFilter<T, TileT>;

using FilterVariant = decltype([] {
    return []<std::size_t... Is>(std::index_sequence<Is...>)
        -> std::variant<std::monostate,filter<static_cast<FilterTag>(Is)>...>
    {
        return std::monostate{}; // default-constructed variant
    }(std::make_index_sequence<static_cast<std::size_t>(FilterTag::All)>{});
}());

struct filters
{
     using TileT = open_viii::graphics::background::Tile1;

     static constexpr std::size_t FiltersSizeT
       = static_cast<std::size_t>(FilterTag::All);
     using FiltersArrayT = std::array<FilterVariant, FiltersSizeT>;

     FiltersArrayT m_filters_array;

     FiltersArrayT load_filters_array(
       const bool                load_config,
       const fme::Configuration &config)
     {

          FiltersArrayT result{};
          fme::for_each_enum<FilterTag>(
            [&]<FilterTag Key>()
            {
                 result[std::to_underlying(Key)]
                   = FilterVariant{ std::in_place_type<filter<Key>>,
                                    load_config, config };
            });
          return result;
     }

     filters(const filters &)                = default;
     filters &operator=(const filters &)     = default;

     filters(filters &&) noexcept            = default;
     filters &operator=(filters &&) noexcept = default;

     filters(
       bool                     load_config,
       fme::Configuration const config = {})
       : m_filters_array(load_filters_array(
           load_config,
           config))
     {
     }


     template<FilterTag Tag>
          requires(FiltersSizeT > static_cast<std::size_t>(Tag))
     filter<Tag> &get()
     {
          static constexpr std::size_t index = static_cast<std::size_t>(Tag);
          // using ValueT = typename ConfigKeys<Tag>::value_type;


          if (
            (m_filters_array[index].index() == 0U)
            || std::holds_alternative<std::monostate>(m_filters_array[index]))
          {
               // if constexpr (std::default_initializable<ValueT>)
               // {
               //      throw std::runtime_error(
               //        "Mutable access to default-initialized value is not "
               //        "supported");
               // }
               // else
               {
                    throw std::runtime_error(
                      "Filter not initialized and not "
                      "default-initializable");
               }
          }

          return std::get<filter<Tag>>(m_filters_array[index]);
     }


     template<FilterTag Tag>
          requires(FiltersSizeT > static_cast<std::size_t>(Tag))
     const filter<Tag> &get() const
     {
          static constexpr std::size_t index = static_cast<std::size_t>(Tag);
          // using ValueT = typename ConfigKeys<Tag>::value_type;

          if (
            (m_filters_array[index].index() == 0U)
            || std::holds_alternative<std::monostate>(m_filters_array[index]))
          {
               // if constexpr (std::default_initializable<ValueT>)
               // {
               //      throw std::runtime_error(
               //        "Mutable access to default-initialized value is not "
               //        "supported");
               // }
               // else
               {
                    throw std::runtime_error(
                      "Filter not initialized and not "
                      "default-initializable");
               }
          }

          return std::get<filter<Tag>>(m_filters_array[index]);
     }


     template<FilterTag Tag>
          requires(FiltersSizeT > static_cast<std::size_t>(Tag))
     ConfigKeys<Tag>::value_type &value()
     {

          return get<Tag>().value();
     }

     template<FilterTag Tag>
          requires(FiltersSizeT > static_cast<std::size_t>(Tag))
     const ConfigKeys<Tag>::value_type &value() const
     {

          return get<Tag>().value();
     }


     template<
       FilterTag Tag,
       typename TypeT>
          requires(
            FiltersSizeT > static_cast<std::size_t>(Tag)
            && !std::same_as<
               std::remove_cvref_t<TypeT>,
               toml::table>)
     filter<Tag> &update(TypeT &&input)
     {
          return get<Tag>().update(std::forward<TypeT>(input));
     }


     template<FilterTag Tag>
          requires(FiltersSizeT > static_cast<std::size_t>(Tag))
     bool enabled() const
     {

          return get<Tag>().enabled();
     }


     template<FilterTag Tag>
          requires(FiltersSizeT > static_cast<std::size_t>(Tag))
     filter<Tag> &enable()
     {
          return get<Tag>().enable();
     }

     template<FilterTag Tag>
          requires(FiltersSizeT > static_cast<std::size_t>(Tag))
     filter<Tag> &disable()
     {
          return get<Tag>().disable();
     }

     void reload(const toml::table &table)
     {
          fme::for_each_enum<FilterTag>(
            [&]<FilterTag Key>()
            {
                 using ValueT = typename ConfigKeys<Key>::value_type;
                 if constexpr (!std::is_same_v<ValueT, std::filesystem::path>)
                      get<Key>().reload(table);
            });
     }


     void combine(const toml::table &table)
     {
          fme::for_each_enum<FilterTag>(
            [&]<FilterTag Key>()
            {
                 using ValueT = typename ConfigKeys<Key>::value_type;
                 if constexpr (!std::is_same_v<ValueT, std::filesystem::path>)
                      get<Key>().combine(table);
            });
     }


     void update_table(toml::table &table)
     {
          fme::for_each_enum<FilterTag>(
            [&]<FilterTag Key>()
            {
                 using ValueT = typename ConfigKeys<Key>::value_type;
                 if constexpr (!std::is_same_v<ValueT, std::filesystem::path>)
                      get<Key>().update(table);
            });
     }

     template<open_viii::graphics::background::is_tile ThisTileT>
     bool operator()(const ThisTileT &tile) const
     {

          std::bitset<static_cast<std::size_t>(FilterTag::All)> results;
          fme::for_each_enum<FilterTag>(
            [&]<FilterTag Key>()
            {
                 if constexpr (HasOperationType<Key>)
                 {
                      results.set(
                        std::to_underlying(Key), std::invoke(get<Key>(), tile));
                 }
                 else
                 {
                      results.set(std::to_underlying(Key), true);
                 }
            });
          return results.all();
     }
};
namespace tile_operations
{
     template<open_viii::graphics::background::is_tile tileT>
     bool fail_any_filters(
       const ff_8::filters &filters,
       const tileT         &tile)
     {
          return !std::invoke(filters, tile);
     }
}// namespace tile_operations
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_FILTER_HPP
