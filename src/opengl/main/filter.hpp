//
// Created by pcvii on 10/4/2021.
//

#ifndef FIELD_MAP_EDITOR_FILTER_HPP
#define FIELD_MAP_EDITOR_FILTER_HPP
#include "Configuration.hpp"
#include "draw_bit_t.hpp"
#include "gui/colors.hpp"
#include "gui/compact_type.hpp"
#include "open_viii/graphics/background/BlendModeT.hpp"
#include "open_viii/graphics/BPPT.hpp"
#include "PupuID.hpp"
#include "tile_operations.hpp"
#include <concepts.hpp>
#include <cstdint>
#include <filesystem>
#include <fmt/format.h>
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

[[nodiscard]] inline FilterSettings operator|(FilterSettings lhs, FilterSettings rhs)
{
     return static_cast<FilterSettings>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
}

[[nodiscard]] inline FilterSettings operator&(FilterSettings lhs, FilterSettings rhs)
{
     return static_cast<FilterSettings>(static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
}

inline FilterSettings &operator|=(FilterSettings &lhs, FilterSettings rhs)
{
     lhs = lhs | rhs;
     return lhs;
}

inline FilterSettings &operator&=(FilterSettings &lhs, FilterSettings rhs)
{
     lhs = lhs & rhs;
     return lhs;
}

[[nodiscard]] inline FilterSettings operator~(FilterSettings val)
{
     return static_cast<FilterSettings>(~static_cast<std::uint8_t>(val));
}

[[nodiscard]] inline bool HasFlag(FilterSettings settings, FilterSettings flag)
{
     return (settings & flag) != FilterSettings::All_Disabled;
}

inline void SetFlag(FilterSettings &settings, FilterSettings flag, bool enabled)
{
     if (enabled)
          settings |= flag;
     else
          settings &= ~flag;
}

[[nodiscard]] inline FilterSettings WithFlag(const FilterSettings settings, FilterSettings flag, bool enabled)
{
     if (enabled)
          return settings | flag;
     else
          return settings & ~flag;
}


enum class FilterTag : std::uint8_t
{
     None,
     Pupu,
     MultiPupu,
     Swizzle,
     Deswizzle,
     SwizzleAsOneImage,
     Map,
     DrawBit,
     Z,
     Palette,
     AnimationId,
     AnimationFrame,
     LayerId,
     TexturePageId,
     BlendMode,
     BlendOther,
     Bpp,
     Compact,
     Flatten
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
     using value_type                                   = std::vector<PupuID>;
     static constexpr std::string_view key_name         = "filter_multi_pupu";
     static constexpr std::string_view enabled_key_name = "filter_multi_pupu_enabled";
};

template<>
struct ConfigKeys<FilterTag::Swizzle>
{
     using value_type                                   = std::filesystem::path;
     static constexpr std::string_view key_name         = "filter_swizzle";
     static constexpr std::string_view enabled_key_name = "filter_swizzle_enabled";
};

template<>
struct ConfigKeys<FilterTag::Deswizzle>
{
     using value_type                                   = std::filesystem::path;
     static constexpr std::string_view key_name         = "filter_deswizzle";
     static constexpr std::string_view enabled_key_name = "filter_deswizzle_enabled";
};

template<>
struct ConfigKeys<FilterTag::SwizzleAsOneImage>
{
     using value_type                                   = std::filesystem::path;
     static constexpr std::string_view key_name         = "filter_swizzle_as_one_image";
     static constexpr std::string_view enabled_key_name = "filter_swizzle_as_one_image_enabled";
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
     using value_type                                   = draw_bitT;
     using operation_type                               = ff_8::tile_operations::Draw;
     static constexpr std::string_view key_name         = "filter_draw";
     static constexpr std::string_view enabled_key_name = "filter_draw_enabled";
};

template<>
struct ConfigKeys<FilterTag::Z>
{
     using TileT                                        = open_viii::graphics::background::Tile1;
     using value_type                                   = ff_8::tile_operations::ZT<TileT>;
     using operation_type                               = ff_8::tile_operations::Z;
     static constexpr std::string_view key_name         = "filter_z";
     static constexpr std::string_view enabled_key_name = "filter_z_enabled";
};

template<>
struct ConfigKeys<FilterTag::Palette>
{
     using TileT                                        = open_viii::graphics::background::Tile1;
     using value_type                                   = ff_8::tile_operations::PaletteIdT<TileT>;
     using operation_type                               = ff_8::tile_operations::PaletteId;
     static constexpr std::string_view key_name         = "selections_palette";
     static constexpr std::string_view enabled_key_name = "selections_palette_enabled";
};

template<>
struct ConfigKeys<FilterTag::AnimationId>
{
     using TileT                                        = open_viii::graphics::background::Tile1;
     using value_type                                   = ff_8::tile_operations::AnimationIdT<TileT>;
     using operation_type                               = ff_8::tile_operations::AnimationId;
     static constexpr std::string_view key_name         = "filter_animation_id";
     static constexpr std::string_view enabled_key_name = "filter_animation_id_enabled";
};

template<>
struct ConfigKeys<FilterTag::AnimationFrame>
{
     using TileT                                        = open_viii::graphics::background::Tile1;
     using value_type                                   = ff_8::tile_operations::AnimationStateT<TileT>;
     using operation_type                               = ff_8::tile_operations::AnimationState;
     static constexpr std::string_view key_name         = "filter_animation_frame";
     static constexpr std::string_view enabled_key_name = "filter_animation_frame_enabled";
};

template<>
struct ConfigKeys<FilterTag::LayerId>
{
     using TileT                                        = open_viii::graphics::background::Tile1;
     using value_type                                   = ff_8::tile_operations::LayerIdT<TileT>;
     using operation_type                               = ff_8::tile_operations::LayerId;
     static constexpr std::string_view key_name         = "filter_layer_id";
     static constexpr std::string_view enabled_key_name = "filter_layer_id_enabled";
};

template<>
struct ConfigKeys<FilterTag::TexturePageId>
{
     using TileT                                        = open_viii::graphics::background::Tile1;
     using value_type                                   = ff_8::tile_operations::TextureIdT<TileT>;
     using operation_type                               = ff_8::tile_operations::TextureId;
     static constexpr std::string_view key_name         = "filter_texture_page_id";
     static constexpr std::string_view enabled_key_name = "filter_texture_page_id_enabled";
};

template<>
struct ConfigKeys<FilterTag::BlendMode>
{
     using TileT                                        = open_viii::graphics::background::Tile1;
     using value_type                                   = ff_8::tile_operations::BlendModeT<TileT>;
     using operation_type                               = ff_8::tile_operations::BlendMode;
     static constexpr std::string_view key_name         = "filter_blend_mode";
     static constexpr std::string_view enabled_key_name = "filter_blend_mode_enabled";
};

template<>
struct ConfigKeys<FilterTag::BlendOther>
{
     using TileT                                        = open_viii::graphics::background::Tile1;
     using value_type                                   = ff_8::tile_operations::BlendT<TileT>;
     using operation_type                               = ff_8::tile_operations::Blend;
     static constexpr std::string_view key_name         = "filter_blend_other";
     static constexpr std::string_view enabled_key_name = "filter_blend_other_enabled";
};

template<>
struct ConfigKeys<FilterTag::Bpp>
{
     using TileT                                        = open_viii::graphics::background::Tile1;
     using value_type                                   = ff_8::tile_operations::DepthT<TileT>;
     using operation_type                               = ff_8::tile_operations::Depth;
     static constexpr std::string_view key_name         = "selections_bpp";
     static constexpr std::string_view enabled_key_name = "selections_bpp_enabled";
};

template<>
struct ConfigKeys<FilterTag::Compact>
{
     using value_type                                   = fme::compact_type;
     static constexpr std::string_view key_name         = "batch_compact_type";
     static constexpr std::string_view enabled_key_name = "batch_compact_enabled";
};

template<>
struct ConfigKeys<FilterTag::Flatten>
{
     using value_type                                   = fme::flatten_type;
     static constexpr std::string_view key_name         = "batch_flatten_type";
     static constexpr std::string_view enabled_key_name = "batch_flatten_enabled";
};


template<std::default_initializable ValueT>
struct FilterLoadStrategy
{
     static ValueT load_value(bool load_config, const fme::Configuration &config, std::string_view id)
     {
          ValueT value = {};
          if (!config->contains(id) || !load_config)
          {
               return value;
          }
          if constexpr (std::same_as<ValueT, std::filesystem::path>)
          {
               value = config[id].value_or(value.u8string());
          }
          else if constexpr (std::convertible_to<ValueT, fme::color>)
          {
               value = std::bit_cast<fme::color>(config[id].value_or(std::bit_cast<std::uint32_t>(value)));
          }
          else if constexpr (requires { std::declval<ValueT>().raw(); })
          {
               value = ValueT{ config[id].value_or(value.raw()) };
          }
          else if constexpr (std::is_enum_v<ValueT>)
          {
               value = static_cast<ValueT>(config[id].value_or(std::to_underlying(value)));
          }
          else if constexpr (glengine::is_std_vector<ValueT>)
          {
               if constexpr (std::same_as<glengine::vector_elem_type_t<ValueT>, PupuID>)
               {
                    (void)config.load_array<glengine::vector_elem_type_t<ValueT>, std::uint32_t>(id, value);
               }
               else
               {
                    (void)config.load_array<glengine::vector_elem_type_t<ValueT>>(id, value);
               }
          }
          else
          {
               value = config[id].value_or(value);
          }
          return value;
     }

     static constexpr FilterSettings load_settings(bool load_config, const fme::Configuration &config, std::string_view enabled_key_name)
     {
          if (load_config)
          {
               return WithFlag(FilterSettings::Default, FilterSettings::Toggle_Enabled, config[enabled_key_name].value_or(false));
          }
          return FilterSettings::All_Disabled;
     }
};
template<typename ValueT>
struct FilterConversionStrategy;

template<typename ValueT>
struct FilterUpdateStrategy
{

     static void update(fme::Configuration &config, std::string_view id, const ValueT &value)
     {
          if constexpr (std::same_as<ValueT, std::filesystem::path>)
          {
               std::u8string str_val = value.u8string();
               std::ranges::replace(str_val, u8'\\', u8'/');// normalize to forward slashes
               spdlog::info("selection<{}>: \"{}\"", id, std::filesystem::path(str_val).string());
               config->insert_or_assign(id, str_val);
          }
          else if constexpr (std::convertible_to<ValueT, fme::color>)
          {
               spdlog::info("selection<{}>: {}", id, value);
               config->insert_or_assign(id, std::bit_cast<std::uint32_t>(value));
          }
          else if constexpr (requires { std::declval<ValueT>().raw(); })
          {
               spdlog::info("selection<{}>: {}", id, value);
               config->insert_or_assign(id, value.raw());
          }
          else if constexpr (std::is_enum_v<ValueT>)
          {
               spdlog::info("selection<{}>: {}", id, value);
               config->insert_or_assign(id, std::to_underlying(value));
          }
          else if constexpr (glengine::is_std_vector<ValueT>)
          {
               if constexpr (std::same_as<glengine::vector_elem_type_t<ValueT>, PupuID>)
               {
                    config.update_array<glengine::vector_elem_type_t<ValueT>, std::uint32_t>(id, value);
               }
               else
               {
                    config.update_array<glengine::vector_elem_type_t<ValueT>>(id, value);
               }
          }
          else
          {
               spdlog::info("selection<{}>: {}", id, value);
               config->insert_or_assign(id, value);
          }
     }
};


template<FilterTag Tag>
struct filter_old
{
   public:
     using value_type                            = ConfigKeys<Tag>::value_type;
     static constexpr inline FilterTag tag_value = Tag;

   private:
     value_type     m_value    = {};
     FilterSettings m_settings = {};

   public:
     filter_old(value_type value, FilterSettings settings)// FilterSettings::Default
       : m_value(std::move(value))
       , m_settings(settings)
     {
     }
     filter_old(bool load_config, const fme::Configuration &config)
       : filter_old(
           FilterLoadStrategy<value_type>::load_value(load_config, config, ConfigKeys<Tag>::key_name),
           FilterLoadStrategy<value_type>::load_settings(load_config, config, ConfigKeys<Tag>::enabled_key_name))
     {
     }
     filter_old(FilterSettings settings)
       : filter_old(HasFlag(settings, FilterSettings::Config_Enabled), fme::Configuration{})
     {
     }
     template<typename U>
     filter_old &update(U &&value)
     {
          if constexpr (
            !std::same_as<std::remove_cvref_t<U>, value_type> && std::ranges::range<std::remove_cvref_t<U>>
            && std::ranges::range<value_type>)
          {
               if (!std::ranges::equal(m_value, value))
               {
                    if constexpr (requires(value_type v) { v.clear(); })
                    {
                         m_value.clear();
                    }
                    std::ranges::move(value, std::back_inserter(m_value));
                    if constexpr (std::same_as<std::remove_cvref_t<decltype(ConfigKeys<Tag>::key_name)>, std::string_view>)
                    {
                         if (HasFlag(m_settings, FilterSettings::Config_Enabled))
                         {
                              fme::Configuration config{};
                              FilterUpdateStrategy<value_type>::update(config, ConfigKeys<Tag>::key_name, m_value);
                              config.save();
                         }
                    }
               }
          }
          else
          {
               if (m_value != value)
               {
                    m_value = std::forward<U>(value);
                    if constexpr (std::same_as<std::remove_cvref_t<decltype(ConfigKeys<Tag>::key_name)>, std::string_view>)
                    {
                         if (HasFlag(m_settings, FilterSettings::Config_Enabled))
                         {
                              fme::Configuration config{};
                              FilterUpdateStrategy<value_type>::update(config, ConfigKeys<Tag>::key_name, m_value);
                              config.save();
                         }
                    }
               }
          }
          return *this;
     }
     [[nodiscard]] const value_type &value() const
     {
          return m_value;
     }
     [[nodiscard]] bool enabled() const
     {
          return HasFlag(m_settings, FilterSettings::Toggle_Enabled);
     }
     filter_old &enable()
     {
          if (HasFlag(m_settings, FilterSettings::Toggle_Enabled))
          {
               return *this;
          }
          SetFlag(m_settings, FilterSettings::Toggle_Enabled, true);
          if constexpr (std::same_as<std::remove_cvref_t<decltype(ConfigKeys<Tag>::enabled_key_name)>, std::string_view>)
          {
               if (HasFlag(m_settings, FilterSettings::Config_Enabled))
               {
                    spdlog::info("filter_old<{}>: enabled", ConfigKeys<Tag>::key_name);
                    fme::Configuration config{};
                    config->insert_or_assign(ConfigKeys<Tag>::enabled_key_name, enabled());
                    config.save();
               }
          }
          return *this;
     }
     filter_old &disable()
     {
          if (!HasFlag(m_settings, FilterSettings::Toggle_Enabled))
          {
               return *this;
          }
          SetFlag(m_settings, FilterSettings::Toggle_Enabled, false);
          if constexpr (std::same_as<std::remove_cvref_t<decltype(ConfigKeys<Tag>::enabled_key_name)>, std::string_view>)
          {
               if (HasFlag(m_settings, FilterSettings::Config_Enabled))
               {
                    spdlog::info("filter_old<{}>: disabled", ConfigKeys<Tag>::key_name);
                    fme::Configuration config{};
                    config->insert_or_assign(ConfigKeys<Tag>::enabled_key_name, enabled());
                    config.save();
               }
          }
          return *this;
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
struct filter
{
   public:
     using value_type                            = ConfigKeys<Tag>::value_type;
     using operation_type                        = ConfigKeys<Tag>::operation_type;
     static constexpr inline FilterTag tag_value = Tag;

   private:
     value_type                         m_value     = {};
     FilterSettings                     m_settings  = {};
     static const inline operation_type s_operation = {};

   public:
     filter(value_type value, FilterSettings settings)// FilterSettings::Default
       : m_value(std::move(value))
       , m_settings(settings)
     {
     }
     filter(bool load_config, const fme::Configuration &config)
       : filter(
           FilterLoadStrategy<value_type>::load_value(load_config, config, ConfigKeys<Tag>::key_name),
           FilterLoadStrategy<value_type>::load_settings(load_config, config, ConfigKeys<Tag>::enabled_key_name))
     {
     }
     filter(FilterSettings settings)
       : filter(HasFlag(settings, FilterSettings::Config_Enabled), fme::Configuration{})
     {
     }

     template<open_viii::graphics::background::is_tile TileT>
     filter &update(const TileT &tile)
     {
          return update(std::invoke(s_operation, tile));
     }

     template<typename U>
     filter &update(U &&value)
     {
          if (m_value != value)
          {
               m_value = std::forward<U>(value);
               if constexpr (std::same_as<std::remove_cvref_t<decltype(ConfigKeys<Tag>::key_name)>, std::string_view>)
               {
                    if (HasFlag(m_settings, FilterSettings::Config_Enabled))
                    {
                         fme::Configuration config{};
                         FilterUpdateStrategy<value_type>::update(config, ConfigKeys<Tag>::key_name, m_value);
                         config.save();
                    }
               }
          }
          return *this;
     }
     [[nodiscard]] const value_type &value() const
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
          if constexpr (std::same_as<std::remove_cvref_t<decltype(ConfigKeys<Tag>::enabled_key_name)>, std::string_view>)
          {
               if (HasFlag(m_settings, FilterSettings::Config_Enabled))
               {
                    spdlog::info("filter<{}>: enabled", ConfigKeys<Tag>::key_name);
                    fme::Configuration config{};
                    config->insert_or_assign(ConfigKeys<Tag>::enabled_key_name, enabled());
                    config.save();
               }
          }
          return *this;
     }
     filter &disable()
     {
          SetFlag(m_settings, FilterSettings::Toggle_Enabled, false);
          if constexpr (std::same_as<std::remove_cvref_t<decltype(ConfigKeys<Tag>::enabled_key_name)>, std::string_view>)
          {
               if (HasFlag(m_settings, FilterSettings::Config_Enabled))
               {
                    spdlog::info("filter<{}>: disabled", ConfigKeys<Tag>::key_name);
                    fme::Configuration config{};
                    config->insert_or_assign(ConfigKeys<Tag>::enabled_key_name, enabled());
                    config.save();
               }
          }
          return *this;
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
          return !enabled() || (m_value == std::invoke(s_operation, tile));
     }
};

template<typename T>
concept IsFilterOld = requires(T obj) {
     { obj.update(typename std::remove_cvref_t<T>::value_type{}) } -> std::same_as<T &>;
     { obj.value() } -> std::convertible_to<typename std::remove_cvref_t<T>::value_type const &>;
     { obj.enabled() } -> std::convertible_to<const bool &>;
     { obj.enable() } -> std::same_as<T &>;
     { obj.disable() } -> std::same_as<T &>;
     { static_cast<bool>(obj) } -> std::convertible_to<bool>;
     { static_cast<typename std::remove_cvref_t<T>::value_type>(obj) } -> std::convertible_to<typename std::remove_cvref_t<T>::value_type>;
};

template<typename T, typename TileT>
concept IsFilter = open_viii::graphics::background::is_tile<TileT> && IsFilterOld<T> && requires(T obj, TileT tile) {
     { obj(tile) } -> std::convertible_to<bool>;// Ensure it works as a predicate on tiles
};

template<typename T, typename TileT = void>
concept IsEitherFilter = IsFilterOld<T> || IsFilter<T, TileT>;

struct filters
{
     using TileT = open_viii::graphics::background::Tile1;
     filter_old<FilterTag::Pupu>              pupu;
     filter_old<FilterTag::MultiPupu>         multi_pupu;
     filter_old<FilterTag::Swizzle>           swizzle;
     filter_old<FilterTag::Deswizzle>         deswizzle;
     filter_old<FilterTag::SwizzleAsOneImage> swizzle_as_one_image;
     filter_old<FilterTag::Map>               map;
     filter<FilterTag::DrawBit>               draw_bit;
     filter<FilterTag::Z>                     z;
     filter<FilterTag::Palette>               palette;
     filter<FilterTag::AnimationId>           animation_id;
     filter<FilterTag::AnimationFrame>        animation_frame;
     filter<FilterTag::LayerId>               layer_id;
     filter<FilterTag::TexturePageId>         texture_page_id;
     filter<FilterTag::BlendMode>             blend_mode;
     filter<FilterTag::BlendOther>            blend_other;
     filter<FilterTag::Bpp>                   bpp;

     filters(bool load_config, fme::Configuration const config = {})
       : pupu(load_config, config)
       , multi_pupu(load_config, config)
       , swizzle(load_config, config)
       , deswizzle(load_config, config)
       , swizzle_as_one_image(load_config, config)
       , map(load_config, config)
       , draw_bit(load_config, config)
       , z(load_config, config)
       , palette(load_config, config)
       , animation_id(load_config, config)
       , animation_frame(load_config, config)
       , layer_id(load_config, config)
       , texture_page_id(load_config, config)
       , blend_mode(load_config, config)
       , blend_other(load_config, config)
       , bpp(load_config, config)
     {
     }


     template<open_viii::graphics::background::is_tile ThisTileT>
     bool operator()(const ThisTileT &tile) const
     {
          return [&tile](const auto &...operations) -> bool {
               return (std::invoke(operations, tile) && ...);
          }(draw_bit, z, palette, animation_id, animation_frame, layer_id, texture_page_id, blend_mode, blend_other, bpp);
     }
};
namespace tile_operations
{
     template<open_viii::graphics::background::is_tile tileT>
     bool fail_any_filters(const ff_8::filters &filters, const tileT &tile)
     {
          return !std::invoke(filters, tile);
     }
}// namespace tile_operations
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_FILTER_HPP
