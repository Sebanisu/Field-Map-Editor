//
// Created by pcvii on 10/4/2021.
//

#ifndef FIELD_MAP_EDITOR_FILTER_HPP
#define FIELD_MAP_EDITOR_FILTER_HPP
#include "Configuration.hpp"
#include "draw_bit_t.hpp"
#include "open_viii/graphics/background/BlendModeT.hpp"
#include "open_viii/graphics/BPPT.hpp"
#include "PupuID.hpp"
#include "tile_operations.hpp"
#include <cstdint>
#include <filesystem>
#include <fmt/format.h>
#include <map>
#include <string>
#include <utility>
#include <vector>
namespace ff_8
{
enum class FilterTag : std::uint8_t
{
     Pupu,
     Upscale,
     Deswizzle,
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
     Flatten,
};

template<FilterTag Tag>
struct ConfigKeys;

template<>
struct ConfigKeys<FilterTag::Pupu>
{
     static constexpr auto key_name         = "filter_pupu";
     static constexpr auto enabled_key_name = "filter_pupu_enabled";
};

template<>
struct ConfigKeys<FilterTag::Upscale>
{
     static constexpr auto key_name         = "filter_upscale";
     static constexpr auto enabled_key_name = "filter_upscale_enabled";
};

template<>
struct ConfigKeys<FilterTag::Deswizzle>
{
     static constexpr auto key_name         = "filter_deswizzle";
     static constexpr auto enabled_key_name = "filter_deswizzle_enabled";
};

template<>
struct ConfigKeys<FilterTag::DrawBit>
{
     static constexpr auto key_name         = "filter_draw";
     static constexpr auto enabled_key_name = "filter_draw_enabled";
};

template<>
struct ConfigKeys<FilterTag::Z>
{
     static constexpr auto key_name         = "filter_z";
     static constexpr auto enabled_key_name = "filter_z_enabled";
};

template<>
struct ConfigKeys<FilterTag::Palette>
{
     static constexpr auto key_name         = "filter_palette";
     static constexpr auto enabled_key_name = "filter_palette_enabled";
};

template<>
struct ConfigKeys<FilterTag::AnimationId>
{
     static constexpr auto key_name         = "filter_animation_id";
     static constexpr auto enabled_key_name = "filter_animation_id_enabled";
};

template<>
struct ConfigKeys<FilterTag::AnimationFrame>
{
     static constexpr auto key_name         = "filter_animation_frame";
     static constexpr auto enabled_key_name = "filter_animation_frame_enabled";
};

template<>
struct ConfigKeys<FilterTag::LayerId>
{
     static constexpr auto key_name         = "filter_layer_id";
     static constexpr auto enabled_key_name = "filter_layer_id_enabled";
};

template<>
struct ConfigKeys<FilterTag::TexturePageId>
{
     static constexpr auto key_name         = "filter_texture_page_id";
     static constexpr auto enabled_key_name = "filter_texture_page_id_enabled";
};

template<>
struct ConfigKeys<FilterTag::BlendMode>
{
     static constexpr auto key_name         = "filter_blend_mode";
     static constexpr auto enabled_key_name = "filter_blend_mode_enabled";
};

template<>
struct ConfigKeys<FilterTag::BlendOther>
{
     static constexpr auto key_name         = "filter_blend_other";
     static constexpr auto enabled_key_name = "filter_blend_other_enabled";
};

template<>
struct ConfigKeys<FilterTag::Bpp>
{
     static constexpr auto key_name         = "filter_bpp";
     static constexpr auto enabled_key_name = "filter_bpp_enabled";
};

template<>
struct ConfigKeys<FilterTag::Compact>
{
     static constexpr auto key_name         = "filter_compact";
     static constexpr auto enabled_key_name = "filter_compact_enabled";
};

template<>
struct ConfigKeys<FilterTag::Flatten>
{
     static constexpr auto key_name         = "filter_flatten";
     static constexpr auto enabled_key_name = "filter_flatten_enabled";
};


template<typename T, FilterTag Tag>
struct filter_old
{
   private:
     T    m_value   = {};
     bool m_enabled = { false };

   public:
     using value_type = T;
     filter_old()     = default;
     explicit filter_old(T value, bool enabled = false)
       : m_value(std::move(value))
       , m_enabled(enabled)
     {
     }
     template<typename U>
     filter_old &update(U &&value)
     {
          m_value = std::forward<U>(value);
          return *this;
     }
     [[nodiscard]] const T &value() const
     {
          return m_value;
     }
     [[nodiscard]] const bool &enabled() const
     {
          return m_enabled;
     }
     filter_old &enable()
     {
          m_enabled = true;
          return *this;
     }
     filter_old &disable()
     {
          m_enabled = false;
          return *this;
     }
     [[nodiscard]] bool operator==(const T &cmp) const
     {
          return m_value == cmp;
     }
     [[nodiscard]] bool operator!=(const T &cmp) const
     {
          return m_value != cmp;
     }
     [[nodiscard]] explicit operator bool() const
     {
          return m_enabled;
     }
     explicit operator T() const
     {
          return m_value;
     }
};
template<typename T, typename OpT, FilterTag Tag>
struct filter
{
   private:
     T    m_value     = {};
     bool m_enabled   = { false };
     OpT  m_operation = {};

   public:
     using value_type = T;
     filter()         = default;
     explicit filter(T value, bool enabled = false)
       : m_value(std::move(value))
       , m_enabled(enabled)
     {
     }
     template<typename U>
     filter &update(U &&value)
     {
          m_value = std::forward<U>(value);
          return *this;
     }
     [[nodiscard]] const T &value() const
     {
          return m_value;
     }
     [[nodiscard]] const bool &enabled() const
     {
          return m_enabled;
     }
     filter &enable()
     {
          m_enabled = true;
          return *this;
     }
     filter &disable()
     {
          m_enabled = false;
          return *this;
     }
     [[nodiscard]] bool operator==(const T &cmp) const
     {
          return m_value == cmp;
     }
     [[nodiscard]] bool operator!=(const T &cmp) const
     {
          return m_value != cmp;
     }
     [[nodiscard]] explicit operator bool() const
     {
          return m_enabled;
     }
     explicit operator T() const
     {
          return m_value;
     }
     template<open_viii::graphics::background::is_tile TileT>
     bool operator()(const TileT &tile) const
     {
          return !m_enabled || (m_value == std::invoke(m_operation, tile));
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
     filter_old<PupuID, FilterTag::Pupu>                                                                           pupu            = {};
     filter_old<std::filesystem::path, FilterTag::Upscale>                                                            upscale         = {};
     filter_old<std::filesystem::path, FilterTag::Deswizzle>                                                            deswizzle       = {};
     filter<draw_bitT, ff_8::tile_operations::Draw, FilterTag::DrawBit>                                                        draw_bit = {};
     filter<ff_8::tile_operations::ZT<TileT>, ff_8::tile_operations::Z, FilterTag::Z>                           z               = {};
     filter<ff_8::tile_operations::PaletteIdT<TileT>, ff_8::tile_operations::PaletteId, FilterTag::Palette>           palette         = {};
     filter<ff_8::tile_operations::AnimationIdT<TileT>, ff_8::tile_operations::AnimationId, FilterTag::AnimationId>       animation_id    = {};
     filter<ff_8::tile_operations::AnimationStateT<TileT>, ff_8::tile_operations::AnimationState, FilterTag::AnimationFrame> animation_frame = {};
     filter<ff_8::tile_operations::LayerIdT<TileT>, ff_8::tile_operations::LayerId, FilterTag::LayerId>               layer_id        = {};
     filter<ff_8::tile_operations::TextureIdT<TileT>, ff_8::tile_operations::TextureId, FilterTag::TexturePageId>           texture_page_id = {};
     filter<ff_8::tile_operations::BlendModeT<TileT>, ff_8::tile_operations::BlendMode, FilterTag::BlendMode>           blend_mode      = {};
     filter<ff_8::tile_operations::BlendT<TileT>, ff_8::tile_operations::Blend, FilterTag::BlendOther>                   blend_other     = {};
     filter<ff_8::tile_operations::DepthT<TileT>, ff_8::tile_operations::Depth, FilterTag::Bpp>                          bpp =
       filter<ff_8::tile_operations::DepthT<TileT>, ff_8::tile_operations::Depth, FilterTag::Bpp>{ open_viii::graphics::BPPT::BPP4_CONST() };


     filters()
     {
          fme::Configuration const config{};

          pupu      = decltype(pupu){ std::bit_cast<PupuID>(config["filter_pupu"].value_or(PupuID{}.raw())),
                                      config["filter_pupu_enabled"].value_or(false) };
          upscale   = decltype(upscale){ std::filesystem::path{ config["filter_upscale"].value_or(std::string{}) },
                                         config["filter_upscale_enabled"].value_or(false) };
          deswizzle = decltype(deswizzle){ std::filesystem::path{ config["filter_deswizzle"].value_or(std::string{}) },
                                           config["filter_deswizzle_enabled"].value_or(false) };
          draw_bit  = decltype(draw_bit){ static_cast<draw_bitT>(config["filter_draw"].value_or(std::to_underlying(draw_bitT{}))),
                                          config["filter_draw_enabled"].value_or(false) };
          z = decltype(z){ config["filter_z"].value_or(ff_8::tile_operations::ZT<TileT>{}), config["filter_z_enabled"].value_or(false) };
          palette      = decltype(palette){ config["filter_palette"].value_or(ff_8::tile_operations::PaletteIdT<TileT>{}) & 0xFU,
                                            config["filter_palette_enabled"].value_or(false) };
          animation_id = decltype(animation_id){ config["filter_animation_id"].value_or(ff_8::tile_operations::AnimationIdT<TileT>{}),
                                                 config["filter_animation_id_enabled"].value_or(false) };
          animation_frame =
            decltype(animation_frame){ config["filter_animation_frame"].value_or(ff_8::tile_operations::AnimationStateT<TileT>{}),
                                       config["filter_animation_frame_enabled"].value_or(false) };
          layer_id = decltype(layer_id){ config["filter_layer_id"].value_or(ff_8::tile_operations::LayerIdT<TileT>{}),
                                         config["filter_layer_id_enabled"].value_or(false) };
          texture_page_id =
            decltype(texture_page_id){ config["filter_texture_page_id"].value_or(ff_8::tile_operations::TextureIdT<TileT>{}),
                                       config["filter_texture_page_id_enabled"].value_or(false) };
          blend_mode = decltype(blend_mode){
               static_cast<ff_8::tile_operations::BlendModeT<TileT>>(config["filter_blend_mode"].value_or(
                 std::to_underlying(ff_8::tile_operations::BlendModeT<TileT>{ ff_8::tile_operations::BlendModeT<TileT>::none }))),
               config["filter_blend_mode_enabled"].value_or(false)
          };
          blend_other = decltype(blend_other){ config["filter_blend_other"].value_or(ff_8::tile_operations::BlendT<TileT>{}),
                                               config["filter_blend_other_enabled"].value_or(false) };
          bpp         = decltype(bpp){ ff_8::tile_operations::DepthT<TileT>{
                                 config["filter_bpp"].value_or(ff_8::tile_operations::DepthT<TileT>::BPP4_CONST().raw()) & 3U },
                                       config["filter_bpp_enabled"].value_or(false) };
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
