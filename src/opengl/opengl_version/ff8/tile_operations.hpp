//
// Created by pcvii on 6/14/2022.
//

#ifndef FIELD_MAP_EDITOR_TILE_OPERATIONS_HPP
#define FIELD_MAP_EDITOR_TILE_OPERATIONS_HPP
namespace ff_8
{
namespace tile_operations
{
  template<open_viii::graphics::background::is_tile TileT>
  static constexpr TileT MaxTile = []() {
    std::array<std::uint8_t, sizeof(TileT)> tmp{};
    //std::fill(tmp.begin(), tmp.end(), 0xFFU);
    tmp.fill(0xFFU);
    return std::bit_cast<TileT>(tmp);
  }();
#define TILE_OPERATION(STRING, FUNCTION)                                     \
  template<open_viii::graphics::background::is_tile TileT>                   \
  using STRING##T = typename std::remove_cvref_t<                            \
    std::invoke_result_t<decltype(&TileT::FUNCTION), TileT>>;                \
  struct STRING                                                              \
  {                                                                          \
    template<open_viii::graphics::background::is_tile TileT>                 \
    constexpr STRING##T<TileT> operator()(const TileT &tile) const noexcept  \
    {                                                                        \
      return tile.FUNCTION();                                                \
    }                                                                        \
  };                                                                         \
  template<typename ValueT>                                                  \
  struct STRING##Match                                                       \
  {                                                                          \
    template<open_viii::graphics::background::is_tile TileT>                 \
    constexpr STRING##Match(TileT tile)                                      \
      : STRING##Match(tile.FUNCTION())                                       \
    {                                                                        \
    }                                                                        \
    constexpr STRING##Match(ValueT value)                                    \
      requires(!open_viii::graphics::background::is_tile<ValueT>)            \
    : m_value(std::move(value))                                              \
    {                                                                        \
    }                                                                        \
    template<open_viii::graphics::background::is_tile TileT>                 \
    constexpr bool operator()(const TileT &tile) const noexcept              \
    {                                                                        \
      return static_cast<STRING##T<TileT>>(m_value) == tile.FUNCTION();      \
    }                                                                        \
    auto                 operator<=>(const STRING##Match &) const = default; \
    std::strong_ordering operator<=>(const ValueT &that) const               \
    {                                                                        \
      return m_value <=> that;                                               \
    }                                                                        \
    template<open_viii::graphics::background::is_tile TileT>                 \
    std::strong_ordering operator<=>(const TileT &tile) const                \
    {                                                                        \
      return static_cast<STRING##T<TileT>>(m_value) <=> tile.FUNCTION();     \
    }                                                                        \
                                                                             \
  private:                                                                   \
    ValueT m_value = {};                                                     \
  };                                                                         \
  struct STRING##DefaultValue                                                \
  {                                                                          \
    template<open_viii::graphics::background::is_tile T>                     \
    constexpr STRING##T<T> operator()(const T &) const noexcept              \
    {                                                                        \
      return {};                                                             \
    }                                                                        \
  };                                                                         \
  template<typename T>                                                       \
  concept has_with_##FUNCTION =                                              \
    requires(std::remove_cvref_t<T> t)                                       \
  {                                                                          \
    t = t.with_##FUNCTION(STRING##T<T>{});                                   \
  };                                                                         \
  template<typename ValueT>                                                  \
  struct With##STRING                                                        \
  {                                                                          \
    constexpr With##STRING(ValueT value)                                     \
      : m_value(std::move(value))                                            \
    {                                                                        \
    }                                                                        \
    template<open_viii::graphics::background::is_tile TileT>                 \
    constexpr decltype(auto) operator()(TileT &&tile) const noexcept         \
    {                                                                        \
      if constexpr (has_with_##FUNCTION<TileT>)                              \
      {                                                                      \
        return tile.with_##FUNCTION(static_cast<STRING##T<TileT>>(m_value)); \
      }                                                                      \
      else                                                                   \
      {                                                                      \
        return std::forward<TileT>(tile);                                    \
      }                                                                      \
    }                                                                        \
                                                                             \
  private:                                                                   \
    ValueT m_value = {};                                                     \
  };                                                                         \
  template<open_viii::graphics::background::is_tile TileT>                   \
  struct STRING##Group                                                       \
  {                                                                          \
    using value_type          = STRING##T<TileT>;                            \
    using get                 = STRING;                                      \
    using get_default         = STRING##DefaultValue;                        \
    constexpr STRING##Group() = default;                                     \
    constexpr STRING##Group(value_type value)                                \
      : current(std::move(value))                                            \
    {                                                                        \
    }                                                                        \
    constexpr STRING##Group(TileT tile)                                      \
      : current(get{}(tile))                                                 \
    {                                                                        \
    }                                                                        \
    static constexpr value_type min_value = []() -> value_type {             \
      if constexpr (std::signed_integral<value_type>)                        \
      {                                                                      \
        return (std::numeric_limits<value_type>::min)();                     \
      }                                                                      \
      else                                                                   \
      {                                                                      \
        const auto get_f = get{};                                            \
        return get_f(TileT{});                                               \
      }                                                                      \
    }();                                                                     \
    static constexpr value_type max_value = []() -> value_type {             \
      if constexpr (std::signed_integral<value_type>)                        \
      {                                                                      \
        return (std::numeric_limits<value_type>::max)();                     \
      }                                                                      \
      else                                                                   \
      {                                                                      \
        const auto get_f = get{};                                            \
        return get_f(MaxTile<TileT>);                                        \
      }                                                                      \
    }();                                                                     \
    value_type            current   = {};                                    \
    constexpr static bool read_only = !has_with_##FUNCTION<TileT>;           \
    using transform_with            = With##STRING<value_type>;              \
    using match_with                = STRING##Match<value_type>;             \
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
  struct InvalidTile
  {
    template<open_viii::graphics::background::is_tile T>
    constexpr bool operator()(const T &tile) const noexcept
    {
      return (std::cmp_not_equal(tile.x(), s_end_x));
    }

  private:
    static constexpr std::uint16_t s_end_x = { 0x7FFFU };
  };
}// namespace tile_operations
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_TILE_OPERATIONS_HPP
