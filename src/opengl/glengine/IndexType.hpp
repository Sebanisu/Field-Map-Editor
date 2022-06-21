//
// Created by pcvii on 12/6/2021.
//

#ifndef FIELD_MAP_EDITOR_INDEXTYPE_HPP
#define FIELD_MAP_EDITOR_INDEXTYPE_HPP
namespace glengine
{
enum class IndexType : GLenum
{
  None          = 0,
  UnsignedByte  = GL_UNSIGNED_BYTE,
  UnsignedShort = GL_UNSIGNED_SHORT,
  UnsignedInt   = GL_UNSIGNED_INT,
};

inline constexpr GLenum operator+(IndexType it)
{
  return static_cast<GLenum>(it);
}
inline constexpr IndexType operator+(IndexType l, IndexType r)
{
  auto value = static_cast<IndexType>((+l) + (+r));
  assert(
    value == IndexType::None || value == IndexType::UnsignedByte
    || value == IndexType::UnsignedShort || value == IndexType::UnsignedInt);
  return value;
}
template<typename T>
inline constexpr IndexType GetIndexType() = delete;
template<>
inline constexpr IndexType GetIndexType<GLbyte>()
{
  return IndexType::UnsignedByte;
}
template<>
inline constexpr IndexType GetIndexType<GLushort>()
{
  return IndexType::UnsignedShort;
}
template<>
inline constexpr IndexType GetIndexType<GLuint>()
{
  return IndexType::UnsignedInt;
}
template<typename T>
concept has_Type_for_IndexType =
  requires(const T &t) { t.type() + IndexType::None; };
template<typename T>
// clang-format off
  requires(!has_Type_for_IndexType<T>)
// clang-format on
inline IndexType Type(const T &) noexcept
{
  return IndexType::None;
}
template<has_Type_for_IndexType T>
inline IndexType Type(const T &typed) noexcept
{
  return typed.type();
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
inline consteval bool CheckHasTypeForIndexType()
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
  requires(sizeof...(T) > 1U && CheckHasTypeForIndexType<T...>())
// clang-format on
inline IndexType Type(const T &...typed)
{
  return (glengine::Type<T>(typed) + ...);
}
}// namespace glengine
#endif// FIELD_MAP_EDITOR_INDEXTYPE_HPP
