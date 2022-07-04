//
// Created by pcvii on 6/14/2022.
//

#ifndef FIELD_MAP_EDITOR_TILE_OPERATIONS_HPP
#define FIELD_MAP_EDITOR_TILE_OPERATIONS_HPP
namespace ff_8
{
namespace tile_operations
{
#define TILE_OPERATION(STRING, FUNCTION)                                     \
  template<open_viii::graphics::background::is_tile T>                       \
  using STRING##T = typename std::remove_cvref_t<                            \
    std::invoke_result_t<decltype(&T::FUNCTION), T>>;                        \
  struct STRING                                                              \
  {                                                                          \
    template<open_viii::graphics::background::is_tile T>                     \
    constexpr STRING##T<T> operator()(const T &tile) const noexcept          \
    {                                                                        \
      return tile.FUNCTION();                                                \
    }                                                                        \
  };                                                                         \
  template<typename V>                                                       \
  struct STRING##Match                                                       \
  {                                                                          \
    constexpr STRING##Match(V value)                                         \
      : m_value(std::move(value))                                            \
    {                                                                        \
    }                                                                        \
    template<open_viii::graphics::background::is_tile T>                     \
    constexpr bool operator()(const T &tile) const noexcept                  \
    {                                                                        \
      return static_cast<STRING##T<T>>(m_value) == tile.FUNCTION();          \
    }                                                                        \
    auto                 operator<=>(const STRING##Match &) const = default; \
    std::strong_ordering operator<=>(const V &that) const                    \
    {                                                                        \
      return m_value <=> that;                                               \
    }                                                                        \
    template<open_viii::graphics::background::is_tile T>                     \
    std::strong_ordering operator<=>(const T &tile) const                    \
    {                                                                        \
      return static_cast<STRING##T<T>>(m_value) <=> tile.FUNCTION();         \
    }                                                                        \
                                                                             \
  private:                                                                   \
    V m_value = {};                                                          \
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
