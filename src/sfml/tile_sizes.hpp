//
// Created by pcvii on 2/15/2023.
//

#ifndef FIELD_MAP_EDITOR_TILE_SIZES_HPP
#define FIELD_MAP_EDITOR_TILE_SIZES_HPP
#include <cstdint>
#include <fmt/format.h>
enum class tile_sizes : std::uint16_t
{
  default_size = 16U,
  x_2_size     = default_size << 1U,
  x_4_size     = default_size << 2U,
  x_8_size     = default_size << 3U,
  x_16_size    = default_size << 4U,
};

template<>
struct fmt::formatter<tile_sizes>
  : fmt::formatter<std::underlying_type_t<tile_sizes>>
{
  // parse is inherited from formatter<std::underlying_type_t<tile_sizes>>.
  template<typename FormatContext>
  auto format(tile_sizes tile_size, FormatContext &ctx) const
  {
    return fmt::formatter<std::underlying_type_t<tile_sizes>>::format(
      static_cast<std::underlying_type_t<tile_sizes>>(tile_size), ctx);
  }
};
template<typename T>
inline static auto operator*(const T &num, tile_sizes tile_size)
{
  return num * static_cast<std::underlying_type_t<tile_sizes>>(tile_size);
}

template<typename T>
inline static auto operator*(tile_sizes tile_size, const T &num)
{
  return static_cast<std::underlying_type_t<tile_sizes>>(tile_size) * num;
}

template<typename T>
inline static auto operator+(const T &num, tile_sizes tile_size)
{
  return num + static_cast<std::underlying_type_t<tile_sizes>>(tile_size);
}

template<typename T>
inline static auto operator+(tile_sizes tile_size, const T &num)
{
  return static_cast<std::underlying_type_t<tile_sizes>>(tile_size) + num;
}
#endif// FIELD_MAP_EDITOR_TILE_SIZES_HPP
