//
// Created by pcvii on 6/14/2022.
//

#ifndef FIELD_MAP_EDITOR_TILE_OPERATIONS_HPP
#define FIELD_MAP_EDITOR_TILE_OPERATIONS_HPP
#include <open_viii/graphics/background/Map.hpp>
#include <spdlog/spdlog.h>
namespace ff_8::tile_operations
{
template<open_viii::graphics::background::is_tile TileT>
static constexpr TileT MaxTile = []() {
     std::array<std::uint8_t, sizeof(TileT)> tmp{};
     // std::fill(tmp.begin(), tmp.end(), 0xFFU);
     tmp.fill((std::numeric_limits<std::uint8_t>::max)());
     return std::bit_cast<TileT>(tmp);
}();
#define CONCAT(a, b) a##b
#define TILE_OPERATION(STRING, FUNCTION)                                                                                            \
     template<open_viii::graphics::background::is_tile TileT>                                                                       \
     using CONCAT(STRING, T) = typename std::remove_cvref_t<std::invoke_result_t<decltype(&TileT::FUNCTION), TileT>>;               \
     struct STRING                                                                                                                  \
     {                                                                                                                              \
          template<open_viii::graphics::background::is_tile TileT>                                                                  \
          constexpr CONCAT(STRING, T)<TileT> operator()(const TileT &tile) const noexcept                                           \
          {                                                                                                                         \
               return tile.FUNCTION();                                                                                              \
          }                                                                                                                         \
     };                                                                                                                             \
     template<open_viii::graphics::background::is_tile TileT>                                                                       \
     struct CONCAT(STRING, Match)                                                                                                   \
     {                                                                                                                              \
          using ValueT = CONCAT(STRING, T)<TileT>;                                                                                  \
          constexpr explicit CONCAT(STRING, Match)(TileT tile)                                                                      \
            : CONCAT(STRING, Match)(tile.FUNCTION())                                                                                \
          {                                                                                                                         \
          }                                                                                                                         \
          constexpr explicit CONCAT(STRING, Match)(ValueT value)                                                                    \
               requires(!open_viii::graphics::background::is_tile<ValueT>)                                                          \
            : m_value(std::move(value))                                                                                             \
          {                                                                                                                         \
          }                                                                                                                         \
                                                                                                                                    \
          constexpr bool operator()(const TileT &tile) const noexcept                                                               \
          {                                                                                                                         \
               return static_cast < CONCAT(STRING, T) < TileT >> (m_value) == tile.FUNCTION();                                      \
          }                                                                                                                         \
          auto                 operator<=>(const CONCAT(STRING, Match) &) const = default;                                          \
          std::strong_ordering operator<=>(const ValueT &that) const                                                                \
          {                                                                                                                         \
               return m_value <=> that;                                                                                             \
          }                                                                                                                         \
                                                                                                                                    \
          std::strong_ordering operator<=>(const TileT &tile) const                                                                 \
          {                                                                                                                         \
               return static_cast < CONCAT(STRING, T) < TileT >> (m_value) <=> tile.FUNCTION();                                     \
          }                                                                                                                         \
                                                                                                                                    \
        private:                                                                                                                    \
          ValueT m_value = {};                                                                                                      \
     };                                                                                                                             \
     struct CONCAT(STRING, DefaultValue)                                                                                            \
     {                                                                                                                              \
          template<open_viii::graphics::background::is_tile T>                                                                      \
          constexpr CONCAT(STRING, T)<T> operator()(const T &) const noexcept                                                       \
          {                                                                                                                         \
               return {};                                                                                                           \
          }                                                                                                                         \
     };                                                                                                                             \
     template<typename TileT>                                                                                                       \
     concept CONCAT(has_with_, FUNCTION) =                                                                                           \
       requires(std::remove_cvref_t<TileT> tile, CONCAT(STRING, T) < TileT > value) { tile = tile.CONCAT(with_, FUNCTION)(value); }; \
     template<open_viii::graphics::background::is_tile TileT>                                                                       \
     struct CONCAT(With, STRING)                                                                                                    \
     {                                                                                                                              \
          constexpr explicit CONCAT(With, STRING)(CONCAT(STRING, T) < TileT > value)                                                \
            : m_value(std::move(value))                                                                                             \
          {                                                                                                                         \
          }                                                                                                                         \
                                                                                                                                    \
          [[nodiscard]] constexpr TileT operator()(const TileT &tile) const noexcept                                                \
               requires(CONCAT(has_with_, FUNCTION) < TileT >)                                                                       \
          {                                                                                                                         \
               spdlog::debug("Used with_" #FUNCTION " to assign: {}", m_value);                                                     \
               return tile.CONCAT(with_, FUNCTION)(static_cast < CONCAT(STRING, T) < TileT >> (m_value));                            \
          }                                                                                                                         \
          constexpr decltype(auto) operator()(const TileT &tile) const noexcept                                                     \
               requires(!CONCAT(has_with_, FUNCTION) < TileT >)                                                                      \
          { /*donno why I can not forward or move when not has_with*/                                                               \
               spdlog::debug("Has no with_" #FUNCTION);                                                                             \
               return tile;                                                                                                         \
          }                                                                                                                         \
          friend constexpr TileT operator|(const TileT &tile, const CONCAT(With, STRING) & operation)                               \
          {                                                                                                                         \
               return operation.operator()(tile);                                                                                   \
          }                                                                                                                         \
                                                                                                                                    \
        private:                                                                                                                    \
          CONCAT(STRING, T)<TileT> m_value = {};                                                                                    \
     };                                                                                                                             \
     template<open_viii::graphics::background::is_tile TileT>                                                                       \
     struct CONCAT(TranslateWith, STRING)                                                                                           \
     {                                                                                                                              \
          using ValueT = CONCAT(STRING, T)<TileT>;                                                                                  \
          constexpr explicit CONCAT(TranslateWith, STRING)(const TileT &tile, ValueT to_value)                                      \
            : m_to(std::move(to_value))                                                                                             \
            , m_from(tile.FUNCTION())                                                                                               \
          {                                                                                                                         \
          }                                                                                                                         \
          constexpr explicit CONCAT(TranslateWith, STRING)(ValueT from_value, ValueT to_value)                                      \
            : m_to(std::move(to_value))                                                                                             \
            , m_from(std::move(from_value))                                                                                         \
          {                                                                                                                         \
          }                                                                                                                         \
                                                                                                                                    \
                                                                                                                                    \
          [[nodiscard]] constexpr TileT operator()(const TileT &tile) const noexcept                                                \
               requires(CONCAT(has_with_, FUNCTION) < TileT >)                                                                       \
          {                                                                                                                         \
               const auto current = m_to + tile.FUNCTION() - m_from;                                                                \
               spdlog::debug("Used with_" #FUNCTION " to assign: {}", current);                                                     \
               return tile.CONCAT(with_, FUNCTION)(static_cast<ValueT>(current));                                                    \
          }                                                                                                                         \
                                                                                                                                    \
                                                                                                                                    \
          constexpr decltype(auto) operator()(const TileT &tile) const noexcept                                                     \
               requires(!CONCAT(has_with_, FUNCTION) < TileT >)                                                                      \
          { /*donno why I can not forward or move when not has_with*/                                                               \
               spdlog::debug("Has no with_" #FUNCTION);                                                                             \
               return tile;                                                                                                         \
          }                                                                                                                         \
                                                                                                                                    \
        private:                                                                                                                    \
          ValueT m_to   = {};                                                                                                       \
          ValueT m_from = {}; /*used to calc offset using original*/                                                                \
     };                                                                                                                             \
     template<open_viii::graphics::background::is_tile TileT>                                                                       \
     struct CONCAT(STRING, Group)                                                                                                   \
     {                                                                                                                              \
          using value_type                  = CONCAT(STRING, T)<TileT>;                                                             \
          using get                         = STRING;                                                                               \
          using get_default                 = CONCAT(STRING, DefaultValue);                                                         \
          constexpr CONCAT(STRING, Group)() = default;                                                                              \
          constexpr explicit CONCAT(STRING, Group)(value_type value)                                                                \
            : current(std::move(value))                                                                                             \
          {                                                                                                                         \
          }                                                                                                                         \
          constexpr explicit CONCAT(STRING, Group)(TileT tile)                                                                      \
            : current(get{}(tile))                                                                                                  \
          {                                                                                                                         \
          }                                                                                                                         \
          static constexpr value_type min_value = []() -> value_type {                                                              \
               if constexpr (std::signed_integral<value_type>)                                                                      \
               {                                                                                                                    \
                    return (std::numeric_limits<value_type>::min)();                                                                \
               }                                                                                                                    \
               else                                                                                                                 \
               {                                                                                                                    \
                    const auto get_f = get{};                                                                                       \
                    return get_f(TileT{});                                                                                          \
               }                                                                                                                    \
          }();                                                                                                                      \
          static constexpr value_type max_value = []() -> value_type {                                                              \
               if constexpr (std::signed_integral<value_type>)                                                                      \
               {                                                                                                                    \
                    return (std::numeric_limits<value_type>::max)();                                                                \
               }                                                                                                                    \
               else                                                                                                                 \
               {                                                                                                                    \
                    const auto get_f = get{};                                                                                       \
                    return get_f(MaxTile<TileT>);                                                                                   \
               }                                                                                                                    \
          }();                                                                                                                      \
          value_type            current   = {};                                                                                     \
          constexpr static bool read_only = !CONCAT(has_with_, FUNCTION)<TileT>;                                                     \
          using transform_with            = CONCAT(With, STRING)<value_type>;                                                       \
          using match_with                = CONCAT(STRING, Match)<value_type>;                                                      \
     }

TILE_OPERATION(X, x);
TILE_OPERATION(Y, y);
TILE_OPERATION(XY, xy);
TILE_OPERATION(Z, z);
TILE_OPERATION(SourceX, source_x);
TILE_OPERATION(SourceY, source_y);
TILE_OPERATION(SourceXY, source_xy);
TILE_OPERATION(TextureId, texture_id);
TILE_OPERATION(BlendMode, blend_mode);
TILE_OPERATION(Blend, blend);
TILE_OPERATION(Draw, draw);
TILE_OPERATION(Depth, depth);
TILE_OPERATION(LayerId, layer_id);
TILE_OPERATION(PaletteId, palette_id);
TILE_OPERATION(AnimationId, animation_id);
TILE_OPERATION(AnimationState, animation_state);
#undef TILE_OPERATION
#undef CONCAT
struct NotInvalidTile
{
     template<open_viii::graphics::background::is_tile T>
     constexpr bool operator()(const T &tile) const noexcept
     {
          return (std::cmp_not_equal(tile.x(), s_end_x));
     }

   private:
     static constexpr std::uint16_t s_end_x = { 0x7FFFU };
};
}// namespace ff_8::tile_operations
#endif// FIELD_MAP_EDITOR_TILE_OPERATIONS_HPP
