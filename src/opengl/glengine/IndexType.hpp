//
// Created by pcvii on 12/6/2021.
//

#ifndef FIELD_MAP_EDITOR_INDEXTYPE_HPP
#define FIELD_MAP_EDITOR_INDEXTYPE_HPP
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iosfwd>
namespace glengine
{
enum class IndexType : GLenum
{
  none           = 0,
  UNSIGNED_BYTE  = GL_UNSIGNED_BYTE,
  UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
  UNSIGNED_INT   = GL_UNSIGNED_INT,
};

inline constexpr GLenum operator+(IndexType it)
{
  return static_cast<GLenum>(it);
}
inline constexpr IndexType operator+(IndexType l, IndexType r)
{
  auto value = static_cast<IndexType>((+l) + (+r));
  assert(
    value == IndexType::none || value == IndexType::UNSIGNED_BYTE
    || value == IndexType::UNSIGNED_SHORT || value == IndexType::UNSIGNED_INT);
  return value;
}
template<typename T>
inline constexpr IndexType GetIndexType() = delete;
template<>
inline constexpr IndexType GetIndexType<GLbyte>()
{
  return IndexType::UNSIGNED_BYTE;
}
template<>
inline constexpr IndexType GetIndexType<GLushort>()
{
  return IndexType::UNSIGNED_SHORT;
}
template<>
inline constexpr IndexType GetIndexType<GLuint>()
{
  return IndexType::UNSIGNED_INT;
}
template<typename T>
concept has_Type_for_IndexType =
  requires(const T &t) { t.Type() + IndexType::none; };
template<typename T>
// clang-format off
  requires(!has_Type_for_IndexType<T>)
// clang-format on
inline IndexType Type(const T &) noexcept
{
  return IndexType::none;
}
template<has_Type_for_IndexType T>
inline IndexType Type(const T &typed) noexcept
{
  return typed.Type();
}
template<typename T>
// clang-format off
  requires(!has_Type_for_IndexType<T>)
// clang-format on
consteval inline std::size_t CountType() noexcept
{
  return {};
}
template<has_Type_for_IndexType T>
consteval inline std::size_t CountType() noexcept
{
  return 1U;
}
template<typename... T>
inline consteval bool check_has_Type_for_IndexType()
{
  if constexpr (sizeof...(T) != 0U)
  {
    return (CountType<T>() + ...) == std::size_t{ 1U };
  }
  else
  {
    return false;
  }
}
template<typename... T>
// clang-format off
  requires(sizeof...(T) > 1U && check_has_Type_for_IndexType<T...>())
// clang-format on
inline IndexType Type(const T &...typed)
{
  return (glengine::Type<T>(typed) + ...);
}
}// namespace glengine
#endif// FIELD_MAP_EDITOR_INDEXTYPE_HPP
