//
// Created by pcvii on 6/14/2022.
//

#ifndef FIELD_MAP_EDITOR_TILE_OPERATIONS_HPP
#define FIELD_MAP_EDITOR_TILE_OPERATIONS_HPP
namespace ff_8
{
namespace tile_operations
{
#define TILE_OPERATION(STRING)                                                \
  template<open_viii::graphics::background::is_tile T>                        \
  using STRING##_t = typename std::invoke_result_t<decltype(&T::STRING), T>;  \
  struct STRING                                                               \
  {                                                                           \
    template<open_viii::graphics::background::is_tile T>                      \
    constexpr STRING##_t<T> operator()(const T &tile) const noexcept          \
    {                                                                         \
      return tile.STRING();                                                   \
    }                                                                         \
  };                                                                          \
  template<typename V>                                                        \
  struct STRING##_match                                                       \
  {                                                                           \
    constexpr STRING##_match(V value)                                         \
      : m_value(std::move(value))                                             \
    {                                                                         \
    }                                                                         \
    template<open_viii::graphics::background::is_tile T>                      \
    constexpr bool operator()(const T &tile) const noexcept                   \
    {                                                                         \
      return static_cast<STRING##_t<T>>(m_value) == tile.STRING();            \
    }                                                                         \
    auto                 operator<=>(const STRING##_match &) const = default; \
    std::strong_ordering operator<=>(const V &that) const                     \
    {                                                                         \
      return m_value <=> that;                                                \
    }                                                                         \
    template<open_viii::graphics::background::is_tile T>                      \
    std::strong_ordering operator<=>(const T &tile) const                     \
    {                                                                         \
      return static_cast<STRING##_t<T>>(m_value) <=> tile.STRING();           \
    }                                                                         \
                                                                              \
  private:                                                                    \
    V m_value = {};                                                           \
  };                                                                          \
  struct STRING##_default_value                                               \
  {                                                                           \
    template<open_viii::graphics::background::is_tile T>                      \
    constexpr STRING##_t<T> operator()(const T &) const noexcept              \
    {                                                                         \
      return {};                                                              \
    }                                                                         \
  }
  TILE_OPERATION(x);
  TILE_OPERATION(y);
  TILE_OPERATION(z);
  TILE_OPERATION(source_x);
  TILE_OPERATION(source_y);
  TILE_OPERATION(texture_id);
  TILE_OPERATION(blend_mode);
  TILE_OPERATION(blend);
  TILE_OPERATION(draw);
  TILE_OPERATION(layer_id);
  TILE_OPERATION(palette_id);
  TILE_OPERATION(animation_id);
  TILE_OPERATION(animation_state);
#undef TILE_OPERATION
  struct invalid_tile
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
