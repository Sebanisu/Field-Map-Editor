//
// Created by pcvii on 6/14/2022.
//

#ifndef FIELD_MAP_EDITOR_TILE_OPERATIONS_HPP
#define FIELD_MAP_EDITOR_TILE_OPERATIONS_HPP
namespace ff8
{
namespace tile_operations
{
#define TILE_OPERATION(STRING)                                               \
  template<open_viii::graphics::background::is_tile T>                       \
  using STRING##_t = typename std::invoke_result_t<decltype(&T::STRING), T>; \
  struct STRING                                                              \
  {                                                                          \
    template<open_viii::graphics::background::is_tile T>                     \
    constexpr STRING##_t<T> operator()(const T &tile) const noexcept         \
    {                                                                        \
      return tile.STRING();                                                  \
    }                                                                        \
  };                                                                         \
  struct STRING##_default_value                                              \
  {                                                                          \
    template<open_viii::graphics::background::is_tile T>                     \
    constexpr STRING##_t<T> operator()(const T &) const noexcept             \
    {                                                                        \
      return {};                                                             \
    }                                                                        \
  }
  TILE_OPERATION(x);
  TILE_OPERATION(y);
  TILE_OPERATION(z);
  TILE_OPERATION(source_x);
  TILE_OPERATION(source_y);
  TILE_OPERATION(texture_id);
#undef TILE_OPERATION
}// namespace tile_operations
}// namespace ff8
#endif// FIELD_MAP_EDITOR_TILE_OPERATIONS_HPP
